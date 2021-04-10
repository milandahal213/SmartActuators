# Write your code here :-)
import time
import board
import pwmio
from adafruit_motor import servo
from analogio import AnalogIn
from digitalio import DigitalInOut, Direction
import wifi


pwm1 = pwmio.PWMOut(board.A1, frequency=50)
my_servo1 = servo.ContinuousServo(pwm1)

pwm2 = pwmio.PWMOut(board.A4, frequency=50)
my_servo2 = servo.ContinuousServo(pwm2)


light=AnalogIn(board.A8)

slider = AnalogIn(board.A2)

switch = DigitalInOut(board.D5)

led = DigitalInOut(board.D9)
led.direction=Direction.OUTPUT

def check_button():
	print("button pressed")
	enter=time.monotonic()
	while(switch.value==True):
	    pass
	now=time.monotonic()
	print(enter,now)
	return (now-enter)

def blinker(time_on, time_off, count=1):
	for i in range(count):
		led.value=True
		time.sleep(time_on)
		led.value=False
		time.sleep(time_off)
	led.value=True

blinker(0.2,0.2,3) #indicate start of program

w=wifi.ESPNEW(115200)
print("connecting to the internet")
w.connect("virus","smoothie123")



x=0  # x
y=0   # y

sx=0    #sum of x
sy=0    #sum of y

sxsq=0  #sum of square of x
sxy =0  #sum of product of x and y

m=0     #slope
b=0     #intercept

N=0 # number of training data
old_y=0
#flags
training_state=True
running_state=False
first_run=True

led.value=True

w.POSTTWstate(b'Training')
while True:

	y = (2*slider.value/65520)-1
	x=light.value/100
	my_servo1.throttle=y
	my_servo2.throttle=-1*y


	if (switch.value):
		press_time=check_button()					
		if(press_time<2): #Training mode
			blinker(0.2,0.1,2)
			x=light.value/100
			
			sx=sx+x
			sy=sy+y
			N=N+1
			
			sxy=sxy + x*y
			sxsq= sxsq + x * x
			print(x,y,"brightness and speed")
			print("N",N,"sx",sx,"sy",sy,"sxsq",sxsq)
			print("sleeping")
			time.sleep(0.01)

		elif(press_time>2): 
			blinker(0.1,0.1,4)	#Running mode


			m=(N *sxy - sx*sy)/(N*sxsq -sx*sx) # calculate the slope
			b=(sy-m*sx)/N                       #calculate the intercept
			print(m,b)

			while(True):
				if(first_run==True):
					if(running_state==False ):
						w.POSTTWstate(b'RUNNING MODE- TESTING')
						first_run=False
					else:
						w.POSTTWstate(b'RUNNING MODE')
						first_run=False


				if (switch.value):
					press_time=check_button()
					if(press_time<2):
						w.POSTTWstate(b'REJECTED. Train again!! ')
						first_run=True
						running_state=False
						#reset all values

						sx=0    #sum of x
						sy=0    #sum of y

						sxsq=0  #sum of square of x
						sxy =0  #sum of product of x and y

						N=0 # number of training data
						old_y=0

						break
					else: 
						w.POSTTWstate(b'READY TO RUN')
						my_servo1.throttle=0
						my_servo2.throttle=0
						while(switch.value!=True):
							blinker(0.1,0.4)
						first_run=True
						running_state=True

				else:
					x=light.value/100
					y= x * m + b
					if(abs(y)<1):
						my_servo1.throttle=y
						my_servo2.throttle=-1*y
					time.sleep(0.1)
			print("nowstopping running")
		else:
			print("that is impossible")

	try:
		w.POSTTW(str(y),str(x))
		
	except:
		print("error with POSTING to TW")
		blinker(0.1,0.1,5)


