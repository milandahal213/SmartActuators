
from machine import Pin
import time
import json
import struct

       
def start_uart():
    from machine import UART
    uart = UART(1, 9600, tx = Pin(16), rx = Pin(17))
    #uart.init(38400, bits=8, parity=None, stop=1) # init with given parameters
    return uart

uart = start_uart()

motor_id = 0xE0
encoder_value = 0x30
number_of_pulses = 0x33
motor_shaft_angle_error = 0x39
move_mot = 0xF6


def add_chksum(command):
    checksum = sum(command) & 0xFF
    command.append(checksum)
    return command 

def check_chksum(data):
    _data = data[0:-1]
    checksum = sum(_data) & 0xFF
    if(checksum == data[-1]):
        return True
    else:
        return False

def send_motor_command(to_send):
    command = [motor_id]
    for i in to_send:
        command.append(i)

    new_command = add_chksum(command)
    uart.write(bytes(command))
    time.sleep(0.2)
    ret = uart.read(uart.any())
    return(ret)

def move_motor(direction, speed): #0 is CW and 1 is CCW, speed is 7 bit val (0 - 127)
    if speed > 127:
        speed =127
    command = (direction <<7 | speed) & 0xFF
    send_motor_command([move_mot , command])

def read_encoder_value():
    encoder_value = send_motor_command([0x30])
    if check_chksum(encoder_value):
        carry = int.from_bytes(encoder_value[1:5]) 
        value = int.from_bytes(encoder_value[5:7])

        if carry >= 2**31:  # If the value is above the signed 32-bit limit
            carry -= 2**32  # Convert to negative range
    
    return carry,value
def set_microsteps(steps):
    send_motor_command([0x84 , steps])

def calibrate_motor():
    send_motor_command([0x80, 0x00])

def set_work_mode(mode):
    # 1 is open
    # 2 is CR_vFOC
    # 3 is UART
    send_motor_command([0x82, mode])
def set_baudrate(baudrate):
    send_motor_command([0x8A, baudrate])
    
# set_microsteps(32)
print(read_encoder_value())

#send_motor_command([move_motor,0x30])





        


