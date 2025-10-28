
import time

from Tufts_ble import Sniff, Yell

def car():
    c = Sniff('!', verbose = False)
    c.scan(0)   # 0ms = scans forever 
    for i in range(100):
        if c.last:
            steering = int(c.last[1:])
            print(steering)
        time.sleep(0.1)

    time.sleep(1)

def joystick():    
    p = Yell()
    for i in range(100):
        angle = i
        p.advertise(f'!{angle}')
        time.sleep(0.1)
    p.stop_advertising()
    
