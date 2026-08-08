#define buzzerPin 9 // Connect your buzzer to pin 9

// Define the melody (frequencies in Hz) and note durations (in milliseconds)
int melody[] = {262, 262, 294, 262, 349, 330, // "Happy birthday to you"
                262, 262, 294, 262, 392, 349, // "Happy birthday to you"
                262, 262, 523, 440, 349, 330, 294, // "Happy birthday dear [Name]"
                466, 466, 440, 349, 392, 349};    // "Happy birthday to you"
int noteDurations[] = {500, 500, 1000, 1000, 1000, 2000, // Durations for each note
                       500, 500, 1000, 1000, 1000, 2000,
                       500, 500, 1000, 1000, 500, 500, 1000,
                       500, 500, 1000, 1000, 1000, 2000};

void setup() {
  pinMode(buzzerPin, OUTPUT); // Set the buzzer pin as OUTPUT
}

void loop() {
  // Play each note in the melody
  for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++) {
    tone(buzzerPin, melody[i]);          // Play the note
    delay(noteDurations[i]);            // Wait for the note duration
    noTone(buzzerPin);                  // Stop the tone between notes
    delay(100);                         // Short gap between notes
  }
  delay(5000); // Wait 5 seconds before playing the tune again
}
