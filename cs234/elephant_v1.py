
#!/usr/bin/env python
import requests
import json

rt = 'http://127.0.0.1:8008'

flow = {'keys':'link:inputifindex,ipsource,tcpsourceport,ipdestination,tcpdestinationport','value':'bytes'}


requests.put(rt+'/flow/pair/json',data=json.dumps(flow))

threshold = {'metric':'pair','value':1000000/8,'byFlow':True,'timeout':1}
requests.put(rt+'/threshold/elephant/json',data=json.dumps(threshold))

eventurl = rt+'/events/json?thresholdID=elephant&maxEvents=10&timeout=60'
eventID = -1
while 1 == 1:
  r = requests.get(eventurl + "&eventID=" + str(eventID))
  if r.status_code != 200: break

  print("r - {}".format(r.content))
  events = r.json()

  print("events - {}".format(events))
  print("type events - {}".format(type(events)))



  if len(events) == 0: continue

  eventID = events[0]["eventID"]
  events.reverse()
  for e in events:
    print(e['flowKey'])
