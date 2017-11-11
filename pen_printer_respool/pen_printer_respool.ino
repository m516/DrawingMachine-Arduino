#include <AccelStepper.h>
#define MAX_SPEED 700.0
//Stepper motors
//I'm using two 28BYJ-48 steppers with ULN2003 Darlington arrays
//This one is on ports 2, 3, 4, and 5...
AccelStepper stepper1(AccelStepper::HALF4WIRE, 5, 3, 4, 2);
//And this one is on ports 8, 9, 10, and 11.
AccelStepper stepper2(AccelStepper::HALF4WIRE, 11, 9, 10, 8);



void setup() {
  //Set properties of motors
  stepper1.setMaxSpeed(MAX_SPEED);
  stepper2.setMaxSpeed(MAX_SPEED);
  pinMode(7, INPUT_PULLUP);
}

void loop() {
  if(digitalRead(7)==HIGH){
    stepper1.setSpeed(MAX_SPEED);
    stepper2.setSpeed(MAX_SPEED);
  }
  else if(stepper1.currentPosition()>0){
    stepper1.setSpeed(-MAX_SPEED);
    stepper2.setSpeed(-MAX_SPEED);
  }
  else{
    stepper1.setSpeed(0);
    stepper2.setSpeed(0);
  }
  stepper1.runSpeed();
  stepper2.runSpeed();
}


