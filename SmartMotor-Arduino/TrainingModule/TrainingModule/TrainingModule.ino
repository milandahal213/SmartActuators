#include"TFT_eSPI.h"
#include "RTC_SAMD51.h"
#include "DateTime.h"
RTC_SAMD51 rtc;
DateTime now;
int prev_time, currenttime;

int tData[30][2];
int trainingData[30][2];
int count=0;
int tDataLength = 0;


#define w 70
#define h 50


  int X=12;  //for Dashboard values
  int Y=70;
  int widthRect2=15;
  
int oldx = 0;
int oldy = 0;

int oldLight=0;
int oldMotor=0;

int _index[2] = {2, 1};


//colors
int page1bg=TFT_BLUE;
#define Motor TFT_GREEN

char ending[500];
char *SMmessage;

int x[16] = {5, 85, 165, 245, 5, 85, 165, 245, 5, 85, 165, 245, 5, 85, 165, 245};
int y[16] = {5, 5, 5, 5, 65, 65, 65, 65, 125, 125, 125, 125, 185, 185, 185, 185};

int menu[2][16] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1}, {0, 1, 1, 1, 0, 0, 0, 0, 0, 0,0 , 1, 0, 0, 0,0 }};
char menuWORD[3][4][20] = {{"Train", "View", "Setting"}, {"View1", "Graph", "Quit"}};

#define LCD_BACKLIGHT (72Ul) // Control Pin of LCD
TFT_eSPI tft;
int sizeoflist = 3;
char list[3][10] = {"Motor", "Speaker", "Color"};

String messageFromESP;
int textColor = TFT_BLACK;
int Speed2Send = 0;
int oldSpeed2Send = 0;
int lightValue = 0;

char message[100];
char retValue[100]; //for the message from SM
char Name[4][10]; //data from the SM



char *ret;
char *_ret;
String request;


char *training = "training=[(30,300),(75,100)]\r\n";

char *PassThroughMessage[] = {
  "import network, ubinascii, ujson\r\n",
  "import machine\r\n",
  "import time\r\n",
  "from machine import Pin, ADC\r\n",
  "LightSensor = ADC(0)\r\n",
};

char *mainScript[] = {
  "import uos, machine\r\n",
  "import time\r\n",
  "name=\"Motor\"\r\n",
  "from machine import Pin, ADC\r\n",
  "TMConnect=Pin(5,Pin.OUT)\r\n",
  "LightSensor = ADC(0)\r\n",
  "TMConnect.off()",
  "BuiltInLED = Pin(2, Pin.OUT)\r\n",
  "p2 = machine.Pin(0)\r\n",
  "pwm2 = machine.PWM(p2)\r\n",
  "pwm2.freq(50)\r\n",
  "pwm2.duty(75)\r\n",
  training,
  "def loop():\r\n",
  "\twhile True:\r\n",
  "\t\tbright = LightSensor.read()\r\n",
  "\t\tmin = 1000\r\n",
  "\t\tpos = 0\r\n",
  "\t\tfor (a,l) in training:\r\n",
  "\t\t\tdist = abs(bright - l)\r\n",
  "\t\t\tif dist < min:\r\n",
  "\t\t\t\tmin = dist\r\n",
  "\t\t\t\tpos = a\r\n",
  "\t\t\tpwm2.duty(pos)\r\n",
  "\t\t\tprint(\"$Ullu\",\"Motor\",bright,pos)\r\n",
  "\t\ttime.sleep(.1)\r\n\r\n\r\n\r\n",

  "loop()\r\n"
};



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
        tft.drawRect(x[i], y[i], w, h, selection_color);
      }
      else
        tft.drawRect(x[i], y[i], w, h, color);
      tft.setTextSize(1);
      tft.setTextColor(TFT_BLACK, TFT_BLACK);
      //tft.drawString("hello wsir ",10,20);
      tft.drawString(menuWORD[menuNumber][_ind - 1], x[i] + 35 - tft.textWidth(menuWORD[menuNumber][_ind - 1]) / 2, y[i] + 20);
      Serial.println("**********");
      Serial.println(x[i]);
      Serial.println(y[i]);
      Serial.println(menuWORD[menuNumber][_ind - 1]);
      Serial.println("**********");
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
  strcpy(retValue, " ");

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
  tft.drawLine(50, 220, 300, 220, TFT_RED);
  tft.drawLine(75, 220, 75, 230, TFT_RED);
  tft.drawLine(255, 220, 255, 230, TFT_RED);
  tft.drawString("0", 72, 230 );
  tft.drawString("180", 252, 230 );
  tft.drawString("MOTOR Position", 110, 230 );
  tft.drawLine(50, 70, 50, 220, TFT_RED);
  tft.drawLine(40, 220, 50, 220, TFT_RED);
  tft.drawLine(40, 70, 50, 70, TFT_RED);
  tft.drawString("MIN", 20, 220 );
  tft.drawString("MAX", 20, 70 );
  tft.setRotation(2);
  tft.drawString("LIGHT INTENSITY", 50, 30 );
  tft.setRotation(3);
  for (int i = 0; i < DC; i++) {
    tft.fillCircle(75 + trainingData[i][0], 220 - 150 * (1 - pow(2, -1 * float(trainingData[i][1]) / 300)) , 5, TFT_BLUE);
  }
}



void drawGraph(int DC) {


  tft.setTextColor(textColor, textColor);
  tft.setTextSize(1);
  tft.drawLine(50, 220, 300, 220, TFT_RED);
  tft.drawLine(75, 220, 75, 230, TFT_RED);
  tft.drawLine(255, 220, 255, 230, TFT_RED);
  tft.drawString("0", 32, 230 );
  tft.drawString("180", 252, 230 );
  tft.drawString("MOTOR Position", 110, 230 );
  tft.drawLine(50, 70, 50, 220, TFT_RED);
  tft.drawLine(40, 220, 50, 220, TFT_RED);
  tft.drawLine(40, 70, 50, 70, TFT_RED);
  tft.drawString("MIN", 20, 220 );
  tft.drawString("MAX", 20, 70 );
  tft.setRotation(2);
  tft.drawString("LIGHT INTENSITY", 50, 30 );
  tft.setRotation(3);
  for (int i = 0; i < DC; i++) {
    tft.fillCircle(35 + tData[i][0], 220 - 150 * (1 - pow(2, -1 * float(tData[i][1]) / 300)) , 5, TFT_BLUE);
  }
}


void drawDashboard() {

  tft.setTextSize(2);


  int heightRect2=150;
  int radius1=50;
  int radius2=80;  
  


  
  int heightRect1=heightRect2+10;
  int widthRect1=widthRect2+35;
  int xc= X+135;
  int yc= Y + 80;
  tft.fillRoundRect(X-2,Y-2,xc+radius2+4, heightRect1+4,10,32);
  tft.fillRoundRect(X, Y, widthRect1, heightRect1, 10, TFT_BLUE);
  tft.drawRoundRect(X, Y, widthRect1, heightRect1, 10, TFT_BLACK);
  tft.fillRoundRect(X+30, Y+5, widthRect2, heightRect2, 10, TFT_WHITE);
  tft.fillCircle(xc, yc, radius2, TFT_BLUE);
  tft.drawCircle(xc, yc, radius2, TFT_BLACK);
  tft.fillCircle(xc, yc, radius1, TFT_WHITE);
  tft.setTextColor(TFT_WHITE, TFT_WHITE);
  tft.drawString("0 ", xc-4, yc-radius1-25);
  tft.setTextColor(TFT_WHITE, TFT_BLUE);
  tft.drawString(Name[1], xc-25, yc +radius1 + 8 );
  tft.setRotation(2);
  tft.drawString("LIGHT", 60, X+5 );
  tft.setTextColor(TFT_WHITE, TFT_WHITE);
  tft.drawString("CW", 78, xc-radius1-25 );
  tft.setRotation(0);
  tft.drawString("CCW ", 138,  320-xc-radius1-25 );
  tft.setRotation(3);
  
  tft.drawLine(xc-radius1-5, yc, xc-radius1+5, yc, TFT_RED);
  tft.drawLine(xc+radius1-5, yc, xc+radius1+5, yc, TFT_RED);
  tft.drawLine(xc, yc - radius1-5, xc, yc - radius1+5, TFT_RED);

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
  delay(200);
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
    tft.drawCircle(75 + atoi(Name[3]), 220 - 150 * (1 - pow(2, -1 * float(atoi(Name[2])) / 300)), 5, TFT_RED);
  }
  oldx = 75 + atoi(Name[3]);
  oldy = 220 - 150 * (1 - pow(2, -1 * float(atoi(Name[2])) / 300));
  return 1;
}

int drawDashboardItems() {


  int xc= X+135;
  int yc= Y + 80;
  int widthLight=widthRect2-5;
  int xR= X+30+ int(widthRect2/2)- int(widthLight/2); 
  
  int t = scanSMData();
  int light=150 * (1 - pow(2, -1 * float(atoi(Name[2])) / 300));
  int motor=atoi(Name[3]);


  if (t == 1 && validConnection() == 1) {
    tft.fillRect( xR,215-oldLight, widthLight, oldLight, TFT_WHITE);
    tft.drawLine(xc, yc, xc-int(50*cos(oldMotor*PI/180)), yc- int(50*sin(oldMotor*PI/180)),TFT_WHITE); 
    
    tft.drawLine(xc, yc, xc- int(50*cos(motor*PI/180)),yc- int(50*sin(motor*PI/180)),TFT_BLACK); 
    tft.fillRect(xR,215-light,widthLight,light, TFT_RED);
   

    oldLight = light;
    oldMotor=motor;
  }

  return 1;
}


void loop() {


  if (digitalRead(BCM18) == LOW) { //connected to a SM system // triggers the scan and display function
    int t = scanSMData(); //displays the name of the motor and type == if the return is 0 something is wrong

    if (t == 1 && validConnection() == 1) {
      tft.fillScreen(page1bg);
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
      drawEmptyBoxes(0, _index[0], TFT_RED, TFT_WHITE);


      //run in loop until the button is pressed - First menu
      while (digitalRead(WIO_5S_PRESS) == HIGH && digitalRead(BCM18) == LOW) {
        if (digitalRead(WIO_5S_LEFT) == LOW && _index[0] > 1 ) {
          while (digitalRead(WIO_5S_LEFT) == LOW);
          _index[0] -= 1;
          drawEmptyBoxes(0, _index[0], TFT_RED, TFT_WHITE);
        }
        if (digitalRead(WIO_5S_RIGHT) == LOW && _index[0] < 3) {
          while (digitalRead(WIO_5S_RIGHT) == LOW);
          _index[0] += 1;
          drawEmptyBoxes(0, _index[0], TFT_RED, TFT_WHITE);
        }
      }
      while (digitalRead(WIO_5S_PRESS) == LOW);


      // selection to train new data
      if (_index[0] == 1 && digitalRead(BCM18) == LOW) {

        
        Serial.print("train new data");
        drawDashboard();
        drawDashboardItems();
        
      }


      // selection to display data
      if (_index[0] == 2 && digitalRead(BCM18) == LOW) {
        tft.fillScreen(TFT_GREEN);
        tft.drawString("Getting Data... ", 10, 100);
        delay(500);
        tft.fillScreen(TFT_GREEN);
        Serial1.print("\x03");
        int DataCount = getDataFromSM(); //stop main loop_ get training data
        runSM();         // restart main.py
        drawEmptyBoxes(1, 1, TFT_BLUE, TFT_WHITE);
        int menu2State = 1;
        drawDashboard();

        while (menu2State != 3 && digitalRead(BCM18) == LOW) {
          if (digitalRead(WIO_5S_PRESS) == LOW) {
            while (digitalRead(WIO_5S_PRESS));
            menu2State = _index[1];
            tft.fillRect(0, 65, 320, 240, TFT_GREEN);
            Serial.print("pressed");
            if(menu2State==1) drawDashboard();  
            if(menu2State==2) drawGraph(DataCount);       //draw the graph
          }
          else if (digitalRead(WIO_5S_LEFT) == LOW && _index[1] > 1 ) {
            while (digitalRead(WIO_5S_LEFT) == LOW);
            _index[1] -= 1;
            drawEmptyBoxes(1, _index[1], TFT_BLUE, TFT_WHITE);

          }
          else if (digitalRead(WIO_5S_RIGHT) == LOW && _index[1] < 3) {
            while (digitalRead(WIO_5S_RIGHT) == LOW);
            _index[1] += 1;
            drawEmptyBoxes(1, _index[1], TFT_BLUE, TFT_WHITE);


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
