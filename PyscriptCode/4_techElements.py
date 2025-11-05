from pyscript import document, window, when

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


import Hub 
class Element:
    """You can use this function to read or write to a tech element.
        element.update_rate(1000) will update all sensor readings every 1 sec
        element.value has the latest sensor reading selected by the dropdown menu
    """
    def __init__(self, divName = 'all_things_hub', suffix = '_repl', hub = 2): # hub0 = spike, hub1 = Old TE, hub2 = TE
        self._hub = Hub.Hub_PS(divName, suffix, hub)  
        self.value = -1
        self.hubType = False
        self._hub.final_callback = self._new_data
        self._hub.info_callback = self._information

    async def _information(self, info):
        self.info = info
        try:
            self.hubType = info['GroupID']
            window.console.log(self.hubType)
            if self.hubType == 512:
                def run(port = 1, direction = 2):
                    fmt, ID, val = self._hub.hubInfo.commands.get('motor_run')
                    val['values']['port'] = port
                    val['values']['direction'] = direction & 0x03
                    self._send(fmt, ID, val) 
                
                def myspeed(speed_value = 100, port = 1): 
                    fmt, ID, val = self._hub.hubInfo.commands.get('motor_speed')
                    val['values']['port'] = port
                    val['values']['speed'] = speed_value  
                    self._send(fmt, ID, val) 
                
                def stop(port = 1):  
                    fmt, ID, val = self._hub.hubInfo.commands.get('motor_stop')
                    val['values']['port'] = port
                    self.set_speed(port, 0)
                
                self.run = run
                self.stop = stop
                self.set_speed = myspeed                 
        except Exception as e:
            window.console.log('Error in _information: ',e)

    async def _sendIt(self, fmt, ID, val):
        return await self._hub.send(fmt, ID, val) 
        
    def _send(self, fmt, ID, val):
        loop = asyncio.get_event_loop()
        success = loop.create_task(self._sendIt(fmt, ID, val))

    async def _new_data(self, reply):
        try:
            self.value = self._hub.value
            reply = self._hub.reply
            if 'Motor_1' in reply.keys():
                self.position = reply['Motor_1']['position']
                #window.console.log(self.position)
                self.angle = reply['Motor_1']['angle']
                self.speed = reply['Motor_1']['speed']
                self.battery = reply['hub info']['Battery']
                
            if 'Motor_2' in reply.keys():
                self.position2 = reply['Motor_2']['position']
                self.angle2 = reply['Motor_2']['angle']
                self.speed2 = reply['Motor_2']['speed']
                self.battery2 = reply['hub info']['Battery']
                
            if 'Color' in reply.keys(): #'color', 'reflection', 'red', 'green', 'blue', 'hue', 'saturation', 'value'
                self.color = reply['Color']['color']
                self.reflection = reply['Color']['reflection']
                self.rgb = (reply['Color']['red'], reply['Color']['green'], reply['Color']['blue'])
                self.hsv = (reply['Color']['hue'], reply['Color']['stauration'], reply['Color']['value'])
                self.battery = reply['hub info']['Battery']
                
            if 'Joystick' in reply.keys():  #'leftStep', 'rightStep','leftAngle','rightAngle'
                self.leftStep = reply['Joystick']['leftStep']
                self.rightStep = reply['Joystick']['rightStep']
                self.leftAngle = reply['Joystick']['leftAngle']
                self.rightAngle = reply['Joystick']['rightAngle']
                self.battery = reply['hub info']['Battery']
        except:
            pass

    async def update_rate(self,rate = 20):
        await self._hub.feed_rate(rate)  #millisec - 20 is the fastest

# Wrap the await statements in an async function
async def setup_elements():
    global element1, element2
    
    element1 = Element('hub1', '_1', 2)
    await element1.update_rate(20)
    
    element2 = Element('hub2', '_2', 2)
    await element2.update_rate(20)
    
    try:
        document.getElementById('title_2').innerText = ''
        _e1 = document.getElementById('var_1')
        _e1.value = 'element1'
        _e2 = document.getElementById('var_2')
        _e2.value = 'element2'
    except Exception as e:
        window.console.log('Element access error:', e)

# Create a task to run the async setup
asyncio.create_task(setup_elements())

@when('change','#var_1')
def rename():
    _e1.value = _e1.value.replace(' ','_')
    exec(f"{_e1.value} = element1 ")
@when('change','#var_2')
def rename():
    _e2.value = _e2.value.replace(' ','_')
    exec(f"{_e2.value} = element2 ")



from pyscript.js_modules import code_editor
python_terminal = document.getElementById("python-terminal")
@when('click','#REPLRun')
def typeIt():
    python_terminal.process('\x03')
    mycode = code_editor.editor.getValue()
    lines = mycode.split("\n")
    python_terminal.process('\x05')
    for line in lines:
        for char in line:
            python_terminal.terminal.write(char)
        python_terminal.terminal.write("\x1b[2K\r>>> ")
        python_terminal.process(line.strip())
    python_terminal.process('\x04')