/*
  Project: ESP32-C3 BLE Analog Signal Receiver (Client)
  
  Objective:
    Act as a BLE Client to connect to a BLE Server, receive real-time analog data 
    (0-4095) from 3 channels, and display it via Serial Monitor.

  Hardware:
    2 x ESP32-C3 Mini Development Boards

  Instructions:
    1. Select Board: "ESP32C3 Dev Module" in Arduino IDE.
    2. Enable "USB CDC On Boot": In Tools menu (Essential for Serial Monitor).
    3. Upload Code: To the Client board.
    4. Baud Rate: 115200 for Serial Monitor.

  Mechanism:
    - Scans for a specific Service UUID.
    - Connects to the Server.
    - Reads Characteristic value or subscribes to Notifications.
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEClient.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// Define UUIDs to match the Server
#define SERVICE_UUID "19348c39-0336-474b-ad5a-6a805b0e2486"        // Service UUID
#define CHARACTERISTIC_UUID "b83bd7fa-a073-428c-bb58-1eb565ceec46"  // Characteristic UUID

BLERemoteCharacteristic* pRemoteCharacteristic;  // Pointer to remote characteristic
BLEAdvertisedDevice* myDevice;                   // Storage for scanned device info
BLEClient* pClient;                              // BLE Client object

bool doConnect = false;  // Flag: Ready to connect
bool connected = false;  // Flag: Connection status
bool doScan = false;     // Flag: Scan status

/*********************************************************
 * Function: Notify Callback
 * Purpose: Handle incoming data notifications from Server
 *********************************************************/
void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    
  // Called when a notification is received
  Serial.print("Notification Received: ");
  std::string value = std::string((char*)pData, length);
  Serial.println(value.c_str());
}

/*********************************************************
 * Class: Client Callbacks
 * Purpose: Handle connection and disconnection events
 *********************************************************/
class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    Serial.println("Connected to Server");
  }
  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("Disconnected from Server");
  }
};

/*********************************************************
 * Class: Advertised Device Callbacks
 * Purpose: Handle device discovery during scan
 *********************************************************/
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("Device Found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // Check if the device contains the target Service UUID
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(BLEUUID(SERVICE_UUID))) {
      Serial.println("Target Device Found. Stopping Scan.");
      BLEDevice::getScan()->stop();                            // Stop scanning
      myDevice = new BLEAdvertisedDevice(advertisedDevice);    // Save device info
      doConnect = true;                                        // Set connect flag
      doScan = true;                                           // Set scan flag
    }
  }
};

/*********************************************************
 * Function: Connect to Server
 * Purpose: Establish connection and retrieve characteristics
 * Returns: true on success, false on failure
 *********************************************************/
bool connectToServer() {
  Serial.print("Connecting to: ");
  Serial.println(myDevice->getAddress().toString().c_str());

  // Create Client
  pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback()); 

  // Connect
  pClient->connect(myDevice);
  Serial.println("Client connected to Server");

  // Get Service
  BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
  if (pRemoteService == nullptr) {
    Serial.println("Failed to find Service UUID");
    pClient->disconnect();
    return false;
  }

  // Get Characteristic
  pRemoteCharacteristic = pRemoteService->getCharacteristic(CHARACTERISTIC_UUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.println("Failed to find Characteristic UUID");
    pClient->disconnect();
    return false;
  }

  // Read initial value if readable
  if (pRemoteCharacteristic->canRead()) {
    std::string value = pRemoteCharacteristic->readValue();
    Serial.println(value.c_str());
  }

  // Register for notifications
  if (pRemoteCharacteristic->canNotify()) {
    pRemoteCharacteristic->registerForNotify(notifyCallback);
  }

  connected = true;
  return true;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE Client...");

  // Initialize BLE
  BLEDevice::init("ESP32_C3_Client");

  // Start Scanning
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);  // Scan for 5 seconds
}

void loop() {
  // Attempt connection if target found
  if (doConnect) {
    if (connectToServer()) {
      Serial.println("Connected to Service. Waiting for data...");
    } else {
      Serial.println("Connection failed. Restarting scan...");
    }
    doConnect = false;
  }

  // If connected, keep reading (optional, as we use notifications)
  if (connected) {
    // Example: Polling read (commented out as we use Notify)
    // if (pRemoteCharacteristic->canRead()) {
    //   std::string value = pRemoteCharacteristic->readValue();
    //   Serial.print("Read Value: ");
    //   Serial.println(value.c_str());
    // }
  } else if (doScan) {
    // Restart scan if disconnected
    BLEDevice::getScan()->start(0); 
  }

  delay(600); // Loop delay for stability
}