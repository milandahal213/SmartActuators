#include"TFT_eSPI.h"
#include "RTC_SAMD51.h"
#include "DateTime.h"
RTC_SAMD51 rtc;
DateTime now;
#define SPEAKER D1
#define ROTARY A0
int prev_time, currenttime;

int tData[30][2];
int trainingData[30][2];
int count = 0;
int tDataLength = 0;


int inactiveBG = TFT_WHITE;
int activeBG = TFT_YELLOW;
char Tspeed[5];
char Tbright[5];

int Tlight = 0;
int Tmotor = 0;
int Tcount = 0;

bool GraphState = false;
bool DashboardState = true;
#define w 70
#define h 50


int X = 12; //for Dashboard values
int Y = 70;
int widthRect2 = 15;

int oldx = 70;
int oldy = 70;

int oldLight = 70;
int oldMotor = 180;

int _index[5] = {2, 2, 2, 1, 1};


//colors
int pagebg = TFT_LIGHTGREY;
int mainpageBG = 50712;
#define Motor TFT_GREEN
int trainingBG = TFT_GREEN;
int viewBG = TFT_BLUE;
int dashboardBG = TFT_WHITE;
int confirmBG = TFT_PURPLE;
int settingsBG = TFT_MAGENTA;
int namelistBG = TFT_BLACK;

char ending[500];
char *SMmessage;
char boardSM[10][30] = {"Motor", "Speaker", "Color"};
char nameSM[10][30] = {"Orange", "DutchOven", "Ocean", "AprilAir", "Books"};

int x[16] = {5, 85, 165, 245, 5, 85, 165, 245, 5, 85, 165, 245, 5, 85, 165, 245};
int y[16] = {5, 5, 5, 5, 65, 65, 65, 65, 125, 125, 125, 125, 185, 185, 185, 185};
//int x[4][4] = {{5, 85, 165, 245}, {5, 85, 165, 245}, {5, 85, 165, 245}, {5, 85, 165, 245}};
//int y[4][4] = {{5, 5, 5, 5}, {65, 65, 65, 65}, {125, 125, 125, 125}, {185, 185, 185, 185}};

int menu[6][16] = { {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
  {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0 },
  {1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0 , 1, 0, 0, 0, 1 },
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};
//int menu[3][4][4] = {{{0, 0, 0, 1}, {0, 0, 0, 0},{ 0, 1, 1, 0},{ 0, 0, 0, 0}}, {{1, 0, 0, 1}, {0, 0, 0, 0}, {0, 0, 0 , 0},{ 0, 0, 0, 0} }, {{1, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0 , 1}, {0, 0, 0, 1 }}};

char menuWORD[6][16][20] = {{"SETTINGS", "TRAIN", "VIEW"},
  { "BACK", "TOGGLE VIEW"},
  {"CANCEL", "TOGGLE VIEW", "ADD", "DELETE", "TEST"},
  {"RETRAIN", "UPLOAD", "CANCEL"},
  {"CANCEL", "UPDATE", "RENAME"},
  {"Orange", "DutchOven", "Ocean", "AprilAir", "Books", "Crow", "Russel", "Pen", "paper", "Cutter", "Nepal", "Milan", "Fall", "Green", "Red", "Summer"}
};

#define LCD_BACKLIGHT (72Ul) // Control Pin of LCD
TFT_eSPI tft;
int sizeoflist = 3;
char list[3][10] = {"Motor", "Speaker", "Color"};

String messageFromESP;
int textColor = TFT_BLACK;
int Speed2Send = 0;
int oldSpeed2Send = 0;
int lightValue = 0;

char bufMessage[300];
String t;
char bufData[5];
char Speed[5];

char message[100];
char retValue[100]; //for the message from SM
char Name[4][10]; //data from the SM



char *ret;
char *_ret;
String request;


char *training = "training=[(30,300),(75,100)]\r\n";

void Test() {
  char *training;


  char Tspeed[5];
  char Tbright[5];
  training = "";
  strcpy(bufMessage, "");

  strcat(bufMessage, "training=[");
  for (int i = 0; i < Tcount; i++) {
    itoa(trainingData[i][0], Tspeed, 10);
    itoa(trainingData[i][1], Tbright, 10);

    strcat(bufMessage, "(");
    strcat(bufMessage, Tspeed);
    strcat(bufMessage, ",");
    strcat(bufMessage, Tbright);
    strcat(bufMessage, "),");
  }
  strcat(bufMessage, "]\r\n");
  training = bufMessage;

  sendESP(training);
  Serial1.println("loop()\r\n");

}


void resetSM() {
  //_ret=sendESP("\x03\r\n");
  //_ret=sendESP("\x04\r\n");
  //_ret=sendESP("\x04\r\n");
  //_ret=sendESP("\x04\r\n");
  //_ret=sendESP("\x04\r\n");
  Serial1.print("\x04\r\n");
  //_ret=sendESP("import main\r\n");
  //Serial1.print("loop()\r\n");

}
void loadTrain() {
  char *training = "";
  char Tspeed[5];
  char Tbright[5];

  strcpy(bufMessage, "");
  strcat(bufMessage, "training=[");

  for (int i = 0; i < Tcount; i++) {

    itoa(trainingData[i][0], Tspeed, 10);
    itoa(trainingData[i][1], Tbright, 10);

    strcat(bufMessage, "(");
    strcat(bufMessage, Tspeed);
    strcat(bufMessage, ",");
    strcat(bufMessage, Tbright);
    strcat(bufMessage, "),");
  }

  strcat(bufMessage, "]\r\n");
  training = bufMessage;


  Serial1.print("\x03\r\n");
  Serial1.print("f=open(\"trainData.py\",\"w\")\r\n\x05\r\n");
  Serial1.print("e='''\r\ndata=");
  Serial1.print(training);
  Serial1.print("\r\n");
  Serial1.print("'''");
  Serial1.print("\x04\r\n");
  Serial1.print("f.write(e)\r\n");
  Serial1.print("f.close()\r\n");
  Serial1.print("\r\n\r\n\r\n");
  Serial1.print("\x03\r\n");
  Serial1.print("import main\r\n");
  Serial1.print("main.loop()\r\n");


}


void renameSM(int boardSN, int nameSN) {
  Serial1.print("\x03\r\n");
  Serial1.print("f=open(\"name.py\",\"w\")\r\n\x05\r\n");
  Serial1.print("b='''\r\ntype=\"");
  Serial1.print(boardSM[boardSN]);
  Serial1.print("\"\r\n");
  Serial1.print("Name=\"");
  Serial1.print(menuWORD[5][nameSN - 1]);
  Serial1.print("\"\r\n");
  Serial1.print("'''");
  Serial1.print("\x04\r\n");
  Serial1.print("f.write(b)\r\n");
  Serial1.print("f.close()\r\n");

}




void wipeClean(char *board, char *nm) {
  char *training = "training=[(30,300),(75,100)]\r\n";
  char *mainScript[] = {
    "import uos, machine\r\n",
    "import time\r\n",
    "import trainData\r\n",
    "import name\r\n",
    "from machine import Pin, ADC\r\n",
    "TMConnect=Pin(5,Pin.OUT)\r\n",
    "TMConnect.off()\r\n",
    "LightSensor = ADC(0)\r\n",
    "p2 = machine.Pin(0)\r\n",
    "pwm2 = machine.PWM(p2)\r\n",
    "pwm2.freq(50)\r\n",
    "pwm2.duty(75)\r\n",
    "training=trainData.data\r\n",
    "def loop():\r\n",
    "\twhile True:\r\n",
    "\t\tbright = LightSensor.read()\r\n",
    "\t\tmin = 1000\r\n",
    "\t\tpos = 75\r\n",
    "\t\tfor (a,l) in training:\r\n",
    "\t\t\tdist = abs(bright - l)\r\n",
    "\t\t\tif dist < min:\r\n",
    "\t\t\t\tmin = dist\r\n",
    "\t\t\t\tpos = a\r\n",
    "\t\tpwm2.duty(pos)\r\n",
    "\t\tprint(\"$\",",
    "name.Name,",
    "name.type,",
    "bright,pos, \"*\")\r\n",
    "\t\ttime.sleep(.1)\r\n\r\n\r\n\r\n",

    "loop()\r\n"
  };

  Serial1.print("\x03\r\n");
  delay(100);
  Serial1.print("f=open(\"main.py\",\"w\")\r\n");
  delay(100);
  Serial1.print("\x05\r\n");
  delay(100);
  Serial1.print("a='''\r\n");
  delay(100);
  for (int i = 0; i < (sizeof(mainScript) / sizeof(mainScript[0])); i++) {
    Serial1.print(mainScript[i]);
    delay(100);
  }
  Serial1.print("'''");
  delay(100);
  Serial1.print("\x04\r\n");
  delay(100);
  Serial1.print("f.write(a)\r\n");
  delay(100);
  Serial1.print("f.close()\r\n");
  delay(5000);
  Serial1.print("f=open(\"name.py\",\"w\")\r\n\x05\r\n");
  delay(100);
  Serial1.print("b='''\r\ntype=\"");
  delay(100);
  Serial1.print(board);
  delay(100);
  Serial1.print("\"\r\n");
  delay(100);
  Serial1.print("Name=\"");
  delay(100);
  Serial1.print(nm);
  delay(100);
  Serial1.print("\"\r\n");
  delay(100);
  Serial1.print("'''");
  delay(100);
  Serial1.print("\x04\r\n");
  delay(100);
  Serial1.print("f.write(b)\r\n");
  delay(100);
  Serial1.print("f.close()\r\n");
  Serial1.print("f=open(\"trainData.py\",\"w\")\r\n\x05\r\n");
  Serial1.print("e='''\r\ndata=");
  Serial1.print(training);
  Serial1.print("\r\n");
  Serial1.print("'''");
  Serial1.print("\x04\r\n");
  Serial1.print("f.write(e)\r\n");
  Serial1.print("f.close()\r\n");
  Serial1.print("\r\n\r\n\r\n");


  Serial1.print("\x03\r\n");

  Serial1.print("import main\r\n");

  Serial1.print("main.loop()\r\n");


}
/*


  int drawEmptyBoxes(int menuNumber, int ind, int color, int selection_color) {
  int _ind = 0;
  for ( int i = 0; i < 4; i ++) {
    for (int j=0;j<4;j++){
    if (menu[menuNumber][i][j] == 1) {
      _ind = _ind + 1;
      if (_ind == ind) {
        tft.fillRect(x[i][j] + 5, y[i][j] + 50, w, 5, TFT_BLACK);
        tft.fillRect(x[i][j] + 70, y[i][j] + 5, 5, h, TFT_BLACK);
        tft.fillRect(x[i][j], y[i][j], w, h, selection_color);
      }
      else {
        tft.fillRect(x[i][j] + 5, y[i][j] + 50, w, 5, pagebg);
        tft.fillRect(x[i][j] + 70, y[i][j] + 5, 5, h, pagebg);

        tft.fillRect(x[i][j], y[i][j], w, h, color);
      }
      tft.setTextSize(1);
      tft.setTextColor(TFT_BLACK, TFT_BLACK);
      //tft.drawString("hello wsir ",10,20);
      tft.drawString(menuWORD[menuNumber][_ind - 1], x[i][j] + 35 - tft.textWidth(menuWORD[menuNumber][_ind - 1]) / 2, y[i][j] + 20);
    }
  }
  }
  }

*/

int drawEmptyBoxes(int menuNumber, int ind, int color, int selection_color) {
  int _ind = 0;
  for ( int i = 0; i < 16; i ++) {
    if (menu[menuNumber][i] == 1) {
      _ind = _ind + 1;
      if (_ind == ind) {
        tft.fillRect(x[i] + 5, y[i] + 50, w, 5, TFT_BLACK);
        tft.fillRect(x[i] + 70, y[i] + 5, 5, h, TFT_BLACK);
        tft.fillRect(x[i], y[i], w, h, selection_color);
      }
      else {
        tft.fillRect(x[i] + 5, y[i] + 50, w, 5, pagebg);
        tft.fillRect(x[i] + 70, y[i] + 5, 5, h, pagebg);

        tft.fillRect(x[i], y[i], w, h, color);
      }
      tft.setTextSize(1);
      tft.setTextColor(TFT_BLACK, TFT_BLACK);
      //tft.drawString("hello wsir ",10,20);
      tft.drawString(menuWORD[menuNumber][_ind - 1], x[i] + 35 - tft.textWidth(menuWORD[menuNumber][_ind - 1]) / 2, y[i] + 20);
    }
  }
}


int validConnection() {
  for ( int i = 0; i < sizeoflist; i++) {
    if (strcmp(Name[1], list[i]) == 0) {
      return 1;
    }
    else {
    }
  }
  return 0;
}

int parseNameType(char *_ret) {

  int len;
  int index = 0;
  int i = 0;


  // find the first character
  while (_ret[i] != '$' and i < strlen(_ret)) {
    i = i + 1;
  }

  index = i;
  i = i + 1;

  while (_ret[i] != ' ' and i < strlen(_ret)) {
    i = i + 1;
  }
  i = i + 1;
  strcpy(retValue, "");

  // parse name
  while (_ret[i] != ' ' and i < strlen(_ret)) {
    len = strlen(retValue);
    retValue[len] = _ret[i];
    retValue[len + 1] = '\0';
    i = i + 1;
  }

  strcpy(Name[0], retValue);
  Serial.println(Name[0]);

  strcpy(retValue, "");
  i = i + 1;
  // parse type
  while (_ret[i] != ' ' and i < strlen(_ret)) {
    len = strlen(retValue);
    retValue[len] = _ret[i];
    retValue[len + 1] = '\0';
    i = i + 1;
  }

  strcpy(Name[1], retValue);
  strcpy(retValue, "");

  i = i + 1;
  //parse value of light
  while (_ret[i] != ' ' and i < strlen(_ret)) {
    len = strlen(retValue);
    retValue[len] = _ret[i];
    retValue[len + 1] = '\0';
    i = i + 1;
  }

  strcpy(Name[2], retValue);
  strcpy(retValue, "");

  i = i + 1;

  //parse value of motor
  while (_ret[i] != '*' and i < strlen(_ret)) {

    len = strlen(retValue);

    retValue[len] = _ret[i];
    retValue[len + 1] = '\0';
    i = i + 1;
  }

  strcpy(Name[3], retValue);



}

//150*(1-pow(2,-1*float(brightness)/300));



void drawTrainingGraph(int DC) {


  tft.setTextColor(textColor, textColor);
  tft.setTextSize(1);

  tft.drawLine(30, 65, 30, 215, TFT_RED);
  tft.drawString("MAX", 7, 65 );
  tft.drawLine(25, 65, 35, 65, TFT_RED);
  tft.drawString("MIN", 7, 215 );

  tft.drawLine(25, 215, 220, 215, TFT_RED);
  tft.drawLine(35, 215, 35, 225, TFT_RED);
  tft.drawString("0", 32, 225 );
  tft.drawLine(215, 215, 215, 225, TFT_RED);
  tft.drawString("180", 212, 225 );
  //tft.drawLine(40, 220, 50, 220, TFT_RED);

  tft.drawString("MOTOR Position", 90, 225 );

  tft.setRotation(2);
  tft.drawString("LIGHT INTENSITY", 50, 10 );
  tft.setRotation(3);
  for (int i = 0; i < DC; i++) {
    tft.fillCircle(35 + trainingData[i][0], 215 - 150 * (1 - pow(2, -1 * float(trainingData[i][1]) / 300)) , 5, TFT_BLUE);
  }
}


int drawGraphItemsTrain(int light, int motor) {

  tft.drawCircle(oldx, oldy, 5, dashboardBG);
  tft.drawCircle(35 + motor, 220 - 150 * (1 - pow(2, -1 * float(light) / 300)), 5, TFT_RED);
  oldx = 35 + motor;
  oldy = 220 - 150 * (1 - pow(2, -1 * float(light) / 300));
  return 1;
}


void drawGraph(int DC) {


  tft.setTextColor(textColor, textColor);
  tft.setTextSize(1);

  tft.drawLine(30, 65, 30, 215, TFT_RED);
  tft.drawString("MAX", 5, 65 );
  tft.drawLine(25, 65, 35, 65, TFT_RED);
  tft.drawString("MIN", 5, 215 );

  tft.drawLine(25, 215, 220, 215, TFT_RED);
  tft.drawLine(35, 215, 35, 225, TFT_RED);
  tft.drawString("0", 32, 225 );
  tft.drawLine(215, 215, 215, 225, TFT_RED);
  tft.drawString("180", 212, 225 );
  //tft.drawLine(40, 220, 50, 220, TFT_RED);

  tft.drawString("MOTOR Position", 90, 225 );

  tft.setRotation(2);
  tft.drawString("LIGHT INTENSITY", 50, 10 );
  tft.setRotation(3);
  for (int i = 0; i < DC; i++) {
    tft.fillCircle(35 + tData[i][0], 215 - 150 * (1 - pow(2, -1 * float(tData[i][1]) / 300)) , 5, TFT_BLUE);
  }
}


int drawGraphItems() {
  int t = scanSMData();
  Serial.println(oldx);
  Serial.println(oldy);
  Serial.println(dashboardBG);
  tft.drawCircle(oldx, oldy, 5, dashboardBG);
  if (t == 0) {
    return 0;
  }
  if (t == 1 && validConnection() == 1) {
    tft.drawCircle(35 + atoi(Name[3]), 220 - 150 * (1 - pow(2, -1 * float(atoi(Name[2])) / 300)), 5, TFT_RED);
  }
  oldx = 35 + atoi(Name[3]);
  oldy = 220 - 150 * (1 - pow(2, -1 * float(atoi(Name[2])) / 300));
  return 1;
}





void drawDashboard() {

  tft.setTextSize(2);


  int heightRect2 = 150;
  int radius1 = 50;
  int radius2 = 80;




  int heightRect1 = heightRect2 + 10;
  int widthRect1 = widthRect2 + 35;
  int xc = X + 135;
  int yc = Y + 80;

  tft.fillRoundRect(X, Y, widthRect1, heightRect1, 10, TFT_BLUE);
  tft.drawRoundRect(X, Y, widthRect1, heightRect1, 10, TFT_BLACK);
  tft.fillRoundRect(X + 30, Y + 5, widthRect2, heightRect2, 10, TFT_WHITE);
  tft.fillCircle(xc, yc, radius2, TFT_BLUE);
  tft.drawCircle(xc, yc, radius2, TFT_BLACK);
  tft.fillCircle(xc, yc, radius1, TFT_WHITE);
  tft.setTextColor(TFT_WHITE, TFT_WHITE);
  tft.drawString("0 ", xc - 4, yc - radius1 - 25);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.drawString(Name[1], xc - 25, yc + radius1 + 8 );
  tft.setRotation(2);
  tft.drawString("LIGHT", 60, X + 5 );
  tft.setTextColor(TFT_WHITE, TFT_WHITE);
  tft.drawString("CW", 78, xc - radius1 - 25 );
  tft.setRotation(0);
  tft.drawString("CCW ", 138,  320 - xc - radius1 - 25 );
  tft.setRotation(3);

  tft.drawLine(xc - radius1 - 5, yc, xc - radius1 + 5, yc, TFT_RED);
  tft.drawLine(xc + radius1 - 5, yc, xc + radius1 + 5, yc, TFT_RED);
  tft.drawLine(xc, yc - radius1 - 5, xc, yc - radius1 + 5, TFT_RED);

  tft.setTextSize(1);
}

int drawDashboardItems() {


  int xc = X + 135;
  int yc = Y + 80;
  int widthLight = widthRect2 - 5;
  int xR = X + 30 + int(widthRect2 / 2) - int(widthLight / 2);

  int t = scanSMData();
  int light = 150 * (1 - pow(2, -1 * float(atoi(Name[2])) / 300));
  int motor = atoi(Name[3]);


  if (t == 1 && validConnection() == 1) {

    tft.drawLine(xc, yc, xc - int(50 * cos(oldMotor * PI / 180)), yc - int(50 * sin(oldMotor * PI / 180)), TFT_WHITE);
    tft.fillRect( xR, 215 - oldLight, widthLight, oldLight, TFT_WHITE);

    tft.drawLine(xc, yc, xc - int(50 * cos(   motor * PI / 180)), yc - int(50 * sin(   motor * PI / 180)), TFT_BLACK);
    tft.fillRect(xR, 215 - light, widthLight, light, TFT_RED);


    oldLight = light;
    oldMotor = motor;
  }

  return 1;
}



//Dashboard for Training menu
int drawDashboardItemsTrain(int Clight, int motor) {


  int xc = X + 135;
  int yc = Y + 80;
  int widthLight = widthRect2 - 5;
  int xR = X + 30 + int(widthRect2 / 2) - int(widthLight / 2);
  int light = 150 * (1 - pow(2, -1 * float(Clight) / 300));
  //int t = scanSMData();


  tft.fillRect( xR, 215 - oldLight, widthLight, oldLight, TFT_WHITE);
  tft.drawLine(xc, yc, xc - int(50 * cos(oldMotor * PI / 180)), yc - int(50 * sin(oldMotor * PI / 180)), TFT_WHITE);

  tft.drawLine(xc, yc, xc - int(50 * cos(motor * PI / 180)), yc - int(50 * sin(motor * PI / 180)), TFT_BLACK);
  tft.fillRect(xR, 215 - light, widthLight, light, TFT_RED);


  oldLight = light;
  oldMotor = motor;


  return 1;
}


int scanSMData() {
  char c;
  int len;
  strcpy(message, " ");
  delay(200);
  Serial1.flush();
  prev_time = rtc.now().unixtime();
  while (!Serial1.available()) // if nothing in the serial port
  { //add the timeout here
    // return when the timeouts
    if ((rtc.now().unixtime() - prev_time) > 2) {
      return 0;
    }
  }
  c = ' ';
  prev_time = rtc.now().unixtime();
  do {
    if (Serial1.available()) {
      c = Serial1.read();
      len = strlen(message);
      message[len] = c;
      message[len + 1] = '\0';
    }
  } while (String(message).indexOf("* ") < 0 && (rtc.now().unixtime() - prev_time) < 2);
  while (Serial1.available()) {
    Serial1.read();  //emptying everything else
  }
  if (String(message).indexOf("*") > 0) { //if the loop was not prematurely ended by timeout returns 1
    parseNameType(message);
    Serial.println(message);
    return 1;
  }
  else {
    Serial.print("Didn't get it");  // if something else is connected to it
    return 0;
  }
}

char  *sendESP(char *mess) {
  char c;
  int len;
  Serial1.flush();
  Serial1.print("pass\r\n");
  strcpy(ending, " ");
  delay(10);
  while (Serial1.available()) {
    c = Serial1.read();
  }
  Serial1.print(mess);
  Serial1.flush();

  prev_time = rtc.now().unixtime();
  while (!Serial1.available()) {
    if ((rtc.now().unixtime() - prev_time) > 1) {
      Serial.println("Stuck");
      return ("ERROR");
    }
  }

  c = ' ';
  prev_time = rtc.now().unixtime();
  do {
    if (Serial1.available()) {
      c = Serial1.read();
      len = strlen(ending);
      ending[len] = c;
      ending[len + 1] = '\0';
    }
  } while (String(ending).indexOf(">>>") < 0 && (rtc.now().unixtime() - prev_time) < 1 );

  Serial1.flush();
  if (String(ending).indexOf(">>>") > 0) {
    return (ending);
  }
  else {
    Serial.println("Stuck");
    return ("ERROR");
  }
}

int parseTData() {
  int len;
  int index = 0;
  int i = 0;
  int num_of_training_data = 0;

  // find the first character

  if (String(SMmessage).indexOf("[") < 0 || String(SMmessage).indexOf("]") < 0) {
    return 0;
  }

  while (SMmessage[i] != ']') {
    if (SMmessage[i] == ')') {
      num_of_training_data += 1;
    }
    i = i + 1;
  }
  i = 0;

  for ( int count = 0; count < num_of_training_data ; count++) {
    while (SMmessage[i] != '(' and i < strlen(SMmessage)) {
      i = i + 1;
    }
    i += 1;
    strcpy(retValue, " ");

    while (SMmessage[i] != ',' and i < strlen(SMmessage)) {
      len = strlen(retValue);
      retValue[len] = SMmessage[i];
      retValue[len + 1] = '\0';
      i += 1;
    }
    tData[count][0] = atoi(retValue);
    i += 1;
    strcpy(retValue, " ");
    while (SMmessage[i] != ')' and i < strlen(SMmessage)) {
      len = strlen(retValue);
      retValue[len] = SMmessage[i];
      retValue[len + 1] = '\0';
      i += 1;
    }

    tData[count][1] = atoi(retValue);
  }
  return num_of_training_data;
}

void buzz(int t) {
  for (int i = 0; i < t; i++) {
    analogWrite(WIO_BUZZER, 128);
    delay(200);
    analogWrite(WIO_BUZZER, 0);
    delay(200);
  }
}




int getDataFromSM() {
  int ret = 0;
  prev_time = rtc.now().unixtime();
  while (ret == 0) {
    Serial1.print("\x03");
    _ret = sendESP("import trainData\r\n");
    SMmessage = sendESP("trainData.data\r\n");
    ret = parseTData();
    if ((rtc.now().unixtime() - prev_time) > 2 ) {
      break;
    }
  }
  return ret;
}


void runSM() {
  sendESP("import main\r\n");
  Serial1.print("main.loop()\r\n");
}


int light() {
  int i = 0;
  int len;
  _ret = sendESP("LightSensor.read()\r\n");

  strcpy(retValue, " ");
  Serial.println(strlen(_ret));
  while (_ret[i] != '\n' and i < strlen(_ret)) {
    i += 1;
  }
  i = i + 1;

  while (_ret[i] != '\n' and i < strlen(_ret)) {

    len = strlen(retValue);
    retValue[len] = _ret[i];
    retValue[len + 1] = '\0';
    i += 1;
  }
  return (atoi(retValue));

}

void setup() {
  pinMode(BCM18, INPUT);

  Serial1.begin(115200);
  Serial.begin(115200);
  pinMode(WIO_5S_UP, INPUT);
  pinMode(WIO_5S_LEFT, INPUT);
  pinMode(WIO_5S_RIGHT, INPUT);
  pinMode(WIO_5S_DOWN, INPUT);
  pinMode(WIO_5S_PRESS, INPUT);
  pinMode(WIO_LIGHT , INPUT);
  pinMode(WIO_BUZZER, OUTPUT);
  pinMode(WIO_KEY_A, INPUT);
  pinMode(WIO_KEY_B, INPUT);
  pinMode(WIO_KEY_C, INPUT);

  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_PURPLE);

  rtc.begin();
  rtc.adjust(now);
}

void loop() {
  int menu1State = 0;

  //***** This will be removed *******//
  //-----------------------------------//

  if (digitalRead(WIO_KEY_B) == LOW) {
    wipeClean("Motor", "Twister");
  }
  //-----------------------------------//
  delay(500);

  if (digitalRead(BCM18) == LOW) {            //connected to a SM system // triggers the scan and display function
    Serial.println("Start");
    int t = scanSMData();                     //displays the name of the motor and type == if the return is 0 something is wrong
    pagebg = mainpageBG;
    if (t == 1 && validConnection() == 1) {

      //Sets up the first page //
      //-----------------------------------//

      tft.fillScreen(pagebg);
      tft.setTextColor(TFT_WHITE, TFT_WHITE);
      tft.setTextSize(2);
      tft.drawString(Name[0], 100, 10);
      int nameWidth = tft.textWidth(Name[0]);
      tft.setTextSize(1);
      tft.setTextColor(textColor, textColor);
      tft.drawString(" is connected", nameWidth + 100 , 15);
      tft.drawString("You are a Smart ", 100 , 35);
      int nameWidth2 = tft.textWidth("You are a Smart ");
      tft.setTextSize(2);
      tft.setTextColor(TFT_PURPLE, TFT_PURPLE);
      tft.drawString(Name[1], 10 + nameWidth2 + 100, 30);


      //-----------------------------------//


      drawEmptyBoxes(0, _index[0], inactiveBG, activeBG);

      //run in loop until the button is pressed - First menu

      while (digitalRead(WIO_5S_PRESS) == HIGH && digitalRead(BCM18) == LOW) {

        if (digitalRead(WIO_5S_LEFT) == LOW && _index[0] > 1 ) {
          while (digitalRead(WIO_5S_LEFT) == LOW);
          _index[0] -= 1;
          drawEmptyBoxes(0, _index[0], inactiveBG, activeBG);
        }
        if (digitalRead(WIO_5S_RIGHT) == LOW && _index[0] < 3) {
          while (digitalRead(WIO_5S_RIGHT) == LOW);
          _index[0] += 1;
          drawEmptyBoxes(0, _index[0], inactiveBG, activeBG);
        }
        if (digitalRead(WIO_5S_UP) == LOW && _index[0] > 1 ) {
          while (digitalRead(WIO_5S_UP) == LOW);
          _index[0] -= 1;
          drawEmptyBoxes(0, _index[0], inactiveBG, activeBG);
        }
        if (digitalRead(WIO_5S_DOWN) == LOW && _index[0] < 3) {
          while (digitalRead(WIO_5S_DOWN) == LOW);
          _index[0] += 1;
          drawEmptyBoxes(0, _index[0], inactiveBG, activeBG);
        }
      }
      menu1State = _index[0];
      while (digitalRead(WIO_5S_PRESS) == LOW);

      //**************************TRAINING MENU ************************************************//
      // selection to train new data
      if (menu1State == 2 && digitalRead(BCM18) == LOW) {
        tft.fillScreen(TFT_GREEN);
        tft.drawString("Getting Data... ", 10, 100);
        delay(500);
        tft.fillScreen(trainingBG);
        pagebg = trainingBG;
        Serial1.print("\x03");
        //int DataCount = getDataFromSM(); //stop main loop_ get training data
        //runSM();         // restart main.py

        drawEmptyBoxes(2, _index[1], inactiveBG, activeBG);
        int menu2State = 3;
        GraphState = false;

        Tlight = light();
        Tmotor = map(analogRead(ROTARY), 0, 1023, 0, 180);
        tft.fillRect(5, 60, 235, 175, dashboardBG);
        drawDashboard();
        drawDashboardItemsTrain(Tlight, Tmotor);

        /*
         * ***********************************
         *        *       *       *       *
           Cancel *       *       * VIEW
         *        *       *       *       *
         * ***********************************
         * ***********************************
         *        *       *       *       *
         *        *       *       *  ADD  *
         *        *       *       *       *
         * ***********************************
         * ***********************************
         *        *       *       *       *
         *        *       *       *DELETE *
         *        *       *       *       *
         * ***********************************
         * ***********************************
         *        *       *       *       *
         *        *       *       * TEST  *
         *        *       *       *       *
         * ***********************************

        */



        while (menu2State != 1 && digitalRead(BCM18) == LOW) {
          tft.drawString(String(Tcount), 100, 10);
          if (digitalRead(WIO_5S_PRESS) == LOW) {
            while (digitalRead(WIO_5S_PRESS) == LOW);
            menu2State = _index[1];
            if (menu2State == 2) {
              tft.fillRect(5, 60, 235, 175, dashboardBG);
              if (GraphState == true) {
                drawDashboard();
                GraphState = false;

              }
              else {
                drawTrainingGraph(Tcount);
                GraphState = true;
              }
              //drawDashboard();
            }

            if (menu2State == 3) { //
              trainingData[Tcount][1] = Tlight;
              trainingData[Tcount][0] = Tmotor;
              buzz(2);
              Tcount += 1;
              if (GraphState == true) {
                tft.fillRect(5, 60, 235, 175, dashboardBG);
                drawTrainingGraph(Tcount);    //draw the graph
              }
            }
            if (menu2State == 4) {  //Delete is pressed
              if (Tcount > 1) {
                Tcount -= 1;
              }
              buzz(1);
              if (GraphState == true) {
                tft.fillRect(5, 60, 235, 175, dashboardBG);
                drawTrainingGraph(Tcount);    //draw the graph
              }
            }
            if (menu2State == 5) { //Test is pressed

              /*
              * ***********************************
                CLEAR  *       *       *
                and    *       *       *
                retrain *       *       *
              * ***********************************
              * ***********************************
              *        *       *       *       *
              *        *       *       *       *
              *        *       *       *       *
              * ***********************************
              * ***********************************
              *        *       *       *       *
              *        * UPLOAD*Cancel *       *
              *        *       *       *       *
              * ***********************************
              * ***********************************
              *        *       *       *       *
              *        *       *       *       *
              *        *       *       *       *
              * ***********************************

              */
              bool breakcase = false;
              int menu3State = 0;
              _index[2] = 3;
              tft.fillScreen(confirmBG);
              pagebg = confirmBG;

              Test(); //sends test code for testing
              drawEmptyBoxes(3, 3, inactiveBG, activeBG);
              tft.setTextSize(2);
              tft.setTextColor(TFT_WHITE, TFT_WHITE);
              tft.drawString("TEST SCREEN", 80, 20);
              tft.setTextSize(1);

              while (menu3State != 3 && digitalRead(BCM18) == LOW) {

                if (digitalRead(WIO_5S_PRESS) == LOW) {
                  while (digitalRead(WIO_5S_PRESS) == LOW);
                  menu3State = _index[2];


                  if (menu3State == 2) {
                    tft.fillScreen(TFT_WHITE);
                    tft.fillRect( 50, 120, 220, 80, TFT_GREEN);
                    tft.setTextSize(2);
                    tft.drawString("TRAINING BOX...", 160 - tft.textWidth("TRAINING BOX...") / 2, 150);
                    tft.setTextSize(1);
                    tft.drawString("Please wait!!", 160 - tft.textWidth("Please wait!!") / 2, 180);
                    loadTrain();
                    training = "data=[(30,300),(75,100)]\r\n";

                    Serial1.print("\x03");
                    resetSM();
                    training = "data=[(30,300),(75,100)]\r\n";
                    delay(500);
                    scanSMData();
                    breakcase = true;

                    break;
                  }
                  else if (menu3State == 1) {
                    Tcount = 0;
                    break;
                  }
                }

                else if (digitalRead(WIO_5S_UP) == LOW && _index[2] > 1 ) {
                  while (digitalRead(WIO_5S_UP) == LOW);
                  _index[2] -= 1;
                  Serial.println(_index[2]);
                  drawEmptyBoxes(3, _index[2], inactiveBG, activeBG);

                }
                else if (digitalRead(WIO_5S_LEFT) == LOW && _index[2] > 1 ) {
                  while (digitalRead(WIO_5S_LEFT) == LOW);
                  _index[2] -= 1;
                  Serial.println(_index[2]);
                  drawEmptyBoxes(3, _index[2], inactiveBG, activeBG);

                }
                else if (digitalRead(WIO_5S_RIGHT) == LOW && _index[2] < 3) {
                  Serial.println("dfadfa");
                  while (digitalRead(WIO_5S_RIGHT) == LOW);
                  _index[2] += 1;
                  Serial.println(_index[2]);
                  drawEmptyBoxes(3, _index[2], inactiveBG, activeBG);
                }
                else if (digitalRead(WIO_5S_DOWN) == LOW && _index[2] < 3) {
                  while (digitalRead(WIO_5S_DOWN) == LOW);
                  _index[2] += 1;
                  Serial.println(_index[2]);
                  drawEmptyBoxes(3, _index[2], inactiveBG, activeBG);
                }


              }
              if (breakcase == true) {
                break;
              }
              else { //resetting training screen
                tft.fillScreen(trainingBG);
                pagebg = trainingBG;
                Serial1.print("\x03");
                drawEmptyBoxes(2, 3, inactiveBG, activeBG);
                menu2State = 3;
                _index[1] = 3;
                GraphState = false;
                Tlight = light();
                Tmotor = map(analogRead(ROTARY), 0, 1023, 0, 180);
                tft.fillRect(5, 60, 235, 175, dashboardBG);
                drawDashboard();
                drawDashboardItemsTrain(Tlight, Tmotor);

              }
            }
          }





          else if (digitalRead(WIO_5S_LEFT) == LOW && _index[1] > 1 ) {
            while (digitalRead(WIO_5S_LEFT) == LOW);
            _index[1] -= 1;
            drawEmptyBoxes(2, _index[1], inactiveBG, activeBG);

          }
          else if (digitalRead(WIO_5S_RIGHT) == LOW && _index[1] < 5) {
            while (digitalRead(WIO_5S_RIGHT) == LOW);
            _index[1] += 1;
            drawEmptyBoxes(2, _index[1], inactiveBG, activeBG);
          }

          else if (digitalRead(WIO_5S_UP) == LOW && _index[1] > 1 ) {
            while (digitalRead(WIO_5S_UP) == LOW);
            _index[1] -= 1;
            drawEmptyBoxes(2, _index[1], inactiveBG, activeBG);

          }
          else if (digitalRead(WIO_5S_DOWN) == LOW && _index[1] < 5) {
            while (digitalRead(WIO_5S_DOWN) == LOW);
            _index[1] += 1;
            drawEmptyBoxes(2, _index[1], inactiveBG, activeBG);
          }
          //Get data from rotary sensor
          Tlight = light();
          if (Tlight == 0) {
            Serial1.print("\x03");
          }
          Tmotor = map(analogRead(ROTARY), 0, 1023, 0, 180);
          if (abs(Tmotor - oldSpeed2Send) > 3) {
            itoa(Tmotor, Speed, 10);
            strcpy(bufMessage, "pwm2.duty(");
            strcat(bufMessage, Speed);
            strcat(bufMessage, ")\r\n");
            sendESP(bufMessage);
            oldSpeed2Send = Tmotor;
          }

          if (GraphState == true) {
            drawGraphItemsTrain(Tlight, Tmotor);
          }
          else {
            drawDashboardItemsTrain(Tlight, Tmotor);
          }


          delay(50);

        }
        runSM();         // restart main.py
        _index[0] = 2;
        _index[1] = 1;
        Tcount = 0;

      }

      GraphState = true; //always start with Graph view

      //**************************VIEW MENU ************************************************//
      /*
       * ***********************************
       *        *       *       *       *
         Back   *       *       * TOGGLE*
       *        *       *       *       *
       * ***********************************
       * ***********************************
       *        *       *       *       *
       *        *       *       *       *
       *        *       *       *       *
       * ***********************************
       * ***********************************
       *        *       *       *       *
       *        *       *       *       *
       *        *       *       *       *
       * ***********************************
       * ***********************************
       *        *       *       *       *
       *        *       *       *       *
       *        *       *       *       *
       * ***********************************

      */

      // selection to display data
      if (menu1State == 3 && digitalRead(BCM18) == LOW) {
        pagebg = viewBG;
        tft.fillScreen(viewBG);
        Serial1.print("\x03");
        tft.drawString("Getting Data... ", 10, 100);
        delay(500);
        tft.fillScreen(viewBG);
        int DataCount = getDataFromSM(); //stop main loop_ get training data
        runSM();         // restart main.py
        drawEmptyBoxes(1, 2, inactiveBG, activeBG);
        int menu2State = 2;
        _index[1] = 2;
        tft.fillRect(5, 60, 235, 175, dashboardBG);
        drawGraph(DataCount);

        while (menu2State != 1 && digitalRead(BCM18) == LOW) {
          if (digitalRead(WIO_5S_PRESS) == LOW) {
            while (digitalRead(WIO_5S_PRESS) == LOW);
            menu2State = _index[1];

            tft.fillRect(5, 60, 235, 175, dashboardBG);
            Serial.print("pressed");

            if (menu2State == 2) {
              tft.fillRect(5, 60, 235, 175, dashboardBG);
              if (GraphState == true) {
                drawDashboard();
                GraphState = false;

              }
              else {
                drawGraph(DataCount);
                GraphState = true;
              }
              //drawDashboard();
            }

          }
          else if (digitalRead(WIO_5S_LEFT) == LOW && _index[1] > 1 ) {
            while (digitalRead(WIO_5S_LEFT) == LOW);
            _index[1] -= 1;
            drawEmptyBoxes(1, _index[1], inactiveBG, activeBG);

          }
          else if (digitalRead(WIO_5S_RIGHT) == LOW && _index[1] < 2) {
            while (digitalRead(WIO_5S_RIGHT) == LOW);
            _index[1] += 1;
            drawEmptyBoxes(1, _index[1], inactiveBG, activeBG);
          }

          else if (digitalRead(WIO_5S_UP) == LOW && _index[1] > 1 ) {
            while (digitalRead(WIO_5S_UP) == LOW);
            _index[1] -= 1;
            drawEmptyBoxes(1, _index[1], inactiveBG, activeBG);

          }
          else if (digitalRead(WIO_5S_DOWN) == LOW && _index[1] < 2) {
            while (digitalRead(WIO_5S_DOWN) == LOW);
            _index[1] += 1;
            drawEmptyBoxes(1, _index[1], inactiveBG, activeBG);


          }

          if (GraphState == true) {
            drawGraphItems();  //grab data from SM and plot on the screen
          }
          else {
            drawDashboardItems();  //grab data from SM and plot on the screen
          }


        }
        _index[2] = 0;
        _index[0] = 2;
        _index[1] = 1;
      }
      GraphState = false; //always start with Dashboard view


      if (menu1State == 1 && digitalRead(BCM18) == LOW) {


        /*
        * ***********************************
          Cancel *       *       *
                 *       *       *
                 *       *       *
        * ***********************************
        * ***********************************
        *        *       *       *       *
        *        *       *       *       *
        *        *       *       *       *
        * ***********************************
        * ***********************************
        *        *       *       *       *
        *        * Update*Rename *       *
        *        *       *       *       *
        * ***********************************
        * ***********************************
        *        *       *       *       *
        *        *       *       *       *
        *        *       *       *       *
        * ***********************************

        */

        int menu4State = 2;

        tft.fillScreen(settingsBG);
        pagebg = settingsBG;
        _index[3] = 1;
        drawEmptyBoxes(4, _index[3], inactiveBG, activeBG);


        while (menu4State != 1 && digitalRead(BCM18) == LOW) {

          if (digitalRead(WIO_5S_PRESS) == LOW) {
            while (digitalRead(WIO_5S_PRESS) == LOW);
            menu4State = _index[3];


            if (menu4State == 2) {
              tft.fillScreen(TFT_BLUE);
              Serial1.print("\x03");
              training="data=[(65,10),(75,300)]\r\n";   //redefine training with default values
              Serial.println("Updating with new firmware");
              wipeClean("Motor", "Banana");
              break;
            }
            else if (menu4State == 3) {
              Serial.println("renaming with new name");
              tft.fillScreen(namelistBG);
              pagebg = namelistBG;
              drawEmptyBoxes(5, _index[4], inactiveBG, activeBG);

              while (digitalRead(WIO_5S_PRESS)) {
                if (digitalRead(WIO_5S_UP) == LOW && _index[4] > 1 ) {
                  while (digitalRead(WIO_5S_UP) == LOW);
                  _index[4] -= 1;
                  Serial.println(_index[4]);
                  drawEmptyBoxes(5, _index[4], inactiveBG, activeBG);

                }
                else if (digitalRead(WIO_5S_LEFT) == LOW && _index[4] > 1 ) {
                  while (digitalRead(WIO_5S_LEFT) == LOW);
                  _index[4] -= 1;
                  Serial.println(_index[4]);
                  drawEmptyBoxes(5, _index[4], inactiveBG, activeBG);

                }
                else if (digitalRead(WIO_5S_RIGHT) == LOW && _index[4] < 16) {
                  while (digitalRead(WIO_5S_RIGHT) == LOW);
                  _index[4] += 1;
                  Serial.println(_index[4]);
                  drawEmptyBoxes(5, _index[4], inactiveBG, activeBG);
                }
                else if (digitalRead(WIO_5S_DOWN) == LOW && _index[4] < 16) {
                  while (digitalRead(WIO_5S_DOWN) == LOW);
                  _index[4] += 1;
                  Serial.println(_index[4]);
                  drawEmptyBoxes(5, _index[4], inactiveBG, activeBG);
                }
              }
              while (digitalRead(WIO_5S_PRESS) == LOW);
              renameSM(0, _index[4]);
              resetSM();
              scanSMData();
              break;
            }

          }


          else if (digitalRead(WIO_5S_UP) == LOW && _index[3] > 1 ) {
            while (digitalRead(WIO_5S_UP) == LOW);
            _index[3] -= 1;
            Serial.println(_index[3]);
            drawEmptyBoxes(4, _index[3], inactiveBG, activeBG);

          }
          else if (digitalRead(WIO_5S_LEFT) == LOW && _index[3] > 1 ) {
            while (digitalRead(WIO_5S_LEFT) == LOW);
            _index[3] -= 1;
            Serial.println(_index[3]);
            drawEmptyBoxes(4, _index[3], inactiveBG, activeBG);

          }
          else if (digitalRead(WIO_5S_RIGHT) == LOW && _index[3] < 3) {
            Serial.println("dfadfa");
            while (digitalRead(WIO_5S_RIGHT) == LOW);
            _index[3] += 1;
            Serial.println(_index[3]);
            drawEmptyBoxes(4, _index[3], inactiveBG, activeBG);
          }
          else if (digitalRead(WIO_5S_DOWN) == LOW && _index[3] < 3) {
            while (digitalRead(WIO_5S_DOWN) == LOW);
            _index[3] += 1;
            Serial.println(_index[3]);
            drawEmptyBoxes(4, _index[3], inactiveBG, activeBG);
          }


        }
      }
    }


    else
    {
      tft.fillScreen(TFT_RED);
      tft.drawString("ERROR IN THE CONNECTION", 10, 70);
      strcpy(Name[0], "");
      strcpy(Name[1], "");
      strcpy(Name[2], "");
      strcpy(Name[3], "");
      wipeClean("Motor", "Hipster");

    }


    delay(100);

  }

  else {

  }
  tft.fillScreen(TFT_GREEN);
  strcpy(Name[0], "");
  strcpy(Name[1], "");
  strcpy(Name[2], "");
  strcpy(Name[3], "");
}
