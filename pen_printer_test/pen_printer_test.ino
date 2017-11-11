#include <AccelStepper.h>
#define D 15.0
#define MAX_SPEED 700.0
//Stepper motors
//I'm using two 28BYJ-48 steppers with ULN2003 Darlington arrays
//This one is on ports 2, 3, 4, and 5...
AccelStepper stepper1(AccelStepper::HALF4WIRE, 5, 3, 4, 2);
//And this one is on ports 8, 9, 10, and 11.
AccelStepper stepper2(AccelStepper::HALF4WIRE, 11, 9, 10, 8);



//Variables for position
float x;
float y;
float a;
float b;

//Variables that aid in testing -- not required for
//actual drawing
float init_x;
float init_y;

void setup() {
  //Set properties of motors
  stepper1.setMaxSpeed(MAX_SPEED);
  stepper2.setMaxSpeed(MAX_SPEED);

  Serial.begin(9600);
  a = prompt("What is the length of the string on the right?");
  b = prompt("What is the length of the string on the left?");
  calculateXY();
  Serial.print("X must be ");
  Serial.println(x);
  Serial.print("Y must be ");
  Serial.println(y);
  init_x = x;
  init_y = y;
  Serial.println("------------\n\n\n");
}

void loop() {
  for(float radius = 1; radius < 2; radius += 0.05){
    for(float dir = 0; dir < 2*PI; dir += 0.01){
      goTo(init_x+radius*cos(dir),init_y+radius*sin(dir),0.001);
    }
    goTo(init_x+radius, init_y, 0.001);
  }
}

float prompt(String question) {
  Serial.print(question);
  while (Serial.available() < 1) {}
  float f = Serial.parseFloat();
  Serial.print("   ");
  Serial.println(f);
  return f;
}

void calculateXY() {
  x = (a * a - b * b + D * D) / (2 * D);
  y = sqrt(a * a - x * x);
}

void calculateAB() {
  a = sqrt(x * x + y * y);
  b = sqrt(sq(D - x) + y * y);
}

void goTo(float newX, float newY, float threshold) {
  calculateAB();
  stepper1.setCurrentPosition(lengthToSteps(a));
  stepper2.setCurrentPosition(lengthToSteps(b));
  while (sqrt(sq(newX - x) + sq(newY - y)) > threshold) {
    float e,
          f,
          g;
    if (newX == x) {
      f = 0;
      g = newY > y ? 1.0 : 0;
    }
    else {
      e = (newY - y) / (newX - x);
      f = newX > x ? 1.0 : -1.0;
      g = e * f;
    }
    float h = (x * f + y * g) / sqrt(sq(x) + sq(y));
    float i = ((x - D) * f + y * g) / sqrt(sq(x) + sq(y));
    i *= MAX_SPEED / abs(h);
    h = MAX_SPEED * (h > 0 ? 1.0 : -1.0);
    if (abs(i) > MAX_SPEED) {
      h *= MAX_SPEED / abs(i);
      i = MAX_SPEED * (i > 0 ? 1.0 : -1.0);
    }
    stepper1.setSpeed(h);
    stepper2.setSpeed(i);
    stepper1.runSpeed();
    stepper2.runSpeed();
    a = stepsToLength(stepper1.currentPosition());
    b = stepsToLength(stepper2.currentPosition());
    calculateXY();
  }
}

float stepsToLength(long steps) {
  //return steps     *       circumference      / number of steps per full rotation
  return float(steps) *  (0.3346456692913386 * PI) / 4096.0;
}
long lengthToSteps(float len) {
  //           length    * number of steps per full rotation / circumference
  return long(float(len) *           4096.0                  / (0.3346456692913386 * PI));
}


