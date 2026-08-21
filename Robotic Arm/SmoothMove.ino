#include <Servo.h>

// Servo objects
Servo base;
Servo shoulder;
Servo elbow;
Servo gripper;

// Initial Positions
int basePos = 90;
int shoulderPos = 90;
int elbowPos = 90;
int gripperPos = 90;

const int MOVE_DELAY = 5;

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
    elbow.write(90);
    gripper.write(90);

    delay(1000);

    Serial.println("R = Pick & Place RED (LEFT)");
    Serial.println("B = Pick & Place BLUE (RIGHT)");
}

//----------------------------------------------------------
// Loop
//----------------------------------------------------------
void loop()
{
    if (Serial.available())
    {
        char command = Serial.read();
        if (command == 'R') { pickObject(); placeLeft(); }
        if (command == 'B') { pickObject(); placeRight(); }
    }
}
