#include <Servo.h>
#include <Arduino_RouterBridge.h> // MCU <-> MPU communication

// Servo objects
Servo base, shoulder, elbow, gripper;

// Initial positions
int basePos = 90;
int shoulderPos = 90;
int elbowPos = 60;
int gripperPos = 90;

const int MOVE_DELAY = 5;
volatile bool armBusy = false;

//----------------------------------------------------------
// Smooth Servo Movement
//----------------------------------------------------------
void smoothMove(Servo &servo, int &currentPos, int targetPos)
{
    if (currentPos < targetPos)
    {
        for (int pos = currentPos; pos <= targetPos; pos++)
        {
            servo.write(pos);
            delay(MOVE_DELAY);
        }
    }
    else
    {
        for (int pos = currentPos; pos >= targetPos; pos--)
        {
            servo.write(pos);
            delay(MOVE_DELAY);
        }
    }

    currentPos = targetPos;
    delay(200);
}

//----------------------------------------------------------
// Pick Object From Center
//----------------------------------------------------------
void pickObject()
{
    smoothMove(gripper, gripperPos, 180);   // Open Gripper
    delay(300);
    smoothMove(elbow, elbowPos, 120);       // Elbow Forward
    smoothMove(shoulder, shoulderPos, 60);  // Shoulder Down
    smoothMove(gripper, gripperPos, 90);    // Grab Object
    delay(500);
    smoothMove(shoulder, shoulderPos, 90);  // Lift Object
    smoothMove(elbow, elbowPos, 90);        // Elbow Back to Center
}

//----------------------------------------------------------
// Place Object on LEFT (RED)
//----------------------------------------------------------
void placeLeft()
{
    smoothMove(base, basePos, 140);         // Rotate Base Left
    delay(300);
    smoothMove(elbow, elbowPos, 120);
    smoothMove(shoulder, shoulderPos, 60);
    smoothMove(gripper, gripperPos, 180);   // Release Object
    delay(500);
    smoothMove(shoulder, shoulderPos, 90);
    smoothMove(elbow, elbowPos, 90);
    base.write(90);                         // Return Base Center
}

//----------------------------------------------------------
// Place Object on RIGHT (BLUE)
//----------------------------------------------------------
void placeRight()
{
    smoothMove(base, basePos, 40);          // Rotate Base Right
    delay(300);
    smoothMove(elbow, elbowPos, 120);
    smoothMove(shoulder, shoulderPos, 60);
    smoothMove(gripper, gripperPos, 180);   // Release Object
    delay(500);
    smoothMove(shoulder, shoulderPos, 90);
    smoothMove(elbow, elbowPos, 90);
    base.write(90);                         // Return Base Center
}

//----------------------------------------------------------
// RPC FUNCTION - called by Python
//----------------------------------------------------------
int pick_and_place(int colorCode)
{
    if (armBusy) return -1;                        // Arm already busy
    if (colorCode != 1 && colorCode != 2) return -2; // Invalid code

    armBusy = true;
    if (colorCode == 1) { pickObject(); placeLeft(); }   // 1 = RED
    else if (colorCode == 2) { pickObject(); placeRight(); } // 2 = BLUE
    armBusy = false;

    return 1; // Success
}

//----------------------------------------------------------
// Setup
//----------------------------------------------------------
void setup()
{
    Serial.begin(9600);

    base.attach(3);
    shoulder.attach(5);
    elbow.attach(6);
    gripper.attach(9);

    base.write(90);
    shoulder.write(90);
    elbow.write(60);
    gripper.write(90);

    delay(1000);

    Bridge.begin(); // Start MCU <-> MPU link
    Bridge.provide("pick_and_place", pick_and_place); // Register RPC
}

//----------------------------------------------------------
// Loop (event-driven via Bridge)
//----------------------------------------------------------
void loop()
{
    // Nothing here — Python calls functions via Bridge
}
