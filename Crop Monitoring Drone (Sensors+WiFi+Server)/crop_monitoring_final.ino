#include <LiquidCrystal_I2C.h>
#include <SimpleDHT.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int photocellPin = A0;  // Analog input pin for the photocell
const int moisturePin = A1;  // Analog input pin for the moisture sensor
const int DHTPIN = 2; // Pin connected to the DHT sensor
SimpleDHT11 dht11; // Initialize the SimpleDHT library


void setup() 
{
  Serial.begin(9600);  // Initialize the serial communication
  lcd.init();
  lcd.backlight();
}



void loop() 
{
  int sensorValue = analogRead(photocellPin);  // Read the analog value from the photocell
  int lightIntensity = map(sensorValue, 0, 1023, 0, 100);  // Map the sensor value to a percentage between 0 and 100

  Serial.print("Light Intensity: ");
  Serial.print(lightIntensity);
  Serial.println("%");

  delay(1000);  // Delay for 1 second before taking the next reading

//  moisture sensor/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  int sensorValue1 = analogRead(moisturePin);  // Read the analog value from the moisture sensor

  int percentMoisture = map(sensorValue1, 0, 1023, 0, 100);  // Map the sensor value to a percentage between 0 and 100

  Serial.print("Moisture level: ");
  Serial.print(percentMoisture);
  Serial.println("%");

  delay(1000);  // Delay for 1 second before taking the next reading

  /////////////////////////////////temperature and humidity sensor/////////////////////////////

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
