# smart Motor code with PCIO
'''

Wiring method:
black: motor power"-"
red: motor power"+"
brown: encode power "+"
green: encode power "-"
blue: encode output A phase
white: encode output B phase
(Note: Encoder input voltage: 3.3-12v)
'''
from machine import Pin, I2C, PWM , ADC
import time
import ssd1306

training = True #state defines what mode the motor is running on 
data=[]
  
#setting the motor

enable=PWM(Pin(15))
enable.freq(500)
enable.duty_u16(00)

phase = machine.Pin(14, machine.Pin.OUT)
coast = machine.Pin(0, machine.Pin.OUT)

#setting the sensor
sensor=ADC(0)

#setting buttons
recBut=Pin(1,Pin.IN)


#setting the display 
i2c = machine.I2C(0, scl=machine.Pin(17), sda=machine.Pin(16)) #pico 
display = ssd1306.SSD1306_I2C(128, 64, i2c)

import framebuf
fbuf = framebuf.FrameBuffer(bytearray(8 * 8 * 1), 8, 8, framebuf.MONO_VLSB)
fbuf.line(0, 0, 7, 7, 1)
display.blit(fbuf, 10, 10, 0)           # draw on top at x=10, y=10, key=0
display.show()

from machine import Pin, I2C, PWM , ADC
class Count(object):
    def __init__(self,A,B):
        self.A = A
        self.B = B
        self.counter = 0
        A.irq(self.cb,self.A.IRQ_FALLING|self.A.IRQ_RISING) #interrupt on line A
        B.irq(self.cb,self.B.IRQ_FALLING|self.B.IRQ_RISING) #interrupt on line B

    def cb(self,msg):
        other,inc = (self.B,1) if msg == self.A else (self.A,-1) #define other line and increment
        self.counter += -inc if msg.value()!=other.value() else  inc #XOR the two lines and increment
    def value(self):
        return self.counter



position = Count(Pin(2, Pin.IN),Pin(3, Pin.IN))

#setting the inputs


#setting the functions
def check_button(switch):
    enter=time.ticks_ms()
    while(switch.value()==True):
        if(time.ticks_ms()-enter>1500):
            return False
    return True

def stopMotor():
    enable.duty_u16(0)

def enableCoasting():
    coast.value(0)


def disableCoasting():
    coast.value(1)


def moveMotor(pos):
    disableCoasting()
    while(abs(position.value()-pos)>50):
        print("here")
        enable.duty_u16(300000)
        if(position.value()>pos):
            phase.value(1)
        else:
            phase.value(0)
        time.sleep(0.1)
    
    enable.duty_u16(0)
    position.value()

sensor.read_u16()

time.ticks_ms()

while True:
    #sensorData=sensor.read() 
    enableCoasting()
    if (recBut.value()):
        training=check_button(recBut) 
        if(training): #Training mode
            data.append([sensor.read_u16(),position.value()])
            print("training")
            time.sleep(0.01)

        elif(not training):     #Running mode
            while(recBut.value()):
                pass
                time.sleep(0.1)
            print("running")
            while(True):
                if (recBut.value()):
                    while(recBut.value()):
                        pass
                    break
                else:
                    min=100000000
                    pos=0
                    for i in data:
                        if(min>abs(sensor.read_u16()-i[0])):
                            pos=i[1]
                            min=abs(sensor.read_u16()-i[0])
                        print("inside",pos)
                    moveMotor(pos)
                    print("outside",pos)
                    time.sleep(0.1)

    else:
        pass
    time.sleep(0.1)





'''
playgoround

0.4 mHz 


2350 rotation per second 

from machine import Pin, ADC
from time import sleep
pot = ADC(0)
pot.read()
pot.atten(ADC.ATTN_11DB)       #Full range: 3.3v
pyb.Timer(2, freq=4, callback=lambda t: print('.', end=''))
a.irq(print("done"),a.IRQ_FALLING)
'''