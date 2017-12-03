#include <AccelStepper.h>
#define D 15.0
#define MAX_SPEED 700.0
#define WINCH_DIAMETER 0.3346456692913386
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

void setup() {
  //Set properties of motors
  stepper1.setMaxSpeed(MAX_SPEED);
  stepper2.setMaxSpeed(MAX_SPEED);

  pinMode(13, OUTPUT);

  Serial.begin(38400);
  Serial.setTimeout(10);
  Serial.println("Arduino is online!");

  //Initialize a and b
  while (Serial.available() < 1) {}
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  a = Serial.parseFloat();
  Serial.print("A is ");
  Serial.println(a);
  while (Serial.available() < 1) {}
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
  b = Serial.parseFloat();
  Serial.print("B is ");
  Serial.println(b);

  //Initialize the stepper positions
  stepper1.setCurrentPosition(lengthToSteps(a));
  stepper2.setCurrentPosition(lengthToSteps(b));

  //Initialize X and Y
  calculateXY();
  init_x = x;
  init_y = y;
}

void loop() {
  if (Serial.available() > 0) {
    String line = Serial.readString();
    if (line.charAt(0) == '@') {
      String num1 = line;
      String num2 = line;
      num1.remove(0, 1);
      num1.remove(num1.indexOf(','));
      num2.remove(0, num2.indexOf(',') + 1);

      //Display what Arduino recieved
      //      Serial.print("Arduino recieved ");
      //      Serial.print(num1.toFloat());
      //      Serial.print(",");
      //      Serial.println(num2.toFloat());
      Serial.println("g");

      //Move the pen
      digitalWrite(13, HIGH);
      goTo(num1.toFloat() + init_x, num2.toFloat() + init_y, 0.001);
      digitalWrite(13, LOW);
    }
    else{
      Serial.println("e");
    }
  }
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
  //return steps      *       circumference    / number of steps per full rotation
  return float(steps) *  (WINCH_DIAMETER * PI) / 4096.0;
}
long lengthToSteps(float len) {
  //           length    * number of steps per full rotation / circumference
  return long(float(len) *           4096.0                  / (WINCH_DIAMETER * PI));
}


