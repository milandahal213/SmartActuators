#LastEdit- Jan 26 2023
#SmartMotor Code to be used on HubOS SPIKE Prime 
#Motor Port A
#Sensor Port B
#comment line 142 to remove annoying music

import hub, time
from hub import port,button, sound, status, Image, display
from machine import Timer

#setting upImage object with format Image('00000:00000:00000:00000:')
temp=bytearray(30)
Img=Image(temp.decode())
blinkx=-1
blinky=-1
oldx=0
oldy=0

def init_display():
    global temp
    global Img
    global blinkx
    global blinky
    temp=bytearray(30)
    temp[5]=58 #adding : after 5,10,15,20 and 25th index
    temp[11]=58
    temp[17]=58
    temp[23]=58
    temp[29]=58
    Img=Image(temp.decode())
    blinkx=-1
    blinky=-1

#lookup table for indexing getSensor return array
sensorLookup={61:(0,100) ,62:(0,20), 63:(0,10)}
sensorToneLookup={0:[[493,10]],1:[[515,10]],2:[[550,10]],3:[[590,10]],4:[[612,10]]}

#setting up sensor and motor Ports
sensorPort    = 'B'
motorPort    = 'A'

#setting interrupt for current position
Timer(mode=Timer.PERIODIC, period=1000, callback=lambda t:interrupt())

def interrupt():
    #displayScreen(blinkx,blinky,0)
    pass

#defining buttons and setting up callbacks for times
#left -2 , right -1 , center -3, connect -4
hub.button.right.callback(lambda time_ms:setButton(1,time_ms))        #    Take data
hub.button.left.callback(lambda time_ms:setButton(2,time_ms))    #    Run
hub.button.center.callback(lambda time_ms:setButton(3,time_ms))    #    Quit
hub.button.connect.callback(lambda time_ms:setButton(4,time_ms))    #    Delete one data

buttonValue    =    0
buttonTime    =    0

def setButton(but,t):
    global buttonValue
    global buttonTime
    buttonValue=but
    buttonTime=t



#defining tones/tunes for different actions
saveTone=[[493,100],[220,100]]
runTone=[[293,100],[440,100],[587,100],[740,100]]
trainTone=[[740,100],[587,100],[440,100],[293,100]]
shutDown=[[740,100],[587,100],[440,100],[293,100],[740,100],[587,100],[440,100],[293,100]]

#defining default data
data=[]

#mapping function
def mapFromTo(x,b,d):
    y=(x)/(b)*(d)
    return int(y)

#using the lookup table get the relevant data from the sensor/motor array from getStat function
def getsensorValue():
    sensorType=port.B.info()["type"]
    sensvalue=hub.status()["port"]['B']
    motvalue=hub.status()["port"]['A']
    print("motvale",motvalue)
    try:
        if sensvalue[sensorLookup[sensorType][0]] is None :
            return (99,motvalue[1])
        else:
            val=sensvalue[sensorLookup[sensorType][0]] #actual value
            maxVal=sensorLookup[sensorType][1] #maximum value allowed for the sensor to normalize
            if val>maxVal: #always staying with 0-99
                return (99,motvalue[1])
            retValue = mapFromTo(val,maxVal,99) #return noramlized value
            return (retValue,motvalue[1])
    except:
        display.show("ERROR")
        return(0,0)


#function to play tunes
def playmusic(music):
    for note,dur in music:
        sound.beep(note,dur)
        time.sleep_ms(dur)

#function to update the Image
def updateDataImage():
    global Img
    global temp
    init_display()
    barPix=52 #brightness for bar
    dataPix=55 #brightness for data
    #setting bar

    for x,y in data:
        x=x//20 #dividing 100 into 5
        y=y-(y//360)*360 #getting within 0,360 always
        y=y//72 #dividing 360 into 5
        for i in range(5):
            temp[6*i+x]=barPix #finding the index of array from width=5, column and row value
            temp[6*y+i]=barPix
    for x,y in data:
        x=x//20
        y=y-(y//360)*360#getting within 0,360 always
        y=y//72
        temp[6*y+x]=dataPix
    Img=Image(temp.decode())


#function to display the Image on matrix
def displayScreen(sensorValue, motorValue,brightness=100):
    global oldx
    global oldy
    print(motorValue)
    motorValue=motorValue-(motorValue//360)*360
    print(motorValue)
    y=motorValue//72
    x=sensorValue//20
    if(not (oldx == x and oldy==y)): #to reduce flickering
        playmusic(sensorToneLookup[x])
        display.show(Img)
        display.pixel(x,y,brightness) # perhaps blink here

    oldx=x
    oldy=y


playmusic(trainTone)
init_display()

while (not(buttonValue is  3 and buttonTime > 3000)): # it will quit if the centre button is pressed for more than a sec
    (sensorValue,motorValue)=getsensorValue()
    blinkx=sensorValue
    blinky=motorValue
    displayScreen(sensorValue,motorValue)
    hub.led(0,100,0) #GREEN color for Training


    if(buttonValue is 1): #right button is pressed
        if(buttonTime>100): #debounce
            data.append([sensorValue,motorValue])
            updateDataImage()
            playmusic(saveTone)
            print('pressed')
            print(data)

        buttonValue=    0 #reset buttonValue and Timer
        buttonTime    =    0

    elif(buttonValue is 4): #connect button is pressed
        if data and buttonTime<100:
            data.pop()
            updateDataImage()

        buttonValue=0 #reset buttonValue and Timer
        buttonTime    =    0

    elif(buttonValue is 2): #left button is pressed
        # BLUE color for running
        hub.led(0,0,100)
        playmusic(runTone)
        print('running')
        print(data)
        buttonValue    =    0 #reset buttonValue and Timer
        buttonTime    =    0

        while(not (buttonValue == 1 and buttonTime>2000) ):
            sensorValue,motorValue=getsensorValue()
            min = 1000000
            pos = 0
            for (l, a) in data:
                dist = abs(sensorValue - l)
                if dist < min:
                    min = dist
                    pos = a
            hub.port.A.motor.run_to_position(pos,9000)
            displayScreen(sensorValue,pos)
            print(data,sensorValue,pos)
            blinkx=sensorValue
            blinky=pos
            time.sleep(.1)

        buttonValue    =    0 #reset buttonValue and Timer
        buttonTime    =    0
        hub.port.A.motor.float()
        data=[]
        init_display()
        displayScreen(0,0,0)

init_display()
displayScreen(0,0,0)
playmusic(shutDown)
print("Done")