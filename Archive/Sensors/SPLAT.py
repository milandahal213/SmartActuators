from machine import Pin
import neopixel
import time
import machine

v = machine.Pin(7)
pwmV = machine.PWM(v)
pwmV.freq(1000)
pwmV.duty(512)
# Define the number of LEDs
NUM_LEDS = 14

# Set up the pin and neopixel object
# Use the appropriate pin for the data line (e.g., Pin 4 in this example)
pin = Pin(6, Pin.OUT)
strip = neopixel.NeoPixel(pin, NUM_LEDS)

# Function to set all LEDs to a specific color
def set_color(r, g, b):
    for i in range(NUM_LEDS):
        strip[i] = (r, g, b)
    strip.write()

# Function to turn off all LEDs
def clear_strip():
    set_color(0, 0, 0)

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
