// === Motor Pins ===
const int leftMotorForward = 5;
const int leftMotorBackward = 6;
const int rightMotorForward = 9;
const int rightMotorBackward = 10;

// === Digital LDR Sensor Pins ===
const int ldrLeft = 2;
const int ldrCenter = 3;
const int ldrRight = 4;

// === Speed ===
int motorSpeed = 180;

void setup() {
  pinMode(leftMotorForward, OUTPUT);
  pinMode(leftMotorBackward, OUTPUT);
  pinMode(rightMotorForward, OUTPUT);
  pinMode(rightMotorBackward, OUTPUT);

  pinMode(ldrLeft, INPUT);
  pinMode(ldrCenter, INPUT);
  pinMode(ldrRight, INPUT);

  Serial.begin(9600);
}

void loop() {
  int left = digitalRead(ldrLeft);
  int center = digitalRead(ldrCenter);
  int right = digitalRead(ldrRight);

  // Debugging
  Serial.print("L:");
  Serial.print(left);
  Serial.print(" C:");
  Serial.print(center);
  Serial.print(" R:");
  Serial.println(right);

  // === Updated Logic ===
  if (left == LOW && center == LOW && right == LOW) {
    moveForward(); // Light on all 3 sensors
  } else if (center == LOW && left == LOW && right == HIGH) {
    turnLeft(); // Light on center and left
  } else if (center == LOW && left == HIGH && right == LOW) {
    turnRight(); // Light on center and right
  } else if (center == LOW && left == HIGH && right == HIGH) {
    moveForward(); // Light only on center
  } else if (left == LOW) {
    turnLeft();
  } else if (right == LOW) {
    turnRight();
  } else {
    stopMotors(); // No clear light direction
  }

  delay(100);
}

// === Motor Control Functions ===
void moveForward() {
  analogWrite(leftMotorForward, motorSpeed);
  analogWrite(leftMotorBackward, 0);
  analogWrite(rightMotorForward, motorSpeed);
  analogWrite(rightMotorBackward, 0);
}

void turnLeft() {
  analogWrite(leftMotorForward, 0);
  analogWrite(leftMotorBackward, 0);
  analogWrite(rightMotorForward, motorSpeed);
  analogWrite(rightMotorBackward, 0);
}

void turnRight() {
  analogWrite(leftMotorForward, motorSpeed);
  analogWrite(leftMotorBackward, 0);
  analogWrite(rightMotorForward, 0);
  analogWrite(rightMotorBackward, 0);
}

void stopMotors() {
  analogWrite(leftMotorForward, 0);
  analogWrite(leftMotorBackward, 0);
  analogWrite(rightMotorForward, 0);
  analogWrite(rightMotorBackward, 0);
}
