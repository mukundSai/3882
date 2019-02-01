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
  delay(3000);
  blackLightVal = readLightSensor();
  delay(5000)
  greyLightVal = readLightSensor();
}

void travelStraight() {
  
  //go till black line by enabling motor
  while(readColor() != 'b')
  goStraight();
  
  stopMotors();
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
void checkForObstacle(long distance) {
  //read ultrasonic sensor and if object within the distance then turn
  if (readUltrasonicSensor() < distance) {
    turn();
  }
}

void turn() {
  turnCCW(3);
  time = 7;
  for (int i = 0; i <= time; i++) {
      if(readColor() == 'b') break;
      turnCCW(1);
    }
}

void followLine(){
  int counter = 0;
  while (true){
    checkForObstacle(14);
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
        if(counter >= 3) {
          stopMotors();
          Serial.println("stopped");
          delay(40000);
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
  delay(.5);
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

//char readColor() {
//
////  int color = readLightSensor();
////  if (abs(color - whiteLightVal) > 100) {
////    return 'b';
////  } else if (abs(color - whiteLightVal) < 20) {
////    return 'w';
////  } else {
////    return 'g';
////  }
//
//  int color = readLightSensor();
//  if (color < 50)
//  return 'w';
//  if (color < 80)
//  return 'w';
//  return 'b';
//}
int readColor() {
  //1 if black
  //2 if grey
  //3 if white
  int color = readLightSensor();
  //Serial.println(color);
  int black = abs(color - blackLightVal);
  int white = abs(color - whiteLightVal);
  int grey = abs(color - greyLightVal);

  if (black < white && black < grey) {
    Serial.print('b');
    Serial.println(color);
    return 'b';
    
  } else if (grey < white && grey < black) {
    Serial.print('g');
    Serial.println(color);
    return 'g';
    
  } else {
    Serial.print('w');
    Serial.println(color);
    return 'w';
    
  }
}

int readLightSensor() {

  // return average of 5 messuragents
  int buff[15];
  for (int i = 0; i < 15; i++) {
    buff[i] = analogRead(A0);
    delay(2);
  }
//  int val = average(buff, 20);
//  Serial.println(val);
  return average(buff, 15);
}
