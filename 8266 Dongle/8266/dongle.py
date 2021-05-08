# Write your code here :-)
import ujson
import network
import requests
from machine import UART
import machine
import time
import ubinascii

#import uos

#uos.dupterm(None, 1)


ssid = ""
pwd = ""
api = ""
URL = ""
headers=""
led = machine.Pin(0, machine.Pin.OUT)
#uart = UART(0, 115200)
#uart.init(115200, bits=8, parity=None, stop=1)
#time.sleep(1)


def storeAPISecret(api, URL):
    try:
        f = open("api.txt", "w")
        f.write('{"api":"' + api + '","url":"' + URL + '"}')
        f.close()
        send("Done")
    except:
        send("Error")


def storeWIFISecret(ssid, pwd):
    try:
        f = open("wifi.txt", "w")
        f.write('{"ssid":"' + ssid + '","password":"' + pwd + '"}')
        f.close()
        send("Done")
    except:
        send("Error")


def readAPISecret():
    try:
        f = open("api.txt")
        raw = f.read()
        jsonString = ujson.loads(raw)
        f.close()
        return (jsonString["api"], jsonString["url"])
    except:
        return ("0","0")


def readWIFISecret():
    try:
        f = open("wifi.txt", "r")
        raw = f.read()
        jsonString = ujson.loads(raw)
        f.close()
        return (jsonString["ssid"], jsonString["password"])
    except:

        return ("0","0")


def setWiFi():
    try:
        wifi = network.WLAN(network.STA_IF)
        wifi.active(True)
        ssid, password = readWIFISecret()
        if ssid:
            wifi.connect(ssid, password)
            time.sleep(1)
            send(wifi.isconnected())
        else:
            send(False)

    except:
        return 0


def getMAC():
    try:
        send(ubinascii.hexlify(network.WLAN().config("mac"), ":").decode())
    except:
        send("0")


def setTW():
    global api
    global URL
    global headers
    try:
        if api:
            headers = {"Accept": "application/json", "Content-Type": "application/json", "AppKey": api}
        else:
            api, URL = readAPISecret()
            headers = {"Accept": "application/json","Content-Type": "application/json","AppKey": api}
        send("Done")
    except:
        send("Error")


def PutTW(thing, prop, value):
    urlValue = URL + "Things/" + thing + "/Properties/*"
    propValue = {prop: value}
    try:
        response = requests.put(urlValue, headers=headers, json=propValue)
        send(response.text)
        response.close()
    except:
        send(False)

def CallTW(thing, service):
    urlValue = URL + "Things/" + thing + "/Services/" + service
    try:
        response = requests.post(urlValue, headers=headers)
        send(response.text)
        response.close()
    except:
        send(False)
        
def getTW():
    r = 5
    return r

def send(body):

    print("@@")
    print(body)
    print("!!")
