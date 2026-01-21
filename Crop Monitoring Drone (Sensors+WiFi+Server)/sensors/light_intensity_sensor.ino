const int photocellPin = A0;  // Analog input pin for the photocell

void setup() {
  Serial.begin(9600);  // Initialize the serial communication
}

void loop() {
  int sensorValue = analogRead(photocellPin);  // Read the analog value from the photocell
  int lightIntensity = map(sensorValue, 0, 1023, 0, 100);  // Map the sensor value to a percentage between 0 and 100

  Serial.print("Light Intensity: ");
  Serial.print(lightIntensity);
  Serial.println("%");

  delay(1000);  // Delay for 1 second before taking the next reading
}
