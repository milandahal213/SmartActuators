import utime
from umqtt.simple import MQTTClient
import ubinascii
import machine
import network
import esp
import uos
import gc
import ujson

esp.osdebug(None)
gc.collect()

# Define uart and turn the REPL on
uart = machine.UART(0, 115200, timeout = 50)
uos.dupterm(uart, 1)

WIFI_CONFIG = {
    # Configuration Details for the Edge Server
    "SSID" : b'Tufts_Wireless',
    "PASS" : ""
}

MQTT_CONFIG = {
    # Configuration details of the MQTT Broker
    "MQTT_BROKER" : b'10.245.81.184',
    "USER" : "",
    "PASS" : "",
    "PORT" : 1883,
    "PUB_TOPIC1" : b'motors/register',
    "PUB_TOPIC2" : b'motors/'+ubinascii.hexlify(machine.unique_id()),
    "SUB_TOPIC1" : b'motors/list',
    "SUB_TOPIC2" : b'',
    "CLIENT_ID" : ubinascii.hexlify(machine.unique_id())
}

station = network.WLAN(network.STA_IF)

station.active(True)
station.connect(WIFI_CONFIG["SSID"], WIFI_CONFIG["PASS"])

connect_counter = 0

while station.isconnected() == False:
    if connect_counter > 10:
        connect_counter = 0
        print("Trying Again")
        station.active(True)
        station.connect(WIFI_CONFIG["SSID"], WIFI_CONFIG["PASS"])
    utime.sleep(1)
    connect_counter += 1

print('WiFi Connection successful:', end=" ")
print(station.ifconfig())

import random
import ujson as json

sub_topics = []
pub_topics = []
messages = []

global client
client = MQTTClient(client_id = "Milan", server =b'10.245.81.184', port = 1883)
client.connect()

def sub_cb(topic, msg):
    print(msg)
    if topic == MQTT_CONFIG["SUB_TOPIC1"] :
        msgStr = msg.decode('utf-8')
        write(msgStr)
        #print("MsgSTr", msgStr)
     
    elif topic == MQTT_CONFIG["SUB_TOPIC2"] : 
        pass

def connect_and_subscribe():
    client = MQTTClient(client_id = "Milan", server =b'10.245.81.184', port = 1883)
    client.set_callback(sub_cb)
    client.connect()
    print('Connected to %s MQTT broker' % (MQTT_CONFIG["MQTT_BROKER"]), end=" ")
    subscribe(client)
    setup_publish(client)
    return client

def subscribe(client):
    for key in MQTT_CONFIG:
        if key.startswith("SUB") and str(type(MQTT_CONFIG[key])) == "<class 'bytes'>" and MQTT_CONFIG[key] != b'':
            client.subscribe(MQTT_CONFIG[key])
            print('subscribed to %s topic' % (MQTT_CONFIG[key]), end = ", ")
            sub_topics.append(MQTT_CONFIG[key])
    print()

def restart_and_reconnect():
    print('Failed to connect to MQTT broker. Reconnecting...')
    utime.sleep(10)
    #machine.reset()

def read ():
    response = None

    while (response == None):
        response = uart.readline()

    return response

def write(msg):
    uart.write(msg)

def setup_publish(client):
    for key in MQTT_CONFIG:
        if key.startswith("PUB") and str(type(MQTT_CONFIG[key])) == "<class 'bytes'>" and MQTT_CONFIG[key] != b'':
            pub_topics.append(MQTT_CONFIG[key])
 
    #publish_all(client, "Connected to %s" % MQTT_CONFIG['CLIENT_ID'])

def publish_all(client, msg):
    for topic in pub_topics:
        client.publish(topic, msg)

def main():
    try:
        client = connect_and_subscribe()
    except OSError as e:
        restart_and_reconnect()

    uos.dupterm(None, 1) # Turn the REPL off to be able to read a message into a variable

    while True:
        
        dataBytes = read()
        write(">>>")
        if (dataBytes == b'\x03\r\n' or dataBytes == b'\x03'):
           uos.dupterm(uart, 1) # Turn the REPL back on
           print("got a ctrl+c")
           break
        if dataBytes != b'':
            client.publish(MQTT_CONFIG["PUB_TOPIC1"], dataBytes)
        client.check_msg()
        utime.sleep(0.1)

#main()
client = connect_and_subscribe()
reg_json={'id':12,'name':'Milan'}
reg_bytes=ujson.dumps(reg_json)#.encode('utf-8')
client.publish(MQTT_CONFIG["PUB_TOPIC1"], reg_bytes)

while True:
    client.publish(MQTT_CONFIG["SUB_TOPIC1"], b'{"Testing here":1}')
    utime.sleep(1)
#retain flag 