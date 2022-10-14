#Hackathon Code

import time
import board
import pwmio
from digitalio import DigitalInOut, Direction, Pull
from analogio import AnalogIn
from adafruit_motor import servo

pwm = pwmio.PWMOut(board.A3, duty_cycle=2 ** 15, frequency=50)
my_servo = servo.Servo(pwm)
led1=DigitalInOut(board.BLUE_LED1_INVERTED)
led2=pwmio.PWMOut(board.BLUE_LED2_INVERTED, duty_cycle=0, frequency=50)
led1.direction=Direction.OUTPUT
led1.value=True
switch = DigitalInOut(board.D2)
pot= AnalogIn(board.A0)
light = AnalogIn(board.A1)
data=[]
switch.direction = Direction.INPUT
switch.pull  = None

def mapAngle(value, minX=0, maxX=65536, minY=0, maxY=180):
    return(value*(maxY-minY)/(maxX-minX)) # will return a number between 0 and 180 for pot value

def check_button():
    print("button pressed")
    enter=time.monotonic()
    led1.value=False
    while(switch.value==True):
        pass
    led1.value=True
    now=time.monotonic()
    print(enter,now)
    return (now-enter)


while True:
    motorPosition = mapAngle(pot.value)
    my_servo.angle=motorPosition
    lightData=light.value
    led2.duty_cycle=65535-lightData
    if (switch.value):
        press_time=check_button()                   
        if(press_time<2): #Training mode
            data.append([lightData,motorPosition])
            time.sleep(0.01)

        elif(press_time>2):     #Running mode
            led2.deinit()
            led2=DigitalInOut(board.BLUE_LED2_INVERTED)
            led2.direction=Direction.OUTPUT
            while(True):
                if (switch.value):
                    led2.deinit()
                    led2=pwmio.PWMOut(board.BLUE_LED2_INVERTED, duty_cycle=0, frequency=50)
                    break
                else:
                    min=10000
                    for i in data:
                        if(min>abs(light.value-i[0])):
                            pos=i[1]
                            min=abs(light.value-i[0])
                        my_servo.angle=pos
                led2.value=True
                led1.value=False
                time.sleep(0.1)
                led2.value=False
                led1.value=True
                time.sleep(0.1)
    else:
        pass
    time.sleep(0.05)

