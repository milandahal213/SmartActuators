'''
SmartMotor

Run this program then move the motor and cover 
the light sensor and hit the button for each training point.
When you are done, double-click the button - the motor should
move to the closest training point.  Hit the button
again to stop.
'''

import hub, utime

utime.sleep(1)

motor = hub.port.A.motor
light = hub.port.B.device
light.mode(2)
#utime.sleep(4)
led = hub.led
btn = hub.button.center.is_pressed
press = hub.button.center.presses


motor.mode(2)
motor.float()
#light.mode(4)  # read reflected light

training = []
counter = 0

train = True
count = press()
while train:
    led(0,0,200) # blue - training
    while not btn():  
        utime.sleep(.1) # wait for button 
    led(0,0,0) # thinking
    utime.sleep(1)
    
    count = press()
    if count == 1:
        counter += 1
        angle = motor.get()[0]
        bright = light.get()[0]
        while btn():
            utime.sleep(.1)
        training.append((angle,bright))
        print('(%d,%d)'%(angle,bright))
    else:
        train = False
        print(count)

led(0,200,0) # green - running

while btn(): 
    utime.sleep(.1) # debounce

# grab reading
while not btn():
    bright = light.get()[0]
    min = 1000
    pos = 0
    for (a, l) in training:
        dist = abs(bright - l)
        if dist < min:
            min = dist
            pos = a
    motor.run_to_position(pos,50)
    utime.sleep(.1)
    #print('(%d,%d)'%(pos,bright))
    
led(0,0,0) # off
hub.power_off()
