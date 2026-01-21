#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SimpleDHT.h>

// Replace with your WiFi credentials
const char* ssid = "STUD_5G";
const char* password = "myschooliscool";

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
  Serial.println("Setup starting...");
  
  // Initialize multiplexer control pins
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  // Wait for WiFi connection with a timeout
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) { // 10 seconds timeout
    delay(500);
    Serial.print(".");
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect to WiFi. Please check your credentials.");
    return;
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
  int waterLevel = readMux(1);   // Assuming water level sensor is connected to C1
  int photoresistor = readMux(2); // Assuming photoresistor is connected to C2
  byte temperature = 0;
  byte humidity = 0;

  if (dht11.read(DHTPin, &temperature, &humidity, NULL) != SimpleDHTErrSuccess) {
    Serial.println("Failed to read from DHT sensor!");
  }

  // Prepare styled HTML response for web server
  String html = "<html><head><style>";
  html += "body {";
  html += "  background-image: url('https://example.com/background.jpg');"; // Replace with your background image URL
  html += "  background-size: cover;";
  html += "  background-position: center;";
  html += "  font-family: Arial, sans-serif;";
  html += "  color: #333333;";
  html += "}";
  html += ".sensor-container {";
  html += "  background-color: rgba(255, 255, 255, 0.9);"; // White container with slight transparency
  html += "  border: 2px solid #0066cc;"; // Blue border
  html += "  border-radius: 10px;";
  html += "  box-shadow: 0 0 10px rgba(0, 0, 0, 0.3);";
  html += "  padding: 20px;";
  html += "  margin: 20px auto;";
  html += "  max-width: 500px;";
  html += "}";
  html += "h1 {";
  html += "  color: #0066cc;"; // Blue header text
  html += "}";
  html += "p {";
  html += "  margin: 10px 0;";
  html += "}";
  html += "</style></head><body>";
  html += "<div class='sensor-container'>";
  html += "<h1>Sensor Readings</h1>";
  html += "<p>Soil Moisture: " + String(soilMoisture) + "</p>";
  html += "<p>Water Level: " + String(waterLevel) + "</p>";
  html += "<p>Light Intensity: " + String(photoresistor) + "</p>";
  html += "<p>Temperature: " + String((int)temperature) + " °C</p>";
  html += "<p>Humidity: " + String((int)humidity) + " %</p>";
  html += "</div>";
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