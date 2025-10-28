#for SPLAT

from machine import  Pin, SoftI2C, PWM, ADC
import neopixel
import machine
from networking import Networking
import time
from config import configname

# Initiate display
i2c = SoftI2C(scl = Pin(7), sda = Pin(6))


#Initialise network
networking = Networking(True, False) #First bool is for network info messages, second for network debug messages
broadcast_mac = b'\xff\xff\xff\xff\xff\xff'
networking.aen.add_peer(broadcast_mac, "All")
networking.name = "MyName"#Set your own device name (this will overwrite the default name gotten from the config.py)


#Defining buttons
sw1 = Pin(2, Pin.IN, Pin.PULL_UP)
sw2 = Pin(3, Pin.IN, Pin.PULL_UP)
sw3 = Pin(4, Pin.IN, Pin.PULL_UP)
sw4 = Pin(5, Pin.IN, Pin.PULL_UP)

# Define the  LEDs
LED = Pin(6, Pin.OUT)
NUM_LEDS = 14
strip = neopixel.NeoPixel(LED, NUM_LEDS)


#Defining power pin
power_PWM = machine.Pin(7)
pwmV = machine.PWM(power_PWM)
pwmV.freq(1000)
pwmV.duty(1000)


#calibrating for stuck buttons
def read_buttons():
    return sw1.value() | sw2.value()<<1 | sw3.value()<<2 | sw3.value()<<3


calib_vals = read_buttons()


#Variables
hindex = 0
last_press_time = time.ticks_ms()


def is_cooldown(cooldown):
    global last_press_time
    print(f"Cooldown: {time.ticks_ms()-last_press_time}")
    return (time.ticks_ms() - last_press_time) < cooldown #prevent the scan function from being spammed

#Send function
def send(pin):
    global last_press_time
    if(time.ticks_ms()-last_press_time>500):
        last_press_time = time.ticks_ms()
        message = "I have spoken"
        #print("hindex:",hindex, "display_list:",display_list)
        networking.aen.send(broadcast_mac, message)
        



# Function to set all LEDs to a specific color
def set_color(r, g, b):
    for i in range(NUM_LEDS):
        strip[i] = (r, g, b)
    strip.write()

# Function to turn off all LEDs
def clear_strip():
    set_color(0, 0, 0)
    
    
    
def is_pressed(pin):
    if calib_vals ^ read_buttons():
        set_color(255,0,0)
        print("button pressed")
        send(pin)
        return True
    else:
        clear_strip()
        return False

# Set up interrupt handlers for button presses
sw1.irq(trigger=Pin.IRQ_RISING, handler=is_pressed)
sw2.irq(trigger=Pin.IRQ_RISING, handler=is_pressed)
sw3.irq(trigger=Pin.IRQ_RISING, handler=is_pressed)
sw4.irq(trigger=Pin.IRQ_RISING, handler=is_pressed)

#Receive function that displays any received message
def receive():
    for mac, message, rtime in networking.aen.return_messages(): #You can directly iterate over the function
        print(mac, message, rtime)

#Interrupt handler that calls the receive function once a message has been received
networking.aen.irq(receive)


