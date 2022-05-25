import time
import utime
import ubinascii
from machine import I2C, Pin, Timer , ADC
import machine
import trainData
from umqtt.simple import MQTTClient
import network


#WiFI and MQTT
SSID = 'SMHOTSPOT'
KEY =  'cee0prek!'
 
SERVER = "10.3.141.1" #"your mqtt server IP address"
CLIENT_ID = ubinascii.hexlify(machine.unique_id())
TOPIC_id = b"motor/id"
TOPIC_pos = CLIENT_ID+ b'/pos'
TOPIC_sens = CLIENT_ID+b'/sens'
TOPIC_trainData = CLIENT_ID+b'/trainData'

topic_posp = CLIENT_ID+b'/pos_p'
topic_record = CLIENT_ID+b'/record'
topic_state = CLIENT_ID+b'/state'

remoteMode = False
stateFlag = "run"

#SmartMotor variables
training=trainData.data
print(training)
TMConnect=Pin(5,Pin.OUT)
TMConnect.off()
Sensor = ADC(0)
servoPin= machine.Pin(0)
import name
servo = machine.PWM(servoPin)
servo.freq(50)
servo.duty(75)

#MQTT variables
x = 0
c = None

state='run'

#defs
#connect WiFi
def con_WIFI():
 global wlan
 wlan = network.WLAN(network.STA_IF)
 wlan.active(True)
 wlan.connect(SSID,KEY)
 time.sleep(3)

#connect and subscribe to MQTT
def connect_sub():
 c= MQTTClient(CLIENT_ID,SERVER)
 c.set_callback(sub_cb)
 c.connect()
 c.subscribe(topic_posp)
 c.subscribe(topic_record)
 c.subscribe(topic_state)
 return c

def mqConnected():
 try:
  c.ping()
  c.ping()
 except:
  print("lost connection to mqtt broker...")
  return False
 else:
  return True 

#check MQTT and WiFi connect status
def stat():
 if mqConnected() == False:
  c.disconnect()
 if wlan.isconnected()==False:
  wlan.disconnect()
  time_sleep(5)
  con_WIFI()
  time_sleep(1)
  c= connect_sub()

def sub_cb(topic, msg):
 print(topic,msg)

 if topic == topic_state:
  print((topic, msg))
  msg = msg.decode('utf-8')
  msg =msg.lstrip()
  msg = msg.rstrip()
  if msg=="train":
   stateFlag= "train"
   training=[]
  elif(msg=="cancel"):
   stateFlag = "run"
  elif(msg == "save"):
   stateFlag="run"
   updateTrainData()
  else:
   pass

 if topic == topic_record:
  print((topic, msg))
  msg = msg.decode('utf-8')
  msg = msg.lstrip()
  msg = msg.rstrip()
  if msg=="yes": 
   if stateFlag=="train":
    training.append((d,sens))
   else:
    pass
  elif msg=="no":
   pass


 if topic == topic_posp:
  print((topic, msg))
  msg = msg.decode('utf-8')
  msg = msg.lstrip()
  msg = msg.rstrip()
  motor_pos=convert(msg)
  servo.duty(convert(motor_pos))

def updateTrainData():
 f=open("trainData.py",w)
 f.write("data=training=",training)
 f.close()
 machine.soft_reset()


def convert(x): #mapping 0 to 1024 to 21 to around 180
 return int((x * 119 / 1023) + 21)


def find_d(sens):
 min = 1000
 pos = 75
 for (a,l) in training:
  dist = abs(sens - l)
  if dist < min:
   min = dist
   pos = a
 return pos

def loop(): 
 while True:
  sens = Sensor.read()
  d=find_d(sens)
  motor_pos=convert(d)
  servo.duty(convert(motor_pos))   
  print("normal $",name.Name,name.type,sens,d,"*")
  time.sleep(.1)


#connect to WIFI
con_WIFI()

if(wlan.isconnected()==True):
 remoteMode = True 
else :
 remoteMode = False


if remoteMode:
 #connect to MQTT
 c = connect_sub()
 c.publish(TOPIC_id,CLIENT_ID)
 c.publish(TOPIC_trainData,str(training))
 s=wlan.isconnected()
 while(s):
  c.check_msg()
  sens = Sensor.read()
  c.publish(TOPIC_sens,str(sens))
  if(state=='run'):
   d=find_d(sens)
   motor_pos=convert(d)
   servo.duty(motor_pos)
   c.publish(TOPIC_pos,str(motor_pos))
   print("connected$",name.Name,name.type,sens,d,"*")
   time.sleep(.1)
  else:
   pass
  s=wlan.isconnected()
else:
 loop()
