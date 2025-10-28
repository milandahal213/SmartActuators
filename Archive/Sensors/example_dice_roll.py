from machine import Pin, SoftI2C
from machine import Timer
import icons
import time

#Turn off smart motor screen
switch_up= Pin(10, Pin.IN)
i2c = SoftI2C(scl = Pin(7), sda = Pin(6))
display = icons.SSD1306_SMART(128, 64, i2c,switch_up)
display.fill(0)



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


roll_time = False

def selectpressed():
    #declare all global variables, include all flags
    global flags
    global triggered
    global roll_time
    time.sleep(0.05)  
    roll_time=True #log file
    
tim = Timer(0)
tim.init(period=50, mode=Timer.PERIODIC, callback=check_switch)

import ledmatrix
import led_emoji as emoji


print("Starting LED Matrix Test")
matrix=ledmatrix.LEDMATRIX(i2c)

matrix.display_emoji(10,500,1)
time.sleep_ms(200)


def roll_die_animation(roll_duration):
    buffer = (emoji.die_1 + emoji.die_2 + emoji.die_3 + emoji.die_4 + emoji.die_5)
    duration_time = 5000
    forever_flag = 1 # frames displays repeating for the set duration until interrupted by another command
    frames_number = 5
    # Call display_frames to display the animation
    matrix.display_frames(buffer, duration_time, forever_flag, frames_number)
    # Stop the display after a while (e.g., 10 seconds)
    time.sleep_ms(roll_duration)
    
def make_roll():
    global roll_time
    roll_die_animation(3000)
    matrix.display_frames(emoji.roll_die(), 1000, 1, 1)
    roll_time = False
    
while True:
    if roll_time:
        make_roll()
    else:
        time.sleep(1)