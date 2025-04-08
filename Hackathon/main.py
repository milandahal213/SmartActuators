
from machine import Pin, SoftI2C, PWM, ADC
from files import *
import time
import machine
import prefs



#nav switches
switch_down = Pin(8, Pin.IN)
switch_select = Pin(9, Pin.IN)
switch_up= Pin(10, Pin.IN)


def setmode():
    #if up and down buttons are pressed on start up toggle the mode
    mode = prefs.mode
    if not switch_down.value() and not switch_up.value() and switch_select.value():
        import icons
        i2c = SoftI2C(scl = Pin(7), sda = Pin(6))
        display = icons.SSD1306_SMART(128, 64, i2c,switch_up)
        if mode == 'blemode':
            display.showmessage("Web Connect")
            mode = 'webconnect'
        elif mode == 'webconnect':
            display.showmessage("Standalone")
            mode = 'standalone'
        elif mode == 'standalone':
            display.showmessage("BLE mode")
            mode = 'blemode'
            
        resetprefs(mode)  #resets preference file to False

    
    return mode
        



#detect mode on start up

mode = setmode()
       
print(mode)
if mode == 'blemode':
    import blemode
elif mode == 'webconnect':
    import webconnect
elif mode == 'standalone':
    import standalone

