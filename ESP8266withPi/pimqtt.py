import paho.mqtt.client as mqtt
broker="10.3.141.1"
client=mqtt.Client("C1")
client.publish("Topic","Message")

def on_message(client, userdata, message):
    print("message received " ,str(message.payload.decode("utf-8")))
    print("message topic=",message.topic)
    print("message qos=",message.qos)
    print("message retain flag=",message.retain)


client.on_message=on_message
client.loop_start() 




broker_address="10.3.141.1"

print("creating new instance")
client = mqtt.Client("P1") #create new instance
client.on_message=on_message #attach function to callback
print("connecting to broker")
client.connect(broker_address) #connect to broker
client.loop_start() #start the loop
print("Subscribing to topic","mpos")
client.subscribe("mpos")
print("Publishing message to topic","msens")
client.publish("msens","OFF")
time.sleep(4) # wait
client.loop_stop() #stop the loop


motor/id"


Topic names: