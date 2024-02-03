const int ldrPin = 8; // LDR connected to digital pin D2
const int buzzerPin = 9; // Buzzer connected to digital pin D9

void setup() {
  pinMode(ldrPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  int ldrStatus = digitalRead(ldrPin); // Read the status from LDR

  if (ldrStatus == LOW) {
    // If the LDR senses darkness, turn on the buzzer
    digitalWrite(buzzerPin, HIGH);
  } else {
    // If the LDR senses light, turn off the buzzer
    digitalWrite(buzzerPin, LOW);
  }

  delay(100); // Short delay for stability
}