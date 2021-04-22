#include<Servo.h>

int servoPin =4; 
int sensorPin = A0;
int buzzerPin= 5;
int lightPin = A6;
const int button = 6;

Servo Servo1; 
float Rspeed = 0; 
int brightness = 0;
int buttonState = 0;

int turn_angle =0; 
int count=0;
int mini=0;
int pos=0;
int dist=0;

int training[10][2];

void setup() {
    Serial.begin(9600);
    Servo1.attach(servoPin); 
    pinMode(button, INPUT);

 
}

int conv(float ang){
  float angle=ang *180/1023;
  return angle;
}

void loop() {

  Rspeed = analogRead(sensorPin); //rotary encoder value
  turn_angle=conv(Rspeed);        //calculating angle for servo
  Servo1.write(turn_angle);       //turning the servo
  buttonState=digitalRead(button);  //reading button state
  if (buttonState){                 //when button pressed
    if(count<10){                   //if training data is less than 10

      brightness = analogRead(lightPin);  //read light sensor value
      training[count][0]=turn_angle;      //save angle to array
      training[count][1]=brightness;      //save brightness to array
        Serial.print(count);
        Serial.print("\n");
        Serial.print(turn_angle);
        Serial.print("\n");
        Serial.print(brightness);
        Serial.print("\n");
        analogWrite(buzzerPin, 200);
      count++;
      delay(100);
      analogWrite(buzzerPin, 0);
      while(digitalRead(button));
      delay(500);
    }
    else{             //when there are already 10 training data
      analogWrite(buzzerPin, 200);
      delay(500);
      analogWrite(buzzerPin, 0);
      delay(100);
      analogWrite(buzzerPin, 200);
      delay(500);
      analogWrite(buzzerPin, 0);
      Serial.print("in run mode");
      for(int i=0;i<10;i++){
        Serial.print(i);
        Serial.print("\n");
        Serial.print(training[i][0]);
        Serial.print("\n");
        Serial.print(training[i][1]);
        Serial.print("\n");
      }
      
      while(!digitalRead(button)){  //pressing the button will escape the run mode
        brightness = analogRead(lightPin);
        mini=1000;
        pos=0;
        for (int i=0;i<10;i++){
          dist=abs(brightness - training[i][1]);
          if (dist<mini){
            mini=dist;
            pos=training[i][0];
            
          }
        }
       Servo1.write(pos);
       delay(100);
      }
      count=0;
    }
  }
}
