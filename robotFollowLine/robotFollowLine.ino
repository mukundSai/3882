#include <DRV8835MotorShield.h>

DRV8835MotorShield motors;

// Global variables
int whiteLightVal;
int greyLightVal;
int blackLightVal;

void setup() {
  
  // bound rate for serial in/out
  Serial.begin(9600);
  // light sensor calibration
  calibrate();
}

void loop() {
  
  travelStraight();
  findLine();
  followLine();
}

void calibrate() {
  
  // white
  whiteLightVal = readLightSensor(100);
  goStraight(20);
  stopMotors(100);
  // black
  blackLightVal = readLightSensor(100);
  goStraight(20);
  stopMotors(100);
  // grey
  greyLightVal = readLightSensor(100);
  goStraight(20);
  stopMotors(100);
}

void travelStraight() {
  
  // go till black line by enabling motor
  while(readColor() != 'b')
  goStraight(20);
  // stop for reference
  stopMotors(100);
}

void checkForObstacle(long distance) {
  //read ultrasonic sensor and if object within the distance then turn
  if (readUltrasonicSensor() < distance) {
    turn();
  }
}

void turn() {
  Serial.println("turning");
  turnCCW(500);
  int time = 1000;
  for (int i = 0; i <= time; i++) {
      if(readColor() == 'b') break;
      turnCCW(1);
    }
}

void findLine(){
  
  // go foward until you find white
  while (readColor() != 'w')
  goStraight(20);
  stopMotors(100);
  // turn CCW until you find black
  while (readColor() != 'b')
  turnCCW(2);
  stopMotors(100);
  // pause for reference
  turnCCW(2);
  stopMotors(100);
}

void followLine() {
  
  // keep track of greys detected
  int counter = 0;

  // perpetual loop
  while (true){

    // check for obstacle
    checkForObstacle(16);

    // if no obstacle -> move according to color detected
    switch (readColor()){
      // black
      case 'b':
        goStraight(20);
        counter = 0;
        break;
      // white
      case 'w':
        goBacktoBlack();
        counter = 0;
      // grey
      case 'g':
        counter++;
        // given 3 greys -> call confirmGrey()
        if(counter >= 4 && confirmGrey()) {
          stopMotors(100);
          waitForTouchSensor();
          counter = 0;   
        } else if (counter >= 4) {
          counter = 0;
        }
      default:
        // go straight by default
        goStraight(20);
    }
  }
}

boolean confirmGrey() {
  turnCW(150);
  int val1 = readColor();
  turnCCW(300);
  int val2 = readColor();
  turnCW(150);
  return val1 == val2 && val1 == 'g';
}

void goBacktoBlack() {
  
  stopMotors(100);
  int time = 1;
  while(readColor() != 'b') {
    
    for (int i = 0; i <= time; i++) {
      if(readColor() == 'b') break;
      turnCCW(2);
    }
    time *= 2;
    if(readColor() == 'b') break;
    
    for (int i = 0; i <= time; i++) {
      if(readColor() == 'b') break;
      turnCW(2);
    }
    time *= 2;
    if(readColor() == 'b') break;
  }
}

///// Helper functions //////////////////////////////////////////////////////////////////////

void turnCW(int time) {
  motors.setM1Speed(-150);
  motors.setM2Speed(150);
  delay(time);
}

void turnCCW(int time) {
  motors.setM1Speed(150);
  motors.setM2Speed(-150);
  delay(time);
}

void goStraight(int time) {
  motors.setM1Speed(200);
  motors.setM2Speed(200);
  delay(time);
}

void stopMotors(int time){
  motors.setM1Speed(0);
  motors.setM2Speed(0);
  delay(time);
}

int average (int * array, int len) {
  long sum = 0L ;  // sum will be larger than an item, long for safety.
  for (int i = 0 ; i < len ; i++) {
    sum += array [i];
  }
  return (int)(((float) sum) / len);
}

char readColor() {

  int color = readLightSensor(20);
  int black = abs(color - blackLightVal);
  int white = abs(color - whiteLightVal);
  int grey = abs(color - greyLightVal);

  if (black < white && black < grey) {
    return 'w';
  } else if (grey < white && grey < black) {
    return 'g';
  } else {
    return 'b';
  }
}

void waitForTouchSensor() {

  // wait until touch sensor is pressed
  // pressed == 0
  while(readTouchSensor()) {
  
  }
}

int readLightSensor(int len) {

  // return average of len messuragents
  int buff[len];
  for (int i = 0; i < len; i++) {
    buff[i] = analogRead(A0);
    delay(2);
  }
  return average(buff, len);
}

int readTouchSensor() {
  int val = 0;
  delay(100);
  val = analogRead(A1);
  if (val != 0) {
    val = 1;
  }
  return val;
}

long readUltrasonicSensor() {
  int trigPin = 11;    // Trigger
  int echoPin = 12;    // Echo
  long duration, cm;
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  delay(250);
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  // Convert the time into a distance
  cm = (duration/2.0) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  return cm;
}
