#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SimpleDHT.h>

// Replace with your WiFi credentials
const char* ssid = "Nargiz";
const char* password = "12345678";

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

// Create a web server on port 80
ESP8266WebServer server(80);

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

  // Define the web server routes
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  int soilMoisture = readMux(0); // Assuming soil moisture sensor is connected to C0
  int waterLevel = readMux(1); // Assuming water level sensor is connected to C1
  int photoresistor = readMux(2); // Assuming photoresistor is connected to C2
  byte temperature = 0;
  byte humidity = 0;

  if (dht11.read(DHTPin, &temperature, &humidity, NULL) != SimpleDHTErrSuccess) {
    Serial.println("Failed to read from DHT sensor!");
  }

  String html = "<html><body>";
  html += "<h1>Sensor Readings</h1>";
  html += "<p>Soil Moisture: " + String(soilMoisture) + "</p>";
  html += "<p>Water Level: " + String(waterLevel) + "</p>";
  html += "<p>Light Intensity: " + String(photoresistor) + "</p>";
  html += "<p>Temperature: " + String((int)temperature) + " °C</p>";
  html += "<p>Humidity: " + String((int)humidity) + " %</p>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

int readMux(int channel) {
  int controlPin[] = {s0, s1, s2, s3};

  for (int i = 0; i < 4; i++) {
    if (channel & (1 << i)) {
      digitalWrite(controlPin[i], HIGH);
    } else {
      digitalWrite(controlPin[i], LOW);
    }
  }
  delay(1);
  return analogRead(analogPin);
}
