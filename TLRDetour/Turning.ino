#include <MeMCore.h>

MeRGBLed rgb(0,16);
MeUltrasonicSensor ultr(PORT_3);
MeLineFollower lineIn(PORT_2);

MeDCMotor MotorL(M1);
MeDCMotor MotorR(M2);

int rspeed = 100;
int lspeed = -90;
bool lineL = false;
bool lineR = false;
int Rtemp = rspeed;
int Ltemp = lspeed;
//
//
//

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int Rtemp = rspeed;
  int Ltemp = lspeed;
  
  lineL = lineIn.readSensor1();
  lineR = lineIn.readSensor2();
  if ((millis() % 400) == 1) {
    Serial.println((String)lineL + " -- " + (String)lineR);
  }
  
  if (lineL && lineR) {
  
  }
  
  MotorL.run(Ltemp);
  MotorR.run(Rtemp);
}

void turnRight() {

}
