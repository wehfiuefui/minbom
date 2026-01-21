#include <ESP8266WiFi.h>

const char* ssid = "TECNO CAMON 20 Pro";         // Your Wi-Fi SSID
const char* password = "66533300";   // Your Wi-Fi password

void setup() {
  Serial.begin(115200);
  delay(1000);  // Short delay for serial communication to initialize

  Serial.println("Starting WiFi connection...");

  // Start Wi-Fi connection
  WiFi.begin(ssid, password);

  // Debug output
  Serial.print("Connecting to Wi-Fi");

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    // Timeout after 30 seconds if Wi-Fi connection is not established
    if (millis() - startTime >= 30000) {
      Serial.println("\nWi-Fi connection timed out!");
      return;  // Exit setup if Wi-Fi doesn't connect
    }
  }

  // Successful Wi-Fi connection
  Serial.println("\nWi-Fi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Your main code here
}
