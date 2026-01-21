#include <SimpleDHT.h>

// Pin connected to the DHT sensor
const int DHTPIN = 2;

// Initialize the SimpleDHT library
SimpleDHT11 dht11;

void setup() {
  Serial.begin(9600);  // Initialize the serial communication
}

void loop() {
  byte temperature = 0;
  byte humidity = 0;
  byte data[40] = {0};

  // Read temperature and humidity from the DHT sensor
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(DHTPIN, &temperature, &humidity, data)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err=");
    Serial.println(err);
    delay(1000);
    return;
  }

  // Print temperature and humidity values
  Serial.print("Temperature: ");
  Serial.print((int)temperature);
  Serial.print(" °C\t");

  Serial.print("Humidity: ");
  Serial.print((int)humidity);
  Serial.println(" %");

  delay(2000);  // Delay between readings
}
