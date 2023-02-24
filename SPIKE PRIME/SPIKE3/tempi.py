
import hub
import motor
import port
import time
import rgb
import button
import display
from machine import Timer
import sound


deviceTypeLookup={48:'M',65:'M',61:'S',62:'S',63:'S',255:'N'}
state="BLINK"

#light sensor - (-1, 4, 19, 21, 27, 50, 0, 0)
#force sensor - (0, 0, 0, 0, 382, 387, 0, 0) first one is force, second one is on/off
#distance sensor - (88, 0, 0, 0, 0, 0, 0, 0) first one is distance
SMModes="" # SS, MS


#defining tones/tunes for different actions
saveTone=[[493,100],[220,100]]
runTone=[[293,100],[440,100],[587,100],[740,100]]
trainTone=[[740,100],[587,100],[440,100],[293,100]]
shutDown=[[740,100],[587,100],[440,100],[293,100],[740,100],[587,100],[440,100],[293,100]]

#save button
bt=button.button_pressed
Next=button.BUTTON_RIGHT
Play=button.BUTTON_LEFT
Select=button.BUTTON_ON_OFF
deleteData=button.BUTTON_CONNECT


#Timer(mode=Timer.PERIODIC, period=2000, callback=lambda t:animateState())
#Timer(mode=Timer.PERIODIC, period=5000, callback=lambda t:getLine())
Timer(mode=Timer.PERIODIC, period=2000, callback=lambda t:blink())

def blink():
  if(state=="BLINK"):
    for i in range(4,-1,-1):
      ypos=[]
      for j in range(4,-1,-1):
        if(display.display_get_pixel(i,j)==100):
          ypos.append(j)
      for y in ypos:
          display.display_set_pixel(i,y,0)
      time.sleep(0.1)
      for y in ypos:
          display.display_set_pixel(i,y,100)
  else:
    pass



class SM:

  def __init__(self):
    deviceTypeLookup={48:'M',65:'M',61:'S',62:'S',63:'S',255:'N'}
    self.deviceType=[0]*6
    self.temp = [0] * 25
    for i in range(6):
      self.deviceType[i]= deviceTypeLookup[port.port_getDevice(i)]
    self.SMModes="SS" #by default it is a single sensor system 
    self.numberOfMotors=self.deviceType.count('M')
    self.numberOfSensors=self.deviceType.count('S')
    self.locationOfSensors=[i-1 for i in range(len(self.deviceType)) if self.deviceType[i] == 'S']
    self.locationOfMotors = [i for i in range(len(self.deviceType)) if self.deviceType[i] == 'M']
    self.sensors=[]
    self.motors=[]

  def reReadPorts(self):
    for i in range(6):
      self.deviceType[i]= deviceTypeLookup[port.port_getDevice(i)]

    self.numberOfMotors=self.deviceType.count('M')
    self.numberOfSensors=self.deviceType.count('S')
    self.locationOfSensors=[i-1 for i in range(len(self.deviceType)) if self.deviceType[i] == 'S']
    self.locationOfMotors = [i for i in range(len(self.deviceType)) if self.deviceType[i] == 'M']

  def ShowAllConnections(self):
    self.SMpairs=[]
    self.temp = [0] * 25
    self.reReadPorts()
    
    if(self.numberOfSensors==1):
      self.SMModes="SS"  # This is single sensor
      self.drawLines()

    elif(self.numberOfSensors>1):
      self.SMModes="MS"  # This is multi sensor
      for ls in self.locationOfSensors:
        try:
          if(self.locationOfMotors.count(ls)):
            self.SMpairs.append([ls,ls])
            self.drawIndLines(ls,ls)
        except:
          print("something weird")
          pass
    Image=bytes(self.temp)
    display.display_show_image(Image) 


  def drawImage(self,s=0,m=0):
    self.temp = [0] * 25
    if(SMModes=="SS"):
      self.drawLines()
    elif(SMModes=="MS"):
      self.drawIndLines(s,m)
    
    Image=bytes(self.temp)
    display.display_show_image(Image)

    

  def drawLines(self): #draws multi lines for singgle sensor condition
    self.temp[(self.locationOfSensors[0])*5+3]=100
    self.temp[(self.locationOfSensors[0])*5+4]=100
    for motorlocation in self.locationOfMotors:
      self.SMpairs.append([self.locationOfSensors[0],motorlocation])
      self.temp[motorlocation*5+0]=100
      self.temp[motorlocation*5+1]=100

    minval=min(self.locationOfMotors+self.locationOfSensors)
    maxval=max(self.locationOfMotors+self.locationOfSensors)
    for i in range(minval,maxval+1): #drawing the vertical lines
      self.temp[i*5+2]=100 

  def drawIndLines(self,sensor,motor): # draws individual lines for multi sensor condition 
    self.temp[motor*5+0]=100
    self.temp[motor*5+1]=100
    self.temp[sensor*5+3]=100
    self.temp[sensor*5+4]=100
    for i in range(sensor,motor+1): #drawing the vertical lines
      self.temp[i*5+2]=100

  def data(self):
    if (self.SMModes=="SS"):
      self.drawImage()
      while(not bt(Run)):
        if(bt(Next)):
          print(SMpairs)
          sensorValue=getsensorValue(SMpairs[0][0])
          for sensorPort,motorPort in SMPairs:
            motorValue.append(port.getsensorValue(motorPort)[1])

    elif(self.SMModes=="MS"):
      for sensorPort,motorPort in SMPairs: 
        self.drawImage(sensorPort,motorPort)
        while(not bt(Run)):
          if(bt(Next)):
            print(SMpairs)
            sensorValue=getsensorValue(SMpairs[0][0])
            for sensorPort,motorPort in SMPairs:
              motorValue.append(port.getsensorValue(motorPort)[1])

    else:
      pass


s=SM()
while(not bt(Select)):
  try:
    SMpairs=s.ShowAllConnections()
    time.sleep(1)
  except Exception as e:
    print(e)
    time.sleep(2)
    display.display_text_for_time("err",2000,2000)

#state="TRAINING"
s.data() #taking data
s.run()  #running SM code

for ls,lm in SMpairs:
  s.drawIndLines(ls,lm)
  while(not bt(Next)):
    s.data()
    time.sleep(1)


while(state=="TRAINING"):


def getsensorValue(portNumber):
 sensorType=port.port_getDevice(portNumber)
 value=port.port_getSensor(portNumber)
 return value[sensorLookup[sensorType]]




  elif(numberOfSensors>1):
    for motors in locationOfMotors:
      if()





  if(numberOfSensors ==1 and numberOfMotors ==1 ):
    SMModes="SSSM"
  elif(numberOfSensors ==1 and numberOfMotors > 1 ):
    SMModes="SSMM"
  elif(numberOfSensors >1 and numberOfMotors > 1 ):
    SMModes="MSMM"






  locationOfSensors=[i for i in range(len(deviceType)) if deviceType[i] == 'S']
  locationOfMotors = [i for i in range(len(deviceType)) if deviceType[i] == 'M']
  print(deviceType)
  if (numberOfSensors==1): #there is one sensor and it is on the right
    print(numberOfSensors)
    print(locationOfSensors)
    if(locationOfSensors[0]%2==1):
      for l in locationOfMotors:
        temp[l*5+0]=100
        temp[l*5+1]=100
        temp[l*5+2]=100
      for l in locationOfSensors:
        temp[(l-1)*5+2]=100
        temp[(l-1)*5+3]=100
        temp[(l-1)*5+4]=100
      minval=min(locationOfMotors+locationOfSensors)
      maxval=max(locationOfMotors+locationOfSensors)
      for i in range(minval,maxval): #drawing the vertical lines
        print("should be a line",i)
        temp[i*5+2]=100

  else: #there are multiple sensors and multiple 
    for i in range(3):  
      if((deviceType[i*2]=='M') and deviceType[i*2+1]=='S'):
        line.append(i*2) #saving which lines are ON
    for l in line:
      for j in range(5):
        temp[l*5+j]=100

  Image=bytes(temp)
  display.display_show_image(Image) 
  return line



def IndividualConnections(connectionNumber):
  global line
  line=[]
  deviceType=[0]*6
  temp = [0] * 25
  for i in range(6):
    deviceType[i]= deviceTypeLookup[port.port_getDevice(i)]

  numberOfMotors=deviceType.count('M')
  numberOfSensors=deviceType.count('S')
  locationOfSensors=[i for i in range(len(deviceType)) if deviceType[i] == 'S']
  locationOfMotors = [i for i in range(len(deviceType)) if deviceType[i] == 'M']






def drawLines():
  getLineList() 




def animateLines(l):
  for i in range(4,-1,-1):
    for li in l:
      display.display_set_pixel(i,li,100)
    time.sleep(0.1)
    for li in l:
      display.display_set_pixel(i,li,0)


getLineList()




def __main__():
  while True:
    getLineList()
    for l in line:
      animateLines([l])
    time.sleep(0.5)


def getLineList()

def animateLine():
  line=drawLines()
  for i in range(5):
    for l in line:
      display.display_set_pixel(i+1,l,0)
      display.display_set_pixel(i,l,100)
    time.sleep(0.1)

    print(i,pixelLookup[i][0],pixelLookup[i][1])
    if (not deviceType[i] ==255):
       display.display_set_pixel(pixelLookup[i][0],pixelLookup[i][1],100) 
    time.sleep(1)