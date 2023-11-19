#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <esp_sleep.h>
#include <DHT.h>

#define SERVICE_UUID        "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "abcdef01-2345-6789-1234-56789abcdef0"

BLECharacteristic *pCharacteristic;
const uint64_t broadcastInterval = 60000000; 
const uint64_t sleepInterval = 60000000;   

#define DHTPIN 4     // Change this to the pin you connected the DHT sensor to
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

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

  // Wake up and read data from the DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
      // Go to deep sleep again
      esp_sleep_enable_timer_wakeup(sleepInterval);
      esp_deep_sleep_start();
  }

  // Create a data string that combines temperature and humidity
  char sensorData[50];
  snprintf(sensorData, sizeof(sensorData), "Temp: %.1f C, Hum: %.1f %%", temperature, humidity);
  pCharacteristic->setValue(sensorData);

  pService->start();
  Serial.println("Sensor data read, start broadcasting...");

  // Start broadcasting
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  // Set a timer to stop advertising after 30 seconds
  esp_sleep_enable_timer_wakeup(broadcastInterval);
  esp_light_sleep_start(); // Use light sleep to keep the program state

  Serial.println("Stop broadcasting, going to deep sleep...");
  // Go to deep sleep for 9 minutes and 30 seconds
  esp_sleep_enable_timer_wakeup(sleepInterval);
  esp_deep_sleep_start();
}

void loop() {

}
