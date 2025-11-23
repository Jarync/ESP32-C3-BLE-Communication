# ESP32-C3 BLE Communication System (Server-Client)

![Hardware](https://img.shields.io/badge/Hardware-ESP32--C3%20Mini-blue.svg)
![Protocol](https://img.shields.io/badge/Protocol-BLE%20%28GATT%29-green.svg)
![ADC](https://img.shields.io/badge/Feature-Multi--Channel%20ADC-orange.svg)

## 📖 Introduction

This project implements a **Bluetooth Low Energy (BLE)** communication link between two ESP32-C3 microcontrollers. It demonstrates a robust Server-Client architecture where the Server collects multi-channel analog sensor data and transmits it wirelessly to the Client using **GATT Notifications**.

This system is ideal for low-power wireless sensor networks (WSN) where real-time data telemetry is required without the overhead of Wi-Fi.

### ✨ Key Features
* **BLE GATT Architecture:** Implements custom Service and Characteristic UUIDs.
* **Real-time Telemetry:** Streams 3-channel ADC data (0-4095 resolution) continuously.
* **Notification Mechanism:** Uses `PROPERTY_NOTIFY` for efficient, low-latency data push, rather than polling.
* **Auto-Reconnection:** Server automatically restarts advertising upon disconnection to ensure system resilience.

---

## 🛠️ Hardware & Configuration

| Component | Model | Role |
| :--- | :--- | :--- |
| **Microcontroller A** | ESP32-C3 Mini | **BLE Server** (Transmitter / Sensor Node) |
| **Microcontroller B** | ESP32-C3 Mini | **BLE Client** (Receiver / Central Node) |
| **Input** | Analog Sensors | Connected to GPIO 0, 1, 3 on Server |

### ⚙️ IDE Setup (Important!)
When uploading code to ESP32-C3, ensure the following settings in Arduino IDE:
1.  **Board:** `ESP32C3 Dev Module`
2.  **USB CDC On Boot:** `Enable` (Crucial for Serial Monitor output)
3.  **Baud Rate:** `115200`

---

## 💻 Software Logic

### 1. Server (Transmitter) Logic
* **Initialization:** Creates a BLE Server and starts advertising the Service UUID.
* **Data Acquisition:** Reads analog values from Pins 0, 1, and 3 every 600ms using non-blocking `millis()` timers.
* **Transmission:** Updates the Characteristic value and triggers `notify()` to push data to the subscribed Client.

### 2. Client (Receiver) Logic
* **Discovery:** Scans for devices advertising the specific Target Service UUID.
* **Connection:** Connects to the Server and discovers the Target Characteristic.
* **Subscription:** Registers a callback function to handle incoming Notifications asynchronously.
* **Output:** Parses the received string and prints formatted data to the Serial Monitor.

### 🛡️ UUID Configuration
* **Service UUID:** `19348c39-0336-474b-ad5a-6a805b0e2486`
* **Characteristic UUID:** `b83bd7fa-a073-428c-bb58-1eb565ceec46`

---

## 🚀 How to Run

1.  **Flash the Server:** Upload `sender.ino` to the first ESP32-C3.
2.  **Flash the Client:** Upload `receiver.ino` to the second ESP32-C3.
3.  **Power On:** Connect both boards to USB power.
4.  **Monitor:** Open the Serial Monitor for the **Client** board. You should see:
    ```text
    Device Found: ...
    Connected to Server
    Notification Received: Pin 0: 1024,  Pin 1: 2048,  Pin 3: 4095
    ```

---

## ©️ Intellectual Property & License

**Copyright © 2025 Chen Junxu. All Rights Reserved.**

### ⚠️ Disclaimer (Prototype Use Only)
This repository contains **prototype firmware** developed for educational and demonstration purposes. It does NOT represent a final commercial product. Specific proprietary parameters have been excluded.

### 📜 Usage Policy (CC BY-NC 4.0)
This project is licensed under the **Creative Commons Attribution-NonCommercial 4.0 International License**.

1.  **Non-Commercial Use:** Free to use for personal learning and academic research.
2.  **No Commercial Deployment:** Strictly prohibited for commercial products without permission.
3.  **Attribution:** Must credit the author (**Chen Junxu**) when using this code.
