#!/bins/python3
"""DWD and ICOS data """
import sys
import os
import json
import csv

from datetime import datetime
import pandas as pd
import numpy as np

# for dwd
import requests
import zipfile
import io

# for icos
from icoscp.cpb.dobj import Dobj
from icoscp.sparql.runsparql import RunSparql

# for sql connection
# see https://www.tutorialspoint.com/sqlalchemy/sqlalchemy_core_sql_expressions.htm
from sqlalchemy import create_engine
from sqlalchemy import Table, Column, Integer, SmallInteger, Float, String, DateTime, MetaData

# ######################################################
# get dwd data
def d(x):
    return datetime.strptime(str(x),"%Y%m%d%H")

# dwd: Alle Zeitangaben in diesem Verzeichnis sind in UTC.
sources = [
{"data":"temp","name":"TT_TU","url":"https://opendata.dwd.de/climate_environment/CDC/observations_germany/climate/hourly/air_temperature/recent/stundenwerte_TU_04177_akt.zip"},
{"data":"hum","name":"RF_TU","url":"https://opendata.dwd.de/climate_environment/CDC/observations_germany/climate/hourly/air_temperature/recent/stundenwerte_TU_04177_akt.zip"},
{"data":"light","name":"SD_SO","url":"https://opendata.dwd.de/climate_environment/CDC/observations_germany/climate/hourly/sun/recent/stundenwerte_SD_04177_akt.zip"},
{"data":"pres","name":"  P0","url":"https://opendata.dwd.de/climate_environment/CDC/observations_germany/climate/hourly/pressure/recent/stundenwerte_P0_04177_akt.zip"}
]


def extract(s,data):
    df = pd.read_csv(io.StringIO(data),sep=";")
    df["date"] = df.MESS_DATUM.apply(d)
    #df.drop(columns=["MESS_DATUM","STATIONS_ID","QN_9","eor"],inplace=True)
    df.drop(df.loc[df.date < "2020-10-01 00:00:00"].index,inplace=True)
    df.rename(columns={s["name"]:s["data"]},inplace=True)
    df.reset_index(inplace=True)
    if s["data"] == "light":
        df.light = df.light / 60 * 100
    #print(s["data"],df)
    return df[["date",s["data"]]]



dwd = pd.DataFrame()

for s in sources:
    r = requests.get(s["url"])
    z = zipfile.ZipFile(io.BytesIO(r.content))
    #z.extractall("/path/to/destination_directory")
    #files = z.filelist
    target = z.filelist[-1].filename
    #print("Target:",target)
    data = z.read(target).decode("utf-8")
    df = extract(s,data)
    #print(df)
    if dwd.empty:
        dwd = df
    else:
        dwd = dwd.merge(df,on="date")

#print(dwd)

# ######################################################
# get icos data
query = """prefix cpmeta: <http://meta.icos-cp.eu/ontologies/cpmeta/>
prefix prov: <http://www.w3.org/ns/prov#>
select ?dobj ?spec ?station ?fileName ?submTime ?timeStart ?timeEnd ?samplingHeight
where {
	VALUES ?spec {<http://meta.icos-cp.eu/resources/cpmeta/atcCo2NrtGrowingDataObject>}
	?dobj cpmeta:hasObjectSpec ?spec .
	?dobj cpmeta:hasName ?fileName .
	?dobj cpmeta:wasSubmittedBy/prov:endedAtTime ?submTime .
	?dobj cpmeta:wasAcquiredBy [
		prov:startedAtTime ?timeStart ;
		prov:endedAtTime ?timeEnd ;
		prov:wasAssociatedWith ?station ;
		cpmeta:hasSamplingHeight ?samplingHeight
	]
	FILTER NOT EXISTS {[] cpmeta:isNextVersionOf ?dobj}
	FILTER( ?samplingHeight = "30"^^xsd:float)
        FILTER(?station = <http://meta.icos-cp.eu/resources/stations/AS_KIT>)
}
order by desc(?timeEnd)
limit 1
"""

s = RunSparql(sparql_query=query, output_format='json')
#The ouput format is by default (txt/json) but you can adjust
# with the following formats ['json', 'csv', 'dict', 'pandas', 'array', 'html'].

r = json.loads(s.run())
print("result: ",r)

pid = r["results"]["bindings"][0]["dobj"]["value"].split("/")[-1]
print("PID: ",pid)

# get the data 
f = Dobj(pid)

if f.valid:
    data = f.get()
else:        
    raise SystemExit('no binary data available', 0)

# print citation
print('citation: ', f.citation)

# convert to dataframe
#df = pd.DataFrame(f.getColumns())
icos = f.get()
#print("Icos keys: ",icos.keys())
icos.drop(index=icos[icos.co2.isnull()].index,inplace=True)
icos.rename(columns={"TIMESTAMP":"date"},inplace=True)
icos.drop(index=icos.loc[icos.date < "2020-10-01 00:00:00"].index,inplace=True)
icos.drop(["Flag","NbPoints","Stdev"],axis=1,inplace=True)
#icos.reset_index(inplace=True)
#icos.drop(["index"],axis=1,inplace=True)
fn = "co2_" + datetime.now().strftime("%Y%m%d-%H%M%S") + ".csv"
icos.to_csv(fn)    

#print(icos)


# ######################################


# merge dwd and icos
# inner join => only rows with entries in both
ref = dwd.merge(icos,on="date")
# or out join => max number of rows with NaNs
# ref = dwd.merge(icos,on="date",how="outer").fillna(0)

# adjust timezone and DST
ref["date"] += pd.Timedelta(hours=1)
ref.loc[ref.date < "2020-10-25 02:00:00","date"] += pd.Timedelta(hours=1)

# add missing columns
ref["rssi"] = -256
ref["count"] = 0
ref["raw"] = ""
ref["rep"] = 0
ref["pkt"] = 0
ref["id"] = 0 # reference is sensor 0
ref["bat"] = 100
ref["req"] = 1

# ######################################
# update sql
"""Store data object into sql """
try:
    #engine = create_engine("mysql://mosquser:mosquitto@localhost/cz")
    engine = create_engine("mysql://cz:1234abcd@localhost/cz")
    tables = engine.table_names()
except:
    print("Sql failed")
    sys.exit()

# connect and write
db = engine.connect()

print("Checking latest value for sensor 0")
srcQry = "select date from sensors where id = 0" \
    +  " order by date desc limit 1;"
#print("Q: ",srcQry)
df = pd.read_sql(srcQry,db)
print("Check result:", df)
# just update repetition counter
if len(df) > 0:
    limit = str(df.date.values[0])
    print("removing old entries before: ",limit)
    ref.drop(index=ref[ref.date <= limit].index,inplace=True)
    print("Remaining: ",ref)
    if ref.empty:
        print("No new data")
        sys.exit()

   

ref.reset_index(inplace=True)
print("Adding data: ",len(ref))

# create new table from the dataframe
ref.to_sql(
    "sensors",
    engine,
    if_exists='append',
    index=False, #True,
    chunksize=500,
    dtype={
        "id":SmallInteger,
        "req":SmallInteger,
        "count":Integer,
        "light":Float,
        "temp":Float,
        "co2":Float,
        "rfu":Float,
        "bat":Float,
        "date":String(50),
        "rssi":SmallInteger,
        "pkt":Integer,
        "raw":String(50),
        "pres":Float,
        "rep":SmallInteger
        }
)
# close
db.close()


