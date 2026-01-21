#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SimpleDHT.h>

// Replace with your WiFi credentials
const char* ssid = "IT-SCHOOL";
const char* password = "12345678!!";

// Multiplexer control pins
const int s0 = D1;  // GPIO 5
const int s1 = D2;  // GPIO 4
const int s2 = D3;  // GPIO 0
const int s3 = D4;  // GPIO 2

// Analog pin connected to the multiplexer
const int analogPin = A0;

// DHT11 sensor
const int DHTPin = D5;  // GPIO 14
SimpleDHT11 dht11;

// Server endpoint
const char* serverEndpoint = "https://cropyieldpredictortool.000webhostapp.com/receive_data.php";

// Function to read from the multiplexer
int readMux(int channel) {
  int controlPin[] = {s0, s1, s2, s3};

  for (int i = 0; i < 4; i++) {
    digitalWrite(controlPin[i], (channel & (1 << i)) ? HIGH : LOW);
  }

  delay(5); // Small delay for stability
  return analogRead(analogPin);
}

// Function to ensure WiFi is connected
void ensureWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Reconnecting...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);

    int retryCount = 0;
    while (WiFi.status() != WL_CONNECTED && retryCount < 20) {
      delay(500);
      Serial.print(".");
      retryCount++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi Reconnected!");
      Serial.print("New IP Address: ");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("\nFailed to reconnect to WiFi.");
    }
  }
}

// Function to send data to the server
void sendDataToServer(int soilMoisture, int waterLevel, int photoresistor, int temperature, int humidity) {
  ensureWiFiConnection(); // Ensure WiFi is connected before sending data

  if (WiFi.status() == WL_CONNECTED) {
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
      Serial.println("Data sent successfully!");
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response: " + http.getString());
    } else {
      Serial.println("Error in sending POST request. Code: " + String(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("WiFi not connected, skipping data transmission.");
  }
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
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi. Check credentials or signal strength.");
  }
}

void loop() {
  // Read sensor values
  int soilMoisture = readMux(0); // Soil moisture sensor at C0
  int waterLevel = readMux(1);   // Water level sensor at C1
  int photoresistor = readMux(2); // Photoresistor at C2
  byte temperature = 0, humidity = 0;

  if (dht11.read(DHTPin, &temperature, &humidity, NULL) != SimpleDHTErrSuccess) {
    Serial.println("Failed to read from DHT sensor!");
  }

  // Print values for debugging
  Serial.println("------ Sensor Readings ------");
  Serial.println("Soil Moisture: " + String(soilMoisture));
  Serial.println("Water Level: " + String(waterLevel));
  Serial.println("Photoresistor: " + String(photoresistor));
  Serial.println("Temperature: " + String(temperature));
  Serial.println("Humidity: " + String(humidity));
  Serial.println("-----------------------------");

  // Send data to the server
  sendDataToServer(soilMoisture, waterLevel, photoresistor, (int)temperature, (int)humidity);

  delay(10000); // Send data every 10 seconds
}
