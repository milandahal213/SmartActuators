from machine import Pin, SoftI2C
from machine import Timer

i2c = SoftI2C(scl=Pin(7), sda=Pin(6))

import ledmatrix
a=ledmatrix.LEDMATRIX(i2c)

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


def check_switch(p):
    global switch_state_up
    global switch_state_down
    global switch_state_select
    
    global switched_up
    global switched_down
    global switched_select
    
    global last_switch_state_up
    global last_switch_state_down
    global last_switch_state_select
    
    switch_state_up = switch_up.value()
    switch_state_down = switch_down.value()
    switch_state_select = switch_select.value()
         
    if switch_state_up != last_switch_state_up:
        switched_up = True
        
    elif switch_state_down != last_switch_state_down:
        switched_down = True
        
    elif switch_state_select != last_switch_state_select:
        switched_select = True
                
    if switched_up:
        if switch_state_up == 0:
            uppressed()
        switched_up = False
    elif switched_down:
        if switch_state_down == 0:
            downpressed()
        switched_down = False
    elif switched_select:
        if switch_state_select == 0:
            selectpressed()
        switched_select = False
    
    last_switch_state_up = switch_state_up
    last_switch_state_down = switch_state_down
    last_switch_state_select = switch_state_select




tim = Timer(0)
tim.init(period=50, mode=Timer.PERIODIC, callback=check_switch)

def uppressed():
    global emoji
    emoji +=1
    print("Up was pressed")
    a.display_emoji(emoji, duration_time, forever_flag)

def downpressed():
    global emoji
    emoji -=1
    print("Down was pressed")
    a.display_emoji(emoji, duration_time, forever_flag)
    
def selectpressed():
    global emoji
    emoji -=1
    print("Select was pressed")
    a.display_emoji(emoji, duration_time, forever_flag)

from machine import I2C, Pin
import time

# Initialize I2C
#i2c = I2C(0, scl=Pin(22), sda=Pin(21))

# Create an LEDMATRIX object


# Example buffer data for frames (5 frames, each 64 bytes)
# Each frame represents an 8x8 grid of pixel data (64 bytes)
frame_1 = [0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF, 0x00] * 8  # Example frame 1
frame_2 = [0x00, 0xFF, 0x81, 0x81, 0x81, 0xFF, 0x00, 0x00] * 8  # Example frame 2
frame_3 = [0x00, 0x00, 0xFF, 0x81, 0xFF, 0x00, 0x00, 0x00] * 8  # Example frame 3
frame_4 = [0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xFF] * 8  # Example frame 4
frame_5 = [0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xFF] * 8  # Example frame 5

# Combine frames into one buffer
buffer = frame_1 + frame_2 + frame_3 + frame_4 + frame_5

# Duration for each frame (e.g., 1000ms)
duration_time = 1000

# Whether the frames should loop forever (1 for true, 0 for false)
forever_flag = 1

# Number of frames in the buffer (5 in this case)
frames_number = 5

# Call display_frames to display the animation
a.display_frames(buffer, duration_time, forever_flag, frames_number)

# Stop the display after a while (e.g., 10 seconds)
time.sleep(10)
a.stop_display()


