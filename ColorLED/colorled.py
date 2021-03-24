from digitalio import DigitalInOut, Direction
import board
import time
cpin=DigitalInOut(board.D1)
cpin.direction=Direction.OUTPUT

lpin=DigitalInOut(board.D2)
lpin.direction=Direction.OUTPUT

def clk():
	cpin.value=False
	time.sleep(0.00002)
	cpin.value=True
	time.sleep(0.00002)


def sendByte(b):
	for i in range(8):
		if(b & 0x80) != 0:
			lpin.value=True
		else:
			lpin.value=False
		clk()

		b=b<<1


def sendColor(red,green,blue):
	prefix=0xC0
	if (blue & 0x80) == 0:     
	#prefix |= B00100000
		prefix |= 0x20
	if (blue & 0x40) == 0:     
	#prefix |= B00010000
		prefix |= 0x10
	if (green & 0x80) == 0:    
	#prefix |= B00001000
		prefix |= 0x08
	if (green & 0x40) == 0:    
	#prefix |= B00000100
		prefix |= 0x04
	if (red & 0x80) == 0:
	#prefix |= B00000010
		prefix |= 0x02
	if (red & 0x40) == 0:
	#prefix |= B00000001
		prefix |= 0x01
	
	sendByte(prefix)
	sendByte(blue)
	sendByte(green)
	sendByte(red)

def sendColorRGB(r,g,b):


	sendByte(0x00)
	sendByte(0x00)
	sendByte(0x00)
	sendByte(0x00)

	sendColor(r,g,b)


	sendByte(0x00)
	sendByte(0x00)
	sendByte(0x00)
	sendByte(0x00)


