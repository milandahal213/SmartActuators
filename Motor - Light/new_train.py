# Write your code here :-)
import time
import board
import pwmio
from adafruit_motor import servo
from analogio import AnalogIn
from digitalio import DigitalInOut, Direction

pwm = pwmio.PWMOut(board.A2, frequency=50)
my_servo = servo.ContinuousServo(pwm)

slider = AnalogIn(board.A7)
print(slider.value)

switch = DigitalInOut(board.D9)

light=AnalogIn(board.A5)
print(light.value)

def check_button():
	print("button pressed")
	enter=time.monotonic()
	while(switch.value==True):
	    pass
	now=time.monotonic()
	print(enter,now)
	return (now-enter)
x=light.value
x=0  # x
y=0   # y

sx=0    #sum of x
sy=0    #sum of y

sxsq=0  #sum of square of x
sxy =0  #sum of product of x and y

m=0     #slope
b=0     #intercept

N=0 # number of training data

while True:
	y = slider.value/65520
	my_servo.throttle=y
	if (switch.value):
		press_time=check_button()					#Training mode
		if(press_time<2):
			x=light.value/100
			sx=sx+x
			sy=sy+y
			N=N+1

			sxy=sxy + x*y
			sxsq= sxsq + x * x
			print(x,y,"brightness and speed")
			print("N",N,"sx",sx,"sy",sy,"sxsq",sxsq)
			print("sleeping")
			time.sleep(1)

		elif(press_time>2): 	#Running mode
			m=(N *sxy - sx*sy)/(N*sxsq -sx*sx) # calculate the slope
			b=(sy-m*sx)/N                       #calculate the intercept
			print(m,b)
			while(True):
				if (switch.value):
					break
				else:
					x=light.value/100
					y= x * m + b
					if(abs(y)<1):
						my_servo.throttle=y
					time.sleep(0.1)
			print("nowstopping running")
			time.sleep(1)
		else:
			print("that is impossible")




