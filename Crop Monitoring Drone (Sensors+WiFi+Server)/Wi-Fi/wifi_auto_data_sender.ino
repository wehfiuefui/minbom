#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SimpleDHT.h>

// Replace with your WiFi credentials
const char* ssid = "";
const char* password = "12345678!!";

// Multiplexer control pins
const int s0 = D1;
const int s1 = D2;
const int s2 = D3;
const int s3 = D4;

// Analog pin connected to the multiplexer
const int analogPin = A0;

// DHT11 sensor
const int DHTPin = D5;
SimpleDHT11 dht11;

// Server endpoint
const char* serverEndpoint = "https://travelwithtripster.net/upload.php";

// Function to read from the multiplexer
int readMux(int channel) {
  int controlPin[] = {s0, s1, s2, s3};

  for (int i = 0; i < 4; i++) {
    digitalWrite(controlPin[i], (channel & (1 << i)) ? HIGH : LOW);
  }

  delay(1);
  return analogRead(analogPin);
}

// Function to send data to the server
void sendDataToServer(int soilMoisture, int waterLevel, int photoresistor, int temperature, int humidity) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverEndpoint);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String postData = "soilMoisture=" + String(soilMoisture)
                  + "&waterLevel=" + String(waterLevel)
                  + "&photoresistor=" + String(photoresistor)
                  + "&temperature=" + String(temperature)
                  + "&humidity=" + String(humidity);

  int httpResponseCode = http.POST(postData);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
  } else {
    Serial.println("Error in sending POST request");
  }

  http.end();
}

void setup() {
  Serial.begin(115200);

  // Initialize multiplexer control pins
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int soilMoisture = readMux(0); // Assuming soil moisture sensor is connected to C0
  int waterLevel = readMux(1); // Assuming water level sensor is connected to C1
  int photoresistor = readMux(2); // Assuming photoresistor is connected to C2
  byte temperature = 0;
  byte humidity = 0;

  if (dht11.read(DHTPin, &temperature, &humidity, NULL) != SimpleDHTErrSuccess) {
    Serial.println("Failed to read from DHT sensor!");
  }

  sendDataToServer(soilMoisture, waterLevel, photoresistor, (int)temperature, (int)humidity);

  delay(10000); // Send data every 10 seconds
}
