from machine import Pin, PWM
import utime
from machine import UART
import time
import json
import math

dataCount=0


count=0
Flag=True
print("Ok run")



class Count(object):
    def __init__(self,A):
        self.A = A
        self.m=''
        self.counter = 0
        A.irq(self.cb,self.A.IRQ_FALLING)#|self.A.IRQ_RISING) 

    def cb(self,msg):   
        self.counter += 1 
        self.m=msg
        
    def value(self):
        return (self.counter,self.m)

c=Count(Pin(0))


t=Pin(0)
def C(flag,count):
	if(flag==True):
		return (count+1)
	else:
		return count

while True:

	if t.value()==1:
		count=C(Flag,count)
		print(count)
		Flag=False
	elif t.value()==0:
		Flag=True


bark=Pin(18,Pin.OUT)
#pin numbers for Encoders
#15- D2
#25- D3
#16- D4
#17- D5

#pin numbers for PWMs 
#26 - A0
#27 - A1
#28 - A2
#29 - A3

duty = 0 #between 0-65000
pwm=[]
pos=[0]*4
PWMPins=[26,27,28,29]
EncoderPins=[16,17,15,25] 

for i in range(2):
    pos[i]=Count(Pin(EncoderPins[i*2], Pin.IN),Pin(EncoderPins[i*2+1], Pin.IN))


def __init__(self, name, pin,
                 rising=False, falling=False,
                 pullup=True, on_change=None, report_change=True):
        if rising and falling:
            trigger = Pin.IRQ_RISING | Pin.IRQ_FALLING
        elif not rising and falling:
            trigger = Pin.IRQ_FALLING
        else:  # also if both all false
            trigger = Pin.IRQ_RISING
        pin.irq(trigger=trigger, handler=self._cb)
        self.counter = 0
        self.report_counter = 0
        self.triggered = False
        Device.__init__(self, name, pin, on_change=on_change,
                        report_change=report_change)
        self.getters[""] = self.value 
