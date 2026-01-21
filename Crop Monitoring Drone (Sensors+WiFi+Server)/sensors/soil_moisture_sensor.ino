const int moisturePin = A0;  // Analog input pin for the moisture sensor

void setup() {
  Serial.begin(9600);  // Initialize the serial communication
}

void loop() {
  int sensorValue = analogRead(moisturePin);  // Read the analog value from the moisture sensor

  int percentMoisture = map(sensorValue, 0, 1023, 0, 100);  // Map the sensor value to a percentage between 0 and 100

  Serial.print("Moisture level: ");
  Serial.print(percentMoisture);
  Serial.println("%");

  delay(1000);  // Delay for 1 second before taking the next reading
}
