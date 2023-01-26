#include "Servo.h"
#include"TFT_eSPI.h"
#include"LIS3DHTR.h"
#include"icons.h"
#include "Encoder.h"

Encoder knob(D1, D0);

LIS3DHTR<TwoWire> lis;
#include<arduino.h>

#define LCD_BACKLIGHT (72Ul) // Control Pin of LCD

Servo myservo2;



//rotary sensor variables


// Linear regression variables
float sx = 0; //sum of x
float sy = 0; //sum of y

float sxsq = 0; //sum of square of x
float sxy = 0; //sum of product of x and y

float m = 0 ; //slope
float b = 0 ; //intercept


//color variables
int red = 31 << 11;
int green = 63 << 5;
int blue = 31 << 0;


int buttonBG = TFT_BLUE; //button color
int menuBG = TFT_PURPLE; //menu color
int infoBG = TFT_DARKGREY;
int trainBG = TFT_GREEN;
int runBG = TFT_ORANGE;


int textColor = TFT_BLACK;
int textColor2 = TFT_WHITE;

int MODE1Color = TFT_YELLOW;
int MODE2Color = TFT_WHITE;
int MODE3Color = TFT_GREEN;


int graphBG = TFT_PURPLE;
int dataBG = TFT_RED;
int dataBGLatest = TFT_DARKGREEN;


// MENU Variables
int selection = 0;
int x = 0;
int y = 30;
int old_y = 30;
int selectd = 0;

int leftButton = 15;
int midButton = 90;
int rightButton = 175;

int h = 50;

int choiceSensor = 3;
int choiceMotor = 1;
int choiceModel = 1;
//running mode

bool graphMode = true;
//training variables
int training[50][2];
int inc = 1;
int count = 0;
int pos = 4;
int motorPos = 80;
int old_pos = -999;
long sensorValue = 0;
long old_sensorValue = 0;

int t = 0;
int i = 0;
int dist = 0;
int mini = 0;
int DCount = 0;
String state = "train";


//training mode

char * menuInformation[] = {"Select Sensor", "Select Motor", "", "Select Training Model"};
char * xaxis[] = {"LIGHT INTENSITY", "ANGLE", "TILT"};
char * yaxis[] = {"MOTOR Position", "MOTOR Direction"};
char * yaxisI1[] = {"0", "<<<--"};
char * yaxisI2[] = {"180", "-->>>"};

//info variables
int pageN = 0;
bool lightsensorFlag = false;
bool potFlag = false;
bool accelFlag = false;

int mCount = 0;

//display motor values
float x_motor = 0.0;
float y_motor = 0.0;
float yRect = 0.0;
float oldyRect = 0.0;



// ICONS



//TFT_eSPI tft;
#define LCD_BACKLIGHT (72Ul) // Control Pin of LCD
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
TFT_eSprite img = TFT_eSprite(&tft);  // Sprite
void setup() {

  // setup screen
  tft.begin();
  tft.setRotation(3);
  Serial.begin(115200);
  tft.fillScreen(red);
  //set up accelerometer
  lis.begin(Wire1);
  if (!lis) {
    Serial.println("ERROR");
    while (1);
  }
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ); //Data output rate
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G); //Scale range set to 2g
  tft.fillScreen(green);

 // myservo2.attach(SDA); //for m5stack servos
  myservo2.attach(SCL); //for Grove Servos


  pinMode(WIO_5S_UP, INPUT);
  pinMode(WIO_5S_DOWN, INPUT);

  pinMode(WIO_5S_LEFT, INPUT);
  pinMode(WIO_5S_RIGHT, INPUT);
  pinMode(WIO_5S_DOWN, INPUT);
  pinMode(WIO_5S_PRESS, INPUT);
  pinMode(WIO_LIGHT , INPUT);
  pinMode(A0,INPUT);
  pinMode(WIO_BUZZER, OUTPUT);
  pinMode(WIO_KEY_A, INPUT);
  pinMode(WIO_KEY_B, INPUT);
  pinMode(WIO_KEY_C, INPUT);

  tft.setTextColor(textColor, textColor);
}

void loop() {
  menu(); // main loop
}



int menu() {

  //menu_shell 1 is sensor select, 2 is motor type selection, 3 is main menu, 4 is training type
  menu_shell(3); //

  while (1) {
    //left most is for training page
    if (digitalRead(WIO_KEY_C) == LOW) {
      buttonPressed(true, leftButton, "A");
      while (digitalRead(WIO_KEY_C) == LOW);
      buttonPressed(false, leftButton, "A");
      smartMotor(choiceSensor);
      break;
    }

    //middle one is for sensor selector page
    if (digitalRead(WIO_KEY_B) == LOW) {
      buttonPressed(true, midButton, "B");
      while (digitalRead(WIO_KEY_B) == LOW);
      buttonPressed(false, midButton, "B");
      sensorSelectorpage();
      break;
    }
    delay(200);
    mainScreenLevel();
    moveMotor();
  }
}




void menu_shell(int menuStage) {
  h = 30;

  tft.fillScreen(menuBG);
  tft.setTextColor(textColor2, textColor2);
  tft.setTextSize(3);

  tft.fillRect(leftButton - 25, 0, 50, h, (buttonBG));
  tft.fillRect(midButton - 25, 0, 50, h, (buttonBG));


  tft.drawString("A", leftButton - tft.textWidth("A") / 2, 3);
  tft.drawString("B", midButton - tft.textWidth("B") / 2, 3);


  tft.fillRect(leftButton, h, 5, h + 20, (buttonBG));
  tft.fillRect(midButton, h, 5, h + 20, (buttonBG));


  tft.fillRect(leftButton, 2 * h + 20, 70 - leftButton, 5, (buttonBG));
  tft.fillRect(midButton, 2 * h + 20, 160 - midButton, 5, (buttonBG));


  tft.fillRect(70, 2 * h + 20, 5, 30, (buttonBG));
  tft.fillRect(160, 2 * h + 20, 5, 40, (buttonBG));


  tft.fillRect(30, 3 * h + 10, 80, h * 3 - 10, (buttonBG));
  tft.fillRect(120, 3 * h + 10, 80, h * 3 - 10, (buttonBG));


  if (menuStage == 1) { //sensor type
    tft.fillRect(rightButton - 25, 0, 50, h, (buttonBG));
    tft.drawString("C", rightButton - tft.textWidth("A") / 2, 3);
    tft.fillRect(rightButton, h, 5, h + 20, (buttonBG));
    tft.fillRect(rightButton, 2 * h + 20, 250 - rightButton, 5, (buttonBG));
    tft.fillRect(250, 2 * h + 20, 5, 50, (buttonBG));
    tft.fillRect(210, 3 * h + 10, 80, h * 3 - 10, (buttonBG));
    tft.drawRect(20, 3 * h, 280, h * 3 + 10, (buttonBG));
    tft.drawXBitmap(38, 3 * h + 20, MOTORLIGHT, 64, 60, MODE1Color);
    tft.drawXBitmap(128, 3 * h + 20, TILT, 64  , 60, MODE2Color);
    tft.drawXBitmap(218, 3 * h + 20, ROTORY, 64, 60, MODE3Color);
    tft.fillRect(250, 3 * h + 80, 5, h + 20, (buttonBG));
  }


  else if (menuStage == 3) { //main screen with sensor selected


    tft.drawXBitmap(38, 3 * h + 20, BIGRUN, 64, 60, MODE1Color);
    tft.drawXBitmap(128, 3 * h + 20, SETTING, 64  , 60, MODE2Color);

    if (choiceSensor == 1) {
      tft.drawXBitmap(220, 10, MOTORLIGHT, 64, 60, MODE1Color);
    }
    else if (choiceSensor == 2) {
      tft.drawXBitmap(220, 10, ROTORY, 64  , 60, MODE2Color);
    }
    else if (choiceSensor == 3) {
      tft.drawXBitmap(220, 10, TILT, 64, 60, MODE3Color);
    }
  }

  else if (menuStage == 4) { //training model
    tft.drawRect(20, 3 * h, 190, h * 3 + 10, (buttonBG));
    tft.drawXBitmap(38, 3 * h + 20, CLUSTER, 64, 60, MODE1Color);
    tft.drawXBitmap(128, 3 * h + 20, LINEAR, 64  , 60, MODE2Color);
  }

  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_WHITE);
  tft.drawString(menuInformation[menuStage - 1], 20, 200);

}


void sensorSelectorpage() {
  choiceSensor = 0;
  menu_shell(1); //1 is sensor selector page

  while (1) {
    if (digitalRead(WIO_KEY_C) == LOW) {
      buttonPressed(true, leftButton, "A");
      while (digitalRead(WIO_KEY_C) == LOW);
      buttonPressed(false, leftButton, "A");
      choiceSensor = 1;
      break;

    }

    if (digitalRead(WIO_KEY_B) == LOW) {
      buttonPressed(true, midButton, "B");
      while (digitalRead(WIO_KEY_B) == LOW);
      buttonPressed(false, midButton, "B");
      choiceSensor = 3;
      break;

    }
    if (digitalRead(WIO_KEY_A) == LOW) {
      buttonPressed(true, rightButton, "C");
      while (digitalRead(WIO_KEY_A) == LOW);
      buttonPressed(false, rightButton, "C");
      choiceSensor = 2;
      break;
    }
    delay(200);

  }
}





void trainSelectorpage() {
  choiceModel = 0;
  menu_shell(4);

  while (1) {
    if (digitalRead(WIO_KEY_C) == LOW) {
      buttonPressed(true, leftButton, "A");
      while (digitalRead(WIO_KEY_C) == LOW);
      buttonPressed(false, leftButton, "A");
      choiceModel = 1;
      break;
    }
    /*if (digitalRead(WIO_KEY_B) == LOW) {
      buttonPressed(true, midButton, "B");
      while (digitalRead(WIO_KEY_B) == LOW);
      buttonPressed(false, midButton, "B");
      choiceModel = 2;
      break;
    }*/
    delay(200);
  }
}


void buttonPressed(bool buttState, int button, String buttonText) {
  tft.fillRect(button - 25, 0, 50, h + 5, (buttonBG));
  if (buttState) {
    tft.drawString(buttonText, button - tft.textWidth(buttonText) / 2, 3);
  }
  else {
    tft.fillRect(button - 25, 0, 50, h, (buttonBG));
    tft.drawString(buttonText, button - tft.textWidth(buttonText) / 2, 3);

  }
}






void wiodisplay() {

  if (state == "run") {
    tft.fillScreen(runBG);

    tft.fillRect(-20, -10, 50, 45, (buttonBG));
    //tft.fillRect(65, -10, 50, 45, (buttonBG));
    tft.fillRect(150, -10, 50, 45, (buttonBG));
    tft.setTextColor(TFT_WHITE, TFT_WHITE);
    tft.setTextSize(2);

    tft.drawXBitmap(4, 3, QUIT, 32, 30, TFT_RED);
   // tft.drawString("f(x)", midButton - tft.textWidth("f(x)") / 2, 8);
    tft.drawXBitmap(159, 3, EDIT, 32, 30, TFT_YELLOW);
    tft.setTextSize(1);
    tft.drawString(" Running Mode", 220, 5 );

    if (DCount == 0) {
      tft.drawString("Edit to take some data", 145, 38 );
    }
    else if (DCount == 1) {
      tft.drawString(" !!! Edit to take more data", 145, 38 );
    }


  }
  else if (state == "train") {
    tft.fillScreen(trainBG);

    tft.fillRect(-20, -10, 50, 45, (buttonBG));
    tft.fillRect(65, -10, 50, 45, (buttonBG));
    tft.fillRect(150, -10, 50, 45, (buttonBG));

    tft.drawXBitmap(4, 3, QUIT, 32, 30, TFT_RED);
    tft.drawXBitmap(74, 3, TRASH1, 32, 30, TFT_WHITE);
    tft.drawXBitmap(74, 3, TRASH2, 32, 30, 20 << 11 | 25 << 6 | 25);
    tft.drawXBitmap(159, 3, RUN, 32, 30, TFT_GREEN);

    tft.setTextColor(textColor, textColor);
    tft.setTextSize(1);
    tft.drawString(" Training Mode", 220, 5 );
    drawGraph();
  }
}


void drawXlabel() {

  img.createSprite(200, 31);
  img.fillSprite(TFT_WHITE);
  img.setTextColor(textColor, textColor);
  img.drawFastHLine(20, 30, 175, TFT_PURPLE);

  img.drawFastVLine(195, 25, 5, TFT_RED);
  img.drawString("MAX", 180, 10 );

  img.drawFastVLine(35, 25, 5, TFT_RED);
  img.drawString("MIN", 20, 10 );


  img.drawString(xaxis[choiceSensor - 1], 115 - (tft.textWidth(xaxis[choiceSensor - 1])) / 2, 10 );
  tft.setRotation(2);
  img.pushSprite(0, 40);

  tft.setRotation(3);
  img.deleteSprite();
}

void drawYlabel() {

  img.createSprite(205, 25);
  img.setTextColor(textColor, textColor);
  img.fillSprite(TFT_WHITE);

  img.drawFastHLine(0, 0, 190, TFT_PURPLE);

  img.drawFastVLine(10, 0, 5, TFT_RED);
  img.drawString(yaxisI1[choiceMotor - 1], 5, 10 );


  img.drawFastVLine(190, 0, 5, TFT_RED);
  img.drawString(yaxisI2[choiceMotor - 1], 180, 10 );
  img.pushSprite(70, 215);

  img.deleteSprite();
}



void drawGraph() {
  drawYlabel();
  drawXlabel();
}


void buzz(int t) {
  for (int i = 0; i < t; i++) {
    analogWrite(WIO_BUZZER, 128);
    delay(200);
    analogWrite(WIO_BUZZER, 0);
    delay(200);
  }
}



void displayTraining() {
  int SWidth = 180;
  int SHeight = 160;
  float scale = 20;
  img.createSprite(SWidth, SHeight);
  img.setTextColor(textColor, textColor);
  img.fillSprite(TFT_WHITE);
  img.drawRect(0, 0, SWidth, SHeight, TFT_BLUE);
  for (int n = 0; n < 8; n++) {
    img.drawFastHLine(0, n * scale, SWidth, TFT_BLUE);
    img.drawFastVLine(n * scale, 0, SHeight, TFT_BLUE);
  }
  img.drawFastVLine(8 * scale, 0, SHeight, TFT_BLUE);

  if (DCount > 0) {
    for (int i = 0; i < DCount - 1; i++) {

      img.fillRect((training[i][0] * scale) + 2, ((8 - training[i][1]) * scale) + 2, 16, 16, dataBG);
    }
  }
  img.fillRect((training[DCount - 1][0] * scale) + 2, ((8 - training[DCount - 1][1]) * scale) + 2,  16, 16, dataBGLatest);
  img.drawRect((pos * scale) + 3, ((8 - sensorValue ) * scale) + 3, 14, 14, TFT_WHITE);
  img.fillRect((pos * scale) + 4, ((8 - sensorValue ) * scale) + 4, 12, 12, TFT_PURPLE);
  img.pushSprite(80, 50);
  img.deleteSprite();
}



void displaySensorMotor() {
  int SWidth = 90;
  int SHeight = 80;
  float scale = 10;
  img.createSprite(SWidth, SHeight);
  img.setTextColor(textColor, textColor);
  img.fillSprite(TFT_WHITE);
  img.drawRect(0, 0, SWidth, SHeight, TFT_BLUE);
  for (int n = 0; n < 8; n++) {
    img.drawFastHLine(0, n * scale, SWidth, TFT_BLUE);
    img.drawFastVLine(n * scale, 0, SHeight, TFT_BLUE);
  }
  img.drawFastVLine(8 * scale, 0, SHeight, TFT_BLUE);
  img.fillRect((pos * scale), ((8 - sensorValue ) * scale), scale, scale, TFT_PURPLE);
  img.pushSprite(220, 100);
  img.deleteSprite();
}

void reset_wio() { //resets all the training data to 0
  DCount = 0;
  state = "train";

  pos = 4;
  motorPos = map(pos, 0, 8, 0, 180);
  myservo2.write(motorPos);
  i = 0;

  sx = 0; //sum of x
  sy = 0; //sum of y

  sxsq = 0; //sum of square of x
  sxy = 0; //sum of product of x and y
  tft.fillScreen(menuBG);
}






void moveMotor() {
  if (digitalRead(WIO_5S_LEFT) == LOW) {
    while (digitalRead(WIO_5S_LEFT) == LOW) {
      if (pos == 0) {
        pos = 1;
      }
      pos = pos - inc ;

      motorPos = map(pos, 0, 8, 0, 175);
      myservo2.write(motorPos);
      delay(500);
    }
  }
  else if (digitalRead(WIO_5S_RIGHT) == LOW) {
    while (digitalRead(WIO_5S_RIGHT) == LOW) {
      if (pos == 8) {
        pos = 7;
      }
      pos = pos + inc ;
      motorPos = map(pos, 0, 8, 0, 170);
      myservo2.write(motorPos);
      delay(500);
    }
  }
}



void getSensor() {
  if (choiceSensor == 1) {
    sensorValue = analogRead(WIO_LIGHT);    
    sensorValue = 160 * (1 - pow(2, -1 * float(sensorValue)/300));
  }
  else if (choiceSensor == 2) {
    sensorValue = analogRead(A0);
    sensorValue = map(sensorValue, 0, 1024, 0, 160);
  }
  else if (choiceSensor == 3) {
    sensorValue = lis.getAccelerationX() * 80 + 78;

  }
  sensorValue = map(sensorValue, 0, 160, 1, 9);
  old_sensorValue = sensorValue;
}


void mainScreenLevel() {
  getSensor();
  displaySensorMotor();
}


void deleteData() {
  tft.fillRect(65, -5, 50, 45, (buttonBG));
  tft.drawXBitmap(74, 8, TRASH1, 32, 30, TFT_WHITE);
  tft.drawXBitmap(74, 8, TRASH2, 32, 30, 20 << 11 | 25 << 6 | 25);
  while (digitalRead(WIO_KEY_B) == LOW) {
    delay(100);
  }
  tft.fillRect(65, -5, 50, 45, (trainBG));
  tft.fillRect(65, -10, 50, 45, (buttonBG));
  tft.drawXBitmap(74, 3, TRASH1, 32, 30, TFT_WHITE);
  tft.drawXBitmap(74, 3, TRASH2, 32, 30, 20 << 11 | 25 << 6 | 25);

  if (DCount > 0) {
    DCount = DCount - 1;
    displayTraining();

  }
}

int smartMotor(int choiceSensor) {
  wiodisplay();

  while (digitalRead(WIO_KEY_C)) { //until the run button is pressed
    getSensor();
    displayTraining();
    moveMotor();

    if (digitalRead(WIO_KEY_B) == LOW) { //Delete data
      deleteData();
    }


    if (digitalRead(WIO_KEY_A) == LOW) { // go to run
      state = "run";
      buzz(3);
      old_pos = -99;
      m = float((DCount * sxy - sx * sy)) / float((DCount * sxsq - sx * sx)); // calculate the slope
      b = float((sy - m * sx)) / float(DCount);              //calculate the intercept
      displayTraining();
      Serial.println(b);
      Serial.println(m);
      wiodisplay();
      while (digitalRead(WIO_KEY_C)) { //pressing the button will escape the run mode
        drawGraph();

      /*  if (digitalRead(WIO_KEY_B) == LOW) { // change the type of model
          while (digitalRead(WIO_KEY_B) == LOW) ;
          trainSelectorpage();
          wiodisplay();
        }*/

        if (digitalRead(WIO_KEY_A) == LOW) { // escape
          while (digitalRead(WIO_KEY_A) == LOW) ;
          state = "train";
          wiodisplay();
          break;
        }



        getSensor();
        if (choiceModel == 1) {
          mini = 1000;
          pos = 4;
          for (int i = 0; i < DCount; i++) {
            dist = abs(sensorValue - training[i][1]);
            if (dist < mini) {
              mini = dist;
              pos = training[i][0];
            }
          }
        }

        else if (choiceModel == 2) {
          tft.drawLine(75 + float(-b) / float(m) , 220, 75 + float((150 - b)) / float(m), 70, TFT_GREEN);
          pos = float(sensorValue *m + b) ;
        }

        displayTraining();
        motorPos = map(pos, 0, 8, 0, 180);
        myservo2.write(motorPos);
        old_pos = pos;
        delay(100);
      }
    }

    if (digitalRead(WIO_5S_PRESS) == LOW) { //store data
      //for linear regression
      sx = sx + pos;
      sy = sy + sensorValue;

      sxy = sxy + pos * sensorValue;
      sxsq = sxsq + pos * pos;
      buzz(1);

      training[DCount][0] = pos;
      training[DCount][1] = sensorValue;
      i = i + 1;
      DCount += 1;
    }

  }
  reset_wio();

}
