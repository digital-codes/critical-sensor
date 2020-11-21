#!/usr/bin/python3
"""Critical Zones sensor display """
import sys
import os

#maybe we need json
import json

from datetime import datetime
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

import shutil

# show or copy file
Show = True
# set to false on server


plt.figure(figsize=(15,10))

df = pd.read_json("https://critical-sensors.de/srv.php")
# subset fortesting
#df = df[-100:]
#df = df[df.id==2]

# sensor names from javascript
SensorLabels = {"de":["Referenz","Wiese1","Wiese2","Büro","ZKM innen","ZKM außen"],
"en":["Reference","Orchard1","Orchard2","Office","ZKM indoor","ZKM outdoor"]}

def mapEn(x):
    return SensorLabels["en"][x]
def mapDe(x):
    return SensorLabels["de"][x]


sns.set_theme(style="white")

# number of colors mus match number of items
BasePal = ["#cc0000", "#00cc00", "#0000cc", "#e74c3c", "#34495e", "#2ecc71"]
pal = BasePal[:len(df.id.unique())]
#sns.set_palette(flatui)

sns.set(font_scale=1.5)
# for axis legend:
# https://stackoverflow.com/questions/39961147/how-to-align-y-labels-in-a-seaborn-pairgrid


def mkPlot(df,lang):
    features = ['co2', 'light', "temp", "hum","Sensor"]
    pltFeatures = features
    file = "splom_en.png"
    title = "Sensors" #"Pair Grid"
    if lang == "de":
        df["Sensor"] = df.id.apply(mapDe)
        features_de = ['CO2', 'Licht', "Temp", "Feucht","Sensor"]
        pltFeatures = features_de
        featureMap = {}
        for i,f in enumerate(features):
            featureMap.update({f:features_de[i]})
        df.rename(columns=featureMap,inplace=True)
        file = "splom_de.png"
        #title = "Paarweise\nDarstellung"
        title = "Sensoren"
    else:
        df["Sensor"] = df.id.apply(mapEn)

    g = sns.PairGrid(df[pltFeatures],diag_sharey=False,
                     dropna=True,layout_pad = 1, height=2,
                     aspect=4/3 , hue="Sensor",palette=pal)

    g.map_upper(sns.scatterplot)
    g.map_lower(sns.kdeplot)
    g.map_diag(sns.kdeplot)

    g.add_legend(title=title)

    for ax in g.axes[:,0]:
        ax.get_yaxis().set_label_coords(-.4,0.3)

    # force output ratio and size (approx)
    g.fig.set_dpi(96)
    g.fig.set_figwidth(12)
    g.fig.set_figheight(9)
    g.fig.set_tight_layout(False) # legend inside if ture

    g.savefig(file)
    if not Show:
        shutil.copy(file,"/var/www/html/cs/img/")

    return g

# #############
g = mkPlot(df,"en")
g = mkPlot(df,"de")

        
if Show:
    plt.show()

