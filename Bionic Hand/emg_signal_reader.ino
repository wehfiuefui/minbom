// Water sensor connected to A0 (analog pin)
int waterSensorPin = A0;

void setup() {
  Serial.begin(9600);
  Serial.println("Water Sensor Test");
}

void loop() {
  // Read analog value (0–1023)
  int waterValue = analogRead(waterSensorPin);

  // Print the raw value
  Serial.print("Water Sensor Value: ");
  Serial.println(waterValue);

  // Interpret the value (optional)
  if (waterValue < 300) {
    Serial.println("Dry");
  } else if (waterValue < 700) {
    Serial.println("Moist");
  } else {
    Serial.println("Wet");
  }

  delay(1000); // 1 second delay
}
