//Original code Olivia Tomassetti

#include <Arduino.h>
#include<Servo.h>


//Define pins connected to Seeeduino xiao
int servoPin = A1; 
int LEDPin = 8; 
int sensorPin = A9;
int buzzerPin= 5;
int lightPin = A2;
const int button = 7;

//Define servo and starting states for pins
Servo Servo1; 
float Rspeed = 0; 
int brightness = 0;
int buttonState = 0;
char C_bright[]="1000";
char C_angle[]="180";
char C_count[]="10";

//Set starting values
int turn_angle =0; 
int fade_value=0;
int count=0;
int mini=0;
int pos=0;
int dist=0;

char *Tstatus;
char *Mode="Mode1";

int training[10][2];

int conv(float ang){
  float angle=ang *180/1023;
  return angle;
}

int fade(float ang){
  float fadeValue=ang *255/1023;
  return fadeValue;
}


void buzz(int count, int timer=500){
      for (int i=0;i<count;i++){
      analogWrite(buzzerPin, 20);
      delay(timer);
      analogWrite(buzzerPin, 0);
      delay(100);
      }
      
}

/*void buzz(int count, int timer=500){
      for (int i=0;i<count;i++){
      digitalWrite(buzzerPin, HIGH);
      delay(500);
      digitalWrite(buzzerPin, LOW);
      delay(100);
      }
}
*/

void setup() {

    Servo1.attach(servoPin); 
    pinMode(button, INPUT);
    pinMode(LEDPin, OUTPUT);

    //analogWrite(LEDPin,0);
}

void loop() {

 //if(Mode=="Mode2"){
    //fade_value=fade(Rspeed);
   // analogWrite(LEDPin,fade_value);
  //}/*
  Rspeed = analogRead(sensorPin); //rotary encoder value
  turn_angle=conv(Rspeed);        //calculating angle for servo
  Servo1.write(turn_angle);       //turning the servo
  brightness = analogRead(lightPin);
  buttonState=digitalRead(button);    //reading button state
  if (buttonState){  

    //Serial.println("Recorded: " + String(brightness));
    while(digitalRead(button)){  // to hold while the button is still pressed  
      delay(50);
    }
    if(count<0){ // condition for when they come out of different modes
      count=0;  
    }
    else if(count>=0 && count<10){        //if training data is less than 10
      training[count][0]=turn_angle;      //save angle to array
      training[count][1]=brightness;      //save brightness to array
      training[count][2]=fade_value;      //save fade value to array  
      buzz(1);  
      count++;
    }
    else{             //when there are already 10 training data
      buzz(2);
      Tstatus="Running";
      while(!digitalRead(button)){  //pressing the button will escape the run mode
        //fade_value=fade(Rspeed);
        //analogWrite(LEDPin,fade_value);
        brightness = analogRead(lightPin);
        digitalWrite(LEDPin, HIGH);
        mini=1000;
        turn_angle=0;
        for (int i=0;i<10;i++){
          dist=abs(brightness - training[i][1]);
          if (dist<mini){
            mini=dist;
            turn_angle=training[i][0];
            fade_value=training[i][2];
          }
        }
        Servo1.write(turn_angle);
        delay(100);
      }
      count=0;
      Tstatus="Training";
      digitalWrite(LEDPin, LOW);
    }
    delay(50);
  }
 
}
