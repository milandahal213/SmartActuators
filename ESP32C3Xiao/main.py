from machine import Pin, SoftI2C , PWM , ADC
import time
import ssd1306
import servo


#setting display lines and varibales

from machine import Pin, SoftI2C , PWM , ADC
import time
import ssd1306
i2c=SoftI2C(scl=Pin(7), sda=Pin(6))
display = ssd1306.SSD1306_I2C(128, 64, i2c)
display.text("Booting Up",40,0)
display.show()

oldmsg=""
oldsensor=""
oldmotor=""


#setting the sensor
sensor=ADC(Pin(5))
sensor.atten(ADC.ATTN_11DB)

#setting the pot
pot=ADC(Pin(3))
pot.atten(ADC.ATTN_11DB)
#setting the motor

s=servo.Servo(Pin(2))

down= Pin(10,Pin.IN)
select = Pin(9, Pin.IN)
up = Pin(8, Pin.IN)

training = True #state defines what mode the motor is running on 
data=[]

#setting the functions


def showState(msg):
    global oldmsg
    display.text(oldmsg,40,0,0)
    display.text(msg, 40, 0, 1)
    display.show()
    oldmsg=msg

def showData(sensor, motor):
    global oldsensor, oldmotor
    display.text(oldsensor,10,30,0)
    display.text(oldmotor,10,50,0)
    display.text(sensor,10,30,1)
    display.text(motor,10,50,1)
    display.show()
    oldsensor=sensor
    oldmotor=motor

def showButtonState(msg,color):
    display.text(msg,10,0,color)
    display.show()


def check_button(switch):
    enter=time.ticks_ms()
    while(not switch.value()==True):
        if(time.ticks_ms()-enter>1500):
            return False
    return True



def play():
    pos=0
    while(True):
        if (not select.value()):
            while(not select.value()):
                pass
            break
        else:
            min=65000
            sens=sensor.read_u16()
            for i in data:
                if(min>abs(sens-i[0])):
                    pos=i[1]
                    min=abs(sens-i[0])
                print("inside",pos)
            s.write_angle(pos)
            print("outside",pos)
            showData(str(sens),str(pos))
            time.sleep(0.1)


while True:
    #sensorData=sensor.read() 
    showState("Training")
    sens=sensor.read_u16()
    pos=int(pot.read_u16()/65535 * 180)
    s.write_angle(pos)
    showData(str(sens),str(pos))
    if (not select.value()):
        training=check_button(select) 
        if(training): #Training mode
            data.append([sens,pos])
            showButtonState("Saving",1)
            time.sleep(0.5)
            showButtonState("Saving",0)

        elif(not training):     #Running mode
            while(not select.value()):
                pass
                time.sleep(0.1)
            showState("Running")
            play()

    else:
        pass
    time.sleep(0.1)