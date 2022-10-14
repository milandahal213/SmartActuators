# smart Motor code with 8266

#Pins 
#D0 - 
#D1 -SCL
#D2 -SDA
#D3
#D4 
#D5 -INA 	- motor Driver
#D6 -INB 	- Motor Driver 
#D7 -PWMA 	- Motor Driver
#D8
#D9

from machine import Pin, I2C
import ssd1306
i2c = I2C(sda=Pin(4), scl=Pin(5))
display = ssd1306.SSD1306_I2C(128, 64, i2c) #SCL - D1 SDA - D2
display.text('Hello, World!', 0, 0, 1)
display.show()


setting the motor
import machine
p13=machine.Pin(13)
PWM13=machine.PWM(p13) #D7 - PWM pin
PWM13.freq(500)
PWM13.duty(00)


setting the inputs
 
p1 = machine.Pin(14, machine.Pin.OUT) # D5 direction
p2 = machine.Pin(12, machine.Pin.OUT) #D6 on /off
# set the value low then high
p1.value(0)
p2.value(1)