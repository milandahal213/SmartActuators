from machine import Pin, PWM, ADC, I2C
from ssd1306 import SSD1306_I2C
import utime

# set up inputs/outputs
motor = PWM(Pin(3))  # servo signal is in D10 which is Pin 3
motor_position = ADC(28)   # servo analog position value is on A2 which is Pin 28
sensor = ADC(26)  # grove light sensor is plugged into port for A0
beep = PWM(Pin(29))  # Xiao expanion board buzzer is on A3/D3 which is Pin 29
i2c_dev = I2C(1,scl=Pin(7),sda=Pin(6),freq=200000)  # set-up for screen
screen = SSD1306_I2C(128, 64, i2c_dev)
grove_button = Pin(1,Pin.IN,Pin.PULL_UP) # grove button is plugged into port for D7 which is Pin 1
save = grove_button.value   # grove button is used for our 'save' in our user interface
exp_button = Pin(27,Pin.IN,Pin.PULL_UP)  # expansion board button is D1 which is Pin 27
done_not_pressed = exp_button.value #NOTE -> done button registers 1 when not pressed; 0 when pressed

# beep_tune function
def beep_tune():
    beep.duty_u16(65535//2)  # on 50% of the time
    beep.freq (262)  # C
    utime.sleep(0.2)
    beep.freq (330) # E
    utime.sleep(0.2)
    beep.freq (262)
    utime.sleep (0.2)
    beep.duty_u16 (0)

# set up _map function similar to arduino "map" scaling function (see reference R1)
def _map(x, in_min, in_max, out_min, out_max):
    return int((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)


# set up training
training = []
beep_tune()

counter = 0
while done_not_pressed():
    screen.text (str(counter),10,10)
    screen.show()
    while not save() and done_not_pressed():
        utime.sleep(0.1)  #debounce
    if done_not_pressed():
        counter += 1
        angle = motor_position.read_u16()
        bright = sensor.read_u16()
        print (bright)
        while save():
            utime.sleep(0.1)
            training.append((angle,bright))
            print('(%d,%d)'%(angle,bright))
    screen.fill(0)   # clear screen
    screen.show()

beep_tune()
screen.fill(0)
screen.show()

while not done_not_pressed():  # done button is pressed
    utime.sleep(0.1) # debounce

while done_not_pressed():  # grab reading
    bright = sensor.read_u16()
    min = 100000
    pos = 0
    for (a,l) in training:
        dist = abs(bright - l)
        if dist < min:
            min = dist
            pos = a
    print('(%d,%d)'%(pos,bright))
    pos_servo = _map(pos,5537,47979,1000,9000)
    motor.freq(50)
    motor.duty_u16(pos_servo)   # move servo motor to position
    utime.sleep(0.1)

# Exit
beep_tune()
utime.sleep(0.4)
beep_tune()
motor.deinit()

# References
# R1 --> https://www.theamplituhedron.com/articles/How-to-replicate-the-Arduino-map-function-in-Python-for-Raspberry-Pi/








