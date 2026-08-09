#include <Servo.h>

const int trigPin = 9;
const int echoPin = 10;
Servo myServo;

void setup() {
  Serial.begin(9600); // Initialize serial communication
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  myServo.attach(3); // Attach servo to pin 3
}

void loop() {
  for (int angle = 0; angle <= 180; angle += 1) {
    myServo.write(angle); // Rotate servo to the current angle
    delay(15); // Wait for the servo to move

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    float distance = (duration * 0.034) / 2; // Calculate distance in cm

    Serial.print(angle); // Send servo angle
    Serial.print(",");
    Serial.println(distance); // Send distance
  }

  for (int angle = 180; angle >= 0; angle -= 1) {
    myServo.write(angle); // Rotate servo back to the current angle
    delay(15);

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    float distance = (duration * 0.034) / 2;

    Serial.print(angle); // Send servo angle
    Serial.print(",");
    Serial.println(distance); // Send distance
  }
}
