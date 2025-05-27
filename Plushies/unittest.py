from machine import Pin, PWM
import machine
s = Pin(0, Pin.IN)

print("press and hold the switch and press enter")
input()
print(s.value())

print("let go of the switch and press enter")
input()
print(s.value())

import time

buzzer = Pin(21, Pin.OUT)
buzzer.on()
time.sleep(0.2)
buzzer.off()


import neopixel
np = neopixel.NeoPixel(Pin(20), 12)
np[0] = (255,0,0)
np.write()
print("One RED Led should be on")


from machine import Pin, PWM
import machine
import time

class SOUND():
    def __init__(self, p):
        self.pwm = PWM(Pin(p, Pin.OUT), freq=1000, duty=1000)  # Pin(5) in PWM mode here
        self.pwm.duty(0)
        
    def play(self, frequency , hold):
        self.pwm.duty(400)
        self.pwm.freq(int(frequency))
        time.sleep(hold)
        self.pwm.duty(0)




s= SOUND(19)

def playsound():
    s.play(659.26,0.15)
    s.play(587.33,0.15)
    s.play(369.99,0.3)
    s.play(415.3,0.3)
    s.play(554.37,0.15)
    s.play(493.88,0.15)
    s.play(293.66,0.3)
    s.play(329.63,0.3)
    s.play(493.88,0.15)
    s.play(440,0.15)
    s.play(277.18,0.3)
    s.play(329.62,0.3)
    s.play(440,0.6)


playsound()