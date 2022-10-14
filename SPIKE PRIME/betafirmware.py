import hub
import motor
import port
import time
import rgb
import button
import display
from machine import Timer
import sound
sensorPort=port.PORTB
motorPort=port.PORTA
data=[]
blinkx=-1
blinky=-1
temp = [0] * 25  
Image=bytes(temp)
Training=True
sensorLookup={61:1,62:0,63:0,48:2,65:2,255:0} #lookup table for indexing getSensor return array
Timer(mode=Timer.PERIODIC, period=1000, callback=lambda t:interrupt())

def getsensorValue(portNumber):
 sensorType=port.port_getDevice(portNumber)
 value=port.port_getSensor(portNumber)
 return value[sensorLookup[sensorType]]

def interrupt():
 displayScreen(blinkx,blinky,0)
 pass

inputbutton=button.button_isPressed

saveData=button.BUTTON_RIGHT
runMode=button.BUTTON_LEFT
quitProgram=button.BUTTON_ON_OFF
deleteData=button.BUTTON_CONNECT

def updateDataImage():
 global temp
 global Image
 temp = [0] * 25    
 barPix=70
 dataPix=100
 #setting bar
 for x,y in data:
  x=x//20
  y=y//72
  for i in range(5):
   temp[5*i+x]=barPix  #finding the index of array from width=5, column and row value
   temp[5*y+i]=barPix
 for x,y in data:
  x=x//20
  y=y//72
  temp[5*y+x]=dataPix
 Image=bytes(temp)

def displayScreen(sensorValue, motorValue,brightness=100):
 display.display_show_image(Image)    
 y=motorValue//72
 x=sensorValue//20
 display.display_set_pixel(x,y,brightness) # perhaps blink here

 
saveTone=[[493,100],[220,100]]
runTone=[[293,100],[440,100],[587,100],[740,100]]
trainTone=[[740,100],[587,100],[440,100],[293,100]]
def playmusic(music):
 for note,dur in music:
  sound.beepPlay(note)
  time.sleep_ms(dur)
  sound.soundStop()

playmusic(trainTone) 

while not inputbutton(quitProgram)[1]>1000: # it will quit if the centre button is pressed for more than a sec
 sensorValue=getsensorValue(sensorPort)
 motorValue=getsensorValue(motorPort)+180 # to make the angle positive
 blinkx=sensorValue
 blinky=motorValue
 displayScreen(sensorValue,motorValue)
 rgb.rgb_setColor(0,3) #BLUE color for Training


 if(inputbutton(saveData)[0]):
  while(inputbutton(saveData)[0]):
   pass #wait till the right button is released
  data.append([sensorValue,motorValue])
  updateDataImage()
  playmusic(saveTone)
  print('pressed')
  print(data)
 elif(inputbutton(deleteData)[0]):
  while(inputbutton(deleteData)[0]):
   if(inputbutton(deleteData)[1]>2000):
    data=[[0,0]]
   time.sleep(0.1)
  if data: 
    data.pop()
  updateDataImage()


 elif(inputbutton(runMode)[0]):
  rgb.rgb_setColor(0,6) # GREEN color for running
  #playmusic(runTone) 
  print('running')
  print(data)
  while(inputbutton(runMode)[0]):
   pass #wait till the left button is released
  # now we go into the run mode

  while  (inputbutton(quitProgram)[1]<1000 and (inputbutton(saveData)[1]<1000)):
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
  playmusic(trainTone) 