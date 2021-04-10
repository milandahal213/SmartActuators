from flask import Flask, redirect, url_for, request, jsonify
import json
import requests
# Thingworx

Key = "pp92f871d8-c3bd-4fd7-8fab-ee50e1fbd0e5"
urlBase = "https://-2008281301ci.portal.ptc.io:8443/Thingworx/"
headers = {"Accept":"application/json","Content-Type":"application/json","AppKey":Key}

def Put(thing, property, type, value):
     urlValue = urlBase + 'Things/' + thing + '/Properties/*'

     propValue = {property:value}
    
     try:
          response = requests.put(urlValue,headers=headers,json=propValue)
          print(response.text)
          response.close()
     except:
          print('error with Put')

app = Flask(__name__)

old_speed=0
delayer=0

@app.route('/thing',methods = ['POST', 'GET'])

def thing():
   global old_speed
   global delayer
   if request.method == 'POST':
      user = request.data
      d=json.loads(user)
      if(abs((float(d['speed'])-old_speed))>0.08):		
            print(d['speed'])
            print(d['bright'])
            print(old_speed)
            Put('Test1','speed','STRING',d['speed'])
            Put('Test1','bright','STRING',d['bright'])
            delayer=0 
      if(delayer>10):
            Put('Test1','bright','STRING',d['bright'])
            delayer=0      
      old_speed=float(d['speed'])
      delayer+=1
      return jsonify(user)
   else:
      user = request.args.get('nm')
      return user

if __name__ == '__main__':
   app.run(debug = True, host='0.0.0.0')
