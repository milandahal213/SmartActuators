from machine import Pin,I2C
from ssd1306 import SSD1306_I2C,framebuf
import utime


i2c_dev = I2C(1,scl=Pin(7),sda=Pin(6),freq=200000)  # set-up for screen
screen = SSD1306_I2C(128, 64, i2c_dev)

# resource: https://www.mfitzp.com/displaying-images-oled-displays/
images=[]
for n in range (1,6):  
    with open('milan_circle%s.pbm' % n, 'rb') as f:
        f.readline() # Magic number
        f.readline() # Creator comment
        f.readline() # Dimensions
        data = bytearray(f.read()   )
    fbuf = framebuf.FrameBuffer(data, 128, 64, framebuf.MONO_HLSB)
    images.append(fbuf)


start = len(images) - 1
reversed_images = [images[i] for i in range(start, -1, -1)]

while True:
    for i in images:
        screen.blit(i, 0, 0)
        screen.show()
        utime.sleep(0.5)
    for i in reversed_images:
        screen.blit(i, 0, 0)
        screen.show()
        utime.sleep(0.5)

