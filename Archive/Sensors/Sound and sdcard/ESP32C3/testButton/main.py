import machine, neopixel
import time

np = neopixel.NeoPixel(machine.Pin(1), 4)
button = machine.Pin(0, machine.Pin.IN)



ledPin= 0
def light_up():
    np[0] = (122, 0, 0) # set to red, full brightness
    np[1] = (122, 0, 0) # set to red, full brightness
    np[2] = (122, 0, 0) # set to red, full brightness
    np[3] = (122, 0, 0) # set to red, full brightness
    np.write()
    
def rotate(a):

    np[a%4-1] = (122, 0, 0) # set to red, full brightness
    np[a%4] = (122, 122, 0) # set to red, full brightness
    np.write()
    print("called")
    
    
def handle_interrupt(pin):
    global ledPin
    ledPin += 1
    rotate(ledPin)
    time.sleep(0.2)

button.irq(trigger=machine.Pin.IRQ_RISING, handler=handle_interrupt)
light_up()

        
