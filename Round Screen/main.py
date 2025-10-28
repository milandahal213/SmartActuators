
import random
from machine import Pin, SPI
import gc9a01 as gc9a01
from touch import CST816
import RTC
import time
import json


#ESPNow protocol
import network
import espnow

# A WLAN interface must be active to send()/recv()
sta = network.WLAN(network.STA_IF)
sta.active(True)
sta.disconnect()   # Because ESP8266 auto-connects to last Access Point

e = espnow.ESPNow()
e.active(True)

'''
while not sta.isconnected():
    sta.connect("virus","lilypad123")
    time.sleep(0.5)
print(sta.isconnected())
time = urequests.get("http://worldtimeapi.org/api/timezone/America/New_York")
print(time.text)

LCD_CS
CS
LCD_DC
SDA)    Touch Screen IIC
SCL)    Touch Screen IIC
XIAO_BL D6
OUCH_INT D7
SCK
MISO
MOSI
'''

#ESP32C6
D0 = 0
D1 = 1 #LCD_CS
D2 = 2
D3 = 21 #LCD_DC
D4 = 22 #(SDA)    Touch Screen IIC
D5 = 23 #(SCL)    Touch Screen IIC
D6 = 16 #XIAO_BL D6
D7 = 17 #TOUCH_INT D7
D8 = 19 #SCK
D9 = 20 #MISO
D10 = 18 #MOSI

'''
#ESP32C3
D0 = 2
D1 = 3
D2 = 4
D3 = 5
D4 = 6
D5 = 7
D6 = 21
D7 = 20
D8 = 8
D9 = 9
D10 = 10
'''

#reset=Pin(26, Pin.OUT),
backlight=Pin(D6, Pin.OUT),
scl=Pin(D5)
sda = Pin(D4)
pint = Pin(D7, Pin.IN)  # Touch interrupt
touch= CST816(scl, sda)

rtc = RTC.PCF8563(scl,sda)


x= 0
y =0

width = 240
height = 240


def isr(p):
    global x,y
    touch.set_mode(2)
    #touch.get_point()
    x,y = touch.get_point()


pint.irq(isr, trigger=Pin.IRQ_RISING)

'''
!3 - D1    LCD_CS
4- D2    CS
!5- D3    LCD_DC
!6- D4 (SDA)    Touch Screen IIC
!7- D5 (SCL)    Touch Screen IIC
!21- #define XIAO_BL D6
!20- #define TOUCH_INT D7
8- D8    SCK
9 - D9    MISO
10 - D10    MOSI
'''

spi = SPI(1, baudrate=60000000, sck=Pin(D8), mosi=Pin(D10))
tft = gc9a01.GC9A01(
    spi,
    dc=Pin(D3, Pin.OUT),
    cs=Pin(D1, Pin.OUT),
    #reset=Pin(26, Pin.OUT),
    backlight=Pin(21, Pin.OUT),
    rotation=0)
    
tft.fill(gc9a01.GREEN)
        
        
   
def main():
    while True:
        width = 5
        height =  5
        col = x -2
        row = y-2

            
        tft.fill_rect(
            col,
            row,
            width,
            height,
            gc9a01.color565(
                random.getrandbits(8),
                random.getrandbits(8),
                random.getrandbits(8)
            )
        )
        time.sleep(0.01)
main()





