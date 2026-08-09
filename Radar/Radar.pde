import processing.serial.*;

Serial myPort; // Serial communication object
float[] distances = new float[19]; // Stores distances for 0° to 180°, at 10° intervals
int servoAngle = 0; // Current angle of the servo
boolean dataReceived = false; // Flag to check if data has been received
boolean sweepCompleted = false; // Tracks whether the sweep has just completed
float currentDistance = 0; // Stores the most recently detected distance

void setup() {
  size(800, 400); // Canvas size for semicircular radar
  myPort = new Serial(this, "COM3", 9600); // Replace "COM3" with your Arduino port
  myPort.bufferUntil('\n'); // Wait for newline character to process data
}

void draw() {
  background(0); // Black background

  // Display the distance counter at the top
  drawDistanceCounter();

  translate(width / 2, height); // Move origin to the bottom center

  drawRadarGrid(); // Draw the radar grid
  drawStoredTargets(); // Draw all stored targets
  drawRadarBeam(); // Animate the radar beam based on servo angle

  // Reset distances if a full sweep has just completed
  if (sweepCompleted) {
    resetDistances();
    sweepCompleted = false; // Reset the completion flag
  }
}

void drawDistanceCounter() {
  textSize(32); // Set text size for counter
  fill(0, 255, 0); // Green text for distance
  textAlign(CENTER); // Center the text
  text("Distance: " + int(currentDistance) + " cm", width / 2, 50); // Display the latest distance value at the top
}

void drawRadarGrid() {
  stroke(0, 0, 255); // Green for radar grid
  noFill();

  // Draw concentric semicircles (range indicators)
  for (int r = 100; r <= 400; r += 100) {
    arc(0, 0, r * 2, r * 2, PI, TWO_PI); // Semicircles expanding outward
  }

  // Draw angle spokes
  for (int a = 0; a <= 180; a += 30) {
    float x = 400 * cos(radians(a)); // X-coordinate for the spoke endpoint
    float y = -400 * sin(radians(a)); // Y-coordinate for the spoke endpoint
    line(0, 0, x, y); // Line from center outward
  }
}

void drawStoredTargets() {
  strokeWeight(2); // Line thickness for stored targets

  // Loop through all stored distances and draw lines
  for (int i = 0; i < distances.length; i++) {
    if (distances[i] > 0) { // Plot valid distances only
      float x = distances[i] * cos(radians(i * 10)); // X-coordinate for target
      float y = -distances[i] * sin(radians(i * 10)); // Y-coordinate for target

      stroke(0, 255, 0); // Green lines for far targets
      if (distances[i] < 150) {
        stroke(255, 0, 0); // Red for close targets
      } else if (distances[i] < 300) {
        stroke(255, 255, 0); // Yellow for medium targets
      }

      line(0, 0, x, y); // Draw the stored target line
    }
  }
}

void drawRadarBeam() {
  stroke(0, 255, 0); // Green radar beam
  strokeWeight(3);

  // Calculate the beam's endpoint based on the servo angle
  float x = 400 * cos(radians(servoAngle)); // X-coordinate of beam endpoint
  float y = -400 * sin(radians(servoAngle)); // Y-coordinate of beam endpoint

  line(0, 0, x, y); // Draw the beam at the servo's angle

  // Check if a sweep is complete
  if (servoAngle == 180 || servoAngle == 0) {
    sweepCompleted = true; // Mark the sweep as complete
  }
}

void resetDistances() {
  println("Resetting data after sweep...");
  for (int i = 0; i < distances.length; i++) {
    distances[i] = 0; // Clear all stored distances
  }
}

void serialEvent(Serial myPort) {
  String data = myPort.readStringUntil('\n'); // Read data from Serial port
  println("Data Received: " + data); // Debugging

  if (data != null && data.length() > 0) {
    String[] parts = data.split(","); // Split angle and distance
    try {
      servoAngle = int(parts[0]); // Parse servo angle
      float distance = float(parts[1]); // Parse distance
      distances[servoAngle / 10] = distance; // Store distance in the array for its angle
      currentDistance = distance; // Update the current distance for the counter
      dataReceived = true; // Indicate that valid data was received
    } catch (Exception e) {
      println("Error parsing data: " + data); // Handle invalid data
    }
  }
}
