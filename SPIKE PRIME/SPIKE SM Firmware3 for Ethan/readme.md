# Be careful
The steps will remove the contents on your SLOT 0 of your SPIKE Prime. 

1. Update your SPIKE Prime hub to SPIKE3. (make sure your hub light is green)
2. Open the SPIKE Prime on REPL mode (follow instructions here - https://www.instructables.com/MicroPython-on-SPIKE-Prime/ )
3. Copy the contents of multiPairSM.py file.
4. Paste the code on your REPL
5. Reset the SPIKE Prime and now you can run the code from your Slot 0

## Few tips
1. The sensors go to the ports on the left
2. The motors go to the ports on the right
3. Once you have a set of sensor motor pair, press the RIGHT BUTTON to save the robot configuration
4. Now take data with the LEFT Button, for each pair of SmartMotors.
5. Once you are happy with the dataset press the RIGHT Button. If you have other SmartMotor pairs in your configuration take data for those pairs as well.
6. Now your SPIKE Prime is trained

# what is the code doing??
There are two parts to the code. One is saving the file on your SmartMotors and the other one is the actual SmartMotors code. 
1. Saving the file on SmartMotors:
* The code changes directory to programs and creates a folder 00 which is your slot. 
* If your slot already has code in it, it will get deleted. 
* Then it proceeds to write the SmartMotors code to a file named program.py that will get executed on slot 0. 

2. SmartMotors code
* First the code is waiting for you to save a configuration of sensor and robot pairs. 
* You will be able to see different sensor and motor pairings. if you plug in and out the sensors the connections will change based on what is allowed. You can either have one sensor control multiple motors or one sensor control one motor (three pairs) 
* Then the program allows you to save data for each SmartMotors pair
* When you finally train all the SmartMotors pair, the motors will move according to the training. 

Please send any feedback to milan.dahal@tufts.edu
