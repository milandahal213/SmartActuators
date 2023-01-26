from machine import Pin, PWM, ADC, I2C
from ssd1306 import SSD1306_I2C
from writer import Writer # from Peter Hinch - See R1
import freesans20  # Font to use
import utime

# set up inputs/outputs
motor = PWM(Pin(3))  # servo signal is in D10 which is Pin 3
motor_position = ADC(28)   # servo analog position value is on A2 which is Pin 28
sensor = ADC(26)  # grove light sensor is plugged into port for A0
beep = PWM(Pin(29))  # Xiao expanion board buzzer is on A3/D3 which is Pin 29
i2c_dev = I2C(1,scl=Pin(7),sda=Pin(6),freq=200000)  # set-up for screen
screen = SSD1306_I2C(128, 64, i2c_dev)
text_out = Writer (screen,freesans20,verbose=False)  # set up text in different font per Peter Hinch's code
grove_button = Pin(1,Pin.IN,Pin.PULL_UP) # grove button is plugged into port for D7 which is Pin 1
save = grove_button.value   # grove button is used for our 'save' in our user interface
exp_button = Pin(27,Pin.IN,Pin.PULL_UP)  # expansion board button is D1 which is Pin 27
done_not_pressed = exp_button.value #NOTE -> done button registers 1 when not pressed; 0 when pressed

# beep_tune function
def beep_tune():
    beep.duty_u16(65535//2)  # on 50% of the time
    beep.freq (262)  # C
    utime.sleep(0.2)
    beep.freq (330) # E
    utime.sleep(0.2)
    beep.freq (262)
    utime.sleep (0.2)
    beep.duty_u16 (0)

# set up _map function similar to arduino "map" scaling function (see reference R2)
def _map(x, in_min, in_max, out_min, out_max):
    return int((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)


# set up training
training = []
beep_tune()

counter = 0
m_pos = [0 for i in range (126)]   # list used in the display of the amount of training per motor position
while done_not_pressed():   # training loop based on Milan's spike code.
    while not save() and done_not_pressed():
        utime.sleep(0.1)  #debounce
    if done_not_pressed():
        counter += 1
        angle = motor_position.read_u16()
        bright = sensor.read_u16()
        # print (bright)
        while save():
            utime.sleep(0.1)
            col = _map (angle,5569,47691,0,125)
            print (col)
            training.append((col,bright))
            m_pos[col] += 1   # add one to the height of the "line" at that column location
            screen.vline(col,0,m_pos[col],1)   # simple thin bar graph showing magnitude of training per motor position
            screen.show()

beep_tune()
screen.fill(0)
screen.show()

while not done_not_pressed():  # done button is pressed
    utime.sleep(0.1) # debounce

K = 10
while done_not_pressed():  # KNN loop -- read sensor, compare with K nearest neighbors, choose output
    bright = sensor.read_u16()
    dist = []   # initialize list to store 'distance' between measurement and training data
    for (a,l) in training:
        dist.append(abs(bright - l))   # calculate distance measuremenent is from training data
  
    sorted_distances = sorted(enumerate(dist),key=lambda i: i[1])  # sort in order of least to greatest distance, keep indecies from original list
    del sorted_distances[K:]  # get top K nearest neighbors 

    position_list = []
    for (i,sd) in sorted_distances:   # create a list of the positions of the nearest neighbors
        position_list.append (training[i][0])   # where the zero'th indecie is the position of the i'th data point in the training where i is the index # from the sorted list of nearest distances

    pos = max (position_list,key=position_list.count)  # we choose the position with the greatest occurances in the nearest neighbor list
    occurrence = {item: position_list.count(item) for item in position_list}  # number of occurances of max
    max_occurence = occurrence.get(pos)
    confidence = max_occurence/len(position_list)
    print ("%.2f" %confidence)
    disp_conf = str(confidence*100)+'%'
    screen.text(disp_conf,55,20)
    screen.show()


    # print('(%d,%d)'%(pos,bright))
    pos_servo = _map(pos,0,125,1000,9000)      
    motor.freq(50)
    motor.duty_u16(pos_servo)   # move servo motor to position
    utime.sleep(0.1)

    for i in range(pos-10,pos+10):
        try:
            print('(%d,%d)'%(i,m_pos[i]))
            screen.vline(i,0,m_pos[i],1)   # simple thin bar graph showing magnitude of training per motor position
        except IndexError:
            print ('err')
            pass
    screen.show()
    utime.sleep(0.4)
    screen.fill(0)
    screen.show()

# Exit
beep_tune()
utime.sleep(0.4)
beep_tune()
motor.deinit()

# References
# R1 --> https://github.com/peterhinch/micropython-font-to-py/blob/master/writer/WRITER.md
# R2 --> https://www.theamplituhedron.com/articles/How-to-replicate-the-Arduino-map-function-in-Python-for-Raspberry-Pi/
