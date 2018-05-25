#include <Servo.h>

#define raspi true
#define rtime 2000

Servo gripper;
Servo rotation;
Servo vertical;
Servo horizontal;
Servo Servos[] = {gripper, vertical, horizontal, rotation};
#define slen 4

// Me
int tops[4] = {180, 70, 85, 180};  // Grip;    Down; Out; Counterclockwise
int bottoms[4] = {120, 30, 10, 0}; // Release; Up;   In;  Clockwise

// Mihail
//int tops[4] = {115, 180, 180, 180};
//int bottoms[4] = {50, 90, 90, 0};

int dests[4] = {tops[0], bottoms[1] + 10, bottoms[2], 90};
int currtime = 0;
int inbyte = 0;
String outstring = "";
int posserial = 60;
int selectservo = 1;
int counter = 10;
int count = 1;
bool extending = false;
int extension = 0;
bool extendingold = false;
long looptime = 0;
long reftime = 0;
long timex = 0;
bool countequalsreadingnum = false;
bool countequalsreadingnumold = false;
int redtime = 0;

int greenpos = 0;
int bluepos = 0;
int redpos = 0;
int blackpos = 0;
bool blacklocal = false;

bool blockplaced = false;
bool firsttime = true;
int blankloop = 0;
bool relocateblack = false;
bool relocateblackold = false;
bool arrt = false;
bool arrivedold = false;
bool arrtex = false;
bool arrtrel = false;
int extst = 0;
int relst = 0;
bool ittemp = false;
int delaytime = 16;
bool scanlock = false;
//int scandests[4] = dests;
long scantime = 0;

// Mihail: 42356; Me: 23456
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
bool cal = false;
bool calold = false;
String color = "Black";
bool colorerror = false;

int readings [24];
int readingnum = 24;
int currentreading[10];
bool done = false;

const int trigPin = 12;
const int echoPin = 13;
long duration, cm;

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
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  
  Serial.begin(9600);
  for (int K = 0; K < readingnum; K++){
    readings [K] = 0;
  }
  for (int r = 0; r < 10; r++) {
    currentreading[r] = 0;
  }
}



void loop() {
  timex = millis() - looptime;
  arrt = arrived();
  
  if (count == (readingnum - 1)) countequalsreadingnum = true;
  else countequalsreadingnum = false;

  if (!relocateblack) {
    if (!firsttime && !ittemp) {
      looptime -= 2000;
      ittemp = true;
      //Serial.println("looptime -= 2000;");
    }
    if (arrt && (!extending)) {
      dests[3] = 110;
    }
    if (timex == (3000 - rtime)) {
      counter = 1;
    }
    if ((timex > (3200 - rtime)) && (timex % 200 == 0) && (count != 100)) {
      counter += 1;
    }
    
    if (counter == count && count < readingnum) {
      dests [3] = 120 + 3 * count;
      for (int n = 0; n < 10; n++) {
        updateDistance();
        //Serial.print("Reading " + (String)(count) + " : "); Serial.println(cm);
        currentreading[n] = (int)(cm);
      }
      uint8_t zeronum = 0;
      int currentaverage = 0;
      for (int n = 0; n < 10; n++) {
        if (currentreading[n] == 0) zeronum ++;
        currentaverage += currentreading[n];
      }
      currentaverage /= 10;
      if (zeronum > 4) readings[count] = 0;
      else readings [count] = (int) (currentaverage);
      //Serial.print("readings[count] = ");
      //Serial.println(readings[count]);
      int target = readings[count];
      if (inrange(target)) {
        dests [3] = 120 + count * 3;
        if (count <= 2) dests[3] += 6;
        if (count >= 13) {
          scanlock = true;
          scantime = millis();
        }
        grip(false);
        extending = true;
        reftime = millis();
        done = false;
        extension = 5;
        count = 100;
        
        //Serial.println("Extending");
        extst = 0;
        blankloop = 0;
      }
      count ++;
      if (count == readingnum) {
        if ((!extending)) {
          resetLoop(); // If no object detected, try again.
          blankloop ++;
          if (blankloop >= 5) {
            relocateblack = true;
          }
        }
      }
    }
  }
  else if (!done) {             // relocate blacks to loading dock

  }
  
  if (extending) {
    arrtex = arrived();
    if (!extendingold) {
      //Serial.println("Extending: " + (String)(extension));
    }
    if ((millis() - scantime == 900) && (scanlock)) {
      dests[3] = 180;
      scanlock = false;
    }
    if (millis() - reftime == 0) {
      dests[1] = bottoms[1];
    }
    if (arrtex) {
      switch(extst) {
      case 0: {
        redtime = 0;
        dests [2] = tops[2];
        dests [1] = tops[1];
        extst = 1;
        } break;
      case 1: {
        grip(true);
        extst = 2;
      } break;
      case 2: {
        dests[2] = percent(tops[2], bottoms[2], 50);
        dests[1] = tops[1]; //percent(tops[1], bottoms[1], 90);
        if (raspi) {
          String result = waitForResult();
          color = result;
        }
        else {
          updateColors();
          printColor();
        }
        extst = 3;
      } break;
      case 3: if (!blockplaced) {
        if (color == "Green") dests[3] = 85;
        else if (color == "Blue") dests[3] = 45;
        else if (color == "Red") {
          dests[3] = 5;
          redtime = 1200;
        }
        else if (color == "Black") dests[3] = 180;
        
        blockplaced = true;
        extst = 4;
      } break;
      case 4: {
        if (color != "Black") {
          dests[2] = tops[2];
          dests[1] = tops[1] - 10;
        }
        else {
            dests[1] = tops[1] - 10;
            dests[2] = tops[1] - 10;
        }
        extst = 5;
      }
      break;
      case 5: {
        grip(false);
        extst = 6;
      }
      break;
      case 6: {
        dests[2] = bottoms[2] + 40;
        dests[1] = bottoms[1] - 10;
        extst = 7;
      }
      break;
      case 7: {
        resetLoop();
        extst = 8;
      }
      break;
      }
    }
  }


  if (!raspi) {
  
    if ((millis() > 200) && (millis() < 1000)) { // Calibrate colors to green
      cal = true;
      updateColors();
      if (!((red == 0) || (green == 0) || (blue == 0))) {
      redcal += (float)green / (float)red;
      bluecal += (float)green / (float)blue;
      blackcal += red + green + blue;
      calcount += 1;
      }
    } else cal = false;
  
    if ((calold) && (!cal)) {
      redcal /= calcount;
      bluecal /= calcount;
      blackcal /= calcount;
      blackcal += redcal + bluecal;
      blackcal *= 0.75;
      String cals = "Redcal: " + (String)(int)(redcal * 100);
      cals += ", Bluecal: " + (String)(int)(bluecal * 100);
      cals += ", Blackcal: " + (String)(int)(blackcal);
      Serial.println("-- Calibration finished --");
      Serial.println(cals);
      if ((int)(blackcal) == 0) {
        Serial.println("Color sensor error!");
        colorerror = true;
      }
      Serial.println("Calcount: " + (String)(calcount));
    }
  }
  
  if (!raspi) {
    if (!colorerror) digitalWrite(7, HIGH);
    else {
      if ((millis() % 400) == 0) digitalWrite(7, LOW);
      else if ((millis() % 400) == 200) digitalWrite(7, HIGH);
    }
  }
  else digitalWrite(7, LOW);
  
  if ((millis() % delaytime) == 0) {
    for (uint8_t i = 0; i < slen; i ++) {
      moveServo(Servos[i], dests[i]);
    }
  }
  
  calold = cal;
  countequalsreadingnumold = countequalsreadingnum;
  arrivedold = arrt;
  relocateblackold = relocateblack;
  extendingold = extending;
}

// Function definitions

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

void printColor() {
  red *= redcal;
  blue *= bluecal;
  if ((red + blue + green) < blackcal) {
    if ((red < green) && (red < blue)) color = "Red";
    else if ((green < red) && (green < blue)) color = "Green";
    else if ((blue < green) && (blue < red)) color = "Blue";
  }
  else color = "Black";
  Serial.println("Color: "  + color);
}

void updateDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 10000);

  cm = microsecondsToCentimeters(duration);
}
long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}
bool inrange(int distance) {
  if ((distance == 0) || (distance > 20)) return false;
  else return true;
}
int percent(int top, int bottom, int p) {
  return ((top - bottom) * (float)p / 100.0) + bottom;
}
void resetLoop() {
  dests[3] = 120;
  dests[1] = 50;
  dests[2] = 10;
  extending = false;
//  extendclose = false;
//  extendmiddle = false;
//  extendfar = false;
  blockplaced = false;
  counter = 0;
  count = 1;
  looptime = millis();
  looptime += redtime;
  redtime = 0;
  Serial.println("Looptime: " + (String)(looptime));
  Serial.println("Done: " + (String)(done));
  ittemp = false;
  firsttime = false;
}

bool arrived() {
  bool out = true;
  for (int k = 0; k < 4; k++) {
    if (!(abs(dests[k] - Servos[k].read()) < 3)) {
      out = false;
//      Serial.print("dests[" + (String)(k) + "]" + (String)(dests[k]));
//      Serial.println("Servos[" + (String)(k) + "].read()" + (String)(Servos[k].read()));
      break;
    }
  }
  return out;
}

String waitForResult() {
  Serial.print("?");
  bool doneR = false;
  String r = "";
  while (!doneR) {
    if (Serial.available() > 0) { // User interface and variable queries
      inbyte = Serial.read();
      
      // A to Z, a to z
      if (((inbyte >= 65) && (inbyte <= 90)) || ((inbyte >= 97) && (inbyte <= 122))) {
        outstring += (char)inbyte;
      }
      else if (inbyte == 33) {
        //int tests = outstring.toInt();
        r = outstring;
        Serial.println("Result: " + outstring);
        outstring = "";
        doneR = true;
      }
    }
  }
  return r;
}
