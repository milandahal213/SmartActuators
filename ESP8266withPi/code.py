#Smart Motor Training Code Prototype 2.2.2 - NO AR
    #This code trains the Smart Motor based on different light sensor values. The user can use 
    #as many different positions and light sensor values as desired. To switch from training to 
    #run mode, press and hold the single button, then press the same button again to retrain. 
​
#Import libraries
import machine
import time
from machine import Pin, ADC
​
#Initialize servo pin and set pwm frequency
p2 = machine.Pin(0)
pwm2 = machine.PWM(p2)
pwm2.freq(50)
pwm2.duty(90)
​
#Initialize other pins
Buzzer = Pin(4, Pin.OUT)
BuiltInLED = Pin(2, Pin.OUT)
LightSensor = Pin(3, Pin.OUT)
Potentiometer = Pin(13, Pin.OUT)
#Left = Pin(13, Pin.IN) #button to turn motor one way
Button = Pin(14, Pin.IN)
#Right = Pin(15, Pin.IN) #button to turn motor other way
​
LightSensor.off()
Potentiometer.off()
​
value = ADC(0)
​
def buzzer(repeats):
    for i in range(repeats):
        for i in range(2000):
            Buzzer.on()
            time.sleep(.001)
            Buzzer.off()
            time.sleep(.0005)
​
        time.sleep(0.2)
​
​
def speaker():
    for i in range(2000):
        
​
def mapSensorRange(potValue):
    mappedValue = 26 + ((potValue-9)*(138-26))/946
    return int(mappedValue)
​
​
#Main loop
while True:
    LightSensor.off()
    Potentiometer.off()
    print("in loop")
    training = [] #array to hold training values
    BuiltInLED.on() #turns built-in LED off
    while True:
        Potentiometer.off()
        LightSensor.off()
        while Button.value() == False:
            Potentiometer.on()
            potValue = value.read()
            dutyValue = mapSensorRange(potValue)
            #Map potentiometer value to motor value
            pwm2.duty(dutyValue)
            time.sleep(.01)
        
        print("Button pressed")
        Potentiometer.off()
        LightSensor.on()
        angle = pwm2.duty()
        bright = value.read()
        pressTime = 0
        #Go to run mode if long press
        while  Button.value():
            BuiltInLED.off()
            time.sleep(.1)
            pressTime += 0.1
            print(pressTime)
            if pressTime == 1.0:
                break
        BuiltInLED.on()
        if pressTime > 1:
            break
        buzzer(1)
        #Add training to array
        training.append((angle,bright))
        print(angle)
        print(bright)
    buzzer(2)
    BuiltInLED.off()
    while  Button.value():
        time.sleep(.1)  
​
    LightSensor.on()
    #Run mode
    while True:
        print("In the loop")
        bright = value.read()
        min = 1000
        pos = 0
        for (a,l) in training:
            dist = abs(bright - l)
            if dist < min:
                min = dist
                pos = a
        pwm2.duty(pos)
        time.sleep(.01)
        if Button.value() == True:
            break
​
    BuiltInLED.on()
    buzzer(1)
    while  Button.value():
        time.sleep(.1)
​