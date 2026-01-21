#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SimpleDHT.h>

const char* ssid = "001";
const char* password = "12345678";

const int s0 = D1;
const int s1 = D2;
const int s2 = D3;
const int s3 = D4;

const int analogPin = A0;

const int DHTPin = D5;
SimpleDHT11 dht11;

ESP8266WebServer server(80);

// Function to read from the multiplexer
int readMux(int channel) {
  int controlPin[] = {s0, s1, s2, s3};

  for (int i = 0; i < 4; i++) {
    digitalWrite(controlPin[i], (channel & (1 << i)) ? HIGH : LOW);
  }

  delay(1);
  return analogRead(analogPin);
}

// Function to handle root path "/"
void handleRoot() {
  int soilMoisture = readMux(0); // Assuming soil moisture sensor is connected to C0
  int waterLevel = readMux(1); // Assuming water level sensor is connected to C1
  int photoresistor = readMux(2); // Assuming photoresistor is connected to C2
  byte temperature = 0;
  byte humidity = 0;

  if (dht11.read(DHTPin, &temperature, &humidity, NULL) != SimpleDHTErrSuccess) {
    Serial.println("Failed to read from DHT sensor!");
  }

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>🍅 Tomato Yield Prediction App</title>
  <link rel="stylesheet" href="style.css">
</head>
<body>
  <header>
    <h1>Sensor Data</h1>
  </header>
  <div class="container">
    <p>Soil Moisture: )rawliteral" + String(soilMoisture) + R"rawliteral(</p>
    <p>Water Level: )rawliteral" + String(waterLevel) + R"rawliteral(</p>
    <p>Light Intensity: )rawliteral" + String(photoresistor) + R"rawliteral(</p>
    <p>Temperature: )rawliteral" + String((int)temperature) + R"rawliteral(°C</p>
    <p>Humidity: )rawliteral" + String((int)humidity) + R"rawliteral(%</p>
  </div>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

// Function to handle style.css
void handleStyle() {
  String css = R"rawliteral(
* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
  max-width: 1000px;
  margin-left: auto;
  margin-right: auto;
}

body {
  font-family: 'Poppins', sans-serif;
  line-height: 1.6;
  background-color: #ffc0cb; /* Light pink background color */
  animation: fade-in 1s ease;
}

/* Header and Navigation */
header {
  background-color: #008000; /* Green background color */
  padding: 1rem;
  color: #fff;
  display: flex;
  justify-content: space-between;
  align-items: center;
}

nav ul {
  display: flex;
  list-style-type: none;
}

nav ul li {
  margin-left: 1rem;
}

nav ul li a {
  color: #fff;
  text-decoration: none;
  padding: 0.5rem;
}

nav ul li a:hover {
  background-color: #006400; /* Dark green background color */
}

.menu-toggle {
  display: none;
  font-size: 1.5rem;
  color: #fff;
  cursor: pointer;
}

.container {
  max-width: 600px;
  margin: 0 auto;
  padding: 20px;
  background-color: #fff;
  box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
  border-radius: 8px;
  animation: slide-up 1s ease;
}

h1 {
  text-align: center;
  margin-bottom: 20px;
  color: #333;
  text-shadow: 1px 1px 0 #fff;
}
)rawliteral";
  server.send(200, "text/css", css);
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
    Serial.print("10");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.on("/", handleRoot);
  server.on("/style.css", handleStyle);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
