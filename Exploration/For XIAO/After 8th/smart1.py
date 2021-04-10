
import time
import board
import pwmio
from adafruit_motor import servo
from analogio import AnalogIn
from digitalio import DigitalInOut, Direction


pwmR = pwmio.PWMOut(board.A4, frequency=50)
my_servoR = servo.ContinuousServo(pwmR)


pwmL = pwmio.PWMOut(board.A4, frequency=50)
my_servoL = servo.ContinuousServo(pwmL)

light=AnalogIn(board.A0)

slider = AnalogIn(board.A5)

my_servo.throttle=1
print("succes")
print("this ran by the way")

def s():
	return slider.value

def l():
	return light.value



