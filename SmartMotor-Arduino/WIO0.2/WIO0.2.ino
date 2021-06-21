#include "Servo.h"
#include"TFT_eSPI.h"

#define LCD_BACKLIGHT (72Ul) // Control Pin of LCD
Servo myservo1;


int red=31<<11;
int green =63<<5;
int blue= 31<<0;

// MENU Variables
int selection=0;
int x=0;
int y=30;
int old_y=30;
int selectd=0;


//training variables
int training[10][2];
int inc=1;
int count=0;
int pos = 0;
int brightness = 0;
int i=0;
int dist=0;
int mini=0;
int DCount=0;
String state="Training";
bool state_flag= true ;

char * menuNames[]={"Start Application","How to ..."};

TFT_eSPI tft;
#define LCD_BACKLIGHT (72Ul) // Control Pin of LCD

void setup() {
  // put your setup code here, to run once:

  tft.begin();
  tft.setRotation(3);
  Serial.begin(115200);
  tft.fillScreen(green);
  myservo1.attach(D1);
  pinMode(WIO_5S_UP,INPUT);
  pinMode(WIO_5S_DOWN,INPUT);
    
  pinMode(WIO_5S_LEFT,INPUT);
  pinMode(WIO_5S_RIGHT,INPUT);
  pinMode(WIO_5S_DOWN,INPUT);
  pinMode(WIO_5S_PRESS, INPUT);
  pinMode(WIO_LIGHT , INPUT);
  pinMode(WIO_BUZZER, OUTPUT);
  pinMode(WIO_KEY_A, INPUT);
  pinMode(WIO_KEY_B, INPUT);
  pinMode(WIO_KEY_C, INPUT);
  

  tft.setTextColor(TFT_BLACK, TFT_BLACK);


}

void loop() {
  selectd=menu();
  if( selectd==0){
    smartMotor();
  }
  else if(selectd==1){
    info();
    
  }

  
  // put your main code here, to run repeatedly:

}



int menu(){
  while(digitalRead(WIO_5S_PRESS)){
    if (digitalRead(WIO_5S_UP) == LOW){
      selection=0;
      drawGraphics(0);
      }
    else if(digitalRead(WIO_5S_DOWN) == LOW){
      selection=1;
      drawGraphics(1);
      }


    }
   return selection;  
}



void drawGraphics(int s){
  tft.drawRect(x,y,280,40,green);
  x=20;
  y=10 + 40*s;
  tft.drawRect(x,y,280,40,31);
  for(int i=0;i<2;i++){
  tft.setCursor((320 - tft.textWidth(menuNames[i]))/2, 20+i*40);
  tft.print(menuNames[i]);
   }
}


void info(){
  tft.fillScreen(green);
  while(digitalRead(WIO_KEY_A)){
    tft.drawString("not for a long time",50,50);
}
 tft.fillScreen(green);
}
void buzz(int t){
  for (int i=0;i <t; i++){
      analogWrite(WIO_BUZZER,128);
      delay(200);
      analogWrite(WIO_BUZZER,0);
      delay(200);
  }
}


void reset_wio(){ //resets all the training data to 0 
      DCount=0;
      state="Training";
      state_flag=true;
      myservo1.write(0);
      pos=0;
      i=0;
}

int smartMotor(){
  while(digitalRead(WIO_KEY_A)){
    brightness = analogRead(WIO_LIGHT);
    if(digitalRead(WIO_5S_DOWN) == LOW){
      for (int i=0;i<10;i++){
        Serial.println(training[i][0]);
        Serial.println(training[i][1]);
      }
    }
    if(digitalRead(WIO_5S_LEFT) == LOW) {
      while (digitalRead(WIO_5S_LEFT) == LOW){
            count=count+1;
            pos = pos-inc ;
            myservo1.write(pos);
            pos = max(0, min(pos,180));
            if (count>5){
              inc=min(10,count);
            }
            delay(100);
            Serial.println(pos);
            Serial.println(brightness);
      }
      count=0;
      inc=1;
    }
     else if(digitalRead(WIO_5S_RIGHT) == LOW) {
       while (digitalRead(WIO_5S_RIGHT) == LOW){
            count=count+1;
            pos = pos+inc ;
            myservo1.write(pos);
            pos = max(0, min(pos,180));
            if (count>5){
              inc=min(10,count);
            }
            delay(100);
            Serial.println(pos);
            Serial.println(brightness);
      }
      count=0;
      inc=1;
    }
    if(digitalRead(WIO_5S_PRESS) == LOW){
      if(DCount<10){
        buzz(1);
        training[i][0]=pos;
        training[i][1]=brightness;
        i=i+1;
        DCount+=1;
       }
       else{
        state="Running";
        state_flag=true;
       // wio_display();
        buzz(3);
  
        while(digitalRead(WIO_KEY_A)){  //pressing the button will escape the run mode
          brightness = analogRead(WIO_LIGHT);
          mini=1000;
          pos=0;
          for (int i=0;i<10;i++){
            dist=abs(brightness - training[i][1]);
            if (dist<mini){
              mini=dist;
              pos=training[i][0];
              
            }
          }
         myservo1.write(pos);
         delay(100);
        }
        reset_wio();
       }
      }
  }
}
