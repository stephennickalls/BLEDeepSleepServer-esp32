#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_sleep.h>

#define SERVICE_UUID        "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "abcdef01-2345-6789-1234-56789abcdef0"

BLECharacteristic *pCharacteristic;
const uint64_t broadcastInterval = 30000000; // Broadcast for 30 seconds (30,000,000 microseconds)
const uint64_t sleepInterval = 30000000;     // Sleep for 30 seconds (30,000,000 microseconds)

void setup() {
  Serial.begin(115200);
  
  // Check the wake-up reason
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  
  BLEDevice::init("ESP32_BLE_Server");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
                       CHARACTERISTIC_UUID,
                       BLECharacteristic::PROPERTY_READ |
                       BLECharacteristic::PROPERTY_WRITE
                     );

  pCharacteristic->setValue("Hello World");
  pService->start();

  if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER) {
    Serial.println("Wakeup caused by timer, start broadcasting...");
    // Start broadcasting again
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();

    // Set a timer to stop advertising after 30 seconds
    esp_sleep_enable_timer_wakeup(broadcastInterval);
    esp_light_sleep_start(); // Use light sleep to keep the program state

    Serial.println("Stop broadcasting, going to deep sleep...");
    // Now go to deep sleep for 30 seconds
    esp_sleep_enable_timer_wakeup(sleepInterval);
    esp_deep_sleep_start();
  } else {
    Serial.println("Fresh start or non-timer wakeup");
    // Fresh start, start advertising
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();

    // Set a timer to stop advertising after 30 seconds
    esp_sleep_enable_timer_wakeup(broadcastInterval);
    esp_light_sleep_start(); // Use light sleep to keep the program state

    Serial.println("Stop broadcasting, going to deep sleep...");
    // Go to deep sleep for 30 seconds
    esp_sleep_enable_timer_wakeup(sleepInterval);
    esp_deep_sleep_start();
  }
}

void loop() {
  // Empty, as the ESP32 will not run the loop after waking up from deep sleep
}
