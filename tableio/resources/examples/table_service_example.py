"""
Example of how to write an I3TableService in python 
This saves the table as dictionary and then prints out the data after the tray is finished
"""
import os.path
from I3Tray import I3Tray
from icecube import dataio,tableio

class I3SimpleTable(tableio.I3Table):
    def __init__(self,table_service,table_name,description):
        super(I3SimpleTable, self).__init__(table_service,table_name,description)
        self.data = { n:[] for n in description.field_names}

    def WriteRows(self,row):
        for i in range(row.number_of_rows):
            row.current_row = i
            for k in row.keys():
                self.data[k].append(row[k])

class I3SimpleTableService(tableio.I3TableService):
    def __init__(self):
        super(I3SimpleTableService, self).__init__()
    def CreateTable(self,table_name,description):
        self.table = I3SimpleTable(self,table_name,description)
        return self.table
        
    def CloseFile(self):
        pass
    
table_service = I3SimpleTableService()

fname = os.path.join(os.environ["I3_TESTDATA"],"sim","Level3_nugen_numu_IC86.2012.011069.000000_20events.i3.bz2")
tray=I3Tray()
tray.Add("I3Reader",FileNameList=[fname])
tray.AddModule(tableio.I3TableWriter,
    tableservice = [table_service],
    keys         = ['MPEFit_TWHV'],
    SubEventStreams=['Final'])
tray.Execute()

data = table_service.table.data
fmt = [("Run","{:10d}"),("Event","{:05d}"),("x","{:8.3f}"),("y","{:8.3f}"),
        ("z","{:8.3f}"),("time","{:9.3f}"),("zenith","{:5.5f}"),('azimuth',"{:5.5f}")]
print("       Run Event        x        y        z      time  zenith azimuth")
print("---------- ----- -------- -------- -------- --------- ------- -------")
for i in range(len(data['Run'])):
    print(" ".join([f.format(data[n][i]) for n,f in fmt]))


