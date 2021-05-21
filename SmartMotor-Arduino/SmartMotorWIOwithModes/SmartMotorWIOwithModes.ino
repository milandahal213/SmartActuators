#include <Arduino.h>
#include <U8g2lib.h>
 
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#include "LIS3DHTR.h"
LIS3DHTR<TwoWire> LIS; //IIC
#define WIRE Wire
 
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);  // High speed I2C

#include<Servo.h>

int servoPin =7; 
int LEDPin =3; 
int sensorPin = A0;
int buzzerPin= 5;
int lightPin = A6;
const int button = 6;

Servo Servo1; 
float Rspeed = 0; 
int brightness = 0;
int buttonState = 0;
char C_bright[]="1000";
char C_angle[]="180";
char C_count[]="10";


int turn_angle =0; 
int fade_value=0;
int count=0;
int mini=0;
int pos=0;
int dist=0;

char *Tstatus;
char *Mode="Mode1";

int flipState=0;
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
      analogWrite(buzzerPin, 200);
      delay(timer);
      analogWrite(buzzerPin, 0);
      delay(100);
      }
}

void disp(){
  itoa(turn_angle, C_angle, 10);
  itoa(brightness, C_bright, 10);
  itoa(count, C_count,10);
  u8g2.clearBuffer();                   // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);   // choose a suitable font
  u8g2.drawStr(5,10,Tstatus);    //print the status
  u8g2.drawStr(60,10,"Count: ");
  u8g2.drawStr(98,10,C_count); 
  u8g2.drawStr(110,10,"/10"); 
  u8g2.drawStr(10,30,C_bright);
  u8g2.drawStr(80,30,C_angle);
  u8g2.drawStr(10,50,"Brightness");
  u8g2.drawStr(80,50,"Motor");
  u8g2.sendBuffer();                    // transfer internal memory to the display

  

}

void setup() {
    Serial.begin(9600);
    Servo1.attach(servoPin); 
    pinMode(button, INPUT);
    analogWrite(LEDPin,0);
    
    u8g2.begin(); //set up the screen
    u8g2.setFlipMode(1); //flip the screen for readablility
    u8g2.clearBuffer();
     LIS.begin(WIRE,0x19); //IIC init
     LIS.setFullScaleRange(LIS3DHTR_RANGE_2G);
     LIS.setOutputDataRate(LIS3DHTR_DATARATE_50HZ);
     Tstatus="Training";
}

void loop() {
  if(Mode=="Mode2"){
    fade_value=fade(Rspeed);
    analogWrite(LEDPin,fade_value);
    disp();
  }
  Rspeed = analogRead(sensorPin); //rotary encoder value
  turn_angle=conv(Rspeed);        //calculating angle for servo
  Servo1.write(turn_angle);       //turning the servo
  brightness = analogRead(lightPin);
  buttonState=digitalRead(button);    //reading button state
  if (buttonState){  
    while(digitalRead(button)){
      if(LIS.getAccelerationZ()>0 and flipState==0){  // make sure the board is upright
        flipState=1;
      }
      else if(LIS.getAccelerationZ()<-0.5 and flipState==1){  // and followed by flipped
        flipState=2;
      }
      else if(LIS.getAccelerationZ()>0 and flipState==2){
        if (Mode=="Mode1"){
          Mode="Mode2";
          
        }
        else{
          Mode="Mode1";
          u8g2.clearBuffer();  
          u8g2.sendBuffer(); 
          analogWrite(LEDPin,0);
          }
        flipState=0;
        count=-1;
        buzz(2,300);
        break;                  // to leave as soon as the board is flipped
      }
      
      delay(10);
    }
    flipState=0;
    while(digitalRead(button)){  // to hold while the button is still pressed
      delay(50);
    }
    if(count<0){ // condition for when they come out of different modes
      count=0;  
    }
    else if(count>=0 && count<10){                   //if training data is less than 10
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
        brightness = analogRead(lightPin);
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
        if(Mode=="Mode2"){
          analogWrite(LEDPin,fade_value); 
          disp();
        }
        delay(100);
      }
      count=0;
      Tstatus="Training";
    }
    delay(50);
  }
}
