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

sns.set_theme(style="white")

df = pd.read_json("https://critical-sensors.de/srv.php")

# sensor names from javascript
SensorLabels = {"de":["Referenz","Wiese1","Wiese2","Büro","ZKM innen","ZKM außen"],
"en":["Reference","Orchard1","Orchard2","Office","ZKM indoor","ZKM outdoor"]}

def mapEn(x):
    return SensorLabels["en"][x]
def mapDe(x):
    return SensorLabels["de"][x]


# select one sensor only 
#df = df.loc[df.id == 2]

# number of colors mus match number of items
BasePal = ["#cc0000", "#00cc00", "#0000cc", "#e74c3c", "#34495e", "#2ecc71"]
pal = BasePal[:len(df.id.unique())]
#sns.set_palette(flatui)

sns.set(font_scale=1.5)

# scatter matrix
df["Sensor"] = df.id.apply(mapEn)
features = ['co2', 'light', "temp", "hum","Sensor"]

g = sns.PairGrid(df[features],diag_sharey=False,
                 dropna=True,layout_pad = 1, height=2,
                 aspect=4/3 , hue="Sensor",palette=pal)
# v1
#g.map_diag(sns.histplot, multiple="stack", element="step")
#g.map_offdiag(sns.scatterplot)
# v2
g.map_upper(sns.scatterplot)
g.map_lower(sns.kdeplot)
g.map_diag(sns.kdeplot)

g.add_legend()
g.savefig("splom_en.png")

df["Sensor"] = df.id.apply(mapDe)
features_de = ['CO2', 'Licht', "Temp", "Feucht","Sensor"]
featureMap = {}
for i,f in enumerate(features):
    featureMap.update({f:features_de[i]})
    
df.rename(columns=featureMap,inplace=True)
g = sns.PairGrid(df[features_de],diag_sharey=False,
                 dropna=True,layout_pad = 1, height=2,
                 aspect=4/3 , hue="Sensor",palette=pal)
# v1
#g.map_diag(sns.histplot, multiple="stack", element="step")
#g.map_offdiag(sns.scatterplot)
# v2
g.map_upper(sns.scatterplot)
g.map_lower(sns.kdeplot)
g.map_diag(sns.kdeplot)

g.add_legend()
g.savefig("splom_de.png")

if Show:
    plt.show()
else:
    shutil.copy("splom_en.png","/var/www/html/cs/img/splom_en.png")
    shutil.copy("splom_de.png","/var/www/html/cs/img/splom_de.png")
    
