from machine import I2S, Pin
import math
import time
import machine

import os
import ustruct


'''
LRC (Left/Right Clock)  D8
BCLK (Bit Clock) - D9
DIN (Data In) -D10

'''


# I2S Setup Function with Variable Rate
def setup_i2s(sample_rate, bit_depth,BUFFER_SIZE = 1024):
    i2s = I2S(
        0,
        sck=Pin(20),   # Serial Clock BCLK 22
        ws=Pin(18),    # Word Select / LR Clock LRC 23
        sd=Pin(19),    # Serial Data 21
        #sck=Pin(22),   # Serial Clock BCLK 22
        #ws=Pin(23),    # Word Select / LR Clock LRC 23
        #sd=Pin(21),    # Serial Data 21
        mode=I2S.TX,
        bits=bit_depth,
        format=I2S.MONO,
        rate=sample_rate,
        ibuf=BUFFER_SIZE
    )
    return i2s

# Helper function to parse the .wav header and extract audio parameters
def parse_wav_header(file):
    file.seek(0)  # Ensure we're at the start of the file
    header = file.read(44)  # Standard .wav header size
    sample_rate = ustruct.unpack('<I', header[24:28])[0]
    bit_depth = ustruct.unpack('<H', header[34:36])[0]
    return sample_rate, bit_depth



def play_sound(path):
        # Open the .wav file from the SD card
    with open(path, "rb") as wav_file:
        sample_rate, bit_depth = parse_wav_header(wav_file)
        print(sample_rate, bit_depth)
        i2s = setup_i2s(sample_rate, bit_depth)

        # Skip the header and begin reading audio data
        wav_file.seek(44)  # Start of PCM data after 44-byte header

        # Buffer to hold PCM data read from the file
        buffer = bytearray(1024)
        
        while True:
            try:
                num_read = wav_file.readinto(buffer)
                if num_read == 0:
                    break  # End of file
                i2s.write(buffer[:num_read])  # Write to I2S in chunks
            except KeyboardInterrupt:
                print("CTRL C pressed")



from machine import Pin
import neopixel
import time
import machine


# Define the number of LEDs
NUM_LEDS = 14


sw1 = Pin(0, Pin.IN, Pin.PULL_UP)
sw2 = Pin(1, Pin.IN, Pin.PULL_UP)
sw3 = Pin(2, Pin.IN, Pin.PULL_UP)
sw4 = Pin(21, Pin.IN, Pin.PULL_UP)


LED = Pin(22, Pin.OUT)
strip = neopixel.NeoPixel(LED, NUM_LEDS)

power_PWM = machine.Pin(23)
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
        play_sound("hello.wav")
        fade_colors()
        time.sleep(1)
        clear_strip()
        time.sleep(1)

except KeyboardInterrupt:
    clear_strip()  # Turn off LEDs on exit

