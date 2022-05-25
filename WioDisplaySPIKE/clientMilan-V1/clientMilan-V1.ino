#include "BLEDevice.h"
#include "Seeed_rpcUnified.h"
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <Arduino_JSON.h>
//Display
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
TFT_eSprite img = TFT_eSprite(&tft);  // Sprite
String Value11;

#define len(arr) sizeof (arr)/sizeof (arr[0])


// The remote service we wish to connect to.
static BLEUUID serviceUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
// The characteristic of the remote service we are interested in.
static BLEUUID    TXUUID("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");
static BLEUUID    RXUUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;

int count_address = 0;
int sCount = 0;
static BLERemoteCharacteristic* RRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;


char *rName = "robot"; //name of the SPIKE we want to connect to ... have to name all the SPIKEs robot
char *data;

//SPIKE port data

String dat;
String addr[10];
String state = "";
int menuState = 0;
int oldmenuState=0;


/*************************************/
/*************************************/
//button interrupt functions

void scanBLE() {
  Serial.println("Button Int PRESS");
  count_address = 0;
  menuState = 0;
  state = "scan";
  if (connected){
  Serial.println("I don't know if this thing is connected");
    BLEDevice::deinit();
  }
}

void switchUP() {
  Serial.println("Button Int UO");
  if (menuState > 0) {
    oldmenuState=menuState;
    menuState -= 1;
  }
  Serial.println(menuState);
  state = "bPressed";

}
void switchDOWN() {
  Serial.println("Button Int DOWN");
  if (menuState < 6 && menuState < count_address-1) {
    oldmenuState=menuState;
    menuState += 1;
  }
  state = "bPressed";
  Serial.println(menuState);
  Serial.println(count_address);
}
void connectBLE() {
  Serial.println("Button Int PRESS");
  state = "connect";
  Serial.println(menuState);
}
/*************************************/
/*************************************/



static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
  data=(char*)pData;  //converting uint8_t to char array
  data[length]='\0';  //adding end of string
  state="Incoming";  
  Serial.println(data); 
}

class MyClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) {
    }
    void onDisconnect(BLEClient* pclient) {
      connected = false;
      Serial.println("onDisconnect");
    }
};




bool connectToServer() {
  BLEClient*  pClient  = BLEDevice::createClient();
  Serial.println(" - Created client");
  pClient->setClientCallbacks(new MyClientCallback());
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  Serial.println(serviceUUID.toString().c_str());
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }

  Serial.println(" - Found our service");
  // Obtain a reference to the characteristic in the service of the remote BLE server.
  RRemoteCharacteristic = pRemoteService->getCharacteristic(TXUUID);

  if (RRemoteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    pClient->disconnect();
    return false;
  }
  RRemoteCharacteristic = pRemoteService->getCharacteristic(RXUUID);
  Serial.println(RRemoteCharacteristic->getUUID().toString().c_str());
  Serial.println(" - Found our characteristic");
  // Read the value of the characteristic.

  if (RRemoteCharacteristic->canNotify()) {
    RRemoteCharacteristic->registerForNotify(notifyCallback);
  }
  connected = true;
  return true;
}



//Scan for BLE servers and find the first one that advertises the service we are looking for.
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.print("BLE Advertised Device found: ");
      Serial.println(advertisedDevice.toString().c_str());
      Serial.println(advertisedDevice.getName().c_str());

      if (memcmp(advertisedDevice.getName().c_str(), rName, 5) == 0) { //if the name is SPIKE then add the address to address variable
        addr[count_address] = advertisedDevice.getAddress().toString().c_str();
        Serial.println(addr[count_address]);
        Serial.println(advertisedDevice.getRSSI());
        count_address += 1;
      }

    }
};



//create a function here that then connects to the selected address and connect
class toConnectMyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.print("BLE Advertised Device found: ");
      //Serial.println(advertisedDevice.toString().c_str());
      //Serial.println(advertisedDevice.getName().c_str());
      Serial.println(advertisedDevice.getRSSI());
      //Serial.println("*******");
      //Serial.println(addr[menuState].c_str());
      //Serial.println(advertisedDevice.getAddress().toString().c_str());
      //Serial.println("*****");

      if (strcmp(advertisedDevice.getAddress().toString().c_str() , addr[menuState].c_str()) == 0) {
        //Serial.print("BATT Device found: ");
        //Serial.println(advertisedDevice.toString().c_str());
        BLEDevice::getScan()->stop();
        Serial.println("new BLEAdvertisedDevice");
        myDevice = new BLEAdvertisedDevice(advertisedDevice);
        Serial.println("new BLEAdvertisedDevice done");
        doConnect = true;
        doScan = true;
      } // onResult
    }
}; // MyAdvertisedDeviceCallbacks



void scanSPIKE() {

  delay(1000);
  Serial.println("Starting Arduino BLE Client application...");
  showMessage("Scanning");
  BLEDevice::deinit();
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
  showAddress();


}


void connectSPIKE() {

  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new toConnectMyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);

}


/*************************/
//Sprite functions


void build_banner(String msg, int xpos) {
  img.fillSprite(TFT_GREEN);
  img.setTextWrap(false);       // Turn of wrap so we can print past end of sprite
  img.setCursor(xpos + 20, 2); // Print text at xpos
  img.print(msg);

}


void showMessage(String msg) {

  img.createSprite(200, 50);
  img.fillSprite(TFT_GREEN);
  img.setTextWrap(false);       // Turn of wrap so we can print past end of sprite
  img.setCursor(20, 2);  // Print text at xpos
  img.print(msg);
  img.pushSprite( 60, 70);
  img.deleteSprite();

}
void hideMessage() {
  img.createSprite(200, 50);
  img.fillSprite(TFT_BLUE);
  img.pushSprite( 60, 70);
  img.deleteSprite();

}
void showAddress() {
  hideMessage();
  img.createSprite(240, 30);
  for (int pos = 0; pos < count_address; pos++) {
    build_banner(addr[pos], pos);
    img.pushSprite( 40, pos * 35);
  }
  delay(1000);
  img.deleteSprite();

}

void drawSelector() {
  hideMessage();
  img.createSprite(30, 30);
  img.fillSprite(TFT_BLUE);
  img.fillRect(0, 0, 20, 10, TFT_BLUE);
  img.pushSprite(10, oldmenuState * 35);
  img.pushSprite(280, oldmenuState * 35);
  img.fillSprite(TFT_RED);
  img.fillRect(0, 0, 20, 10, TFT_RED);
  img.pushSprite(10, menuState * 35);
  img.pushSprite(280, menuState * 35);
  

  img.deleteSprite();
}


void setup() {
  Serial.begin(115200);
  while (!Serial) {};


  //screen stuff
  tft.begin();
  tft.init();
  tft.setRotation(3);

  tft.fillScreen(TFT_BLUE);
  img.setTextSize(1);           // Font size scaling is x1
  img.setTextFont(4);           // Font 4 selected
  img.setTextColor(TFT_BLACK);  // Black text, no background colour


  //scan bluetooth
  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);

  //selection buttons
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(WIO_KEY_C),  scanBLE, FALLING);

  attachInterrupt(digitalPinToInterrupt(WIO_5S_UP),    switchUP, FALLING);
  attachInterrupt(digitalPinToInterrupt(WIO_5S_DOWN),  switchDOWN, FALLING);
  attachInterrupt(digitalPinToInterrupt(WIO_5S_PRESS), connectBLE, FALLING);

  scanSPIKE();
  drawSelector();
  delay(2000);
}


void loop() {

  //scan the available BLE advertisers - save all the SPIKEs with names "robot" to *addr
  if (state == "scan" && not connected) {
    scanSPIKE();
    //the function that initializes and searches for robots
    drawSelector();
    state = "";
    //reset the state to blank
  }
  else if (state == "connect" && not connected ) {
    //if the central button is pressed , connect the selected robot

    connectSPIKE();
    //initializes the BLE and searches and connects to the given BLE
    showMessage("Connecting");

    if (doConnect == true) {
      if (connectToServer()) {
        Serial.println("We are now connected to the BLE Server.");
      } else {
        Serial.println("We have failed to connect to the server; there is nothin more we will do.");
      }
      doConnect = false;
    }
    state = "";
  }
  else if (state == "bPressed" && not connected) { //animating the menu selector
    //move the sprite up and down
    drawSelector();
    state = "";
  }

  else if (state == "Incoming" &&  connected ){
    showMessage(data);
    state="";
    JSONVar myObject = JSON.parse(data);
    String lib;
    lib=myObject["C"]["type"];
      Serial.println(lib);
      Serial.println("this was in C");
      Serial.println("*********");


  }
  
  Serial.printf(".");
  delay(500);
} // End of loop
