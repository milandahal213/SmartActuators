#include"TFT_eSPI.h"
#include "RTC_SAMD51.h"
#include "DateTime.h"
RTC_SAMD51 rtc;
DateTime now;

#define ROTARY A0
int prev_time, currenttime;

int tData[30][2];
int trainingData[30][2];
int count = 0;
int tDataLength = 0;


int inactiveBG=TFT_WHITE;
int activeBG=TFT_YELLOW;
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

int oldx = 0;
int oldy = 0;

int oldLight = 0;
int oldMotor = 0;

int _index[2] = {2, 1};


//colors
int pagebg = TFT_LIGHTGREY;
int mainpageBG=50712;
#define Motor TFT_GREEN
int trainingBG = TFT_GREEN;


char ending[500];
char *SMmessage;

int x[16] = {5, 85, 165, 245, 5, 85, 165, 245, 5, 85, 165, 245, 5, 85, 165, 245};
int y[16] = {5, 5, 5, 5, 65, 65, 65, 65, 125, 125, 125, 125, 185, 185, 185, 185};

int menu[3][16] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1}, {0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0 }, {0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0 , 1, 0, 0, 0, 1 }};
char menuWORD[3][6][20] = {{"TRAIN", "VIEW", "SETTINGS"}, {"VIEW", "Graph", " <- "}, {"View", "Graph", " X ", "+", "-", "Load"}};

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


void loadTrain() {
  //char *training = "training=[(30,300),(75,100)]\r\n";
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

  char *mainScript[] = {
    "import uos, machine\r\n",
    "import time\r\n",
    "import trainData\r\n",
    "name=\"Motor\"\r\n",
    "from machine import Pin, ADC\r\n",
    "TMConnect=Pin(5,Pin.OUT)\r\n",
    "TMConnect.on()\r\n",
    "LightSensor = ADC(0)\r\n",
    "BuiltInLED = Pin(2, Pin.OUT)\r\n",
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
    "\t\tprint(\"$",
    Name[0],
    "\",\"",
    Name[1],
    "\",bright,pos, \"*\")\r\n",
    "\t\ttime.sleep(.1)\r\n\r\n\r\n\r\n",

    "loop()\r\n"
  };
  Serial.print(training);
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
  delay(1000);

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



void wipeClean(char *board, char *nm) {
  char *training = "training=[(30,300),(75,100)]\r\n";
  char *mainScript[] = {
    "import uos, machine\r\n",
    "import time\r\n",
    "import trainData\r\n",
    "name=\"Motor\"\r\n",
    "from machine import Pin, ADC\r\n",
    "TMConnect=Pin(5,Pin.OUT)\r\n",
    "TMConnect.on()\r\n",
    "LightSensor = ADC(0)\r\n",
    "BuiltInLED = Pin(2, Pin.OUT)\r\n",
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
    "\t\tprint(\"$",
    nm,
    "\",\"",
    board,
    "\",bright,pos, \"*\")\r\n",
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


int drawEmptyBoxes(int menuNumber, int ind, int color, int selection_color) {
  int _ind = 0;
  for ( int i = 0; i < 16; i ++) {
    if (menu[menuNumber][i] == 1) {
      _ind = _ind + 1;
      if (_ind == ind) {
        tft.fillRect(x[i]+5, y[i]+50, w, 5, TFT_BLACK);
        tft.fillRect(x[i]+70, y[i]+5, 5, h, TFT_BLACK);
        tft.fillRect(x[i], y[i], w, h, selection_color);
      }
      else{
        tft.fillRect(x[i]+5, y[i]+50, w, 5, pagebg);
        tft.fillRect(x[i]+70, y[i]+5, 5, h, pagebg);

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
  strcpy(retValue, "");

  // parse name
  while (_ret[i] != ' ' and i < strlen(_ret)) {
    len = strlen(retValue);
    retValue[len] = _ret[i];
    retValue[len + 1] = '\0';
    i = i + 1;
  }

  strcpy(Name[0], retValue);

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

  tft.drawLine(30, 70, 30, 220, TFT_RED);
  tft.drawString("MAX", 5, 70 );
  tft.drawLine(25, 70, 35, 70, TFT_RED);
  tft.drawString("MIN", 5, 220 );

  tft.drawLine(25, 220, 220, 220, TFT_RED);
  tft.drawLine(35, 220, 35, 230, TFT_RED);
  tft.drawString("0", 32, 230 );
  tft.drawLine(215, 220, 215, 230, TFT_RED);
  tft.drawString("180", 212, 230 );
  //tft.drawLine(40, 220, 50, 220, TFT_RED);

  tft.drawString("MOTOR Position", 90, 230 );

  tft.setRotation(2);
  tft.drawString("LIGHT INTENSITY", 50, 10 );
  tft.setRotation(3);
  for (int i = 0; i < DC; i++) {
    tft.fillCircle(35 + trainingData[i][0], 220 - 150 * (1 - pow(2, -1 * float(trainingData[i][1]) / 300)) , 5, TFT_BLUE);
  }
}



void drawGraph(int DC) {


  tft.setTextColor(textColor, textColor);
  tft.setTextSize(1);

  tft.drawLine(30, 70, 30, 220, TFT_RED);
  tft.drawString("MAX", 5, 70 );
  tft.drawLine(25, 70, 35, 70, TFT_RED);
  tft.drawString("MIN", 5, 220 );

  tft.drawLine(25, 220, 220, 220, TFT_RED);
  tft.drawLine(35, 220, 35, 230, TFT_RED);
  tft.drawString("0", 32, 230 );
  tft.drawLine(215, 220, 215, 230, TFT_RED);
  tft.drawString("180", 212, 230 );
  //tft.drawLine(40, 220, 50, 220, TFT_RED);

  tft.drawString("MOTOR Position", 90, 230 );

  tft.setRotation(2);
  tft.drawString("LIGHT INTENSITY", 50, 10 );
  tft.setRotation(3);
  for (int i = 0; i < DC; i++) {
    tft.fillCircle(35 + tData[i][0], 220 - 150 * (1 - pow(2, -1 * float(tData[i][1]) / 300)) , 5, TFT_BLUE);
  }
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
  tft.fillRoundRect(X - 2, Y - 2, xc + radius2 + 4, heightRect1 + 4, 10, 32);
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
  } while (String(message).indexOf("*") < 0 && (rtc.now().unixtime() - prev_time) < 2);
  while (Serial1.available()) {
    Serial1.read();  //emptying everything else
  }
  if (String(message).indexOf("*") > 0) { //if the loop was not prematurely ended by timeout returns 1
    parseNameType(message);
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
    if ((rtc.now().unixtime() - prev_time) > 2) {
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
  } while (String(ending).indexOf(">>>") < 0 && (rtc.now().unixtime() - prev_time) < 2 );

  Serial1.flush();
  if (String(ending).indexOf(">>>") > 0) {
    return (ending);
  }
  else {
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



void setup() {
  pinMode(BCM18, INPUT);

  Serial1.begin(115200);
  Serial.begin(115200);

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


int drawGraphItems() {
  int t = scanSMData();
  tft.drawCircle(oldx, oldy, 5, TFT_GREEN);
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

int drawDashboardItems() {


  int xc = X + 135;
  int yc = Y + 80;
  int widthLight = widthRect2 - 5;
  int xR = X + 30 + int(widthRect2 / 2) - int(widthLight / 2);

  int t = scanSMData();
  int light = 150 * (1 - pow(2, -1 * float(atoi(Name[2])) / 300));
  int motor = atoi(Name[3]);


  if (t == 1 && validConnection() == 1) {
    tft.fillRect( xR, 215 - oldLight, widthLight, oldLight, TFT_WHITE);
    tft.drawLine(xc, yc, xc - int(50 * cos(oldMotor * PI / 180)), yc - int(50 * sin(oldMotor * PI / 180)), TFT_WHITE);

    tft.drawLine(xc, yc, xc - int(50 * cos(motor * PI / 180)), yc - int(50 * sin(motor * PI / 180)), TFT_BLACK);
    tft.fillRect(xR, 215 - light, widthLight, light, TFT_RED);


    oldLight = light;
    oldMotor = motor;
  }

  return 1;
}


int light() {
  int i = 0;
  int len;
  _ret = sendESP("LightSensor.read()\r\n");

  strcpy(retValue, " ");

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
  //i = 150 * (1 - pow(2, -1 * float(atoi(retValue)) / 300));
  return (atoi(retValue));

}





int drawGraphItemsTrain(int light, int motor) {

  tft.drawCircle(oldx, oldy, 5, trainingBG);
  tft.drawCircle(35 + motor, 220 - 150 * (1 - pow(2, -1 * float(light) / 300)), 5, TFT_RED);
  oldx = 35 + motor;
  oldy = 220 - 150 * (1 - pow(2, -1 * float(light) / 300));
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

void loop() {
  int menu1State = 0;
  if (digitalRead(WIO_KEY_B) == LOW) {
    wipeClean("Motor", "Twister");
  }

  if (digitalRead(BCM18) == LOW) { //connected to a SM system // triggers the scan and display function
    int t = scanSMData(); //displays the name of the motor and type == if the return is 0 something is wrong
    pagebg = mainpageBG;
    if (t == 1 && validConnection() == 1) {
      tft.fillScreen(pagebg);
      tft.setTextColor(TFT_WHITE, TFT_WHITE);
      tft.setTextSize(2);
      tft.drawString(Name[0], 10, 10);
      int nameWidth = tft.textWidth(Name[0]);
      tft.setTextSize(1);
      tft.setTextColor(textColor, textColor);
      tft.drawString(" is connected", nameWidth + 10 , 15);
      tft.drawString("You are a Smart ", 10 , 35);
      int nameWidth2 = tft.textWidth("You are a Smart ");
      tft.setTextSize(2);
      tft.setTextColor(TFT_PURPLE, TFT_PURPLE);
      tft.drawString(Name[1], 10 + nameWidth2 + 10, 30);
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
      }
      menu1State = _index[0];
      while (digitalRead(WIO_5S_PRESS) == LOW);


      // selection to train new data
      if (menu1State == 1 && digitalRead(BCM18) == LOW) {
        
        tft.fillScreen(TFT_GREEN);
        tft.drawString("Getting Data... ", 10, 100);
        delay(500);
        tft.fillScreen(trainingBG);
        pagebg = trainingBG;
        Serial1.print("\x03");
        //int DataCount = getDataFromSM(); //stop main loop_ get training data
        //runSM();         // restart main.py

        drawEmptyBoxes(2, 1, inactiveBG, activeBG);
        int menu2State = 1;
        GraphState = false;
        drawDashboard();
        Tlight = light();
        Tmotor = map(analogRead(ROTARY), 0, 1023, 0, 180);
        drawDashboardItemsTrain(Tlight, Tmotor);
        Serial.print("train new data");



        while (menu2State != 3 && digitalRead(BCM18) == LOW) {
          tft.drawString(String(Tcount), 10, 10, TFT_BLACK);
          if (digitalRead(WIO_5S_PRESS) == LOW) {
            while (digitalRead(WIO_5S_PRESS) == LOW);
            menu2State = _index[1];

            Serial.print("pressed");
            if (menu2State == 1) {
              tft.fillRect(0, 65, 244, 240, trainingBG);
              drawDashboard();
            }
            if (menu2State == 2) {
              tft.fillRect(0, 65, 244, 240, trainingBG);
              drawTrainingGraph(Tcount);    //draw the graph
            }
            if (menu2State == 4) {
              trainingData[Tcount][1] = Tlight;
              trainingData[Tcount][0] = Tmotor;
              Tcount += 1;
              if (GraphState == true) {
                tft.fillRect(0, 65, 244, 240, trainingBG);
                drawTrainingGraph(Tcount);    //draw the graph
              }
            }
            if (menu2State == 5) {
              Tcount -= 1;
              if (GraphState == true) {
                tft.fillRect(0, 65, 244, 240, trainingBG);
                drawTrainingGraph(Tcount);    //draw the graph
              }
            }
            if (menu2State == 6) {
              loadTrain();
              training = "training=[(30,300),(75,100)]\r\n";
              break;
            }

          }
          else if (digitalRead(WIO_5S_LEFT) == LOW && _index[1] > 1 ) {
            while (digitalRead(WIO_5S_LEFT) == LOW);
            _index[1] -= 1;
            drawEmptyBoxes(2, _index[1], inactiveBG, activeBG);

          }
          else if (digitalRead(WIO_5S_RIGHT) == LOW && _index[1] < 6) {
            while (digitalRead(WIO_5S_RIGHT) == LOW);
            _index[1] += 1;
            drawEmptyBoxes(2, _index[1], inactiveBG, activeBG);
          }



          //change the mode from Graph to Dashboard
          if (menu2State == 1) {
            GraphState = false;
          }

          //change mode from Dashboard to Graph
          else if (menu2State == 2) {
            GraphState = true;
          }

          else if (menu2State == 5) {


          }
          else if (menu2State == 6) {


            // drawTrainingGraph();  //grab data from SM and plot on the screen
          }

          //to show in Dashboard or Graph mode


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

      GraphState = false; //always start with Dashboard view
      // selection to display data
      if (menu1State == 2 && digitalRead(BCM18) == LOW) {
        tft.fillScreen(TFT_GREEN);
        tft.drawString("Getting Data... ", 10, 100);
        delay(500);
        tft.fillScreen(TFT_GREEN);
        Serial1.print("\x03");
        int DataCount = getDataFromSM(); //stop main loop_ get training data
        runSM();         // restart main.py
        drawEmptyBoxes(1, 1, inactiveBG, activeBG);
        int menu2State = 1;
        drawDashboard();

        while (menu2State != 3 && digitalRead(BCM18) == LOW) {
          if (digitalRead(WIO_5S_PRESS) == LOW) {
            while (digitalRead(WIO_5S_PRESS) == LOW);
            menu2State = _index[1];
            tft.fillRect(0, 65, 244, 240, TFT_GREEN);
            Serial.print("pressed");
            if (menu2State == 1) drawDashboard();
            if (menu2State == 2) drawGraph(DataCount);    //draw the graph
          }
          else if (digitalRead(WIO_5S_LEFT) == LOW && _index[1] > 1 ) {
            while (digitalRead(WIO_5S_LEFT) == LOW);
            _index[1] -= 1;
            drawEmptyBoxes(1, _index[1], inactiveBG, activeBG);

          }
          else if (digitalRead(WIO_5S_RIGHT) == LOW && _index[1] < 3) {
            while (digitalRead(WIO_5S_RIGHT) == LOW);
            _index[1] += 1;
            drawEmptyBoxes(1, _index[1],inactiveBG, activeBG);


          }
          if (menu2State == 1) {
            drawDashboardItems();  //grab data from SM and plot on the screen


          }
          else if (menu2State == 2) {
            drawGraphItems();  //grab data from SM and plot on the screen
          }

        }
        _index[0] = 2;
        _index[1] = 1;
      }

      if (_index[0] == 3 && digitalRead(BCM18) == LOW) {

        //settings menu - to be added
        tft.fillScreen(TFT_BLUE);
        Serial1.print("\x03");
        //training="training=[(65,10),(75,300)]\r\n";   //redefine training with default values
        Serial.println(training);
        wipeClean("Motor", "Twister");
      }
    }
    else
    {
      tft.fillScreen(TFT_RED);
      tft.drawString("ERROR IN THE CONNECTION", 10, 70);
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
