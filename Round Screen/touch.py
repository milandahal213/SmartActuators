
'''
Translated by: Milan Dahal

Translated from circuipython library for CST816

Original copyright message:
    # SPDX-FileCopyrightText: Copyright (c) 2023 NeoStormer
    #
    # SPDX-License-Identifier: MIT
    """
    cst816
    ================================================================================

    CircuitPython driver for the CST816 capacitive touch screen IC

    * Author(s): NeoStormer
'''

import time
from machine import Pin, SoftI2C

# Constants
_CST816_ADDR = 46

# Register Addresses
_CST816_GestureID = 0x01
_CST816_FingerNum = 0x02
_CST816_XposH = 0x03
_CST816_XposL = 0x04
_CST816_YposH = 0x05
_CST816_YposL = 0x06

_CST816_ChipID = 0xA7
_CST816_DisAutoSleep = 0xFE
_CST816_IrqCtl = 0xFA
_CST816_MotionMask = 0xAA
_CST816_FwVersion = 0xA9

_CST816_Point_Mode = 1
_CST816_Gesture_Mode = 2

class CST816:
    """Driver for the CST816 Touchscreen connected over I2C."""

    def __init__(self, scl, sda):
        self.i2c = SoftI2C(scl=Pin(scl), sda=Pin(sda))
        self.prev_x = 0
        self.buf=bytearray(10)
        self.prev_y = 0
        self.prev_touch = False
        self.x_point = 0
        self.y_point = 0
        self.x_dist = 0
        self.y_dist = 0
        self.mode = 0
        self.screen_rotation = 1
    
    def _i2c_scan(self):
        print(self.i2c.scan())
        
    def _i2c_write(self, reg, value):
        """Write to a register over I2C."""
        self.i2c.writeto_mem(_CST816_ADDR, reg, bytes([value]))

    def _i2c_read(self, reg):
        """Read a byte from a register over I2C."""
        return self.i2c.readfrom_mem(_CST816_ADDR, reg, 1)[0]

    def who_am_i(self):
        """Check the Chip ID."""
        return self._i2c_read(_CST816_ChipID) == 0xB5

    def reset(self):
        """Reset the chip."""
        self._i2c_write(_CST816_DisAutoSleep, 0x00)
        time.sleep(0.1)
        self._i2c_write(_CST816_DisAutoSleep, 0x01)
        time.sleep(0.1)

    def read_revision(self):
        """Read firmware version."""
        return self._i2c_read(_CST816_FwVersion)

    def wake_up(self):
        """Wake up the chip."""
        self._i2c_write(_CST816_DisAutoSleep, 0x00)
        time.sleep(0.01)
        self._i2c_write(_CST816_DisAutoSleep, 0x01)
        time.sleep(0.05)

    def stop_sleep(self):
        """Prevent the chip from sleeping."""
        self._i2c_write(_CST816_DisAutoSleep, 0x01)

    def set_mode(self, mode):
        """Set the operation mode."""
        if mode == _CST816_Point_Mode:
            self._i2c_write(_CST816_IrqCtl, 0x41)
        elif mode == _CST816_Gesture_Mode:
            self._i2c_write(_CST816_IrqCtl, 0x11)
            self._i2c_write(_CST816_MotionMask, 0x01)
        else:
            self._i2c_write(_CST816_IrqCtl, 0x71)
        self.mode = mode

    def get_point(self):
        """Get the pointer position."""
        #self._i2c_write(0xFA, 0x40)
        temp = self.i2c.readfrom_mem_into(_CST816_ADDR, 0x01, self.buf)
        gesture = self.buf[0]
        if gesture == 1:
            
            points = self.buf[1]
            event = self.buf[2] >> 6

            #print(self.buf[2],self.buf[3],self.buf[4],self.buf[5])

            self.x_point = self.buf[2]#+#self.buf[3]>>8
            self.y_point = self.buf[4]#+self.buf[5]>>8
            #print(gesture, points, event, x, y)
            
  
            return self.x_point, self.y_point
        else:
            return 0,0

    def get_gesture(self):
        """Get the gesture made by the user."""
        return self._i2c_read(_CST816_GestureID)

    def get_touch(self):
        """Detect if the screen is being touched."""
        return self._i2c_read(_CST816_FingerNum) > 0

    def get_distance(self):
        """Get the distance moved between readings."""
        touch_data = self.get_point()
        x, y = touch_data
        if not self.prev_touch and self.get_touch():
            self.x_dist = 0
            self.y_dist = 0
        else:
            self.x_dist = x - self.prev_x
            self.y_dist = y - self.prev_y
        self.prev_touch = self.get_touch()
        self.prev_x = x
        self.prev_y = y
        return self.x_dist, self.y_dist

'''
scl=Pin(7)
sda = Pin(6)
pint = Pin(20, Pin.IN)  # Touch interrupt
a= CST816(scl, sda)

def isr(p):
    #a.set_mode(1)
    a.get_point()
    #print(a.get_point())
pint.irq(isr, trigger=Pin.IRQ_RISING)
'''