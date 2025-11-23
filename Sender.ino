/*
  Project: ESP32-C3 BLE Analog Signal Transmitter (Server)
  
  Objective:
    Act as a BLE Server to acquire analog signals (0-4095) from 3 pins 
    and transmit data to the Client via Notifications.

  Hardware:
    2 x ESP32-C3 Mini Development Boards

  Instructions:
    1. Select Board: "ESP32C3 Dev Module".
    2. Enable "USB CDC On Boot": Essential!
    3. Upload Code: To the Server board.
    4. Baud Rate: 115200.

  Mechanism:
    - Creates a BLE Service and Characteristic.
    - Reads analog pins periodically.
    - Notifies the Client with updated data string.
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define analog1 0  // Analog Pin 0
#define analog2 1  // Analog Pin 1
#define analog3 3  // Analog Pin 3

// UUIDs for Service and Characteristic (Must match Client)
#define SERVICE_UUID "19348c39-0336-474b-ad5a-6a805b0e2486"
#define CHARACTERISTIC_UUID "b83bd7fa-a073-428c-bb58-1eb565ceec46"

BLECharacteristic *pCharacteristic;  // BLE Characteristic object
bool deviceConnected = false;        // Connection status flag
unsigned long previousTime;          // Timer for non-blocking delay
unsigned long Interval = 600;        // Transmission interval (ms)

/*********************************************************
 * Class: Server Callbacks
 * Purpose: Handle Client connection events
 *********************************************************/
class serverCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
    Serial.println("BLE Client Connected");
  };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
    Serial.println("BLE Client Disconnected");
    // Restart advertising so others can connect
    pServer->getAdvertising()->start();
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println(F("Starting BLE Server..."));

  // Initialize BLE Device
  BLEDevice::init("ESP32_C3_Server");
  
  // Create Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new serverCallbacks()); // Set callbacks

  // Create Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create Characteristic with Read and Notify properties
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ | 
                      BLECharacteristic::PROPERTY_NOTIFY // Allow notifications
                    );

  // Add Descriptor (Required for Notifications)
  pCharacteristic->addDescriptor(new BLE2902());
  
  // Set initial value
  pCharacteristic->setValue("Server Ready");

  // Start Service
  pService->start();

  // Start Advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // iOS compatibility tweaks
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  
  Serial.println("Waiting for Client connection...");
}

void loop() {
  if (deviceConnected) {
    // Non-blocking timer loop
    if(millis() - previousTime >= Interval){
      previousTime = millis();
      
      // Read Analog Values (ADC)
      int analogSignal1 = analogRead(analog1);
      int analogSignal2 = analogRead(analog2);
      int analogSignal3 = analogRead(analog3);

      // Format data into a string
      String data = "Pin 0: " + String(analogSignal1) + 
                    ",  Pin 1: " + String(analogSignal2) + 
                    ",  Pin 3: " + String(analogSignal3);
      
      // Update Characteristic Value
      pCharacteristic->setValue(data.c_str());
      
      // Notify connected Client
      pCharacteristic->notify();
      
      // Print to Serial for debugging
      Serial.println("Sent Data: " + data);
    }
  }
}