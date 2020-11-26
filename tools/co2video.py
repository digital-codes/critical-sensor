import numpy as np
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import math
import sys
import os

import datetime as dt


# set to true if you have the csv's saved already
localData = False

oldUrl = "ftp://data.iac.ethz.ch/CMIP6/input4MIPs/UoM/GHGConc/CMIP/mon/atmos/UoM-CMIP-1-1-0/GHGConc/gr3-GMNHSH/v20160701/mole_fraction_of_carbon_dioxide_in_air_input4MIPs_GHGConcentrations_CMIP_UoM-CMIP-1-1-0_gr3-GMNHSH_000001-201412.csv"
newUrl = "https://scrippsco2.ucsd.edu/assets/data/atmospheric/mlo_spo_average/mlo_spo_monthly_mean.csv"

if localData:
        df1 = pd.read_csv("co2-old.csv")
        df = pd.read_csv("co2_mm_mlo.csv")
else:
        # data source for year 0 to 2014 is from
        # https://www.climatecollege.unimelb.edu.au/ghg-factsheets
        print("Downloading data: ",oldUrl)
        dfOld = pd.read_csv(oldUrl)
        # drop before 1850 (too old) and after 1957 (we have newer data) 
        dfOld.drop(index=dfOld[dfOld.year < 1850].index,inplace=True)
        dfOld.drop(index=dfOld[dfOld.year >= 1958].index,inplace=True)

        df1 = dfOld[["year","month","data_mean_global"]].copy()
        df1.rename(columns={"year":"Year","month":"Month","data_mean_global":"Avg"},inplace=True)
        df1.reset_index(drop=True,inplace=True)

        print("Downloading data: ",newUrl)
        dfNew=pd.read_csv(newUrl,skiprows=14,header=None) # file has special header. check manually!
        dfNew.columns=(["Year","Month","Flag","MLO","SPO","Avg"])
        df = dfNew[["Year","Month","Avg"]].copy()
        

#sns.set_theme(style="white") #"seaborn-whitegrid")
sns.set_style("white") #"seaborn-whitegrid")

sns.set_style("darkgrid", {"axes.facecolor": ".5"})


print(df1.keys(),df.keys())


# first 2 months are missing on first year. copy!
y = df[:1].copy()
y = y.append(y,ignore_index=True)
y.loc[0,"Month"] = 1
y.loc[1,"Month"] = 2
df = y.append(df,ignore_index=True)

df.reset_index(inplace=True,drop=True)

# add first dataframe
df = df1.append(df)
df.reset_index(drop=True,inplace=True)

df["phi"] = (df.Month-1)/12*(2*math.pi)
# must make increasing angles, or we dont get continuous lines
df.phi += (df.Year - df.Year.min())*2*math.pi

# resampling: converts date to minute value
# create an datetime like index, for example in minutes
df["p"] = pd.to_timedelta(df.index,unit="m")
df.set_index("p",drop=True,inplace=True)
# upsample and interpolate by factor 3 (20S from 1Minute)
df=df.resample("20S").interpolate()
# Integer months and years
df.Month = df.Month.apply(np.floor)
df.Year = df.Year.apply(np.floor)


sns.set(font_scale=1.3)

# color range
cs = 16 # color steps
cp = sns.color_palette("YlOrBr", cs+1)
# lower limit is 200 ppm
avgMin = 200
cw = (df.Avg.max() - avgMin)/cs
def co2Col(i):
    mx = df[:i].Avg.max()
    # mb = df.Avg.min())
    idx = int((mx-avgMin)//cw)
    col = cp[idx]
    #print("color:",col)
    return col

kws = dict(projection='polar',facecolor="#eeeeee")
ylims = (math.floor(df.Avg.min()*.9), math.ceil(df.Avg.max()*1.05))

def mkPlot(df):
    # background and line color of inner plot change with subplot__kws
    # Set up a grid of axes with a polar projection
    g = sns.FacetGrid(df,subplot_kws=kws,height=8,
                      sharex=False, sharey=False, despine=False)

    # set limit
    g.set(ylim=ylims)
    # Draw a scatterplot onto each axes in the grid
    g.map(sns.lineplot, "phi", "Avg",color=co2Col(len(df)), linewidth=1.5)
    #g.fig.set_size_inches(10,10)
    g.fig.set_dpi(96)
    g.fig.set_figwidth(10)
    g.fig.set_figheight(10)
    g.fig.set_tight_layout(False) # legend inside if ture
    g.fig.patch.set_color('#ffffff') # set the background behind polar plot

    g.set(xlabel="", ylabel = "")
    # adding xticks removes the warning on fixed formatter
    g.set(xticks=np.linspace(0,math.pi*2,5))
    g.set(xticklabels=['Jan', 'Mar', 'Jun', 'Sep',""])
    g.set(yticks=np.linspace(ylims[0],ylims[1]-10,6).round(-1).astype(int))
    g.ax.text(2.5,480 ,str(int(df.Year[-1])), fontsize=24)
    g.add_legend()
    return g

# test image
mkPlot(df)
plt.show()
#sys.exit()

try:
        os.mkdir("out")
except:
        print("Output dir exists")
        pass

# we have 3 entries per month due to supersampling
start = 12*3
steps = 12*3
for i in range(start, df.shape[0],steps):
    print(i/df.shape[0] * 100,"%")
    g = mkPlot(df[:i])
    g.savefig(f"out/co2_anim_{i//steps:06}.png")
    g.fig.clf() # clear figure
    plt.close("all") # required to avoid "more than 20 figs " warning

if i < len(df):
    g = mkPlot(df)
    g.savefig(f"out/co2_anim_{len(df)//steps+1:06}.png")

sys.exit()


#output:
# mogrify: trailing ! is important
# rescale: mogrify -scale 1000x970! *.png
# ffmpeg -r 10 -i out/co2_anim_%03d.png -c:v libx264 -vf fps=25 -pix_fmt yuv420p co2.mp4
# ffmpeg with padding
# ffmpeg -r 80 -i out/co2_anim_%06d.png -c:v libx264 -vf fps=25 -vf "pad=width=1290:height=970:x=145:y=0:color=white" -pix_fmt yuv420p co2.mp4
# fmpeg -i co2.mp4 -c:v libvpx -crf 40 -b:v 0 -c:a libvorbis co2.webm
# ffmpeg -i co2.mp4 co2.mov
# or
# convert -delay 10 *.png gif_of_my_images.gif 

# image trailer
# ffmpeg -r 1/5 -i out/co2_anim_000225.png -c:v libx264 -vf fps=25 -vf "pad=width=1290:height=970:x=145:y=0:color=white" -pix_fmt yuv420p 2020.mp4
# see https://stackoverflow.com/questions/35350607/ffmpeg-add-text-frames-to-the-start-of-video
# text trailer
# ffmpeg -f lavfi -r 30 -i color=white:1290x970 -f lavfi -i anullsrc-vf "drawtext="fontfile=..font.ttf:fontcolor=000000:fontsize=44:text='Text':x=40:y=300",fade=t=in:st=0:d=1,fade=t=out:st=3:d=1" -c:v libx264 -b:v 1000k -pix_fmt yuv420p -video_track_timescale 15360 -c:a aac -ar 48000 -ac 2 -sample_fmt fltp -t 4 trail.mp4
# 
# the timebase of trailer might mismatch, resulting in a too long or short video.
# use ffprobe to report video parameters
# recode trailer with timebase matching the value, like
# ffmpeg -i 2020.mp4 -c copy -video_track_timescale 10240 2020-new.mp4
# if "ffprobe co2_.mp4" gives a tbn of 10240:
#  Duration: 00:00:30.70, start: 0.000000, bitrate: 155 kb/s
#    Stream #0:0(und): Video: h264 (High) (avc1 / 0x31637661), yuv420p, 1290x970 [SAR 1:1 DAR 129:97], 
#    152 kb/s, 20 fps, 20 tbr, 10240 tbn, 40 tbc (default)
# see https://stackoverflow.com/questions/43578882/ffmpeg-concat-makes-video-longer

# write filenames to list.txt
#ffmpeg -f concat -safe 0  -i list.txt -c copy -fflags +genpts co2_.mp4


