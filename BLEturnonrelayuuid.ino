#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define RELAY_PIN 19   // Relay Pin

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

#define SERVICE_UUID           "6e400001-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID_RX "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define CHARACTERISTIC_UUID_TX "6e400003-b5a3-f393-e0a9-e50e24dcca9e"

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Device Connected");
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Device Disconnected");
    pServer->startAdvertising();
  }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
      Serial.print("Received Value: ");
      Serial.println(rxValue.c_str());

      if (rxValue == "ON") {
        digitalWrite(RELAY_PIN, LOW);      // Relay ON
        Serial.println("Relay ON");
      }
      else if (rxValue == "OFF") {
        digitalWrite(RELAY_PIN, HIGH);     // Relay OFF
        Serial.println("Relay OFF");
      }
    }
  }
};

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // Relay initially off

  Serial.begin(115200);
  BLEDevice::init("ESP32-RELAY-BLE");

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic =
    pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);

  BLECharacteristic *pCharacteristicRX =
    pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);

  pCharacteristicRX->setCallbacks(new MyCallbacks());

  pService->start();
  pServer->getAdvertising()->start();

  Serial.println("BLE Ready - Waiting for Client...");
}

void loop() {}
