from pyscript import document, window, when

def unstuck(condition):
    if condition and hasattr(window, "unstuck") and window.unstuck == True:
        window.unstuck = False
        raise Exception("program now unstuck")

    return condition
import channel as _ch
import asyncio

mytopic = '/LEGO'
myChannel = _ch.CEEO_Channel("hackathon", "@chrisrogers", "talking-on-a-channel",
                                 divName = 'all_things_channels', suffix='_test', default_topic = mytopic)
document.getElementById('topic_test').innerHTML = mytopic



class Channel:
    """You can use this function to read or write to the channel.
        channel.send('some text') will send a string (or number) on topic '/LEGO'
        channel.msg has the latest message on the '/LEGO' channel
    """
    def __init__(self):
        self.msg = None
        self.value = -1
        myChannel.callback = self._receive 
    
    def _receive(self, message):
        thetopic = document.getElementById('topic_test').value
        topic, msg = myChannel.check(thetopic, message)
        if msg:  
            self.msg = msg
            try:
                number = float(msg)
                self.value = int(number) if number % 1.0 == 0 else number
            except:
                self.value = -1

    async def _sendIt(self, msg):
        thetopic = document.getElementById('topic_test').value
        return await myChannel.post(thetopic, msg)
        
    def send(self, msg):
        loop = asyncio.get_event_loop()
        success = loop.create_task(self._sendIt(msg))

channel = Channel()

import code
code.interact()


