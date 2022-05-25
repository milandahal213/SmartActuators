
#include<Servo.h>



Servo Servo1;  
#define echoPin 8 
#define trigPin 9 
#define switchPin 7

int sensorPin = A0;
int motor1pin1 = 2;
int motor1pin2 = 3;
int sensorValue = 0;
int buttonState = 0; 
int builtInLed=13;
int enable= 5;
int half=0;
int diff=0;
int dataCount=0;
int mini=0;
int pos=0;
int _dist=0;
char SMMode='a';
long duration; // variable for the duration of sound wave travel
int distance; // variable for the distance measurement
int readDist=0;
int currentTime=0;
int newTime=0;
int training[30][2];

void setup() {
  // put your setup code here, to run once:
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(builtInLed, OUTPUT);
  pinMode(enable, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  pinMode(switchPin, INPUT); // Sets the switch as an INPUT
  Serial.begin(115200);

}

int dist(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  if (distance>50) {
    return -1;
  }
  else{
    return distance;
  }
}

int conv(float ang){
  float angle=ang *180/1023;
  return angle;
}

void moveServo(int sensorValue){
  int angle= conv(sensorValue);
  Servo1.write(angle);
}


char watchTimer(int currentTime){
  while(digitalRead(switchPin));
  newTime=millis();
  if((newTime- currentTime)<1500){
    return('T');
    }
  else {
    return ('R');
  }
}


void loop() {
  sensorValue = analogRead(sensorPin);
  moveServo(sensorValue); //change here for servo
  if(digitalRead(switchPin)){
    currentTime=millis();
    SMMode=watchTimer(currentTime);
  
  if(SMMode=='T'){
    Serial.println("T");
    Serial.println(dataCount);
    readDist=dist(); //change here for different sensor 
    training[dataCount][0]=sensorValue;
    training[dataCount][1]=readDist;
    digitalWrite(builtInLed,HIGH);
    delay(100);
    digitalWrite(builtInLed,LOW);
    dataCount++;
  }
  else if(SMMode=='R'){

    Serial.println("R");
    Serial.println(dataCount);
    delay(1000);
    while(!digitalRead(switchPin)){
      readDist=dist();
      mini=1000;
      pos=0;
      for (int i=0;i<10;i++){
        _dist=abs(readDist - training[i][1]);
        if (_dist<mini){
          mini=_dist;
          pos=training[i][0]; 
          }
        }
      moveServo(pos); //change here for servo 
      delay(100);
      }
  dataCount=0;
  }
  }
delay(100);
}
