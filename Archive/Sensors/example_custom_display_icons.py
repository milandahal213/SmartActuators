from machine import Pin, SoftI2C
from machine import Timer
import icons
import time

i2c = SoftI2C(scl=Pin(7), sda=Pin(6))

import ledmatrix

matrix=ledmatrix.LEDMATRIX(i2c)

vid = matrix.turn_on_led_flash()

print("Starting LED Matrix Test")

emoji = 0
duration_time = 1000
forever_flag=1 
#switch flags
switch_state_up = False
switch_state_down = False
switch_state_select = False

last_switch_state_up = False
last_switch_state_down = False
last_switch_state_select = False

switched_up = False
switched_down = False
switched_select = False

#nav switches
switch_down = Pin(8, Pin.IN)
switch_select = Pin(9, Pin.IN)
switch_up= Pin(10, Pin.IN)

i2c = SoftI2C(scl = Pin(7), sda = Pin(6))
display = icons.SSD1306_SMART(128, 64, i2c,switch_up)
display.fill(0)

"""
CUSTOM EMOJI TESTING
"""

c = {
    'red': 0x00,
    'orange': 0x12,
    'yellow': 0x18,
    'green': 0x52,
    'cyan': 0x7f,
    'blue': 0xaa,
    'purple': 0xc3,
    'pink': 0xdc,
    'white': 0xfe,
    'black': 0xff,
}

"""Here is a static icon for a duration for specified duration"""

ladybug_icon = [
    [c['cyan'], c['cyan'], c['red'],   c['red'],   c['red'],   c['red'],   c['cyan'], c['cyan']],
    [c['cyan'], c['red'],   c['white'], c['red'],   c['red'],   c['white'], c['red'],   c['cyan']],
    [c['red'],   c['red'],   c['red'],   c['black'], c['black'], c['red'],   c['red'],   c['red']],
    [c['red'],   c['black'], c['black'], c['red'],   c['red'],   c['black'], c['black'], c['red']],
    [c['red'],   c['black'], c['red'],   c['red'],   c['red'],   c['red'],   c['black'], c['red']],
    [c['red'],   c['red'],   c['black'], c['black'], c['black'], c['black'], c['red'],   c['red']],
    [c['cyan'], c['red'],   c['black'], c['black'], c['black'], c['black'], c['red'],   c['cyan']],
    [c['cyan'], c['cyan'], c['red'],   c['red'],   c['red'],   c['red'],   c['cyan'], c['cyan']],
]
ladybug_icon_flattened = [pixel for row in ladybug_icon for pixel in row]


# Combine frames into one buffer
buffer = ladybug_icon_flattened

# Duration for each frame (e.g., 1000ms)
duration_time = 2000

## NOTE DURATION does not work immediately following matrix.stop_display() or when first initialized
## Either call display_emoji or display_frames briefly first to fix the duration.

matrix.display_emoji(3, duration_time, forever_flag) #a quick emoji to fix the duration
time.sleep_us(50)

# Whether the frames should loop forever (1 for true, 0 for false)
forever_flag = 0 # frame displays once for the set duration

# Number of frames in the buffer (1 for a single icon)
frames_number = 1

# Call display_frames to display the animation
matrix.display_frames(buffer, duration_time, forever_flag, frames_number)

# Stop the display after a while (e.g., 10 seconds)
time.sleep(5)


"""Here is a static icon shown until interupted"""

red_to_orange = [0x00, 0x03, 0x07, 0x0B, 0x0F, 0x13, 0x17, 0x1B]
orange_to_yellow = [0x1F, 0x23, 0x27, 0x2B, 0x2F, 0x33, 0x37, 0x3B]  
yellow_to_green = [0x3F, 0x43, 0x47, 0x4B, 0x4F, 0x53, 0x56, 0x5A]  
green_to_cyan = [0x5E, 0x62, 0x66, 0x6A, 0x6E, 0x72, 0x76, 0x7A]  
cyan_to_blue = [0x7E, 0x82, 0x86, 0x8A, 0x8E, 0x92, 0x96, 0x9A]  
blue_to_purple = [0x9E, 0xA2, 0xA6, 0xA9, 0xAD, 0xB1, 0xB5, 0xB9]  
purple_to_pink = [0xBD, 0xC1, 0xC5, 0xC9, 0xCD, 0xD1, 0xD5, 0xD9]  
pink_to_red = [0xDD, 0xE1, 0xE5, 0xE9, 0xED, 0xF1, 0xF5, 0xF9]  


buffer = (red_to_orange + orange_to_yellow + yellow_to_green + green_to_cyan + 
                cyan_to_blue + blue_to_purple + purple_to_pink + pink_to_red)


# Duration for each frame (e.g., 1000ms)
duration_time = 1000

## NOTE DURATION does not work immediately following matrix.stop_display() or when first initialized
## Either call display_emoji or display_frames briefly first to fix the duration.


# Whether the frames should loop forever (1 for true, 0 for false)
forever_flag = 1 # frames displays repeating for the set duration until interrupted by another command

# Number of frames in the buffer (1 for a single icon)
frames_number = 1

# Call display_frames to display the animation
matrix.display_frames(buffer, duration_time, forever_flag, frames_number)

# Stop the display after a while (e.g., 10 seconds)
time.sleep(5)


"""Here is an animation of multiple frames"""
# Example buffer data for frames (5 frames, each 64 bytes)
# Each frame represents an 8x8 grid of pixel data (64 bytes)
frame_1 = [0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF, 0x00] * 8  # Example frame 1
frame_2 = [0x00, 0xFF, 0x81, 0x81, 0x81, 0xFF, 0x00, 0x00] * 8  # Example frame 2
frame_3 = [0x00, 0x00, 0xFF, 0x81, 0xFF, 0x00, 0x00, 0x00] * 8  # Example frame 3
frame_4 = [0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF] * 8  # Example frame 4
frame_5 = [0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF] * 8  # Example frame 5

# Combine frames into one buffer
buffer = frame_1 + frame_2 #+ frame_3 + frame_4 + frame_5

# Duration for each frame (e.g., 1000ms)
duration_time = 1000

## NOTE DURATION does not work following matrix.stop_display() or when first initialized
## Either call display_emoji or display_frames briefly first to fix the duration.


# Whether the frames should loop forever (1 for true, 0 for false)
forever_flag = 1 # frames displays repeating for the set duration until interrupted by another command

# Number of frames in the buffer (5 in this case)
frames_number = 2

# Call display_frames to display the animation
matrix.display_frames(buffer, duration_time, forever_flag, frames_number)

# Stop the display after a while (e.g., 10 seconds)
time.sleep(5)


matrix.stop_display()
time.sleep(1)



