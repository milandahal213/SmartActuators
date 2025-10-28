from machine import Pin
import neopixel
import time
import machine


# Define the number of LEDs
NUM_LEDS = 14


sw1 = Pin(21, Pin.IN, Pin.PULL_UP)
sw2 = Pin(7, Pin.IN, Pin.PULL_UP)
sw3 = Pin(6, Pin.IN, Pin.PULL_UP)
sw4 = Pin(5, Pin.IN, Pin.PULL_UP)


LED = Pin(4, Pin.OUT)
strip = neopixel.NeoPixel(LED, NUM_LEDS)

power_PWM = machine.Pin(3)
pwmV = machine.PWM(power_PWM)
pwmV.freq(1000)
pwmV.duty(1000)

# Set up the pin and neopixel object
# Use the appropriate pin for the data line (e.g., Pin 4 in this example)


def calibrate():
    return sw1.value() | sw2.value()<<1 | sw3.value()<<2 | sw4.value()<<3

calib_vals = calibrate()

def read_buttons():
    return sw1.value() | sw2.value()<<1 | sw3.value()<<2 | sw4.value()<<3

def is_pressed():
    if calib_vals ^ read_buttons():
        return True
    else:
        return False

# Function to set all LEDs to a specific color
def set_color(r, g, b):
    for i in range(NUM_LEDS):
        strip[i] = (r, g, b)
    strip.write()

# Function to turn off all LEDs
def clear_strip():
    set_color(0, 0, 0)
    
def animate():
    for i in range(50):
        strip[i%14] = (10, 255, 0)
        strip[(i+1)%14] = (255, 0, 200)
        strip[(i+2)%14] = (255, 0, 0)
        strip.write()
        time.sleep(0.05)
        strip[i%14] = (0, 0, 0)
        strip[(i+1)%14] = (0, 0, 0)
        strip[(i+2)%14] = (0, 0, 0)
        
animate()
            



# Example of a simple animation: Fade between colors
def fade_colors():
    for intensity in range(0, 255, 5):
        set_color(intensity, 0, 0)  # Red
        time.sleep(0.05)
    for intensity in range(255, 0, -5):
        set_color(intensity, 0, 0)  # Red
        time.sleep(0.05)

try:
    while True:
        fade_colors()
        time.sleep(1)
        clear_strip()
        time.sleep(1)

except KeyboardInterrupt:
    clear_strip()  # Turn off LEDs on exit