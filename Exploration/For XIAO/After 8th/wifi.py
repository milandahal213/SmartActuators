import time
import busio
import board

class ESPNEW:
    def __init__(self, baud=9600):
        self.s=busio.UART(board.TX, board.RX, baudrate=baud)
        
        self.s.write(b'import urequests\r\n')
        time.sleep(0.1)
        self.s.write(b'import network\r\n')
        time.sleep(0.1)
        self.s.write(b'sta_if = network.WLAN(network.STA_IF)\r\n')
        time.sleep(0.1)
        self.s.write(b'ap_if = network.WLAN(network.AP_IF)\r\n')
        time.sleep(0.1)


        self.s.write(b'sta_if.active(True)\r\n')
        self.s.write(b' \r\n')

        #self.s.write(b'import ubinascii\r\n')
        #time.sleep(0.1)

        #self.s.write(b'ubinascii.hexlify(network.WLAN().config(\'mac\'),\':\').decode()\r\n')
        #time.sleep(0.1)



        self.clean_return()
        time.sleep(0.1)

    def connect(self, ssid, password):
        self.s.write(b'sta_if.connect("'+ ssid+ b'","'+password+b'") \r\n')
        self.clean_return()

    def isconnected(self):
        self.s.write(b'sta_if.isconnected()\r\n')
        self.clean_return()

    def ifconfig(self):
        self.s.write(b'sta_if.ifconfig()\r\n')
        self.clean_return()

    def prin(self):
        self.s.write(b'print(\"milan\")\r\n')
        time.sleep(0.5)
        i=self.s.in_waiting
        print(self.s.read(i))


    def read(self):
        return(self.s.read())

    def get(self, url, data= None, json= None, headers={}, stream=None):
        body=b'urequests.get(\"' + url + b'\").text'
        self.send(body)
        self.clean_return()

    def putTW(self, Thing, Property, value):
        self.urlValue = self.urlBase + b'Things/' + Thing + b'/Properties/*'
        self.propValue = b'{\"'+ Property + b'\":\"' +value +b'\"}'
        print(self.urlValue)
        print(self.propValue)

        #try:
        body=b'response = urequests.post(\"'+ self.urlValue+b'\",headers='+ self.headers+b',json='+self.propValue+b').text'
        print(body)
        self.send(body)
       # self.clean_return()
          #  print('error with Put')

    def POSTTW(self, speed, bright):
        body=b'response = urequests.post(\"http://10.0.0.146:5000/thing\",json={"speed":"' + speed + b'","bright":"' + bright +b'"})'
        print(body)
        self.send(body)
        self.send(b'response.close()')
        
    def POSTTWstate(self, state):
        body=b'response = urequests.post(\"http://10.0.0.146:5000/thing\",json={"state":"' + state +b'"})'
        print(body)
        self.send(body)
        self.send(b'response.close()')

    def PBright(self, value):
        self.propValue = b'{\"nm\":\"' + value +b'\"}'
        body=b'response = urequests.post(\"http://10.0.0.146:5000/bright\",' + self.propValue + b').text'
        self.send(body)

        
    def put(self, url, data= None, json= None, headers={}, stream=None):
       # self.ret=self.call_function("graphics",3,[x]) #setTextSize
        return 1

    def post(self, url, data= None, json= None, headers={}, stream=None):
       # self.ret=self.call_function("graphics",3,[x]) #setTextSize
        return 2
    def delete(self, url, data= None, json= None, headers={}, stream=None):
        #self.ret=self.call_function("graphics",3,[x]) #setTextSize
        return 3

    def send(self,body):
        while len(body)>40:
            self.s.write(body[:40])
            body=body[40:]
            time.sleep(0.05)
        self.s.write(body)
        time.sleep(0.05)
        self.s.write(b'\r\n')
        time.sleep(0.05)
        #print(self.s.read())

    def clean_return(self):
        print("response")
        print(self.s.read())
        #if(self.s.in_waiting):
         #   raw_ret=self.s.read()
          #  print(raw_ret)
            #print(str(raw_ret).find(">>>"))
            #i=0
           # while str(raw_ret).find(">>>")<0:
            #    if(i>15):
            #        break
             #   raw_ret+=self.s.read(400)
            #    i=i+1
           # print(str(raw_ret)[str(raw_ret).find("\r\n"):str(raw_ret).find("\r\n>>>")])
           # return(str(raw_ret)[str(raw_ret).find("\r\n"):str(raw_ret).find("\r\n>>>")])
         #   time.sleep(0.1)

