#include <DRV8835MotorShield.h>

// ranges:
// (350 - 360) -> white
// (400 - 450) -> grey
// (450+) -> black

// < 50 white
// < 80 grey
// > 300 black

DRV8835MotorShield motors;

// Global variables
int whiteLightVal;
int greyLightVal;
int blackLightVal;

int period = 100;
unsigned long time_now = millis();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //calibrate();
}

void loop() {
  // put your main code here, to run repeatedly:

  travelStraight();
  findLine();
  delay(3000);
  followLine();
}

void calibrate() {
  
  // calibrate light sensor variables
  whiteLightVal = readLightSensor();
  greyLightVal = whiteLightVal + 50;
  blackLightVal = whiteLightVal + 100;
}

void travelStraight() {
  
  //go till black line by enabling motor
  while(readColor() != 'b')
  goStraight();
  
  stopMotors();
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
  goStraight();
  stopMotors();
  // turn CCW until you find black
  while (readColor() != 'b')
  turnCCW(2);
  stopMotors();

  time_now = millis();
  period = 100;
  while (millis() < time_now + period){
    turnCCW(2);
  }
  stopMotors();
}


void followLine(){
  int counter = 0;
  while (true){
    checkForObstacle(16);
    switch (readColor()){
      case 'b':
        goStraight();
        counter = 0;
        break;
      case 'w':
        goBacktoBlack();
        counter = 0;
      case 'g':
        counter++;
        if(counter >= 10) {
          stopMotors();
          Serial.println("stopped");
          delay(4000);
          counter = 0;
          
        }
        
      default:
        goStraight();
    }
  }
}

void goBacktoBlack() {
  stopMotors();
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

void goStraight() {
  motors.setM1Speed(200);
  motors.setM2Speed(200);
  delay(2);
}

void stopMotors(){
  motors.setM1Speed(0);
  motors.setM2Speed(0);
  delay(2);
}

int average (int * array, int len) {
  long sum = 0L ;  // sum will be larger than an item, long for safety.
  for (int i = 0 ; i < len ; i++) {
    sum += array [i];
  }
  return (int)(((float) sum) / len);
}

char readColor() {

//  int color = readLightSensor();
//  if (abs(color - whiteLightVal) > 100) {
//    return 'b';
//  } else if (abs(color - whiteLightVal) < 20) {
//    return 'w';
//  } else {
//    return 'g';
//  }

  int color = readLightSensor();
  if (color < 50)
  return 'w';
  if (color < 80)
  return 'w';
  return 'b';
}

int readLightSensor() {

  // return average of 5 messuragents
  int buff[20];
  for (int i = 0; i < 20; i++) {
    buff[i] = analogRead(A0);
    delay(2);
  }
//  int val = average(buff, 20);
//  Serial.println(val);
  return average(buff, 20);
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
