
import hub, time
from hub import port,button, sound, status, Image, display
from machine import Timer

#setting upImage object with format Image('00000:00000:00000:00000:')
temp=bytearray(30)
temp[5]=58
temp[11]=58
temp[17]=58
temp[23]=58
temp[29]=58
Img=Image(temp.decode())
blinkx=-1
blinky=-1

#lookup table for indexing getSensor return array
sensorLookup={61:0 ,62:0, 63:0, 48:2, 65:2, 255:0}


#setting up sensor and motor Ports
sensorPort    = 'B'
motorPort    = 'A'

#setting interrupt for current position
Timer(mode=Timer.PERIODIC, period=1000, callback=lambda t:interrupt())

def interrupt():
    displayScreen(blinkx,blinky,0)
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

#defining default data
data=[]


#function to return everything connected to the SPIKE Prime Hub (slightly unnecessary- might remove c.d.e and f)
def getStat():
    a=port.A.info()["type"]
    b=port.B.info()["type"]
    c=port.C.info()["type"]
    d=port.D.info()["type"]
    e=port.E.info()["type"]
    f=port.F.info()["type"]
    portStat=hub.status()["port"]
    state={"A":{"t":a,"d":portStat["A"]},"B":{"t":b,"d":portStat["B"]},"C":{"t":c,"d":portStat["C"]},"D":{"t":d,"d":portStat["D"]},"E":{"t":e,"d":portStat["E"]},"F":{"t":f,"d":portStat["F"]}}
    return(state)

#using the lookup table get the relevant data from the sensor/motor array from getStat function
def getsensorValue(port):
    ret=getStat()
    sensorType=ret[port]['t']
    value=ret[port]['d']
    if value[sensorLookup[sensorType]] is None:
        return 0
    return value[sensorLookup[sensorType]]

#function to play tunes
def playmusic(music):
    for note,dur in music:
        sound.beep(note,dur)
        time.sleep_ms(dur)

#function to update the Image
def updateDataImage():
    global Img
    global temp

    barPix=55 #brightness for bar
    dataPix=57 #brightness for data
    #setting bar

    for x,y in data:
        x=x//20 #dividing 100 into 5
        y=y//72 #dividing 360 into 5
        for i in range(5):
            temp[6*i+x]=barPix #finding the index of array from width=5, column and row value
            temp[6*y+i]=barPix
            print(6*i+x,6*y+i,temp)
    for x,y in data:
        x=x//20
        y=y//72
        temp[6*y+x]=dataPix
    print(temp)
    Img=Image(temp.decode())


#function to display the Image on matrix
def displayScreen(sensorValue, motorValue,brightness=100):
 display.show(Img)
 y=motorValue//72
 x=sensorValue//20
 display.pixel(x,y,brightness) # perhaps blink here


playmusic(trainTone)

while (buttonValue is not 3 and buttonTime < 1000): # it will quit if the centre button is pressed for more than a sec
 sensorValue=getsensorValue(sensorPort)
 motorValue=getsensorValue(motorPort)+180 # to make the angle positive
 blinkx=sensorValue
 blinky=motorValue
 displayScreen(sensorValue,motorValue)
 hub.led(0,0,100) #BLUE color for Training


 if(buttonValue is 1): #right button is pressed
    if(buttonTime>100): #debounce

        data.append([sensorValue,motorValue])
        updateDataImage()
        playmusic(saveTone)
        print('pressed')
        print(data)


    buttonValue    =    0 #reset buttonValue and Timer
    buttonTime    =    0

 elif(buttonValue is 4): #connect button is pressed
    if data and buttonTime<100:
        data.pop()
        updateDataImage()

    if(buttonTime > 2000):
        data=[[0,0]]
        time.sleep(0.1)
        updateDataImage()


    buttonValue    =    0 #reset buttonValue and Timer
    buttonTime    =    0

 elif(buttonValue is 2): #left button is pressed
    # GREEN color for running
    hub.led(0,100,0)
    playmusic(runTone)
    print('running')
    print(data)
    buttonValue    =    0 #reset buttonValue and Timer
    buttonTime    =    0


    while((buttonValue is not 3 or 1 and buttonTime > 1000)):
        sensorValue=getsensorValue(sensorPort)
        min = 1000000
        pos = 0
        for (l, a) in data:
            dist = abs(sensorValue - l)
            if dist < min:
                min = dist
                pos = a
                print(pos)
            hub.port.A.motor.run_to_position(pos,9000)
            displayScreen(sensorValue,pos)
            blinkx=sensorValue
            blinky=pos
        time.sleep(.1)

    buttonValue    =    0 #reset buttonValue and Timer
    buttonTime    =    0

    playmusic(trainTone)