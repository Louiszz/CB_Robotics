#include <Servo.h>

#define raspi false

Servo gripper;
Servo rotation;
Servo vertical;
Servo horizontal;
Servo Servos[] = {gripper, vertical, horizontal, rotation};
#define slen 4

// Me
//int tops[4] = {180, 90, 85, 180};  // Grip;    Down; Out; Counterclockwise
//int bottoms[4] = {120, 40, 10, 0}; // Release; Up;   In;  Clockwise

// Mihail
int tops[4] = {115, 180, 180, 180};
int bottoms[4] = {50, 90, 90, 0};

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
#define S0 4
#define S1 2
#define S2 3
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
      Serial.println("looptime -= 2000;");
    }
    if (arrt && (!extending)) {
      dests[3] = 120;
    }
    if (timex == 3000) {
      counter = 1;
    }
    if ((timex > 3200) && (timex % 200 == 0) && (count != 100)) {
      counter += 1;
    }
    
    if (counter == count && count < readingnum) {
      dests [3] = 120 + 3 * count;
      for (int n = 0; n < 10; n++) {
        updateDistance();
        Serial.print("Reading " + (String)(count) + " : "); Serial.println(cm);
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
      Serial.print("readings[count] = ");
      Serial.println(readings[count]);
      int target = readings[count];
      if (inrange(target)) {
        dests [3] = 128 + count * 3;
        if (count <= 2) dests[3] += 6;
        if (count >= 13) {
          scanlock = true;
          scantime = millis();
        }
        grip(false);
        extending = true;
        reftime = millis();
        done = false;
        /* if(target<=2) extension = 1;
        else if(target<=3) extension = 2;
        else if (target<=4) extension = 3;
        else if(target<=5) extension = 4;
        else {
          extension = 5;
        } */
        extension = 5;
        count = 100;
        
        /* extendclose = false;
        extendfar = false;
        extendmiddle = false;
        if (target >= 9)     extendfar = true;
        else if (target <= 6) extendclose = true;
        else                  extendmiddle = true; */
        
        Serial.println("Extending 12345");
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
//    if (!relocateblackold) dests[3] = 115;
////    delaytime = 30;
//    arrtrel = arrived();
////    Serial.println("Blackpos: " + (String)(blackpos));
//    if (arrtrel) {
////      Serial.print("relst: " + (String)(relst));
////      Serial.println(" Time (millis): " + (String)(timex));
//      if (relst == 0) {
//        grip(false);
//        if (blackpos == 0) {
//          dests[1] = 75;
//          dests[2] = 80;
//        }
//        else if (blackpos == 1) {
//          dests[1] = 65;
//          dests[2] = 50;
//        }
//        else if (blackpos == 2) {
//          dests[1] = 55;
//          dests[2] = 30;
//        }
//        else {
//          blackpos += -1;
//        }
//        relst = 1;
//      }
//      else if (relst == 1) {
//        grip(true);
//        relst = 2;
//      }
//      else if (relst == 2) {
//        dests[2] = 50;
//        dests[1] = 0;
//        relst = 3;
//      }
//      else if (relst == 3) {
//        Serial.print("Blackpos: " + (String)(blackpos));
//        Serial.println(" 180 - 12 * blackpos: " + (String)(180 - 12 * blackpos));
//        dests[3] = 180 - 12 * blackpos;
//        relst = 4;
//      }
//      else if (relst == 4) {
//        dests[2] = 90;
//        dests[1] = 70;
//        relst = 5;
//      }
//      else if (relst == 5) {
//        grip(false);
//        relst = 6;
//      }
//      else if (relst == 6) {
//        dests[2] = 50;
//        dests[1] = 0;
//        relst = 7;
//      }
//      else if (relst == 7) {
//        blackpos -= 1;
//        if (blackpos <= -1) done = true;
//        dests[3] = 115;
//        relst = 0;
//      }
//    }
  }
  
  if (extending) {
    arrtex = arrived();
    if (!extendingold) {
      Serial.println("Extending: " + (String)(extension));
    }
    if ((millis() - scantime == 1000) && (scanlock)) {
      dests[3] = 180;
      scanlock = false;
    }
    if (millis() - reftime == 0) {
      dests[1] = bottoms[4] - 5;
    }
    if (arrtex) {
      switch(extst) {

      case 0: {
        Serial.print("Angle: " + (String)(rotation.read()));
        redtime = 0;
        Serial.println("Extend " + extension);
        if (extension == 1) {
          dests[2] = 150; // 60% to top from bottom
        }else if (extension == 2) {
          dests [2] = 160; // 75%
          dests [1] = 105; // 20%
        }else if (extension == 3) {
          //dests [2] = 175; // 90%
          //dests [1] = 59;
        }else if (extension == 4) {
          //dests [2] = 91;
          //dests [1] = 70;
        }
        else if (extension == 5) {
          dests [2] = tops[2];
          dests [1] = tops[1];
        }
        
        extst = 1;
        }
        break;
      case 1: {
//        if (extendmiddle) dests[1] = 55;
//        else if (extendfar) dests[1] = 85;
        extst = 2;
      }
      break;
      case 2: {
        grip(true);
        extst = 3;
      }
      break;
      case 3: {
        if (raspi) {
          String result = waitForResult();
          color = result;
        }
        else {
          updateColors();
          printColor();
        }
        dests[2] = bottoms[2] + 40;
        dests[1] = bottoms[1] - 10;
        extst = 4;
      }
      break;
      case 4: if (!blockplaced) {
        if (color == "Green") {
          Serial.println("Greenpos: " + (String)(greenpos));
          dests[3] = 105 + 12 * greenpos;
//          greenpos ++;
        }
        else if (color == "Blue") {
          Serial.println("Bluepos: " + (String)(bluepos));
          dests[3] = 45 + 12 * bluepos;
//          bluepos ++;
        }
        else if (color == "Red") {
          Serial.println("Redpos: " + (String)(redpos));
          dests[3] = 5 + 12 * redpos;
//          redpos ++;
          redtime = 1200;
        }
        else if (color == "Black") {
          Serial.println("Blackpos: " + (String)(blackpos));
          dests[3] = 180;
//          if (blackpos == 0) {
//            blackpos ++;
//          }
//          else if (blackpos == 1) {
//            dests[1] = 65;
//            dests[2] = 50;
//            blackpos ++;
//          }
//          else if (blackpos == 2) {
//            dests[1] = 55;
//            dests[2] = 30;
//            blackpos ++;
//          }
        }
        blockplaced = true;
        extst = 5;
      }
      break;
      case 5: {
        Serial.println("Got to 5");
        if (color != "Black") {
          dests[2] = tops[2];
          dests[1] = tops[1] - 10;
        }
        else {
            dests[1] = tops[1] - 10;
            dests[2] = tops[2] - 10;
        }
        extst = 6;
      }
      break;
      case 6: {
        Serial.println("Got to 6");
        grip(false);
        extst = 7;
      }
      break;
      case 7: {
        dests[2] = bottoms[2] + 40;
        dests[1] = bottoms[1] - 10;
        extst = 8;
      }
      break;
      case 8: {
        resetLoop();
        extst = 9;
      }
      break;
      }
    }
  }


  /*if (Serial.available() > 0) { // User interface and variable queries
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
    else if (inbyte == 71) Serial.println("Arrived: " + (String)(arrived()));//grip(true);
    else if (inbyte == 82) grip(false);
    else if (inbyte == 63) {
      updateColors();
      printColor();
    }
    else if (inbyte == 70) {
      updateDistance();
      Serial.print(cm);
      Serial.print("cm");
      Serial.println();
    }
  }*/

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

  if (!colorerror) digitalWrite(7, HIGH);
  else {
    if ((millis() % 400) == 0) digitalWrite(7, LOW);
    else if ((millis() % 400) == 200) digitalWrite(7, HIGH);
  }
  
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
  Serial.println("?");
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
