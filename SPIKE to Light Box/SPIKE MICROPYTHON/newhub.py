from bledevice import BLEDevice
import struct
import time

class Hub():
    def __init__(self):
        self.info = None
        self.device = None
        self.myble = BLEDevice()
        self.color_lut = {255: 'NONE', 0: 'BLACK',1:'MAGENTA',2:'PURPLE',3:'BLUE',4:'AZURE',5:'TURQUOISE',6:'GREEN',7:'YELLOW',8:'ORANGE',9:'RED',10:'WHITE'}  

    def callback(self, callback):
        self.myble.callback = callback
        
    def connect(self, Name = 'MOT'):
        self.myble.scan(5000, None, Name)
        while True:
            if self.myble.is_connected():
                self.write([0x00]) # ask for notificationID etc
                break
            time.sleep(0.1)
            
            
    def sayhello(self):
        self.write([100,91])
        
    def ascii_list_to_string(self, ascii_list):
        return "".join(chr(i) for i in ascii_list)

    def parse(self, data):
        value = None
        print(data)
        #reply = self.ascii_list_to_string(data)
        reply = struct.unpack('>fffB', bytes(data))
        return reply
       


h = Hub()
time.sleep(2)
def callback(data):
    data = [d for d in data]
    reply = h.parse(data)
    print(reply)

    
h.callback(callback)
start_feed = True

h.connect('MPY ESP3')

while True:
    if h.myble.is_connected() and start_feed:
        h.feed()
        start_feed = False
    time.sleep(0.1)
    #{'leftStep': 0, 'leftAngle': 0, 'battery': 20, 'rightStep': 0, 'rightAngle': -23}
    angles = h.sayhello()
    
    
    
    #h.motor_speed()
    #h.motor_run()
       