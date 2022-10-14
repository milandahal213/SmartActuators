#Hackathon Code

import time
import board
import pwmio
from digitalio import DigitalInOut, Direction, Pull
from analogio import AnalogIn


my_servo = pwmio.PWMOut(board.D3,frequency=50)
led1=DigitalInOut(board.BLUE_LED1_INVERTED)
led2=pwmio.PWMOut(board.BLUE_LED2_INVERTED, duty_cycle=0, frequency=50)
led1.direction=Direction.OUTPUT
led1.value=True
switch = DigitalInOut(board.D2)
pot= AnalogIn(board.A0)
light = AnalogIn(board.A1)
data=[]
switch.direction = Direction.INPUT
switch.pull = Pull.DOWN

training = True #state defines what mode the motor is running on


zeroDegree=13107
oneEightyDegree=1640


def pot2Angle(value, minX=0, maxX=65535, minY=1640, maxY=13107):
    slope=(maxY-minY)/(maxX-minX)
    angle=slope*(value-minX) + minY
    return int(angle)

def check_button():
    print("button pressed")
    enter=time.monotonic()
    led1.value=False
    while(switch.value==True):
        if(time.monotonic()-enter>2):
            while (switch.value):
                blink(0.1,0.2)
            return False
    led1.value=True
    return True

def blink(on, off):
    led1.value= True
    time.sleep(on)
    led1.value= False
    time.sleep(off)

while True:

    #motorPosition = mapAngle(pot.value)
    
    Angle=pot2Angle(pot.value)
    my_servo.duty_cycle=Angle
    print(("pot value",pot.value, "angle: ",Angle))    
    lightData=light.value
    #print(lightData)
    led2.duty_cycle=65535-lightData
    if (switch.value):
        training=check_button() 
        if(training): #Training mode
            data.append([lightData,Angle])
            print("training")
            time.sleep(0.01)

        elif(not training):     #Running mode
            print("running")
            while(True):
                if (switch.value):
                    while(switch.value):
                        blink(0.4,0.4)
                        pass
                    break
                else:
                    min=10000
                    pos=0
                    for i in data:
                        if(min>abs(light.value-i[0])):
                            pos=i[1]
                            min=abs(light.value-i[0])
                        my_servo.duty_cycle=pos
                        print(pos)

                blink(0.1,0.2)
    else:
        pass
    time.sleep(0.1)

