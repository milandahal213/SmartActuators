# SmartActuators

<iframe width="1280" height="720" src="https://www.youtube.com/watch?v=g_jaH1FCP5I
" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>


## Hardware required: <br><br>
1. Seeduino Xiao (https://www.seeedstudio.com/Seeeduino-XIAO-Arduino-Microcontroller-SAMD21-Cortex-M0+-p-4426.html?gclid=Cj0KCQjwo-aCBhC-ARIsAAkNQivw-3OmuSQJH-566glca2OQdqHqBvfcHzRVrzHG_Yquccrh7D62IB8aAj9nEALw_wcB) 
2. Grove Shield	(https://www.seeedstudio.com/Grove-Shield-for-Seeeduino-XIAO-p-4621.html?gclid=Cj0KCQjwo-aCBhC-ARIsAAkNQivtoBqLyFaTQZwPKE6G-gMXh7EEQH9OkHMKbz3C7OIZ6SGZI7mKTqAaArxgEALw_wcB	)					
3. Continuous Servo Motor	(https://www.digikey.com/en/products/detail/pololu-corporation/2820/10450037?utm_adgroup=Motors%2C%20Solenoids%2C%20Driver%20Boards%2FModules&utm_source=google&utm_medium=cpc&utm_campaign=Shopping_DK%2BSupplier_Pololu%20Corporation&utm_term=&utm_content=Motors%2C%20Solenoids%2C%20Driver%20Boards%2FModules&gclid=Cj0KCQjwo-aCBhC-ARIsAAkNQiuDfFNCriwRq_PkGxHseAj_z2Phtwkvb7zJPDJzzMbXLziZmHYxr3AaAqvkEALw_wcB						
4. Grove Button (https://www.seeedstudio.com/Grove-Button-P.html )						
5. Grove - Slide Potentiometer	(https://www.seeedstudio.com/Grove-Slide-Potentiometer.html)					
6. Grove - Light Sensor		(https://www.seeedstudio.com/Grove-Light-Sensor-v1-2-LS06-S-phototransistor.html	)			


## Connection:<br><br>
Connect the Xiao on Grove Sheild and connect the rest as follows. <br>
Motor - "1 2 3v3 GND" port  <br>
Sliding Potentiometer - "7 6 3v3 GND" port<br>
Button - "9 10 3v3 GND"<br>
Light Sensor - "5 4 3v3 GND" port <br>

## Running the code: <br><br>
Copy the entire folder and place it in the Seeeduino Xiao. main.py file should run on startup.

## Using the system:<br><br>
The program starts on Training Mode. Set the motor speed with potentiometer. Shine light or block the light (however you want to train the speed) on the Light Sensor and press the button to record the data. Repeat if you want. Set different speeds for different light intensites. 
<br>
To switch to the Running Mode long press (more than 2 seconds) the button. The motor should run with speed corresponding to the light intensity on light sensor. 
<br><br>
To go back to the Training Mode, press the button once. 
