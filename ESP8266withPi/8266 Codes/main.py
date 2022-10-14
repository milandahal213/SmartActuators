import uos, machine,neopixel
from math import log
import time
import trainData
import math
training=trainData.data
from machine import Pin, ADC
TMConnect=Pin(5,Pin.OUT)
TMConnect.off()
Sensor = ADC(0)
p2 = machine.Pin(0)
import name
if name.type=="Motor":
 pwm2 = machine.PWM(p2)
 pwm2.freq(50)
 pwm2.duty(75)
elif name.type=="Speaker":
 pwm2 = machine.PWM(p2)
 pwm2.freq(50)
 pwm2.duty(75)
elif name.type=="Light":
 np = neopixel.NeoPixel(p2, 8)

def convert(x): #mapping 0 to 1024 to 21 to around 180
 return int((x * 119 / 1023) + 21)

def getfreq(x):
 minFreq = 130.8
 maxFreq = 523.3
 pInc = 1.059463
 numNotes = int(log(maxFreq / minFreq) / log(pInc)) + 1
 TRange = 1023 / numNotes
 for i in range(numNotes):
  if (x > 0 and x < TRange): 
   return (0)
  elif (x > TRange * (i) and x < TRange * (i + 1)): 
   return int(minFreq * pow(pInc, (i - 1)))
 return -1
def doSomething(d):
 if name.type=="Motor":
  pwm2.duty(convert(d))
 elif name.type=="Speaker":
  ff=getfreq(d)
  print(ff)
  pwm2.freq(ff)
 elif name.type=="Light":
  l=0
  r=0
  g=0
  b=0
  allFlag=1
  if d<23:
   allFlag=0
   pass
  elif d>23 and d<123:
   allFlag=0
   l= math.floor(((d-23)*0.08))
   r=255
  elif d>123 and d<223:
   allFlag=0
   l= math.floor(((d-123)*0.08))
   g=255
  elif d>223 and d<323:
   allFlag=0
   l= math.floor(((d-223)*0.08))
   b=225
  elif d>323 and d<423:
   r=math.floor(((d-323)*2.55))
  elif d>423 and d<523:
   r=255
   g=math.floor(((d-423)*2.55))
  elif d>523 and d<623:
   r=255-math.floor(((d-523)*2.55))
   g=255
  elif d>623 and d<723:
   g=255
   b=math.floor(((d-623)*2.55))
  elif d>723 and d<823:
   g=255-math.floor(((d-723)*2.55))
   b=255
  elif d>823 and d<923:
   r=math.floor(((d-823)*2.55))
   b=255
  elif d>923 and d<1024:
   r=255
   g=math.floor(((d-923)*2.55))
   b=255
  if allFlag:
   for i in range(8):
    np[i] = (r, g, b)
  else:
   for i in range(8):
    np[i] = (0, 0, 0)
   np[l] = (r, g, b)
  np.write()
def loop():
 while True:
  sens = Sensor.read()
  min = 1000
  pos = 75
  for (a,l) in training:
   dist = abs(sens - l)
   if dist < min:
    min = dist
    pos = a
  doSomething(pos)   
  print("$",name.Name,name.type,sens,pos, "*")
  time.sleep(.1)



loop()
