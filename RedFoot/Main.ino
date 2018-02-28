#include <Servo.h>

Servo gripper;
Servo rotation;
Servo vertical;
Servo horizontal;
Servo Servos[] = {gripper, vertical, horizontal, rotation};
#define slen 4

int tops[4] = {180, 90, 85, 180};  // Grip;    Down; Out; Counterclockwise
int bottoms[4] = {120, 40, 10, 0}; // Release; Up;   In;  Clockwise
// (65, 70)
int dests[4] = {180, 50, 10, 90};
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
int delaytime = 10;//7;    // Speed of servo motion
bool scanlock = false;
//int scandests[4] = dests;
long scantime = 0;

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
  if ((millis() % delaytime) == 0) {
    for (uint8_t i = 0; i < slen; i ++) {
      moveServo(Servos[i], dests[i]);
    }
  }
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
