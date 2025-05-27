from machine import Pin, Timer, SoftI2C
import machine
import time
import neopixel
import esp32
from accel import H3LIS331DL
import struct
from bleCEEO import Yell



class Sensor():
    def __init__(self):
        #Defining pins
        self.np = neopixel.NeoPixel(Pin(20), 12)

        self.motor = Pin(21, Pin.OUT)

        self.button = Pin(0, Pin.IN, Pin.PULL_UP)
        esp32.wake_on_ext1(pins = (self.button,), level = esp32.WAKEUP_ALL_LOW)
        
        #delaying
        a = self.button.value()      
        while a == 0:
            a = self.button.value()
            time.sleep(2)
            #keep stalling while the button is pressed
        
        #setting up accelerometer
        i2c = SoftI2C(scl = Pin(23), sda = Pin(22))

        self.h3lis331dl = H3LIS331DL(i2c)
        self.h3lis331dl.select_datarate()
        self.h3lis331dl.select_data_config()
        
        
    
        self.button_value = 1
        self.last_button_value = 1
        self.button_event = False
        self.button_pressed = False
        
        self.time_of_button_press = 0

        self.loading() #show animating LED

        tim = Timer(0)
        tim.init(period=200, mode=Timer.PERIODIC, callback=self.check_switch) #timer to check button press
    
    def readAccel(self):
        accel = self.h3lis331dl.read_accl()
        accel = accel + (self.button_pressed,)
        #print("accelerometer value", accel)
        return accel
    
    
    def is_pressed(self):
        return self.button_pressed
    
    def check_switch(self,p):
        self.button_value = self.button.value()
        if self.button_value != self.last_button_value:
            self.button_event = True
 
        if self.button_event == True:
            
            if self.button.value() == 0:
                self.button_pressed = True
                self.time_of_button_press = time.time()
                self.buzz()
                          
            else:
                self.button_pressed = False
                self.run_time_related_action()
                #machine.deepsleep()
                
                
            self.button_event = False
        self.last_button_value = self.button_value
        
        #to make sure the button is not pressed for too long
        if self.button.value() == 0:
             self.run_time_related_action()
            
            
    
    
    def run_time_related_action(self):
        if(abs(time.time() - self.time_of_button_press)>2):
            self.shuttingdown()
            machine.deepsleep()
        else:
            self.button_pressed = True

        
        
    def buzz(self, dur=0.2, num=1):
        for i in range(num):
            self.motor.on()
            time.sleep(dur)
            self.motor.off()
        
        
    def connectionStatus(self,state):
        if state:
            #buzz(0.5,1)
            for i in range(12):
                self.np[i] = (0,50,0)
                self.np.write()
        else:
            #buzz(0.5,2)
            for i in range(12):
                self.np[i] = (50,0,0)
                self.np.write()
        
            

            
    def shuttingdown(self):
        for i in range(12):
            self.np[i] = (50,0,0)
            self.np.write()
            time.sleep(0.1)

        self.turn_off()
        
        
    def loading(self):
        for j in range(2):
            for i in range(12):
                self.np[i%12] = (50,0,0)
                self.np[(i+1)%12] = (0,50,0)
                self.np.write()
                time.sleep(0.1)
                
    def turn_off(self):
        for i in range(12):
            self.np[i] = [0,0,0]
        self.np.write()
        
    
    def gotosleep(self):
        machine.deepsleep()
        
    def __del__(self):
        print("deleted")
        
        
        
try:
    del s
    
except:
    print("doesn't exist")
s= Sensor()


bleADV = Timer(1)
p = Yell('MPY ESP32', interval_us=600, verbose = False)


connectionStatus = False
data = 0

def advertiseSensor(t):
    global data, connectionStatus
    
    payload = struct.pack('>fffB',*s.readAccel()) #packing sensor value as payload
    s.button_pressed = False
    #payload = b'abcdefghijklmnopqrst'
    p.send(bytes(payload))
    if p.is_any:
        connectionStatus = True
        data = p.read()
        print(data)
    if not p.is_connected:
        
        print('lost connection')
        bleADV.deinit()
            

if p.connect_up():
    bleADV.init(period=500, mode=Timer.PERIODIC, callback=advertiseSensor)
    

    
    
while True:
        
    if connectionStatus == True:
        connectionStatus(True)
        #display.showmessage("Connected")
    
    if not p.is_connected:
        bleADV.init(period=3000, mode=Timer.PERIODIC, callback=advertiseSensor)

    #s.write_angle(int(data))
    time.sleep(0.3)
    




