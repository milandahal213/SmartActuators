
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
buttonPressed=button.button_pressed


Run=button.BUTTON_RIGHT
Data=button.BUTTON_LEFT
Select=button.BUTTON_ON_OFF #to select mode/ Next pair


deleteData=button.BUTTON_CONNECT

deviceTypeLookup={48:'M',65:'M',61:'S',62:'S',63:'S',255:'N'}
#Timer(mode=Timer.PERIODIC, period=2000, callback=lambda t:animateState())
#Timer(mode=Timer.PERIODIC, period=5000, callback=lambda t:getLine())
Timer(mode=Timer.PERIODIC, period=1000, callback=lambda t:blink())

def blink():
  if(state=="BLINK"):
    for i in range(5):
      ypos=[]
      for j in range(5):
        if(display.display_get_pixel(i,j)==100):
          ypos.append(j)
      for y in ypos:
          display.display_set_pixel(i,y,0)
      time.sleep(0.05)
      for y in ypos:
          display.display_set_pixel(i,y,100)
  else:
    pass

def playmusic(music):
 for note,dur in music:
  sound.beepPlay(note)
  time.sleep_ms(dur)
  sound.soundStop()

playmusic(trainTone) #change this to mario

class Connections:

  def __init__(self):
    
    self.deviceType=[0]*6
    self.temp = [0] * 25
    self.pairs=[]
    self.ReadPorts()

  def ReadPorts(self):
    for i in range(6):
      self.deviceType[i] = deviceTypeLookup[port.port_getDevice(i)]

    self.numberOfMotors = self.deviceType.count('M')
    self.numberOfSensors = self.deviceType.count('S')

    self.locationOfSensors = [i for i in range(len(self.deviceType)) if self.deviceType[i] == 'S']
    self.locationOfMotors =  [i for i in range(len(self.deviceType)) if self.deviceType[i] == 'M']

  def ShowAllConnections(self):
    self.temp = [0] * 25
    self.pairs=[]

    #find what is connected to the ports
    self.ReadPorts()
    
    if(self.numberOfSensors==1 and self.locationOfSensors[0]%2==0):
      self.drawLines()

    elif(self.numberOfSensors>1):
      for ls in self.locationOfSensors:
        try:
          if(self.locationOfMotors.count(ls+1)):
            self.pairs.append(([ls],[ls+1]))
            self.drawIndLines(ls,ls)  #only draw straight lines if there are more than one sensors

        except:
          print("something weird")
          pass

    Image=bytes(self.temp)
    display.display_show_image(Image)

  def drawLines(self): #draws multi lines for singgle sensor condition
    self.temp[(self.locationOfSensors[0])*5+0]=100
    self.temp[(self.locationOfSensors[0])*5+1]=100
    self.temp[(self.locationOfSensors[0])*5+2]=100
    self.pairs.append((self.locationOfSensors, self.locationOfMotors))
    for motorlocation in self.locationOfMotors:
      self.temp[(motorlocation-1)*5+2]=100
      self.temp[(motorlocation-1)*5+3]=100
      self.temp[(motorlocation-1)*5+4]=100

    #drawing the vertical lines
    minval=min(self.locationOfMotors+self.locationOfSensors)
    maxval=max(self.locationOfMotors+self.locationOfSensors)

    for i in range(minval,maxval): 
      self.temp[i*5+2]=100 

  def drawIndLines(self,sensor,motor): # draws individual lines for multi sensor condition 
    self.temp[sensor*5+0]=100
    self.temp[sensor*5+1]=100
    self.temp[sensor*5+2]=100
    self.temp[motor*5+2]=100
    self.temp[motor*5+3]=100
    self.temp[motor*5+4]=100

    for i in range(sensor,motor): #drawing the vertical lines
      self.temp[i*5+2]=100
    print("drew individual lines")

  def makepairs(self):
    return(self.pairs)



class SmartMotors:
  def __init__(self, sensors, motors):
    self.sensors=sensors
    self.motors=motors
    self.data=[]


  def train(self):
      while(not buttonPressed(Run)):
        sensorValue=port.port_getSensor(self.sensors[0])[0]
        motorValue=[]
        for motorPort in self.motors: 
          motorValue.append(motor.motor_get_position(motorPort))
          if(buttonPressed(Data)):
            while(buttonPressed(Data)):
              pass #wait till the right button is released
            self.data.append([sensorValue,motorValue])
            time.sleep(0.1)



  def play(self):
    sensorValue=port.port_getSensor(self.sensors[0])[0]
    min = 1000000
    pos = 0
    for (l, a) in self.data:
      dist = abs(sensorValue - l)
      if dist < min:
        min = dist
        pos = a

    for index, motorPort in enumerate(self.motors):   
      motor.motor_move_to_position(motorPort,pos[index],9000)


  def showData(self):
    return(self.data)


s=Connections()


while(not buttonPressed(Select)):
  s.ShowAllConnections()
  time.sleep(1)
 
state="TRAIN"
playmusic(trainTone)
pairs= s.makepairs()


#change the color of LED 
#scroll start training on the screen
SM=[] # that is max number of SmartMotors pairs that can be made
for index,pair in enumerate(pairs):
  try:
    sensorPort,motorPort=pair
    SM.append(SmartMotors(sensorPort,motorPort))
  except:
    pass
    #display error


for smartmotor in SM:
  try:
    smartmotor.train()
    playmusic(trainTone)
  except:
    pass
    #dislay error


state="RUN"
playmusic(runTone)

while(not buttonPressed(Select)):
  for smartmotor in SM:
    smartmotor.play()
    time.sleep(0.1)
 




    for i in range (2):


while not inputbutton(quitProgram)>1000: # it will quit if the centre button is pressed for more than a sec
 sensorValue=getsensorValue(sensorPort)
 motorValue=getsensorValue(motorPort)+180 # to make the angle positive
 blinkx=sensorValue
 blinky=motorValue
 displayScreen(sensorValue,motorValue)
 rgb.rgb_setColor(0,3) #BLUE color for Training


 if(inputbutton(saveData)):
  while(inputbutton(saveData)):
   pass #wait till the right button is released
  data.append([sensorValue,motorValue])
  updateDataImage()
  playmusic(saveTone)
  print('pressed')
  print(data)
 elif(inputbutton(deleteData)):
  while(inputbutton(deleteData)):
   if(inputbutton(deleteData)>2000):
    data=[[0,0]]
   time.sleep(0.1)
  if data: 
    data.pop()
  updateDataImage()


 elif(inputbutton(runMode)):
  rgb.rgb_setColor(0,6) # GREEN color for running
  playmusic(runTone) 
  print('running')
  print(data)
  while(inputbutton(runMode)):
   pass #wait till the left button is released
  # now we go into the run mode

  while  (inputbutton(quitProgram)<1000 and (inputbutton(saveData)<1000)):
   sensorValue=getsensorValue(sensorPort)
   min = 1000000
   pos = 0
   for (l, a) in data:
    dist = abs(sensorValue - l)
    if dist < min:
     min = dist
     pos = a
   print(pos)
   motor.motor_move_to_position(motorPort,pos,9000)
   displayScreen(sensorValue,pos)
   blinkx=sensorValue
   blinky=pos
   time.sleep(.1)



class SmartMotors:
  def __init__(self, sensors, motors):
    self.sensors=sensors
    self.motors=motors


  def train(self):
    for sensor in self.sensors:
      while(not buttonPressed(Run)):
        if(buttonPressed(Next)):
          sensorValue=port.port_getSensor(SMpairs[0][0]+1)[0]
          for sensorPort,motorPort in SMPairs:
            motorValue.append(port.port_getSensor(motorPort)[1])      


    for i in range (2):

#pairs=(sensors[],motors)
s.SmartMotors(pairs)


ef data(self):
    if (self.SMModes=="SS"):
      print("entered the SS mode")
      self.drawImage()
      while(not buttonPressed(Run)):
        if(buttonPressed(Next)):
          print(SMpairs)
          sensorValue=port.port_getSensor(SMpairs[0][0]+1)[0]
          for sensorPort,motorPort in SMPairs:
            motorValue.append(port.port_getSensor(motorPort)[1])

    elif(self.SMModes=="MS"):
      print("entered MS mode")
      print(self.SMpairs)
      for sensorPort,motorPort in self.SMpairs: 
        self.drawImage(sensorPort,motorPort)
        while(not buttonPressed(Run)):
          if(buttonPressed(Next)):
            print(self.SMpairs)
            sensorValue=port.port_getSensor(self.SMpairs[0][0]+1)[0]
            for sensorPort,motorPort in self.SMpairs:
              motorValue.append(port.port_getSensor(motorPort)[1])

    else:
      pass

    def run(self):
      print(data)
s.data() #taking data
s.run()  #running SM code

for ls,lm in SMpairs:
  s.drawIndLines(ls,lm)
  while(not buttonPressed(Next)):
    s.data()
    time.sleep(0.1)


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




  def drawImage(self,s=0,m=0):
    self.temp = [0] * 25
    if(self.SMModes=="SS"):
      self.drawLines()
    elif(self.SMModes=="MS"):
      self.drawIndLines(s,m)
    
    Image=bytes(self.temp)
    display.display_show_image(Image)
    print("drew Image")



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