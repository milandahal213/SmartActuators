#include "Servo.h"
#include"TFT_eSPI.h"
#include"LIS3DHTR.h"
#include"icons.h"
#include "Encoder.h"

Encoder knob(D1, D0);

LIS3DHTR<TwoWire> lis;
#include<arduino.h>

#define LCD_BACKLIGHT (72Ul) // Control Pin of LCD
//Servo myservo1;
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

int choiceSensor = 3;
int choiceMotor = 1;
int choiceModel = 1;
//running mode

bool graphMode = true;
//training variables
int training[50][2];
int inc = 1;
int count = 0;
int pos = 90;
int old_pos = -999;
long brightness = 0;
long old_brightness = 0;

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
  //myservo1.attach(SCL); //for Grove Servos
  //pinMode(SPEAKER, OUTPUT);  //Speaker

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
  menu(); // main loop
}



int menu() {
  //menu_shell 1 is sensor select, 2 is motor type selection, 3 is main menu, 4 is training type
  menu_shell(3); //

  while (1) {
    //left most is for training page
    if (digitalRead(WIO_KEY_C) == LOW) {
      go_down(x1, "A");
      while (digitalRead(WIO_KEY_C) == LOW);
      go_up(x1, "A");
      smartMotor(choiceSensor);
      break;
    }

    //middle one is for sensor selector page
    if (digitalRead(WIO_KEY_B) == LOW) {
      go_down(x2, "B");
      while (digitalRead(WIO_KEY_B) == LOW);
      go_up(x2, "B");
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

  tft.fillRect(x1 - 25, 0, 50, h, (buttonBG));
  tft.fillRect(x2 - 25, 0, 50, h, (buttonBG));


  tft.drawString("A", x1 - tft.textWidth("A") / 2, 3);
  tft.drawString("B", x2 - tft.textWidth("B") / 2, 3);


  tft.fillRect(x1, h, 5, h + 20, (buttonBG));
  tft.fillRect(x2, h, 5, h + 20, (buttonBG));


  tft.fillRect(x1, 2 * h + 20, 70 - x1, 5, (buttonBG));
  tft.fillRect(x2, 2 * h + 20, 160 - x2, 5, (buttonBG));


  tft.fillRect(70, 2 * h + 20, 5, 30, (buttonBG));
  tft.fillRect(160, 2 * h + 20, 5, 40, (buttonBG));


  tft.fillRect(30, 3 * h + 10, 80, h * 3 - 10, (buttonBG));
  tft.fillRect(120, 3 * h + 10, 80, h * 3 - 10, (buttonBG));


  if (menuStage == 1) { //sensor type
    tft.fillRect(x3 - 25, 0, 50, h, (buttonBG));
    tft.drawString("C", x3 - tft.textWidth("A") / 2, 3);
    tft.fillRect(x3, h, 5, h + 20, (buttonBG));
    tft.fillRect(x3, 2 * h + 20, 250 - x3, 5, (buttonBG));
    tft.fillRect(250, 2 * h + 20, 5, 50, (buttonBG));
    tft.fillRect(210, 3 * h + 10, 80, h * 3 - 10, (buttonBG));
    tft.drawRect(20, 3 * h, 280, h * 3 + 10, (buttonBG));
    tft.drawXBitmap(38, 3 * h + 20, MOTORLIGHT, 64, 60, MODE1Color);
    tft.drawXBitmap(128, 3 * h + 20, TILT, 64  , 60, MODE2Color);
    tft.drawXBitmap(218, 3 * h + 20, ROTORY, 64, 60, MODE3Color);
    tft.fillRect(250, 3*h+80, 5, h + 20, (buttonBG));
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
      go_down(x1, "A");
      while (digitalRead(WIO_KEY_C) == LOW);
      go_up(x1, "A");
      choiceSensor = 1;
      break;

    }

    if (digitalRead(WIO_KEY_B) == LOW) {
      go_down(x2, "B");
      while (digitalRead(WIO_KEY_B) == LOW);
      go_up(x2, "B");
      choiceSensor = 3;
      break;

    }
    if (digitalRead(WIO_KEY_A) == LOW) {
      go_down(x3, "C");
      while (digitalRead(WIO_KEY_A) == LOW);
      go_up(x3, "C");
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




void wiodisplay() {

  if (state == "run") {
    tft.fillScreen(runBG);

    tft.fillRect(-20, -10, 50, 45, (buttonBG));
    tft.fillRect(65, -10, 50, 45, (buttonBG));


    tft.drawXBitmap(4, 3, QUIT, 32, 30, TFT_RED);
    tft.setTextColor(TFT_WHITE, TFT_WHITE);
    tft.setTextSize(2);
    tft.drawString("f(x)", x2 - tft.textWidth("f(x)") / 2, 8);
    tft.setTextSize(1);
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
  //myservo1.write(90);
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
      //myservo1.write(pos);
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
      //myservo1.write(pos);
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


void mainScreenLevel() {
  if (choiceSensor == 1) {
    brightness = analogRead(WIO_LIGHT);
    brightness = 150 * (1 - pow(2, -1 * float(brightness) / 100));
  }

  if (choiceSensor == 2) {
    brightness = knob.read();
    if (brightness >= 150) {
      knob.write(149);
    }
    else if (brightness < 0) {
      knob.write(0);
    }
  }

  if (choiceSensor == 3) {
    brightness = lis.getAccelerationX() * 75 + 075;
  }
  if (abs(brightness - old_brightness) > 2) {
    tft.fillRect(240, 220-old_brightness, 10, old_brightness, menuBG);
    tft.fillRect(240, 220-brightness, 10, brightness, TFT_WHITE);
    old_brightness = brightness;
  }
}





int smartMotor(int choiceSensor) {
  wiodisplay();
  while (digitalRead(WIO_KEY_C)) {
    if (choiceSensor == 1) {
      brightness = analogRead(WIO_LIGHT);
      brightness = 150 * (1 - pow(2, -1 * float(brightness) / 300));
    }
    else if (choiceSensor == 2) {
      brightness = knob.read();
      if (brightness > 150) {
        knob.write(149);
      }
      else if (brightness < 0) {
        knob.write(0);
      }

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

/*
      sx = sx + brightness;
      sy = sy + pos;


      sxy = sxy + brightness * pos;
      sxsq = sxsq + brightness * brightness;
*/

      sx = sx + pos;
      sy = sy + brightness;


      sxy = sxy + pos * brightness;
      sxsq = sxsq + pos * pos;


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
      old_pos = -99;
      m = float((DCount * sxy - sx * sy)) / float((DCount * sxsq - sx * sx)); // calculate the slope
      b = float((sy - m * sx)) / float(DCount);              //calculate the intercept

      Serial.println(b);
      Serial.println(m);

      while (digitalRead(WIO_KEY_C)) { //pressing the button will escape the run mode
        drawGraph();
        if (digitalRead(WIO_KEY_B) == LOW) {
          while (digitalRead(WIO_KEY_B) == LOW) ;
          trainSelectorpage();
          wiodisplay();

        }

        if (choiceSensor == 1) {
          brightness = analogRead(WIO_LIGHT);
          brightness = 150 * (1 - pow(2, -1 * float(brightness) / 300));
        }
        else if (choiceSensor == 2) {
          brightness = knob.read();
          if (brightness > 150) {
            knob.write(149);
          }
          else if (brightness < 0) {
            knob.write(0);
          }

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

          tft.drawLine(75+ float(-b)/float(m) ,220, 75+ float((150-b))/float(m),70,TFT_GREEN);

          pos = float(brightness-b)/float(m );
          if (pos != old_pos & graphMode == true) {
            showmotor(pos);
            old_brightness = brightness;
            old_pos = pos;
          }

        }

        //myservo1.write(pos);
        myservo2.write(pos);
        delay(100);
      }
    }
  }
  reset_wio();

}
