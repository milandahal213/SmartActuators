#include "BLEDevice.h"
#include "Seeed_rpcUnified.h"
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// The remote service we wish to connect to.
static BLEUUID serviceUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
// The characteristic of the remote service we are interested in.
static BLEUUID    TXUUID("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");
static BLEUUID    RXUUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLERemoteCharacteristic* RRemoteCharacteristic;

static BLEAdvertisedDevice* myDevice;

uint8_t bd_addr[6] = {0x0A, 0xE4, 0xA3, 0x3C, 0x0B, 0x38};
BLEAddress BattServer(bd_addr);

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {

    //Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    //Serial.print(" of data length ");
    //Serial.println(length);
    //Serial.print("data: ");
    Serial.print(*(uint8_t *)pData);
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
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
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

    if(RRemoteCharacteristic->canNotify()){
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
    if (memcmp(advertisedDevice.getAddress().getNative(),BattServer.getNative(), 6) == 0) {
      Serial.print("BATT Device found: ");
      Serial.println(advertisedDevice.toString().c_str());
      BLEDevice::getScan()->stop();
      Serial.println("new BLEAdvertisedDevice");
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      Serial.println("new BLEAdvertisedDevice done");
      doConnect = true;
      doScan = true;  
  } // onResult
  }
}; // MyAdvertisedDeviceCallbacks

void setup() {
  Serial.begin(115200);
  while(!Serial){};
  delay(2000);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);

} // End of setup.

void loop() {

  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }
  Serial.printf(".");
  
  delay(1000);
} // End of loop
