#include "Servo.h"
#include"TFT_eSPI.h"
#include<arduino.h>

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
int old_pos=0;
int brightness = 0;
int i=0;
int dist=0;
int mini=0;
int DCount=0;
String state="train";
bool state_flag= true ;

//training mode
char  howMany[16];
char howManyNow[16];
char * menuNames[]={"Start Application","Read Information"};



//display motor values
float x_motor=0.0;
float y_motor=0.0;
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

  drawGraphics(0);
}

void loop() {
  selectd=menu();
  if( selectd==0){
    wiodisplay();
    smartMotor();
  }
  else if(selectd==1){
    info();
    
  }

  
  // put your main code here, to run repeatedly:

}



int menu(){
    makeButton(100,140,8);
    makeButton1(100,210,8);
    drawGraphics(0);
    
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
    tft.drawRect(x,y,280,40,TFT_GREEN);
    x=20;
    y=10 + 40*s;
    tft.drawRect(x,y,280,40,31);
    for(int i=0;i<2;i++){
    tft.setTextSize(2);
    tft.setCursor((320 - tft.textWidth(menuNames[i]))/2, 20+i*40);
    tft.print(menuNames[i]);
     }
}

void makeButton(int x,int y,int r){
    tft.drawCircle(x,y,r*4+10 ,TFT_BLUE);
    tft.drawCircle(x,y,r, TFT_BLUE);
    tft.fillTriangle(x,y-r*4,x-r*2,y-r*2,x+r*2,y-r*2, TFT_BLUE);
    tft.fillTriangle(x,y+r*4,x-r*2,y+r*2,x+r*2,y+r*2, TFT_BLUE);
    tft.fillCircle(x,y,r-r/3,TFT_BLUE);
    tft.setTextSize(2);
    tft.drawString("Select", x+r*8,y-r/2);
}

void makeButton1(int x, int y, int r){
    tft.fillCircle(x,y,r,TFT_BLUE);
    tft.drawCircle(x,y,r+r/3, TFT_BLUE);
    tft.drawString("Enter", x+ r*8,y-r/2 );

}

void wiodisplay(){
  tft.fillScreen(green);
  if(state=="run"){
    tft.setTextSize(1);
    tft.fillRect(65,-10,50,25,6000);
    tft.fillRect(150,-10,50,25,6000);
    tft.setTextColor(TFT_BLACK,TFT_BLACK);
    tft.drawString("GRAPH",70,3);
    tft.drawString("EXIT",160,3);
    tft.setTextSize(3);
    
    tft.drawString(" Running Mode", 30,50 );
    tft.drawCircle(160,200,50, TFT_RED);
    tft.fillRect(110,200,100,100, TFT_GREEN);
  }
  else if(state=="train"){
    tft.setTextSize(1);
    tft.fillRect(65,-10,50,25,6000);
    tft.fillRect(150,-10,50,25,6000);
    tft.setTextColor(TFT_BLACK,TFT_BLACK);
    tft.drawString("DELETE",70,3);
    tft.drawString("EXIT",160,3);
    tft.setTextSize(3);
    
    tft.drawString(" Training Mode", 30,50 );
    tft.drawString("________________________",3,15);
    tft.drawRoundRect(80-5,110-5,165,80,20, TFT_WHITE);
    tft.fillRoundRect(80,110,165,80,20, TFT_GREEN);
    displayTraining();
    tft.setTextSize(2);

  }
}
void info(){
  tft.fillScreen(green);
    tft.setTextSize(1);
    tft.fillRect(65,-10,50,25,6000);
    tft.fillRect(150,-10,50,25,6000);
    tft.setTextColor(TFT_BLACK,TFT_BLACK);

    tft.drawString("EXIT",160,3);
        tft.setTextSize(3);
  while(digitalRead(WIO_KEY_A)){
    tft.drawString("Coming soon... ",50,50);
  }
 tft.fillScreen(green);
 selection=0;
}


void buzz(int t){
  for (int i=0;i <t; i++){
      analogWrite(WIO_BUZZER,128);
      delay(200);
      analogWrite(WIO_BUZZER,0);
      delay(200);
  }
}
void displayTraining(){
    tft.setTextSize(5);
    tft.fillRoundRect(80,110,165,80,20, TFT_GREEN);
    itoa(DCount,howMany,10) ;
    strcat(howMany,"/10");
    tft.drawString(howMany,(320 - tft.textWidth(howMany))/2,125); 
    tft.setTextSize(2);
}

void reset_wio(){ //resets all the training data to 0 
      DCount=0;
      state="train";
      state_flag=true;
      myservo1.write(0);
      pos=0;
      i=0;
      tft.fillScreen(green);
}




int showmotor(int pos){
  
  tft.fillCircle(x_motor, y_motor , 10, TFT_BLUE);
  x_motor=160-60*cos((180-pos)*PI/180);
  y_motor=200-60*sin((180-pos)*PI/180);
  tft.fillCircle(x_motor, y_motor , 10, TFT_WHITE);
  
  

  
  
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
    else if(digitalRead(WIO_KEY_B)==LOW){
     while(digitalRead(WIO_KEY_B)==LOW);
     DCount=DCount-1;
     displayTraining();
      
    }
    if(digitalRead(WIO_5S_PRESS) == LOW){
      if(DCount<10){
        buzz(1);
        training[i][0]=pos;
        training[i][1]=brightness;
        i=i+1;
        DCount+=1;
        displayTraining();
       }
       else{
        state="run";
        state_flag=true;
        wiodisplay();
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
         if (pos!=old_pos){
          showmotor(pos);
          old_pos=pos;
          }
          delay(100);
        }
       }
      }
  }
 reset_wio();
}
