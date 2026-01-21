#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SimpleDHT.h>

// WiFi Credentials
const char* ssid = "STUD";
const char* password = "myschooliscool";

// Server URL (Change this to your actual server address)
const char* serverAddress = "https://futureyields.uz//uploaddata.php";

// Sensor Pins
const int DHTPin = D5;
SimpleDHT11 dht11;
const int moisturePin = A0;
const int lightPin = D6;

// Web Server
ESP8266WebServer server(80);
String selectedCrop = "Wheat";

// Crop Data Structure
struct Crop {
  String name;
  int optimalMoisture;
  int optimalLight;
  int optimalTemp;
  int optimalHumidity;
};

// Crop Database
Crop crops[] = {
  {"Wheat", 50, 800, 20, 40},
  {"Corn", 60, 900, 25, 50},
  {"Tomato", 70, 1000, 22, 60},
  {"Cotton", 40, 850, 30, 30},
  {"Rice", 80, 1100, 27, 70}
};

// Setup
void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Web server routes
  server.on("/", handleRoot);
  server.on("/select", handleSelectCrop);
  server.begin();
  Serial.println("HTTP server started");
}

// Loop
void loop() {
  server.handleClient();
  
  static unsigned long lastSendTime = 0;
  if (millis() - lastSendTime > 10000) { // Send data every 10 seconds
    lastSendTime = millis();

    // Read sensors
    byte temperature = 0, humidity = 0;
    dht11.read(DHTPin, &temperature, &humidity, NULL);
    int moisture = analogRead(moisturePin);
    int light = analogRead(lightPin);
    int yieldPercentage = calculateYield(selectedCrop, moisture, light, temperature, humidity);

    // Send data to server
    sendDataToServer(moisture, light, temperature, humidity, yieldPercentage);
  }
}

// Handle Root Web Page
void handleRoot() {
  byte temperature = 0, humidity = 0;
  dht11.read(DHTPin, &temperature, &humidity, NULL);
  int moisture = analogRead(moisturePin);
  int light = analogRead(lightPin);

  int yieldPercentage = calculateYield(selectedCrop, moisture, light, temperature, humidity);
  String yieldQuality = (yieldPercentage > 70) ? "Good" : (yieldPercentage > 40) ? "Average" : "Poor";

  // HTML Response
  String html = "<html><head><style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; background-color: #222; color: white; }";
  html += ".dashboard { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; max-width: 400px; margin: auto; }";
  html += ".card { padding: 20px; border-radius: 10px; font-size: 24px; font-weight: bold; }";
  html += ".moisture { background-color: #d9534f; }";
  html += ".light { background-color: #5bc0de; }";
  html += ".temperature { background-color: #5cb85c; }";
  html += ".humidity { background-color: #31b0d5; }";
  html += ".yield { background-color: #f0ad4e; }";
  html += "button { margin: 5px; padding: 10px; font-size: 18px; cursor: pointer; }";
  html += "</style></head><body>";
  html += "<h2>Selected Crop: " + selectedCrop + "</h2>";
  html += "<div class='dashboard'>";
  html += "<div class='card moisture'>Soil Moisture<br>" + String(moisture) + "</div>";
  html += "<div class='card light'>Light Intensity<br>" + String(light) + "</div>";
  html += "<div class='card temperature'>Temperature<br>" + String(temperature) + " °C</div>";
  html += "<div class='card humidity'>Humidity<br>" + String(humidity) + " %</div>";
  html += "<div class='card yield'>Yield Prediction:<br>" + yieldQuality + " (" + String(yieldPercentage) + "%)</div>";
  html += "</div>";
  html += "<h3>Select Crop</h3>";
  html += "<form action='/select' method='GET'>";
  html += "<button name='crop' value='Wheat'>Wheat</button>";
  html += "<button name='crop' value='Corn'>Corn</button>";
  html += "<button name='crop' value='Tomato'>Tomato</button>";
  html += "<button name='crop' value='Cotton'>Cotton</button>";
  html += "<button name='crop' value='Rice'>Rice</button>";
  html += "</form>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

// Handle Crop Selection
void handleSelectCrop() {
  if (server.hasArg("crop")) {
    selectedCrop = server.arg("crop");
  }
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
}

// Send Data to Remote Server
void sendDataToServer(int moisture, int light, int temp, int humidity, int yieldPercentage) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    String url = String(serverAddress) + "?moisture=" + String(moisture)
                 + "&light=" + String(light)
                 + "&temperature=" + String(temp)
                 + "&humidity=" + String(humidity)
                 + "&yield=" + String(yieldPercentage)
                 + "&crop=" + selectedCrop;

    http.begin(client, url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      Serial.println("Data sent successfully: " + http.getString());
    } else {
      Serial.println("Error sending data: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}

// Yield Calculation
int calculateYield(String crop, int moisture, int light, int temp, int humidity) {
  for (Crop c : crops) {
    if (c.name == crop) {
      float moistureFactor = max(0, 100 - abs(c.optimalMoisture - moisture));
      float lightFactor = max(0, 100 - abs(c.optimalLight - light));
      float tempFactor = max(0, 100 - abs(c.optimalTemp - temp));
      float humidityFactor = max(0, 100 - abs(c.optimalHumidity - humidity));
      return (moistureFactor + lightFactor + tempFactor + humidityFactor) / 4;
    }
  }
  return 0;
}
