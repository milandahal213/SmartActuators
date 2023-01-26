/*Code by : Milan Dahal
  Code written for: Wio Terminal
  The SmartMotor sends a string of $ name Type sensorValue MotorPosition * via serial port
  This code is written to parse that string and plot graph
  This code will also update the firmware that prints out the above information on ESP8266 by pressing button B or using update feature
  This code also lets the users update the trainingData.py file that contains the training data

*/



#include"TFT_eSPI.h"
#include "RTC_SAMD51.h"
#include "DateTime.h"
RTC_SAMD51 rtc;
DateTime now;
#define SPEAKER D1
#define ROTARY A2
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

int _index[6] = {2, 2, 2, 1, 1, 1};


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


int x[16] = {5, 85, 165, 245, 5, 85, 165, 245, 5, 85, 165, 245, 5, 85, 165, 245};
int y[16] = {5, 5, 5, 5, 65, 65, 65, 65, 125, 125, 125, 125, 185, 185, 185, 185};


int menu[7][16] = { {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
  {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0, 0, 0 },
  {1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0 , 1, 0, 0, 0, 1 },
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0}
};
//int menu[3][4][4] = {{{0, 0, 0, 1}, {0, 0, 0, 0},{ 0, 1, 1, 0},{ 0, 0, 0, 0}}, {{1, 0, 0, 1}, {0, 0, 0, 0}, {0, 0, 0 , 0},{ 0, 0, 0, 0} }, {{1, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 0 , 1}, {0, 0, 0, 1 }}};

char menuWORD[7][16][20] = {{"SETTINGS", "TRAIN", "VIEW"},
  { "BACK", "TOGGLE VIEW"},
  {"CANCEL", "TOGGLE VIEW", "ADD", "DELETE", "TEST"},
  {"RETRAIN", "UPLOAD", "CANCEL"},
  {"CANCEL", "UPDATE", "RENAME"},
  {"Orange", "DutchOven", "Ocean", "AprilAir", "Books", "Crow", "Russel", "Pen", "paper", "Cutter", "Nepal", "Milan", "Fall", "Green", "Red", "Summer"},
  {"SmartMotor", "SmartSpeaker", "SmartLight"}
};

#define LCD_BACKLIGHT (72Ul) // Control Pin of LCD
TFT_eSPI tft;
int sizeoflist = 3;
char list[3][10] = {"Motor", "Speaker", "Light"};

String messageFromESP;
int textColor = TFT_BLACK;
int Speed2Send = 0;
int oldSpeed2Send = 0;
int lightValue = 0;

char bufMessage[300];
String t;
char bufData[5];
char noteFreq[5];
char Speed[10];

char message[100];
char retValue[100]; //for the message from SM
char Name[4][10]; //data from the SM



char *ret;
char *_ret;
String request;


char *training = "training=[(50,900),(500,600)]\r\n";

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
  Serial1.print("\x04\r\n");
  delay(1000);
  getDataFromSM(); //stop main loop_ get training data
  runSM();         // restart main.py
}

void loadTrain() {
  char *training = "";
  char Tspeed[5];
  char Tbright[5];

  strcpy(bufMessage, "");
  strcat(bufMessage, "[");

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
  loadTrainScript(training);

}


void renameSM(int nameSN) {
  Serial1.print("\x03\r\n");
  Serial1.print("f=open(\"name.py\",\"w\")\r\n\x05\r\n");
  Serial1.print("b='''\r\ntype=\"");
  Serial1.print(Name[1]);
  Serial1.print("\"\r\n");
  Serial1.print("Name=\"");
  Serial1.print(menuWORD[5][nameSN - 1]);
  Serial1.print("\"\r\n");
  Serial1.print("'''");
  Serial1.print("\x04\r\n");
  Serial1.print("f.write(b)\r\n");
  Serial1.print("f.close()\r\n");

}


void loadTrainScript( char *training) {

  char *trainScript[] = {
    "\x03\r\n",
    "f=open(\"trainData.py\",\"w\")\r\n\x05\r\n",
    "e='''\r\ndata=",
    training,
    "\r\n",
    "'''",
    "\x04\r\n",
    "f.write(e)\r\n",
    "f.close()\r\n",
    "\r\n\r\n\r\n",
    "\x03\r\n",
    "import main\r\n",
    "main.loop()\r\n"

  };

  for (int i = 0; i < (sizeof(trainScript) / sizeof(trainScript[0])); i++) {
    Serial1.print(trainScript[i]);
    delay(100);
  }


}

void wipeClean(char *board, char *nm) {
  char *training = "training=[(50,900),(500,600)]\r\n";

  char *mainScript[] = {
    "\x03\r\n",
    "f=open(\"main.py\",\"w\")\r\n",
    "\x05\r\n",
    "a='''\r\n",
    "import uos, machine,neopixel\r\n",
    "from math import log\r\n",
    "import time\r\n",
    "import trainData\r\n",
    "import math\r\n",
    "training=trainData.data\r\n",
    "from machine import Pin, ADC\r\n",
    "TMConnect=Pin(5,Pin.OUT)\r\n",
    "TMConnect.off()\r\n",
    "Sensor = ADC(0)\r\n",
    "p2 = machine.Pin(0)\r\n",
    "import name\r\n",
    "if name.type==\"Motor\":\r\n",
    " pwm2 = machine.PWM(p2)\r\n",
    " pwm2.freq(50)\r\n",
    " pwm2.duty(75)\r\n",
    "elif name.type==\"Speaker\":\r\n",
    " pwm2 = machine.PWM(p2)\r\n",
    " pwm2.freq(50)\r\n",
    " pwm2.duty(75)\r\n",
    "elif name.type==\"Light\":\r\n",
    " np = neopixel.NeoPixel(p2, 8)\r\n\r\n\r\n",
    "def convert(x):\r\n",
    " return int((x * 119 / 1023) + 21)\r\n\r\n\r\n\r\n",
    "def getfreq(x):\r\n",
    " minFreq = 130.8\r\n",
    " maxFreq = 523.3\r\n",
    " pInc = 1.059463\r\n",
    " numNotes = int(log(maxFreq / minFreq) / log(pInc)) + 1\r\n",
    " TRange = 1023 / numNotes\r\n",
    " for i in range(numNotes):\r\n",
    "  if (x > 0 and x < TRange): \r\n",
    "   return (0)\r\n",
    "  elif (x > TRange * (i) and x < TRange * (i + 1)): \r\n",
    "   return int(minFreq * pow(pInc, (i - 1)))\r\n",
    " return -1\r\n",
    "def doSomething(d):\r\n",
    " if name.type==\"Motor\":\r\n",
    "  pwm2.duty(convert(d))\r\n",
    " elif name.type==\"Speaker\":\r\n",
    "  ff=getfreq(d)\r\n",
    "  print(ff)\r\n",
    "  pwm2.freq(ff)\r\n",
    " elif name.type==\"Light\":\r\n",
    "  l=0\r\n",
    "  r=0\r\n",
    "  g=0\r\n",
    "  b=0\r\n",
    "  allFlag=1\r\n",
    "  if d<23:\r\n",
    "   allFlag=0\r\n",
    "   pass\r\n",
    "  elif d>23 and d<123:\r\n",
    "   allFlag=0\r\n",
    "   l= math.floor(((d-23)*0.08))\r\n",
    "   r=255\r\n",
    "  elif d>123 and d<223:\r\n",
    "   allFlag=0\r\n",
    "   l= math.floor(((d-123)*0.08))\r\n",
    "   g=255\r\n",
    "  elif d>223 and d<323:\r\n",
    "   allFlag=0\r\n",
    "   l= math.floor(((d-223)*0.08))\r\n",
    "   b=225\r\n",
    "  elif d>323 and d<423:\r\n",
    "   r=math.floor(((d-323)*2.55))\r\n",
    "  elif d>423 and d<523:\r\n",
    "   r=255\r\n",
    "   g=math.floor(((d-423)*2.55))\r\n",
    "  elif d>523 and d<623:\r\n",
    "   r=255-math.floor(((d-523)*2.55))\r\n",
    "   g=255\r\n",
    "  elif d>623 and d<723:\r\n",
    "   g=255\r\n",
    "   b=math.floor(((d-623)*2.55))\r\n",
    "  elif d>723 and d<823:\r\n",
    "   g=255-math.floor(((d-723)*2.55))\r\n",
    "   b=255\r\n",
    "  elif d>823 and d<923:\r\n",
    "   r=math.floor(((d-823)*2.55))\r\n",
    "   b=255\r\n",
    "  elif d>923 and d<1024:\r\n",
    "   r=255\r\n",
    "   g=math.floor(((d-923)*2.55))\r\n",
    "   b=255\r\n",
    "  if allFlag:\r\n",
    "   for i in range(8):\r\n",
    "    np[i] = (r, g, b)\r\n",
    "  else:\r\n",
    "   for i in range(8):\r\n",
    "    np[i] = (0, 0, 0)\r\n",
    "   np[l] = (r, g, b)\r\n",
    "  np.write()\r\n",
    "def loop():\r\n",
    " while True:\r\n",
    "  sens = Sensor.read()\r\n",
    "  min = 1000\r\n",
    "  pos = 75\r\n",
    "  for (a,l) in training:\r\n",
    "   dist = abs(sens - l)\r\n",
    "   if dist < min:\r\n",
    "    min = dist\r\n",
    "    pos = a\r\n",
    "  doSomething(pos)   \r\n",
    "  print(\"$\",",
    "name.Name,",
    "name.type,",
    "sens,pos, \"*\")\r\n",
    "  time.sleep(.1)\r\n\r\n\r\n\r\n",
    "loop()\r\n",
    "'''",
    "\x04\r\n",
    "f.write(a)\r\n",
    "f.close()\r\n",
  };

  char *nameScript[] = {

    "f=open(\"name.py\",\"w\")\r\n\x05\r\n",
    "b='''\r\ntype=\"",
    board,
    "\"\r\n",
    "Name=\"",
    nm,
    "\"\r\n",
    "'''",
    "\x04\r\n",
    "f.write(b)\r\n",
    "f.close()\r\n",
  };

  for (int i = 0; i < (sizeof(mainScript) / sizeof(mainScript[0])); i++) {
    Serial1.print(mainScript[i]);
    delay(100);
  }
  for (int i = 0; i < (sizeof(nameScript) / sizeof(nameScript[0])); i++) {
    Serial1.print(nameScript[i]);
    delay(100);
  }
  loadTrainScript(training);
}


int getfreq() {
  float minFreq = 130.8; // can change speaker's min & max frequencies
  float maxFreq = 523.3;
  float pInc = 1.059463;
  int numNotes = (int)(log(maxFreq / minFreq) / log(pInc)) + 1; // number of notes between given frequency range
  int TRange = 1023 / numNotes; // potentiometer range per note
  int potValue = analogRead(ROTARY);

  for (int i = 1; i <= numNotes; i++) { // to get frequencies associated with each potentiometer range
    if (potValue > 0 and potValue < TRange) { // ******WILL THIS BE MESSED UP BC IT'S IN THE FOR LOOP?****** // reserving first pot range for freq = 0 (no sound)
      return (0);
    }
    else if (potValue > TRange * (i) and potValue < TRange * (i + 1)) { // checking if the pot value is big enough to change notes
      return (minFreq * pow(pInc, (i - 1))); // returns the note that that pot range corresponds to
    }
  }
  return -1;
}



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
      tft.drawString(menuWORD[menuNumber][_ind - 1], x[i] + 35 - tft.textWidth(menuWORD[menuNumber][_ind - 1]) / 2, y[i] + 20);
    }
  }
}


int validConnection() { //checks if the connected device returns with a proper name from the list
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

  tft.drawString(Name[1], 90, 225 );

  tft.setRotation(2);
  tft.drawString("Sensor Intensity", 50, 10 );
  tft.setRotation(3);
  for (int i = 0; i < DC; i++) {
    tft.fillCircle(35 + map(trainingData[i][0], 0, 1023, 0, 180), 215 - mapLightdata(float(trainingData[i][1])) , 5, TFT_BLUE);
  }
}


int drawGraphItemsTrain(int light, int motor) {

  tft.drawCircle(35 + map(oldx, 0, 1023, 0, 180), oldy, 5, dashboardBG);
  tft.drawCircle(35 + map(motor, 0, 1023, 0, 180), 220 - mapLightdata(float(light)), 5, TFT_RED);
  oldx = motor ;
  oldy = 220 - mapLightdata(float(light));
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

  tft.drawString(Name[1], 90, 225 );

  tft.setRotation(2);
  tft.drawString("Sensor Intensity", 50, 10 );
  tft.setRotation(3);
  for (int i = 0; i < DC; i++) {
    tft.fillCircle(35 + map(tData[i][0], 0, 1023, 0, 180), 215 - mapLightdata(float(tData[i][1])) , 5, TFT_BLUE);
  }
}

int drawGraphItems() {
  int t = scanSMData();
  tft.drawCircle(35 + map(oldx, 0, 1023, 0, 180), oldy, 5, dashboardBG);
  if (t == 0) {
    return 0;
  }
  if (t == 1 && validConnection() == 1) {
    tft.drawCircle(35 + map(atoi(Name[3]), 0, 1023, 0, 180), 220 - mapLightdata(float(atoi(Name[2]))), 5, TFT_RED);

  }
  oldx = atoi(Name[3]);
  oldy = 220 - mapLightdata( float(atoi(Name[2])));
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
  for (int dum = 0; dum < 182; dum = dum + 10) {
    tft.drawLine(xc, yc, xc - int(70 * cos(dum * PI / 180)), yc - int(70 * sin(dum * PI / 180)), TFT_GREEN);
  }
  tft.drawCircle(xc, yc, radius2, TFT_BLACK);
  tft.fillCircle(xc, yc, radius2 - 20, TFT_BLUE);
  tft.fillCircle(xc, yc, radius1, TFT_WHITE);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.drawString(Name[1], xc - 25, yc + radius1 + 8 );
  tft.setRotation(2);
  tft.drawString("Sensor", 60, X + 5 );
  tft.setTextColor(TFT_WHITE, TFT_WHITE);


  tft.setRotation(3);



  tft.setTextSize(1);
}

int drawDashboardItems() {


  int xc = X + 135;
  int yc = Y + 80;
  int widthLight = widthRect2 - 5;
  int xR = X + 30 + int(widthRect2 / 2) - int(widthLight / 2);

  int t = scanSMData();
  int light = mapLightdata(float(atoi(Name[2])));
  int motor = atoi(Name[3]);


  if (t == 1 && validConnection() == 1) {
    float angle = PI - map(oldMotor, 0, 1023, 0, 179) * PI / 180;
    tft.drawLine(xc, yc, xc - int(50 * cos(angle)), yc - int(50 * sin(angle)), TFT_WHITE);
    tft.fillCircle(xc - int(45 * cos(angle)), yc - int(45 * sin(angle)), 5, TFT_WHITE);
    tft.fillRect( xR, 215 - oldLight, widthLight, oldLight, TFT_WHITE);
    angle = PI - map(motor, 0, 1023, 0, 179) * PI / 180;
    tft.drawLine(xc, yc, xc - int(50 * cos( angle)), yc - int(50 * sin(angle)), TFT_BLACK);
    tft.fillCircle(xc - int(45 * cos(angle)), yc - int(45 * sin(angle)), 5, TFT_RED);
    tft.fillCircle(xc - int(45 * cos(angle)), yc - int(43 * sin(angle)), 1, TFT_WHITE);
    tft.fillRect(xR, 215 - light, widthLight, light, TFT_RED);


    oldLight = light;
    oldMotor = motor;
  }

  return 1;
}

float mapLightdata(float sensorVal) {
  return (135 * sensorVal / 1024);
}

//Dashboard for Training menu
int drawDashboardItemsTrain(int Clight, int motor) {


  int xc = X + 135;
  int yc = Y + 80;
  int widthLight = widthRect2 - 5;
  int xR = X + 30 + int(widthRect2 / 2) - int(widthLight / 2);
  int light = mapLightdata(float(Clight));
  //int t = scanSMData();

  float angle = PI - map(oldMotor, 0, 1023, 0, 179) * PI / 180;
  tft.fillRect( xR, 215 - oldLight, widthLight, oldLight, TFT_WHITE);
  tft.drawLine(xc, yc, xc - int(50 * cos(angle)), yc - int(50 * sin(angle)), TFT_WHITE);
  tft.fillCircle(xc - int(45 * cos(angle)), yc - int(45 * sin(angle)), 5, TFT_WHITE);
  angle = PI - map(motor, 0, 1023, 0, 179) * PI / 180;
  tft.drawLine(xc, yc, xc - int(50 * cos(angle)), yc - int(50 * sin(angle)), TFT_BLACK);
  tft.fillCircle(xc - int(45 * cos(angle)), yc - int(45 * sin(angle)), 5, TFT_RED);
  tft.fillCircle(xc - int(45 * cos(angle)), yc - int(43 * sin(angle)), 1, TFT_WHITE);
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
  _ret = sendESP("Sensor.read()\r\n");

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
  tft.fillScreen(TFT_GREEN);
  rtc.begin();
  rtc.adjust(now);
  tft.fillScreen(TFT_RED);
}

void loop() {
  delay(1000);
  Serial.print("Program Started");
  Serial1.print("\x04");
  int menu1State = 0;

  //***** This will be removed *******//
  //-----------------------------------//

  if (digitalRead(WIO_KEY_B) == LOW) {
    Serial.print("Pressed B");
    wipeClean(list[0], "Twister");
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
        Tmotor = map(analogRead(ROTARY), 0, 1023, 21, 140);
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
              if (Tcount > 0) {
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
                    tft.drawString("Updating Data...", 160 - tft.textWidth("Updating Data...") / 2, 150);
                    tft.setTextSize(1);
                    tft.drawString("Please wait!!", 160 - tft.textWidth("Please wait!!") / 2, 180);
                    loadTrain();
                    Serial1.print("\x03");
                    resetSM();
                    training = "training=[(50,900),(500,600)]\r\n";
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
                Tmotor = map(analogRead(ROTARY), 0, 1023, 21, 140);
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

          Tmotor = analogRead(ROTARY);


          if (abs(Tmotor - oldSpeed2Send) > 3) {
            itoa(Tmotor, Speed, 10);
            strcpy(bufMessage, "doSomething(");
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
          else {}

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

        //Settings menu
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


            if (menu4State == 2) { //update button clicked

              /*
              * ***********************************
              *       *       *
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
                SmartMtr*SmtSPKR*SmrtLht*
              *        *       *       *       *
              * ***********************************
              * ***********************************
              *        *       *       *       *
              *        *       *       *       *
              *        *       *       *       *
              * ***********************************

              */
              int menu5State = 1;
              tft.fillScreen(settingsBG);
              pagebg = settingsBG;
              _index[5] = 1;
              drawEmptyBoxes(6, _index[5], inactiveBG, activeBG);
              while (digitalRead(WIO_5S_PRESS) == LOW);
              while (digitalRead(WIO_5S_PRESS)) {
                if (digitalRead(WIO_5S_UP) == LOW && _index[5] > 1 ) {
                  while (digitalRead(WIO_5S_UP) == LOW);
                  _index[5] -= 1;
                  drawEmptyBoxes(6, _index[5], inactiveBG, activeBG);

                }
                else if (digitalRead(WIO_5S_LEFT) == LOW && _index[5] > 1 ) {
                  while (digitalRead(WIO_5S_LEFT) == LOW);
                  _index[5] -= 1;
                  drawEmptyBoxes(6, _index[5], inactiveBG, activeBG);

                }
                else if (digitalRead(WIO_5S_RIGHT) == LOW && _index[5] < 3) {
                  while (digitalRead(WIO_5S_RIGHT) == LOW);
                  _index[5] += 1;
                  drawEmptyBoxes(6, _index[5], inactiveBG, activeBG);
                }
                else if (digitalRead(WIO_5S_DOWN) == LOW && _index[5] < 3) {
                  while (digitalRead(WIO_5S_DOWN) == LOW);
                  _index[5] += 1;
                  drawEmptyBoxes(6, _index[5], inactiveBG, activeBG);
                }
              }
              while (digitalRead(WIO_5S_PRESS) == LOW);
              tft.fillScreen(TFT_WHITE);
              tft.fillRect( 50, 120, 220, 80, TFT_GREEN);
              tft.setTextSize(2);
              tft.drawString("UPDAING FIRMWARE", 160 - tft.textWidth("UPDATING FIRMWARE") / 2, 150);
              tft.setTextSize(1);
              tft.drawString("Please wait!!", 160 - tft.textWidth("Please wait!!") / 2, 180);
              Serial1.print("\x03");
              delay(1000);
              Serial.print(list[_index[5] - 1]);
              wipeClean(list[_index[5] - 1], "Unnamed");
              Serial1.print("\x03");
              resetSM();
              delay(500);
              runSM();
              scanSMData();
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
                  drawEmptyBoxes(5, _index[4], inactiveBG, activeBG);

                }
                else if (digitalRead(WIO_5S_LEFT) == LOW && _index[4] > 1 ) {
                  while (digitalRead(WIO_5S_LEFT) == LOW);
                  _index[4] -= 1;
                  drawEmptyBoxes(5, _index[4], inactiveBG, activeBG);

                }
                else if (digitalRead(WIO_5S_RIGHT) == LOW && _index[4] < 16) {
                  while (digitalRead(WIO_5S_RIGHT) == LOW);
                  _index[4] += 1;
                  drawEmptyBoxes(5, _index[4], inactiveBG, activeBG);
                }
                else if (digitalRead(WIO_5S_DOWN) == LOW && _index[4] < 16) {
                  while (digitalRead(WIO_5S_DOWN) == LOW);
                  _index[4] += 1;
                  drawEmptyBoxes(5, _index[4], inactiveBG, activeBG);
                }
              }
              while (digitalRead(WIO_5S_PRESS) == LOW);
              renameSM(_index[4]);
              Serial1.print("\x03");
              resetSM();
              delay(500);
              runSM();
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
      tft.fillScreen(TFT_BLUE);
      tft.setTextColor(TFT_BLACK, TFT_BLACK);
      tft.setTextSize(2);
      tft.drawString("SMARTMOTOR NOT CONNECTED", 10, 70);
      strcpy(Name[0], "");
      strcpy(Name[1], "");
      strcpy(Name[2], "");
      strcpy(Name[3], "");


    }


    delay(100);

  }

  else {

  }
  tft.fillScreen(TFT_BLUE);
  tft.drawString("SMARTMOTOR NOT CONNECTED", 10, 70);
  strcpy(Name[0], "");
  strcpy(Name[1], "");
  strcpy(Name[2], "");
  strcpy(Name[3], "");
}
