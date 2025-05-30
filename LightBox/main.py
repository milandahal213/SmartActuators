
from machine import Pin, Timer, SoftI2C, PWM
import machine
import time
import neopixel
import esp32
from accel import H3LIS331DL
import struct
import config
import math

'''
ESPNOW content
import network
import espnow

# A WLAN interface must be active to send()/recv()
sta = network.WLAN(network.WLAN.IF_STA)
sta.active(True)
sta.disconnect()   # Because ESP8266 auto-connects to last Access Point

e = espnow.ESPNow()
e.active(True)
'''

class BOX():
    def __init__(self):
        #Defining pins
        
        
        #****************************
        # Neopixels
        self.num_leds = 12
        self.np = neopixel.NeoPixel(Pin(20), self.num_leds)
        self.color = config.config['color']
        self.sparkle_buildup() #show animating LED
        #****************************


        #****************************
        # Vibration motor
        self.motor = Pin(21, Pin.OUT)
        #****************************
        
        

        #****************************
        # Button 
        self.button = Pin(0, Pin.IN, Pin.PULL_UP)
        esp32.wake_on_ext1(pins = (self.button,), level = esp32.WAKEUP_ALL_LOW)
        
        #delaying
        a = self.button.value()
        while a == 0:
            a = self.button.value()
            time.sleep(2)
            #keep stalling while the button is pressed
        
        self.button_press_threshold = 2 # threshold for the device to go to sleep in seconds
        self.last_button_value = 1
        self.button_event = False
        self.button_pressed = False
        
        self.time_of_button_press = 0
        self.button_press_duration = 0
        #****************************  
            
        #****************************
        # Buzzer
        self.pwm = PWM(Pin(19, Pin.OUT), freq=1000, duty=1000)  # Pin(5) in PWM mode here
        self.pwm.duty(0)
        #****************************
        


        #****************************
        #setting up accelerometer
        i2c = SoftI2C(scl = Pin(23), sda = Pin(22))
        self.h3lis331dl = H3LIS331DL(i2c)
        self.h3lis331dl.select_datarate()
        self.h3lis331dl.select_data_config()
        #****************************
    



        tim0 = Timer(0)
        tim0.init(period=200, mode=Timer.PERIODIC, callback=self.check_switch) #timer to check button press
        
        
        #tim1 = Timer(1)
        #potentially use this to broadcast the message with BLE
        #tim1.init(period=1000, mode=Timer.PERIODIC, callback=self.blinkLED) #blink the LED with new color
        

    def change_sleep_threshold(self, new_threshold):
        self.button_press_threshold = new_threshold
        
        
    def readAccel(self):
        accel = self.h3lis331dl.read_accl()
        return accel
    
    def play_note(self, frequency = 400 , hold = 1):
        self.pwm.duty(400)
        self.pwm.freq(int(frequency))
        time.sleep(hold)
        self.pwm.duty(0)
        
        
    def is_pressed(self):
        return self.button_pressed
    
    
    def pressed_duration(self):
        return self.button_press_duration
    
    def check_switch(self,p):
        self.button_value = self.button.value()
        if self.button_value != self.last_button_value:
            self.button_event = True
 
        if self.button_event == True:
            
            if self.button.value() == 0:
                self.button_pressed = True
                self.time_of_button_press = time.time()
                self.buzz()
                          
            else:
                self.button_pressed = False
                self.run_time_related_action()
                #machine.deepsleep()
                
                
            self.button_event = False
        self.last_button_value = self.button_value
        
        #to make sure the button is not pressed for too long
        if self.button.value() == 0:
             self.run_time_related_action()
            
    def run_time_related_action(self):
        self.button_press_duration = abs(time.time() - self.time_of_button_press)
        if(self.button_press_duration > self.button_press_threshold):
            self.shuttingdown()
            machine.deepsleep()
        else:
            pass
            #do routine task
            
        
        
    def buzz(self, dur=0.2, num=1):
        for i in range(num):
            self.motor.on()
            time.sleep(dur)
            self.motor.off()
        
    def hsv_to_rgb(self, h, s, v):
        """Convert HSV color to RGB tuple"""
        h = h / 255.0
        s = s / 255.0
        v = v / 255.0
        
        i = int(h * 6)
        f = h * 6 - i
        p = v * (1 - s)
        q = v * (1 - f * s)
        t = v * (1 - (1 - f) * s)
        
        if i % 6 == 0:
            r, g, b = v, t, p
        elif i % 6 == 1:
            r, g, b = q, v, p
        elif i % 6 == 2:
            r, g, b = p, v, t
        elif i % 6 == 3:
            r, g, b = p, q, v
        elif i % 6 == 4:
            r, g, b = t, p, v
        else:
            r, g, b = v, p, q
            
        return (int(r * 255), int(g * 255), int(b * 255))
    
    def turn_off(self):
        """Turn off all LEDs"""
        for i in range(self.num_leds):
            self.np[i] = (0, 0, 0)
        self.np.write()
    def shuttingdown(self):
        for i in range(12):
            self.np[i] = (50,0,0)
            self.np.write()
            time.sleep(0.1)
        self.turn_off()
    def sparkle_buildup(self):
        """Random sparkles that gradually fill the ring"""
        import random
        
        final_colors = []
        for i in range(self.num_leds):
            hue = i * 255 // self.num_leds
            final_colors.append(self.hsv_to_rgb(hue, 255, 255))
        
        lit_leds = [False] * self.num_leds
        
        # Sparkle phase
        for _ in range(40):
            # Add some sparkles
            for _ in range(random.randint(1, 3)):
                led = random.randint(0, self.num_leds - 1)
                if not lit_leds[led]:
                    self.np[led] = final_colors[led]
                    lit_leds[led] = True
            
            # Dim unlit LEDs with occasional flickers
            for i in range(self.num_leds):
                if not lit_leds[i]:
                    if random.random() < 0.3:  # 30% chance of flicker
                        brightness = random.randint(10, 60)
                        hue = i * 255 // self.num_leds
                        self.np[i] = self.hsv_to_rgb(hue, 255, brightness)
                    else:
                        self.np[i] = (0, 0, 0)
            
            self.np.write()
            time.sleep(0.1)
        
        # Ensure all are lit at the end
        for i in range(self.num_leds):
            self.np[i] = final_colors[i]
        self.np.write()    
    def breathing_buildup(self):
        """Breathing effect that builds up in intensity and color"""
        for cycle in range(4):
            max_brightness = min(255, (cycle + 1) * 64)
            color_shift = cycle * 60
            
            for step in range(20):
                # Sine wave for smooth breathing
                brightness = int((math.sin(step * math.pi / 10) + 1) * max_brightness / 2)
                
                for i in range(self.num_leds):
                    hue = (i * 30 + color_shift) % 255
                    color = self.hsv_to_rgb(hue, 255, brightness)
                    self.np[i] = color
                
                self.np.write()
                time.sleep(0.05)
        
        # Final bright state
        for i in range(self.num_leds):
            hue = i * 30 % 255
            self.np[i] = self.hsv_to_rgb(hue, 255, 255)
        self.np.write()
        
    def wave_buildup(self):
        """Color waves that build up intensity"""
        for wave_round in range(3):
            wave_length = 4
            max_brightness = min(255, (wave_round + 1) * 85)
            
            for step in range(self.num_leds * 2):
                for i in range(self.num_leds):
                    # Create wave pattern
                    wave_pos = (step - i) % (self.num_leds * 2)
                    if wave_pos < wave_length:
                        brightness = int(math.sin(wave_pos * math.pi / wave_length) * max_brightness)
                    else:
                        brightness = 0
                    
                    # Add some base color that persists
                    base_brightness = wave_round * 30
                    brightness = max(brightness, base_brightness)
                    
                    hue = (i * 255 // self.num_leds + step * 5) % 255
                    color = self.hsv_to_rgb(hue, 255, brightness)
                    self.np[i] = color
                
                self.np.write()
                time.sleep(0.06)
        
        # Final state
        for i in range(self.num_leds):
            hue = i * 255 // self.num_leds
            self.np[i] = self.hsv_to_rgb(hue, 255, 255)
        self.np.write()    
    def color_wheel_startup(self):
        """Spinning color wheel that expands"""
        for expansion in range(4):
            num_active = min(self.num_leds, (expansion + 1) * 3)
            
            for rotation in range(24):
                for i in range(self.num_leds):
                    if i < num_active:
                        hue = (rotation * 10 + i * 255 // num_active) % 255
                        brightness = 255 if expansion == 3 else (expansion + 1) * 64
                        color = self.hsv_to_rgb(hue, 255, brightness)
                        self.np[i] = color
                    else:
                        self.np[i] = (0, 0, 0)
                
                self.np.write()
                time.sleep(0.04)        
    
    def rainbow_spiral_on(self):
        """Rainbow spiral that builds up to full ring"""
        # Build up the spiral
        for round_num in range(3):
            for i in range(self.num_leds):
                # Create rainbow colors
                hue = (i * 255 // self.num_leds + round_num * 85) % 255
                color = self.hsv_to_rgb(hue, 255, min(255, (round_num + 1) * 85))
                self.np[i] = color
                self.np.write()
                time.sleep(0.08)
        
        # Final rainbow
        for i in range(self.num_leds):
            hue = i * 255 // self.num_leds
            self.np[i] = self.hsv_to_rgb(hue, 255, 255)
        self.np.write()
                
    def turn_off(self):
        for i in range(12):
            self.np[i] = [0,0,0]
        self.np.write()
           
    def gotosleep(self):
        machine.deepsleep()
        
    def bytearray_to_numbers(self, byte_arr):
        # Convert bytearray to string
        str_data = byte_arr.decode('utf-8')
        
        # Remove brackets and split by commas
        str_data = str_data.strip('[]')
        num_strings = str_data.split(',')
        
        # Convert each string to integer
        numbers = [int(num) for num in num_strings]
        
        return numbers

s= BOX()


