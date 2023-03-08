
#SmartMotors code 
#Last Edited : March 7, 2023 
#Code by Milan Dahal
#Sensors to the left of the hub
#Motors to the right of the hub
# you can choose your SM sensor motor pair - supports multiple SM pairs in one hub
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
startTone=[[740,100],[587,100],[440,100],[293,100],[740,100],[587,100],[440,100],[293,100]]
saveTone=[[493,100],[220,100]]
runTone=[[293,100],[440,100],[587,100],[740,100]]
trainTone=[[740,100],[587,100],[440,100],[293,100]]
shutDown=[[740,100],[587,100],[440,100],[293,100],[740,100],[587,100],[440,100],[293,100]]

#save button
buttonPressed=button.button_pressed

Run=button.BUTTON_RIGHT
Data=button.BUTTON_LEFT
#Select=button.BUTTON_ON_OFF #to select mode/ Next pair


deleteData=button.BUTTON_CONNECT

deviceTypeLookup={48:'M',65:'M',61:'S',62:'S',63:'S',255:'N'}
#Timer(mode=Timer.PERIODIC, period=2000, callback=lambda t:animateState())
#Timer(mode=Timer.PERIODIC, period=5000, callback=lambda t:getLine())
Timer(mode=Timer.PERIODIC, period=3000, callback=lambda t:blink())

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


  def drawImage(self):
    self.temp = [0] * 25

    Image=bytes(self.temp)
    display.display_show_image(Image)


  def train(self):

    while(not buttonPressed(Run)):
      sensorValue=port.port_getSensor(self.sensors[0])[0]
      motorValue=[]
      for motorPort in self.motors: 
        motorValue.append(motor.motor_get_position(motorPort))
        if(buttonPressed(Data)):
          playmusic(saveTone)
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


def main():
  rgb.rgb_setColor(0,3) #set the blue to indicate stat
  playmusic(startTone) #change this to mario
  s=Connections()
  while(not buttonPressed(Select)):
    s.ShowAllConnections()
    time.sleep(1)
   
  
  playmusic(trainTone)
  state="TRAIN"
  pairs= s.makepairs()

  rgb.rgb_setColor(0,6) # GREEN color for trining is done

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

  while(not buttonPressed(Data)):
    for smartmotor in SM:
      smartmotor.play()
      time.sleep(0.1)
 

