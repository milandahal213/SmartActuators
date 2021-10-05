#include "Servo.h"
#include"TFT_eSPI.h"
#include"LIS3DHTR.h"
#include"icons.h"
LIS3DHTR<TwoWire> lis;
#include<arduino.h>

#define LCD_BACKLIGHT (72Ul) // Control Pin of LCD
Servo myservo1;
Servo myservo2;
#define SPEAKER D1


//rotary sensor variables
#define ROTARY A0

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


int graphBG = TFT_WHITE;
int dataBG = TFT_RED;
int dataBGLatest = TFT_DARKGREEN;


// MENU Variables
int selection = 0;
int x = 0;
int y = 30;
int old_y = 30;
int selectd = 0;

int x1 = 15;
int x2 = 90;
int x3 = 175;

int h = 50;

int choiceSensor = 1;
int choiceMotor = 1;
int choiceModel = 1;
//running mode

bool graphMode = false;
//training variables
int training[50][2];
int inc = 1;
int count = 0;
int pos = 90;
int old_pos = 0;
int brightness = 0;
int old_brightness = 0;

int t = 0;
int i = 0;
int dist = 0;
int mini = 0;
int DCount = 0;
String state = "train";
bool state_flag = true ;

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





TFT_eSPI tft;
#define LCD_BACKLIGHT (72Ul) // Control Pin of LCD

void setup() {
  // put your setup code here, to run once:

  tft.begin();
  tft.setRotation(3);
  Serial.begin(115200);

  lis.begin(Wire1);
  if (!lis) {
    Serial.println("ERROR");
    while (1);
  }
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ); //Data output rate
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G); //Scale range set to 2g

  tft.fillScreen(green);

  myservo2.attach(SDA); //for m5stack servos
  myservo1.attach(SCL); //for Grove Servos
  pinMode(SPEAKER, OUTPUT);  //Speaker

  pinMode(WIO_5S_UP, INPUT);
  pinMode(WIO_5S_DOWN, INPUT);

  pinMode(WIO_5S_LEFT, INPUT);
  pinMode(WIO_5S_RIGHT, INPUT);
  pinMode(WIO_5S_DOWN, INPUT);
  pinMode(WIO_5S_PRESS, INPUT);
  pinMode(WIO_LIGHT , INPUT);
  pinMode(WIO_BUZZER, OUTPUT);
  pinMode(WIO_KEY_A, INPUT);
  pinMode(WIO_KEY_B, INPUT);
  pinMode(WIO_KEY_C, INPUT);


  tft.setTextColor(textColor, textColor);

  //drawGraphics(0);
}

void loop() {
  menu();
}



int menu() {
  menu_shell(3);

  while (1) {

    if (digitalRead(WIO_KEY_C) == LOW) {
      go_down(x1, "A");
      while (digitalRead(WIO_KEY_C) == LOW);
      go_up(x1, "A");
      trainSelectorpage();

      break;
    }

    if (digitalRead(WIO_KEY_B) == LOW) {
      go_down(x2, "B");
      while (digitalRead(WIO_KEY_B) == LOW);
      go_up(x2, "B");

      info();
      menu_shell(3);
    }

    if (digitalRead(WIO_KEY_A) == LOW) {
      go_down(x3, "C");
      while (digitalRead(WIO_KEY_A) == LOW);
      go_up(x3, "C");
      sensorSelectorpage();
      break;

    }
    delay(200);
  }
}




void menu_shell(int menuStage) {
  h = 30;

  tft.fillScreen(menuBG);
  tft.setTextColor(textColor2, textColor2);
  tft.setTextSize(3);

  tft.fillRect(x1 - 25, 0, 50, h, (buttonBG));
  tft.fillRect(x2 - 25, 0, 50, h, (buttonBG));


  tft.drawString("A", x1 - tft.textWidth("C") / 2, 3);
  tft.drawString("B", x2 - tft.textWidth("B") / 2, 3);


  tft.fillRect(x1, h, 5, h + 20, (buttonBG));
  tft.fillRect(x2, h, 5, h + 20, (buttonBG));


  tft.fillRect(x1, 2 * h + 20, 70 - x1, 5, (buttonBG));
  tft.fillRect(x2, 2 * h + 20, 160 - x2, 5, (buttonBG));


  tft.fillRect(70, 2 * h + 20, 5, 30, (buttonBG));
  tft.fillRect(160, 2 * h + 20, 5, 40, (buttonBG));


  tft.fillRect(30, 3 * h + 10, 80, h * 3 - 10, (buttonBG));
  tft.fillRect(120, 3 * h + 10, 80, h * 3 - 10, (buttonBG));


  if (menuStage == 1) {
    tft.fillRect(x3 - 25, 0, 50, h, (buttonBG));
    tft.drawString("C", x3 - tft.textWidth("A") / 2, 3);
    tft.fillRect(x3, h, 5, h + 20, (buttonBG));
    tft.fillRect(x3, 2 * h + 20, 250 - x3, 5, (buttonBG));
    tft.fillRect(250, 2 * h + 20, 5, 50, (buttonBG));
    tft.fillRect(210, 3 * h + 10, 80, h * 3 - 10, (buttonBG));
    tft.drawRect(20, 3 * h, 280, h * 3 + 10, (buttonBG));
    tft.drawXBitmap(38, 3 * h + 20, MOTORLIGHT, 64, 60, MODE1Color);
    tft.drawXBitmap(128, 3 * h + 20, ROTORY, 64  , 60, MODE2Color);
    tft.drawXBitmap(218, 3 * h + 20, TILT, 64, 60, MODE3Color);
  }




  else if (menuStage == 2) {
    tft.drawRect(20, 3 * h, 190, h * 3 + 10, (buttonBG));
    tft.drawXBitmap(38, 3 * h + 20, DISCRETE, 64, 60, MODE1Color);
    tft.drawXBitmap(128, 3 * h + 20, CONTINUOUS, 64  , 60, MODE2Color);


    if (choiceSensor == 1) {
      tft.drawXBitmap(250, 20, MOTORLIGHT, 64, 60, MODE1Color);
    }
    else if (choiceSensor == 2) {
      tft.drawXBitmap(250, 20, ROTORY, 64  , 60, MODE2Color);
    }
    else if (choiceSensor == 3) {
      tft.drawXBitmap(250, 20, TILT, 64, 60, MODE3Color);
    }




  }

  else if (menuStage == 3) {
    tft.fillRect(x3 - 25, 0, 50, h, (buttonBG));
    tft.drawString("C", x3 - tft.textWidth("A") / 2, 3);
    tft.fillRect(x3, h, 5, h + 20, (buttonBG));
    tft.fillRect(x3, 2 * h + 20, 250 - x3, 5, (buttonBG));
    tft.fillRect(250, 2 * h + 20, 5, 50, (buttonBG));
    tft.fillRect(210, 3 * h + 10, 80, h * 3 - 10, (buttonBG));
    tft.drawRect(20, 3 * h, 280, h * 3 + 10, (buttonBG));

    tft.drawXBitmap(38, 3 * h + 20, BIGRUN, 64, 60, MODE1Color);
    tft.drawXBitmap(128, 3 * h + 20, INFO, 64  , 60, MODE2Color);
    tft.drawXBitmap(218, 3 * h + 20, SETTING, 64, 60, MODE3Color);



    if (choiceSensor == 1) {
      tft.drawXBitmap(250, 20, MOTORLIGHT, 64, 60, MODE1Color);
    }
    else if (choiceSensor == 2) {
      tft.drawXBitmap(250, 20, ROTORY, 64  , 60, MODE2Color);
    }
    else if (choiceSensor == 3) {
      tft.drawXBitmap(250, 20, TILT, 64, 60, MODE3Color);
    }

    if (choiceMotor == 1) {
      tft.drawXBitmap(190, 20, DISCRETE, 64, 60, MODE1Color);
    }
    else if (choiceMotor == 2) {
      tft.drawXBitmap(190, 20, CONTINUOUS, 64  , 60, MODE2Color);
    }
  }

  else if (menuStage == 4) {
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
  menu_shell(1);

  while (1) {
    if (digitalRead(WIO_KEY_C) == LOW) {
      go_down(x1, "A");
      while (digitalRead(WIO_KEY_C) == LOW);
      go_up(x1, "A");
      choiceSensor = 1;
      motorSelectorpage();
      break;

    }

    if (digitalRead(WIO_KEY_B) == LOW) {
      go_down(x2, "B");
      while (digitalRead(WIO_KEY_B) == LOW);
      go_up(x2, "B");
      choiceSensor = 2;
      motorSelectorpage();
      break;

    }
    if (digitalRead(WIO_KEY_A) == LOW) {
      go_down(x3, "C");
      while (digitalRead(WIO_KEY_A) == LOW);
      go_up(x3, "C");
      choiceSensor = 3;
      motorSelectorpage();
      break;
    }
    delay(200);
  }
}


void motorSelectorpage() {
  choiceMotor = 0;
  menu_shell(2);

  while (1) {
    if (digitalRead(WIO_KEY_C) == LOW) {
      go_down(x1, "A");
      while (digitalRead(WIO_KEY_C) == LOW);
      go_up(x1, "A");
      choiceMotor = 1;

      menu();
      break;

    }

    if (digitalRead(WIO_KEY_B) == LOW) {
      go_down(x2, "B");
      while (digitalRead(WIO_KEY_B) == LOW);
      go_up(x2, "B");
      choiceMotor = 2;

      menu();
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
      go_down(x1, "A");
      while (digitalRead(WIO_KEY_C) == LOW);
      go_up(x1, "A");
      choiceModel = 1;
      break;

    }

    if (digitalRead(WIO_KEY_B) == LOW) {
      go_down(x2, "B");
      while (digitalRead(WIO_KEY_B) == LOW);
      go_up(x2, "B");
      choiceModel = 2;
      break;

    }

    delay(200);
  }
  smartMotor(choiceSensor, choiceModel);

}


void go_down(int x, String c) {
  tft.fillRect(x - 25, 0, 50, h + 5, (buttonBG));
  tft.drawString(c, x - tft.textWidth(c) / 2, 8);
}

void go_up(int x, String c) {
  tft.fillRect(x - 25, 0, 50, h + 5, (menuBG));
  tft.fillRect(x - 10, h, 20, h, (buttonBG));
  tft.fillRect(x - 25, 0, 50, h, (buttonBG));
  tft.drawString(c, x - tft.textWidth(c) / 2, 3);
}




void printout(char message[100]) {
  char pm[15];
  for (int j = 0; strlen(message) > mCount; j++) {
    strcpy(pm, ".");
    for (int i = 0; i < 15; i++) {
      if (i > 10 && message[mCount] == ' ') {
        pm[i] = ' ';
      }
      else {
        pm[i] = message[mCount];
        mCount = mCount + 1;
      }
    }

    tft.drawString(pm, 220, 100 + j * 10);
  }
  mCount = 0;
}



void pressDown(int x, int y, char button) {
  if (button == 'A') {
    tft.fillRoundRect(x - 10, y - 35, 20, 8, 5, TFT_WHITE); //button C
    tft.fillRoundRect(x - 10, y - 33, 20, 8, 5, TFT_BLUE ); //button C
  }
  else if (button == 'B')
  {
    tft.fillRoundRect(x + 25, y - 35, 20, 8, 5, TFT_WHITE); //button B
    tft.fillRoundRect(x + 25, y - 33, 20, 8, 5, TFT_BLUE ); //button B
  }
  else if (button == 'C') {
    tft.fillRoundRect(x + 58, y - 35, 20, 8, 5, TFT_WHITE); //button A
    tft.fillRoundRect(x + 58, y - 33, 20, 8, 5, TFT_BLUE ); //button A
  }
}


void pressUP(int x, int y, char button) {
  if (button == 'A') {
    tft.fillRoundRect(x - 10, y - 33, 20, 8, 5, TFT_NAVY ); //button C
    tft.fillRoundRect(x - 10, y - 35, 20, 8, 5, TFT_BLUE); //button C
  }
  else if (button == 'B')
  {
    tft.fillRoundRect(x + 25, y - 33, 20, 8, 5, TFT_NAVY ); //button B
    tft.fillRoundRect(x + 25, y - 35, 20, 8, 5, TFT_BLUE); //button B
  }
  else if (button == 'C') {
    tft.fillRoundRect(x + 58, y - 33, 20, 8, 5, TFT_NAVY ); //button A
    tft.fillRoundRect(x + 58, y - 35, 20, 8, 5, TFT_BLUE); //button A
  }
}
void showMotorPosition(int a) {
  //show motor Position
  tft.drawFastVLine(a, 206, 100, TFT_YELLOW);
  tft.drawFastVLine(a + 2, 206, 100, TFT_WHITE);
  tft.drawFastVLine(a + 4, 206, 3090, TFT_RED);
  tft.drawFastVLine(a + 6, 206, 300, TFT_BLACK);
}

void hideMotorPosition(int a) {
  //show motor Position
  tft.drawFastVLine(a, 206, 100, infoBG);
  tft.drawFastVLine(a + 2, 206, 100, infoBG);
  tft.drawFastVLine(a + 4, 206, 3090, infoBG);
  tft.drawFastVLine(a + 6, 206, 300, infoBG);
}
void drawInfo(int x, int y, char * stage) {
  /*
    tft.fillRoundRect(x - 30, y - 35, 206, 166, 10, TFT_WHITE); //outline
    tft.fillRoundRect(x - 10, y - 33, 20, 8, 5, TFT_NAVY ); //button C
    tft.fillRoundRect(x + 25, y - 33, 20, 8, 5, TFT_NAVY ); //button B
    tft.fillRoundRect(x + 58, y - 33, 20, 8, 5, TFT_NAVY ); //button A

    tft.fillRoundRect(x - 10, y - 35, 20, 8, 5, TFT_BLUE); //button C
    tft.fillRoundRect(x + 25, y - 35, 20, 8, 5, TFT_BLUE); //button B
    tft.fillRoundRect(x + 58, y - 35, 20, 8, 5, TFT_BLUE); //button A


    tft.fillRoundRect(x - 30, y - 20, 206, 150, 5, TFT_LIGHTGREY); //inner outline

    tft.fillRoundRect(x - 27, y - 17, 200, 130, 5, TFT_BLACK); //the black glass
    tft.fillCircle(x + 160, y + 110, 10, TFT_BLUE); //the 5 way switch

  */


  tft.drawXBitmap(x - 30, y - 35, Info1White, 201, 148, TFT_WHITE);
  tft.drawXBitmap(x - 30, y - 35, Info1Black, 202, 148, TFT_BLACK);
    tft.drawXBitmap(x - 30, y - 35, Info1Blue, 201, 147, TFT_BLUE);
    tft.drawRect(x - 30, y - 35,201,148,infoBG);
    tft.drawRect(x - 30, y - 34,202,148,infoBG);
    tft.drawRect(x - 30, y - 35,201,147,infoBG);
  if (stage == "connectMotor") {
    tft.setTextColor(TFT_BLUE, TFT_DARKGREY);
   // tft.fillRect(x, y, 150, 90, TFT_GREEN); // main screen
    tft.fillRect(220, 0, 100, 240, infoBG); //cover text area
    tft.setTextSize(2);
    //tft.drawXBitmap(220, 50, CONNECTMOTOR,64,60, TFT_BLUE);
   // tft.drawXBitmap(x + 20, y + 20, MOTORLIGHT, 64, 60, TFT_BLUE);



    tft.setTextSize(1);
    tft.setTextColor(TFT_BLACK, TFT_DARKGREY);
    printout("Connect Motor to the left port.");

  }

  else if (stage == "connectPOT") {
    tft.setTextColor(TFT_BLUE, TFT_DARKGREY);
    tft.fillRect(x, y, 150, 90, TFT_GREEN); // main screen
    tft.fillRect(220, 0, 100, 240, infoBG); //cover text area
    tft.setTextSize(2);
    tft.drawXBitmap(x + 20, y + 20, MOTORLIGHT, 64, 60, TFT_BLUE);

    tft.setTextSize(1);
    tft.setTextColor(TFT_BLACK, TFT_DARKGREY);
    printout("Connect Rotary Sensor to the right port.");

  }


  else if (stage == "enterTrainingMode") {
    tft.fillRect(x, y, 150, 90, TFT_GREEN); // main screen
    tft.fillRect(220, 0, 100, 240, infoBG); //cover text area
    tft.setTextSize(2);
    //tft.drawXBitmap(220, 50, PRESSRUN,64,60, TFT_WHITE);

    tft.setTextColor(TFT_WHITE, TFT_BLUE);
    tft.fillRect(x, y, 15, 17, (TFT_BLUE)); //for x
    tft.fillRect(25 + x, y, 25, 17, (TFT_BLUE)); //for back
    tft.fillRect(55 + x, y, 25, 17, (TFT_BLUE)); //for forward

    tft.drawString("A", 2 + x, 1 + y); //C
    tft.drawString("B", 30 + x, 1 + y); //B
    tft.drawString("C", 60 + x, 1 + y); //A

    tft.drawFastVLine(4 + x, y + 17, 20, TFT_BLUE);
    tft.drawFastVLine(38 + x, y + 17, 20, TFT_BLUE);
    tft.drawFastVLine(68 + x, y + 17, 20, TFT_BLUE);
    tft.drawFastHLine(x + 4, y + 37, 16, TFT_BLUE);
    tft.drawFastHLine(x + 38, y + 37, 27, TFT_BLUE);
    tft.drawFastHLine(x + 68, y + 37, 37, TFT_BLUE);
    tft.drawFastVLine(x + 20, y + 37, 10, TFT_BLUE);
    tft.drawFastVLine(x + 65, y + 37, 10, TFT_BLUE);
    tft.drawFastVLine(x + 105, y + 37, 10, TFT_BLUE);

    tft.drawRect(x + 5, y + 45, 120, 25, TFT_BLUE);

    tft.fillRect(x + 10, y + 47, 20, 18, (TFT_BLUE)); //for x
    tft.fillRect(x + 55, y + 47, 20, 18, (TFT_BLUE)); //for x
    tft.fillRect(x + 95, y + 47, 20, 18, (TFT_BLUE)); //for x

    tft.drawXBitmap(x + 15, y + 50, SETTINGSmall, 16, 15, MODE1Color);
    tft.drawXBitmap(x + 60, y + 50, INFOSmall, 16  , 15, MODE2Color);
    tft.drawXBitmap(x + 100, y + 50, RUNSmall, 16, 15, MODE3Color);

    tft.setTextSize(1);
    tft.setTextColor(TFT_BLACK, TFT_DARKGREY);
    printout("In the MAIN SCREEN Press A to go to the Training Mode. In this mode you can take data to train your Smart System! ");
  }

  else if (stage == "train1") {

    tft.fillRect(x, y, 150, 90, TFT_GREEN); // main screen
    tft.fillRect(x, y, 15, 17, (TFT_BLUE)); //for x
    tft.fillRect(25 + x, y, 25, 17, (TFT_BLUE)); //for back
    tft.fillRect(55 + x, y, 25, 17, (TFT_BLUE)); //for forward

    tft.drawXBitmap(2 + x, 1 + y, QUITSmall, 16, 15, TFT_RED); //x
    tft.drawXBitmap(26 + x, 1 + y, TRASH1Small, 16, 15, TFT_WHITE); //trash
    tft.drawXBitmap(26 + x, 1 + y, TRASH2Small, 16, 15, TFT_GREEN); //trash
    tft.drawXBitmap(56 + x, 1 + y, RUNSmall, 16, 15, TFT_GREEN); //forward

    tft.fillRect(220, 0, 100, 240, infoBG); //cover text area
    //tft.drawXBitmap(220, 50, MOTORINST,64,60, TFT_WHITE);


    tft.setTextSize(1);
    tft.setTextColor(TFT_BLACK, TFT_DARKGREY);
    printout("TRY NOW!! Move the 5 WAY SWITCH to left to turn the motor left. ");
  }

  else if (stage == "train2") {

    tft.fillRect(x, y, 150, 90, TFT_GREEN); // main screen
    tft.fillRect(x, y, 15, 17, (TFT_BLUE)); //for x
    tft.fillRect(25 + x, y, 25, 17, (TFT_BLUE)); //for back
    tft.fillRect(55 + x, y, 25, 17, (TFT_BLUE)); //for forward

    tft.drawXBitmap(2 + x, 1 + y, QUITSmall, 16, 15, TFT_RED); //x
    tft.drawXBitmap(26 + x, 1 + y, TRASH1Small, 16, 15, TFT_WHITE); //trash
    tft.drawXBitmap(26 + x, 1 + y, TRASH2Small, 16, 15, TFT_GREEN); //trash
    tft.drawXBitmap(56 + x, 1 + y, RUNSmall, 16, 15, TFT_GREEN); //forward

    tft.fillRect(220, 0, 100, 240, infoBG); //cover text area
    //tft.drawXBitmap(220, 50, MOTORINST,64,60, TFT_WHITE);

    tft.setTextSize(1);
    tft.setTextColor(TFT_BLACK, TFT_DARKGREY);
    printout("TRY NOW!! Move the 5 WAY SWITCH to right to turn the motor right. ");
  }

  else if (stage == "train3") {

    tft.fillRect(x, y, 150, 90, TFT_GREEN); // main screen
    tft.fillRect(x, y, 15, 17, (TFT_BLUE)); //for x
    tft.fillRect(25 + x, y, 25, 17, (TFT_BLUE)); //for back
    tft.fillRect(55 + x, y, 25, 17, (TFT_BLUE)); //for forward

    tft.drawXBitmap(2 + x, 1 + y, QUITSmall, 16, 15, TFT_RED); //x
    tft.drawXBitmap(26 + x, 1 + y, TRASH1Small, 16, 15, TFT_WHITE); //trash
    tft.drawXBitmap(26 + x, 1 + y, TRASH2Small, 16, 15, TFT_GREEN); //trash
    tft.drawXBitmap(56 + x, 1 + y, RUNSmall, 16, 15, TFT_GREEN); //forward

    tft.fillRect(220, 0, 100, 240, infoBG); //cover text area
    //  tft.drawXBitmap(220, 50, MOTORINST,64,60, TFT_WHITE);

    tft.setTextSize(1);
    tft.setTextColor(TFT_BLACK, TFT_DARKGREY);
    printout("Press 5 WAY SWITCH  to take data. Circles will appear to denote your data points!! ");


  }


  else if (stage == "POT") {

    tft.fillRect(x, y, 150, 90, TFT_GREEN); // main screen
    tft.fillRect(x, y, 15, 17, (TFT_BLUE)); //for x
    tft.fillRect(25 + x, y, 25, 17, (TFT_BLUE)); //for back
    tft.fillRect(55 + x, y, 25, 17, (TFT_BLUE)); //for forward

    tft.drawXBitmap(2 + x, 1 + y, QUITSmall, 16, 15, TFT_RED); //x
    tft.drawXBitmap(26 + x, 1 + y, TRASH1Small, 16, 15, TFT_WHITE); //trash
    tft.drawXBitmap(26 + x, 1 + y, TRASH2Small, 16, 15, TFT_GREEN); //trash
    tft.drawXBitmap(56 + x, 1 + y, RUNSmall, 16, 15, TFT_GREEN); //forward

    tft.fillRect(220, 0, 100, 240, infoBG); //cover text area


    tft.setTextSize(1);
    tft.setTextColor(TFT_BLACK, TFT_DARKGREY);
    printout("Turn the Rotary Sensor to make the circle go big and small!!   ");


  }



  else if (stage == "Accelerometer") {

    tft.fillRect(x, y, 150, 90, TFT_GREEN); // main screen
    tft.fillRect(x, y, 15, 17, (TFT_BLUE)); //for x
    tft.fillRect(25 + x, y, 25, 17, (TFT_BLUE)); //for back
    tft.fillRect(55 + x, y, 25, 17, (TFT_BLUE)); //for forward

    tft.drawXBitmap(2 + x, 1 + y, QUITSmall, 16, 15, TFT_RED); //x
    tft.drawXBitmap(26 + x, 1 + y, TRASH1Small, 16, 15, TFT_WHITE); //trash
    tft.drawXBitmap(26 + x, 1 + y, TRASH2Small, 16, 15, TFT_GREEN); //trash
    tft.drawXBitmap(56 + x, 1 + y, RUNSmall, 16, 15, TFT_GREEN); //forward

    tft.fillRect(220, 0, 100, 240, infoBG); //cover text area


    tft.setTextSize(1);
    tft.setTextColor(TFT_BLACK, TFT_DARKGREY);
    printout(" The Accelerometer in WIO senses tilt. Tilt the WIO Forward and Backward to make the circle big and small!!    ");


  }

  else if (stage == "RUN") {

    tft.fillRect(x, y, 150, 90, TFT_GREEN); // main screen
    tft.fillRect(x, y, 15, 17, (TFT_BLUE)); //for x
    tft.fillRect(25 + x, y, 25, 17, (TFT_BLUE)); //for back
    tft.fillRect(55 + x, y, 25, 17, (TFT_BLUE)); //for forward

    tft.drawXBitmap(2 + x, 1 + y, QUITSmall, 16, 15, TFT_RED); //x
    tft.drawXBitmap(26 + x, 1 + y, TRASH1Small, 16, 15, TFT_WHITE); //trash
    tft.drawXBitmap(26 + x, 1 + y, TRASH2Small, 16, 15, TFT_GREEN); //trash
    tft.drawXBitmap(56 + x, 1 + y, RUNSmall, 16, 15, TFT_GREEN); //forward

    tft.fillRect(220, 0, 100, 240, infoBG); //cover text area


    tft.setTextSize(1);
    tft.setTextColor(TFT_BLACK, TFT_DARKGREY);
    printout(" Once you have enough training data, Press the RUN button to RUN your code. In the RUN Mode you can view your data in a graph by pressing graph icon!! ");


  }

  else if (stage == "DONE") {

    tft.fillRect(220, 0, 100, 240, infoBG); //cover text area

    tft.setTextSize(1);
    tft.setTextColor(TFT_BLACK, TFT_DARKGREY);
    printout(" NOW Press the X to exit the info section and Press C to start Training your data.");


  }
  else if (stage == "light") {

    tft.fillRoundRect(x - 30, y - 35, 206, 166, 10, TFT_WHITE); //outline

    tft.fillRoundRect(x + 91, y - 33, 20, 8, 5, TFT_NAVY ); //button C
    tft.fillRoundRect(x + 124, y - 33, 20, 8, 5, TFT_NAVY ); //button B
    tft.fillRoundRect(x + 58, y - 33, 20, 8, 5, TFT_NAVY ); //button A

    tft.fillRoundRect(x + 58, y - 35, 20, 8, 5, TFT_BLUE); //button C
    tft.fillRoundRect(x + 91, y - 35, 20, 8, 5, TFT_BLUE); //button B
    tft.fillRoundRect(x + 124, y - 35, 20, 8, 5, TFT_BLUE); //button A

    tft.fillRoundRect(x - 30, y - 20, 206, 150, 5, TFT_LIGHTGREY); //inner outline

    tft.fillCircle(x + 160, y + 110, 6, TFT_BLACK); //the 5 way switch
    tft.fillCircle(x - 10, y + 110, 6, TFT_BLACK); //the 5 way switch
    tft.fillCircle(x - 10, y - 10, 6, TFT_BLACK); //the 5 way switch
    tft.fillCircle(x + 160, y - 10, 6, TFT_BLACK); //the 5 way switch

    tft.fillRoundRect(x, y, 155, 25, 2, TFT_BLACK); //inner outline
    tft.fillRoundRect(x, y + 35, 65, 65, 5, TFT_WHITE); //inner outline
    tft.fillRoundRect(x + 90, y + 35, 65, 65, 5, TFT_GREEN); //inner outline

    tft.fillRect(220, 0, 100, 240, infoBG); //cover text area

    tft.setTextSize(1);
    tft.setTextColor(TFT_BLACK, TFT_DARKGREY);
    printout("The LIGHT Sensor is in the back inside the transparent plastic. Cover it make it dark!!  ");


  }
  tft.setTextSize(1);
  tft.setTextColor(TFT_BLACK, TFT_BLACK);

}


void info() {
  tft.fillScreen(infoBG);
  tft.fillRect(-20, -10, 50, 45, (buttonBG));
  tft.fillRect(65, -10, 50, 45, (buttonBG));
  tft.fillRect(150, -10, 50, 45, (buttonBG));

  tft.drawXBitmap(4, 3, QUIT, 32, 30, TFT_RED);
  tft.drawXBitmap(74, 3, ARRLEFT, 32, 30, TFT_WHITE);
  tft.drawXBitmap(159, 3, ARRRIGHT, 32, 30, TFT_WHITE);

  tft.setTextSize(3);


  infoPage(pageN);
  tft.setTextSize(3);
  tft.setTextColor(TFT_GREEN, infoBG);
  tft.drawString(String(pageN + 1), 240 , 30);
  while (digitalRead(WIO_KEY_C)) {
    moveMotor();
    showLight();
    if (digitalRead(WIO_KEY_A) == LOW) {
      while (digitalRead(WIO_KEY_A) == LOW);
      if (pageN < 7) {
        pageN += 1;
        infoPage( pageN);
        tft.setTextSize(3);
        tft.setTextColor(infoBG, infoBG);
        tft.drawString(String(pageN), 240 , 30);
        tft.setTextColor(TFT_GREEN, infoBG);
        tft.drawString(String(pageN + 1), 240 , 30);

      }
    }
    else if (digitalRead(WIO_KEY_B) == LOW) {
      while (digitalRead(WIO_KEY_B) == LOW);
      if (pageN > 0) {
        pageN -= 1;
        infoPage( pageN);
        tft.setTextSize(3);
        tft.setTextColor(infoBG, infoBG);
        tft.drawString(String(pageN + 2), 240 , 30);
        tft.setTextColor(TFT_GREEN, infoBG);
        tft.drawString(String(pageN + 1), 240 , 30);

      }
    }
  }
  lightsensorFlag = false;
  potFlag = false;
  accelFlag = false;

  pageN = 0;
  go_down(x1, "A");
  while (digitalRead(WIO_KEY_C) == LOW);
  go_up(x1, "A");

}

void infoPage(int pageNumber) {


  if (choiceSensor == 1) {
    if (pageNumber == 0) {

      drawInfo(30, 75, "connectMotor");
      tft.setTextSize(2);
      tft.setTextColor(TFT_BLACK, infoBG);
      tft.drawString("MOTOR", 80 - tft.textWidth("MOTOR") / 2 , 210);

      showMotorPosition(30);

    }
    else if (pageNumber == 1) {
      drawInfo(30, 75, "train1");
    }
    else if (pageNumber == 2) {
      showMotorPosition(30);
      hideMotorPosition(160);
      drawInfo(30, 75, "train2");
      lightsensorFlag = false;
    }
    else if (pageNumber == 3) {
      showMotorPosition(160);
      hideMotorPosition(30);
      drawInfo(30, 75, "light");
      lightsensorFlag = true;
    }
    else if (pageNumber == 4) {
      showMotorPosition(30);
      hideMotorPosition(160);
      drawInfo(30, 75, "enterTrainingMode");
      lightsensorFlag = false;
    }
    else if (pageNumber == 5) {
      drawInfo(30, 75, "train3");
    }
    else if (pageNumber == 6) {
      drawInfo(30, 75, "RUN");
    }
    else if (pageNumber == 7) {
      drawInfo(30, 75, "DONE");
    }

  }
  if (choiceSensor == 2) {
    if (pageNumber == 0) {
      drawInfo(30, 75, "connectMotor");
      showMotorPosition(30);
      tft.setTextSize(2);
      tft.setTextColor(TFT_BLACK, infoBG);
      tft.drawString("<<MOTOR", 80 - tft.textWidth("MOTOR") / 2 , 210);
    }
    else if (pageNumber == 1) {
      drawInfo(30, 75, "train1");
    }
    else if (pageNumber == 2) {
      drawInfo(30, 75, "train2");
    }
    else if (pageNumber == 3) {
      showMotorPosition(160);
      tft.setTextSize(2);
      tft.setTextColor(TFT_BLACK, infoBG);
      tft.drawString("ROTARY SENSOR>>", 80 - tft.textWidth("ROTARY SENSOR") / 2 , 225);
      drawInfo(30, 75, "connectPOT");
      potFlag = false;
    }
    else if (pageNumber == 4) {
      drawInfo(30, 75, "POT");
      potFlag = true;
    }
    else if (pageNumber == 5) {
      drawInfo(30, 75, "enterTrainingMode");
      potFlag = false;
    }
    else if (pageNumber == 6) {
      drawInfo(30, 75, "train3");
    }
    else if (pageNumber == 7) {
      drawInfo(30, 75, "RUN");
    }
    else if (pageNumber == 8) {
      drawInfo(30, 75, "DONE");
    }
  }
  if (choiceSensor == 3) {
    if (pageNumber == 0) {
      drawInfo(30, 75, "connectMotor");
      showMotorPosition(30);
      tft.setTextSize(2);
      tft.setTextColor(TFT_BLACK, infoBG);
      tft.drawString("<<MOTOR", 80 - tft.textWidth("MOTOR") / 2 , 215);
    }
    else if (pageNumber == 1) {
      drawInfo(30, 75, "train1");
    }
    else if (pageNumber == 2) {
      drawInfo(30, 75, "train2");
      accelFlag = false;
    }
    else if (pageNumber == 3) {
      drawInfo(30, 75, "Accelerometer");
      accelFlag = true;
    }
    else if (pageNumber == 4) {
      drawInfo(30, 75, "enterTrainingMode");
      accelFlag = false;
    }
    else if (pageNumber == 5) {
      drawInfo(30, 75, "train3");
    }
    else if (pageNumber == 6) {
      drawInfo(30, 75, "RUN");
    }
    else if (pageNumber == 7) {
      drawInfo(30, 75, "DONE");
    }

  }

}


void wiodisplay() {

  if (state == "run") {
    tft.fillScreen(runBG);

    tft.fillRect(-20, -10, 50, 45, (buttonBG));
    tft.fillRect(65, -10, 50, 45, (buttonBG));
    //tft.fillRect(150,-10,50,45,(blue));

    tft.drawXBitmap(4, 3, QUIT, 32, 30, TFT_RED);
    tft.drawXBitmap(74, 3, GRAPH1, 32, 30, TFT_WHITE);
    tft.drawXBitmap(74, 3, GRAPH2, 32, 30, 20 << 11 | 25 << 6 | 25);
    tft.drawXBitmap(74, 3, GRAPH3, 32, 30, red);
    //tft.drawXBitmap(159,3,SAVE, 32, 30,TFT_GREEN);

    tft.drawString(" Running Mode", 220, 5 );

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


void drawGraph() {

  tft.setTextColor(textColor, textColor);
  tft.setTextSize(1);
  tft.drawLine(50, 220, 300, 220, TFT_RED);
  tft.drawLine(75, 220, 75, 230, TFT_RED);
  tft.drawLine(255, 220, 255, 230, TFT_RED);
  tft.drawString(yaxisI1[choiceMotor - 1], 72, 230 );
  tft.drawString(yaxisI2[choiceMotor - 1], 252, 230 );
  tft.drawString(yaxis[choiceMotor - 1], 110, 230 );
  tft.drawLine(50, 50, 50, 220, TFT_RED);
  tft.drawLine(40, 220, 50, 220, TFT_RED);
  tft.drawLine(40, 70, 50, 70, TFT_RED);
  tft.drawString("MIN", 20, 220 );
  tft.drawString("MAX", 20, 70 );
  tft.setRotation(2);

  tft.drawString(xaxis[choiceSensor - 1], 50, 30 );

  tft.setRotation(3);
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
  if (DCount > 0) {
    for (int i = 0; i < DCount - 1; i++) {
      tft.fillCircle(75 + training[i][0], 220 - training[i][1], 5, dataBG);
    }
    tft.fillCircle(75 + training[DCount - 1][0], 220 - training[DCount - 1][1], 5, dataBGLatest);
  }

}

void reset_wio() { //resets all the training data to 0
  DCount = 0;
  state = "train";
  state_flag = true;
  myservo1.write(90);
  myservo2.write(90);
  pos = 90;
  i = 0;

  sx = 0; //sum of x
  sy = 0; //sum of y

  sxsq = 0; //sum of square of x
  sxy = 0; //sum of product of x and y
  tft.fillScreen(menuBG);
}




int showmotor(int pos) {


  if (DCount > 0) {
    tft.fillCircle(75 + old_pos, 220 - old_brightness, 5, runBG);
    for (int i = 0; i < DCount; i++) {
      tft.fillCircle(75 + training[i][0], 220 - training[i][1], 5, dataBG);
    }

    tft.fillCircle(75 + pos, 220 - brightness, 5, dataBGLatest);

  }
}


void moveMotor() {

  if (digitalRead(WIO_5S_LEFT) == LOW) {
    while (digitalRead(WIO_5S_LEFT) == LOW) {
      count = count + 1;
      pos = pos - inc ;
      myservo1.write(pos);
      myservo2.write(pos);
      //Play(pos,30);

      pos = max(0, min(pos, 180));
      if (count > 5) {
        inc = min(10, count);
      }
      delay(100);

    }
    displayTraining();
    count = 0;
    inc = 1;
  }
  else if (digitalRead(WIO_5S_RIGHT) == LOW) {
    while (digitalRead(WIO_5S_RIGHT) == LOW) {
      count = count + 1;
      pos = pos + inc ;
      myservo1.write(pos);
      myservo2.write(pos);
      //Play(pos,30);

      pos = max(0, min(pos, 180));
      if (count > 5) {
        inc = min(10, count);
      }
      delay(100);

    }
    displayTraining();
    count = 0;
    inc = 1;
  }
}


void showLight() {
  if (lightsensorFlag == true) {
    brightness = analogRead(WIO_LIGHT);
    brightness = 65 * (1 - pow(2, -1 * float(brightness) / 100));
    if (abs(brightness - old_brightness) > 2) {
      tft.fillCircle(280, 50, old_brightness, infoBG);
      tft.fillCircle(280, 50, brightness, TFT_WHITE);
      old_brightness = brightness;
    }
  }

  if (potFlag == true) {
    brightness = analogRead(ROTARY);
    brightness = ( (float)brightness / 1023 * 65);
    if (abs(brightness - old_brightness) > 2) {
      tft.fillCircle(280, 50, old_brightness, infoBG);
      tft.fillCircle(280, 50, brightness, TFT_WHITE);
      old_brightness = brightness;
    }
  }

  if (accelFlag == true) {
    brightness = lis.getAccelerationX() * 30 + 030;
    if (abs(brightness - old_brightness) > 2) {
      tft.fillCircle(280, 50, old_brightness, infoBG);
      tft.fillCircle(280, 50, brightness, TFT_WHITE);
      old_brightness = brightness;
    }
  }
}





int smartMotor(int choiceSensor, int choiceModel) {
  wiodisplay();
  while (digitalRead(WIO_KEY_C)) {


    if (choiceSensor == 1) {
      brightness = analogRead(WIO_LIGHT);
      brightness = 150 * (1 - pow(2, -1 * float(brightness) / 300));
    }
    else if (choiceSensor == 2) {
      brightness = analogRead(ROTARY);
      brightness = ( (float)brightness / 1023 * 150);

    }

    else if (choiceSensor == 3) {
      brightness = lis.getAccelerationX() * 75 + 75;

    }

    if (abs(old_brightness - brightness) > 10 || old_pos != pos) {
      tft.fillRect(70 + old_pos, 220 - old_brightness, 10, old_brightness, trainBG);
      tft.fillRect(70 + pos, 220 - brightness, 10, brightness, graphBG);
      old_brightness = brightness;
      old_pos = pos;
      displayTraining();

    }





    moveMotor();




    if (digitalRead(WIO_KEY_B) == LOW) {
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
        tft.fillCircle(75 + training[DCount - 1][0], 220 - training[DCount - 1][1], 5, trainBG);
        DCount = DCount - 1;
        displayTraining();

      }

    }
    if (digitalRead(WIO_5S_PRESS) == LOW) {


      sx = sx + brightness;
      sy = sy + pos;


      sxy = sxy + brightness * pos;
      sxsq = sxsq + brightness * brightness;

      buzz(1);
      training[DCount][0] = pos;
      training[DCount][1] = brightness;
      i = i + 1;
      DCount += 1;

      displayTraining();
    }

    if (digitalRead(WIO_KEY_A) == LOW) {
      state = "run";
      state_flag = true;
      wiodisplay();
      buzz(3);

      m = float((DCount * sxy - sx * sy)) / float((DCount * sxsq - sx * sx)); // calculate the slope
      b = float((sy - m * sx)) / float(DCount);              //calculate the intercept


      Serial.println("slope and intercept");
      Serial.println(DCount);
      Serial.println(m);
      Serial.println(b);

      while (digitalRead(WIO_KEY_C)) { //pressing the button will escape the run mode

        if (digitalRead(WIO_KEY_B) == LOW) {
          delay(100);
          if (graphMode == false) {
            tft.fillScreen(runBG);
            tft.fillRect(65, -10, 50, 45, (buttonBG));
            tft.drawXBitmap(74, 3, QUIT, 32, 30, TFT_RED);
            drawGraph();
            graphMode = true ;
            showmotor(pos);
          }
          else {
            delay(100);
            wiodisplay();
            graphMode = false ;
          }

        }

        if (choiceSensor == 1) {
          brightness = analogRead(WIO_LIGHT);
          brightness = 150 * (1 - pow(2, -1 * float(brightness) / 300));
        }
        else if (choiceSensor == 2) {
          brightness = analogRead(ROTARY);
          brightness = ( (float)brightness / 1023 * 150);


        }
        else if (choiceSensor == 3) {
          brightness = lis.getAccelerationX() * 75 + 75;

        }

        if (choiceModel == 1) {
          mini = 1000;
          pos = 90;
          for (int i = 0; i < DCount; i++) {
            dist = abs(brightness - training[i][1]);
            if (dist < mini) {
              mini = dist;
              pos = training[i][0];
            }
          }

          if (pos != old_pos & graphMode == true) {

            showmotor(pos);
            old_brightness = brightness;
            old_pos = pos;
          }
        }

        else if (choiceModel == 2) {


          pos = float(brightness)  * m + b;
          if (pos != old_pos & graphMode == true) {

            showmotor(pos);
            old_brightness = brightness;
            old_pos = pos;
          }
          Serial.println("Position and brightness");
          Serial.println(brightness);
          Serial.println(pos);

        }

        myservo1.write(pos);
        myservo2.write(pos);
        delay(100);
      }
    }
  }
  reset_wio();


}
