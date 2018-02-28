#include <Servo.h>

Servo gripper;
Servo rotation;
Servo vertical;
Servo horizontal;
Servo Servos[] = {gripper, vertical, horizontal, rotation};
#define slen 4

int tops[4] = {175, 90, 85, 180};  // Grip;    Down; Out; Counterclockwise
int bottoms[4] = {100, 40, 10, 0}; // Release; Up;   In;  Clockwise
// (65, 70)
int dests[4] = {100, 0, 60, 80};
int currtime = 0;
int inbyte = 0;
String outstring = "";
int posserial = 60;
int selectservo = 1;

#define S0 2
#define S1 3
#define S2 4
#define S3 5
const int outpin = 6;
int blue = 0;
int red = 0;
int green = 0;
float bluecal = 0;
float redcal = 0;
float blackcal = 0;
int calcount = 0;
bool cal = true;
bool calold = true;
String color = "None";

void setup() {
  gripper.attach(8);
  vertical.attach(9);
  horizontal.attach(10);
  rotation.attach(11);
  
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(outpin, INPUT);
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
  
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    inbyte = Serial.read();
    if ((inbyte >= 48) && (inbyte <= 57)) {
      outstring += (String)(inbyte - 48);
    }
    else if ((inbyte >= 65) && (inbyte <= 68)) {
      selectservo = inbyte - 65;
      Serial.println("Selected servo: " + (String)(selectservo));
    }
    else if (inbyte == 33) {
      posserial = outstring.toInt();
      dests[selectservo] = posserial;
      Serial.println("Posserial: " + outstring);
      outstring = "";
    }
    else if (inbyte == 71) grip(true);
    else if (inbyte == 82) grip(false);
    else if (inbyte == 63) {
      updateColors();
      red *= redcal;
      blue *= bluecal;
      if ((red + blue + green) < blackcal) {
        if ((red < green) && (red < blue)) color = "Red";
        else if ((green < red) && (green < blue)) color = "Green";
        else if ((blue < green) && (blue < red)) color = "Blue";
      }
      else color = "Black";
      Serial.println("Color: "  + color);
      Serial.println("Colors: Red: " + (String)(red) + "Green: " + (String)(green) + "Bslue: " + (String)(blue));
    }
  }

  if ((millis() > 1000) && (millis() < 2000)) { // Calibrate colors to green
    cal = true;
    updateColors();
    if (!((green == 0) || (blue == 0) || (red == 0))) {
      redcal += (float)green / (float)red;
      bluecal += (float)green / (float)blue;
      blackcal += red + green + blue;
      calcount += 1;
    }
  }
  else cal = false;

  if ((calold) && (!cal)) {
    redcal /= calcount;
    bluecal /= calcount;
    blackcal /= calcount;
    blackcal += redcal + bluecal;
    String cals = "Redcal: " + (String)(redcal) + ", Bluecal: " + (String)(bluecal) + ", Blackcal: " + (String)(blackcal);
    Serial.println(cals);
  }
  
  if ((millis() % 40) == 0) {
    for (uint8_t i = 0; i < slen; i ++) {
      moveServo(Servos[i], dests[i]);
    }
  }
  calold = cal;
}

void moveServo(Servo thisservo, int moveto) {
  int currpos = thisservo.read();
  if (abs(currpos - moveto) > 2) {
    if (currpos > moveto) {
      thisservo.write(currpos - 1);
    }
    if (currpos < moveto) {
      thisservo.write(currpos + 1);
    }
  }
}

void grip(bool gripped) {
  if (gripped) dests[0] = tops[0];
  else dests[0] = bottoms[0];
}

void updateColors() {
  digitalWrite(S2,LOW); digitalWrite(S3,LOW);
  red = pulseIn(outpin, LOW);
  digitalWrite(S2,HIGH); digitalWrite(S3,HIGH);
  green = pulseIn(outpin, LOW);
  digitalWrite(S2,LOW); digitalWrite(S3,HIGH);
  blue = pulseIn(outpin, LOW);
}
