#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_sleep.h>

#define SERVICE_UUID        "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "abcdef01-2345-6789-1234-56789abcdef0"

BLECharacteristic *pCharacteristic;
const uint64_t interval = 60000000;  // Sleep for 1 minute (60,000,000 microseconds)

void setup() {
  Serial.begin(115200);
  
  // Check the wake-up reason
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_TIMER: 
      Serial.println("Wakeup caused by timer");
      break;
    case ESP_SLEEP_WAKEUP_UNDEFINED:
    default:
      Serial.println("Not a deep sleep reset");
  }

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
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  // Set up deep sleep timer and start deep sleep
  esp_sleep_enable_timer_wakeup(interval);
  esp_deep_sleep_start();
}

void loop() {
  // Empty, as the ESP32 will not run the loop after waking up from deep sleep
}
