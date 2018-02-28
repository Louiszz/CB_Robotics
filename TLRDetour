#include <MotorControl.h>

MotorControl right(1);
MotorControl left(2);

int rspeed = 120;
int lspeed = 120;

void setup() {
  Serial.begin(9600);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
}

void loop() {
  right.forward(rspeed);
  left.forward(lspeed);
}
