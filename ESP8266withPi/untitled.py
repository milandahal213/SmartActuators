import machine
import time
import usocket
import network, ubinascii, ujson
import machine
import time
from machine import Pin, ADC

p2 = machine.Pin(0)
pwm2 = machine.PWM(p2)
pwm2.freq(50)

pwm2.duty(0)

#Initialize pins
LED = Pin(5, Pin.OUT)
BuiltInLED = Pin(2, Pin.OUT)
LightSensor = ADC(0)
Left = Pin(13, Pin.IN) 
Button = Pin(12, Pin.IN)
Right = Pin(14, Pin.IN)
print("about to enter")


wlan=network.WLAN()
wlan.active(True)
wlan.connect("SmartMotorHub1","robots1234")
time.sleep(1)



PORT = 5000
base='192.168.50.10' 



WiFi = network.WLAN()
sn = ubinascii.hexlify(network.WLAN().config("mac")).decode()[-6:]

def callPi(method,light):
    addr = usocket.getaddrinfo(base, PORT)[0][4]
    client = usocket.socket(usocket.AF_INET, usocket.SOCK_STREAM)
    client.connect(addr)
    client.settimeout(5)

    payload = {'light': light}
    payload = ujson.dumps(payload)
    request=method + ' /ESP/%s HTTP/1.1\r\n'  % sn
    request += 'Host: %s\r\n' % base
    request += 'Content-Type: application/json\r\n'
    request += 'Accept: application/json\r\n'
    request += 'Content-Length: %d\r\n\r\n' % len(payload)
    request += '%s\r\n\r\n' % payload
    print(request)
    client.write(request)
    response = client.read(1024)
    client.close()
    print(response)
    res=response.decode("utf-8")
    res=eval(res.split('\r\n')[-1])

    return res["sn"],res["GroupYN"],res["speed"],res["state"]

def send(path):
    addr = usocket.getaddrinfo(base, PORT)[0][4]
    client = usocket.socket(usocket.AF_INET, usocket.SOCK_STREAM)
    client.connect(addr)
    client.settimeout(5)

    request='GET /ESP/'+path+'/%s HTTP/1.1\r\n' % sn
    request += 'Host: %s\r\n' % base
    request += 'Content-Type: application/json\r\n'
    request += 'Accept: application/json\r\n\r\n'

    client.write(request)
    response = client.read(1024)
    client.close()

    res=response.decode("utf-8")
    res=eval(res.split('\r\n')[-1])

    return res[path]

#if connected to RPi
training = [] #array to hold training valuesa
GroupYN=0

t=send("setSN")  # begin the program by setting groupID = sn and group to NO
print(t)

while True:
    print("in loop")
    
    BuiltInLED.on() #turns built-in LED off
    bright = LightSensor.read()
    #check if the ESP is part of Group 
    #0= no group
    #1=group/ follower
    #2=group/ leader
    if GroupYN==0 or GroupYN==2: 
        sn, GroupYN, potValue, state = callPi("POST",bright)
    elif GroupYN==1:
        sn, GroupYN, potValue, state = callPi("GET",bright)



    print(potValue,state, sn, GroupYN)
    #if state==0 and button ==1: #train mode
        #post the training data to db
        #handle this case in server
       # training.append((potValue,bright))
    if state==0: #training mode
        pwm2.duty(potValue)

    elif state==1: #run mode
        print("In running")
        print(training)
        t=send("training")
        training=eval(t)
        print("training")
        print(training)
        for (a,l) in training:
            print(a,l)
        #get the model from db
        #train= training(sn)
        while True:
            bright = LightSensor.read()
            print(bright)
            min = 1000
            pos = 0
            for (a,l) in training:
                dist = abs(bright - l)
                if dist < min:
                    min = dist
                    pos = a
            pwm2.duty(pos)
            time.sleep(.01)

    else:
        pass

