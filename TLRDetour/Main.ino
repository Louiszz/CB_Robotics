#include <MeMCore.h>

MeLineFollower lineIn(PORT_2);

MeDCMotor MotorL(M1);
MeDCMotor MotorR(M2);



void setup() {
  Serial.begin(9600);
}

void loop() {

  lineL = lineIn.readSensor1();
  lineR = lineIn.readSensor2();
  
  if ((millis() % 500) == 1) {
    Serial.println((String)lineL + " -- " + (String)lineR);
  }

}
