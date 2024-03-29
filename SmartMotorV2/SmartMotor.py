# smart Motor code with 8266
'''# Pins for PCB design
# short D8 and D4 - no navigation switch on this prototype 
| Pins | Numbers | Roles |
| --- | --- | --- |
| D0 | GPIO 16 | Switch Up |
| D1 | GPIO 5 | OLED - SCL |
| D2 | GPIO 4 | OLED - SDA |
| D3 | GPIO 0 | Motor - Encoder 1 |
| D4 | GPIO 2 | Switch Down |
| D5 | GPIO 14 | Driver Phase |
| D6 | GPIO 12 | Driver Enable |
| D7 | GPIO 13 | Switch T |
| D8 | GPIO 15 | Motor - Encoder 2 |
| A0 |  | Sensor Port - Analog signal |
| Rx |  | #potential sensor |
| Tx |  | #potential sensor |

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

D0=Pin(16) 
D1=Pin(5)
D2=Pin(4)
D3=Pin(0)
D4=Pin(2)
D5=Pin(14)
D6=Pin(12)
D7=Pin(13)
D8=Pin(15)

training = True #state defines what mode the motor is running on 
data=[]
  
#setting the motor

enable=PWM(D6) #D6 - PWM pin
enable.freq(500)
enable.duty(00)

phase = machine.Pin(14, machine.Pin.OUT) # D5 direction

#setting the sensor
sensor=ADC(0)

#setting buttons
recBut=Pin(13,Pin.IN)


#setting the display 
i2c = I2C(sda=Pin(4), scl=Pin(5))
display = ssd1306.SSD1306_I2C(128, 64, i2c) #SCL - D1 SDA - D2
display.text('Hello, World!', 0, 0, 1)
display.show()

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

#setting the inputs


#setting the functions
def check_button(switch):
    enter=time.ticks_ms()
    while(switch.value()==True):
        if(time.ticks_ms()-enter>1500):
            return False
    return True


def moveMotor(pos):
    while(abs(position.value()-pos)>100):
        enable.duty(200)
        if(position.value()>pos):
            phase.value(1)
        else:
            phase.value(0)
        print(position.value())
        time.sleep(0.1)
    
    enable.duty(0)
    position.value()


position = Count(Pin(2, Pin.IN),Pin(0, Pin.IN))

sensor.read()

time.ticks_ms()

while True:
    #sensorData=sensor.read() 
    if (recBut.value()):
        training=check_button(recBut) 
        if(training): #Training mode
            data.append([sensor.read(),position.value()])
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
                    min=10000
                    pos=0
                    for i in data:
                        if(min>abs(sensor.read()-i[0])):
                            pos=i[1]
                            min=abs(sensor.read()-i[0])
                        print("inside",pos)
                    moveMotor(pos)
                    print("outside",pos)
                    time.sleep(0.1)

    else:
        pass
    time.sleep(0.1)





'''
playgoround

from machine import Pin, ADC
from time import sleep
pot = ADC(0)
pot.read()
pot.atten(ADC.ATTN_11DB)       #Full range: 3.3v
pyb.Timer(2, freq=4, callback=lambda t: print('.', end=''))
a.irq(print("done"),a.IRQ_FALLING)
'''