import time
import busio
import board

class ESPNEW:
    def __init__(self, baud=115200):
        self.s=busio.UART(board.TX, board.RX, baudrate=baud)
        return(self.s.read())
        time.sleep(0.1)

    def buildJSON(self, func, data):
        toSend=b'${"f":"'+func+b'","d":['
        for d in data:
            toSend =toSend+b'"'+d +b'",'
        toSend=toSend+b']}&'
        print(toSend)
        return toSend

    def storeAPI(self,api, URL):
        toSend=b'dongle.storeAPISecret("'+api+b'","'+ URL + b'")\r\n'
        self.send(toSend)
        ret=self.clean_return()
        return(ret)

    def storeWifi(self,ssid,password):
        toSend=b'dongle.storeWIFISecret("'+ssid+b'","'+ password + b'")\r\n'
        self.send(toSend)
        ret=self.clean_return()
        return(ret)

    
    def getTW(self):
        self.send()
        return(self.s.read())

    def putTW(self, thing, prop, value):
        toSend=b'dongle.PutTW("'+thing+b'","'+ prop +b'","'+ value +b'")\r\n'
        self.send(toSend)
        ret=self.clean_return()
        return(ret)

    def setWifi(self):
        toSend=b'dongle.setWiFi()\r\n'
        self.send(toSend)
        ret=self.clean_return()
        return(ret)

    def setTW(self):
        toSend=b'dongle.setTW()\r\n'
        self.send(toSend)
        ret=self.clean_return()
        return(ret)

    def getMAC(self):
        toSend=b'dongle.getMAC()\r\n'
        self.send(toSend)
        ret=self.clean_return()
        return(ret)

    def Reset(self):
        toSend=b'import dongle\r\n'
        self.send(toSend)
        return(self.s.read(self.s.in_waiting))

    def read(self):
        return(self.s.read())

    def send(self,body):
        while (len(body)>15):
            self.s.write(body[:15])
            body=body[15:]
            time.sleep(0.1)
        self.s.write(body) 
        time.sleep(1)
        #ret=self.clean_return()
        #return(ret)


    def clean_return(self):
        
        while not (self.s.in_waiting) :
            pass
            time.sleep(0.1)
        ret=self.s.read()
        now= time.monotonic()
        start=time.monotonic()
        while ((now-start)<2 and not ('!!' in ret)):
            now= time.monotonic()
            if(self.s.in_waiting) :
                ret = ret + self.s.read()
            time.sleep(0.1)
        ret=str(ret)
        print(ret)
        ret = ret[ret.index("@@") + 6 : ret.index("!!") - 4]
        if (ret is None):
            return (0)
        else :
            return(ret)
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

#Key = "pp92f871d8-c3bd-4fd7-8fab-ee50e1fbd0e5"
#urlBase = "https://-2008281301ci.portal.ptc.io:8443/Thingworx/"