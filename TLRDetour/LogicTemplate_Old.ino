#include <UltrasonicControl.h>
#include <IRControl.h>
#include <MotorControl.h>

MotorControl right(1);
MotorControl left(2);
UltrasonicControl us(3, 2);

#define lfast 180
#define rfast 180
#define lslow 90
#define rslow 95
int lsens = lfast; //100;  //72;
int rsens = rfast; //100; //90;

int state = 0;  // 0 is forward, 1 is right, 2 is back, 3 is left, 4 is (DONE) or (ERROR?)
int old_state = 0;
int posx = 1;
int posy = 1;
#define destx 4
#define desty 4
long start_time = 0;
long finish_time = 0;
int intersect = 0;
int intersect_reverse = 0;
int intersect_old = 0;
int countint = 0;
bool rs = false;
bool ls = false;
bool rsold= false;
bool lsold = false;
int t_state = 0;
float error;
int rmid = 250;
int lmid = 150;
int turn_time_r = 400;
int turn_time_l = 400;
int turn_time_f = 1000;
int turn_time_c = 0;

bool obstacle_detect = false;
bool obstacle_right = false;
bool obstacle_left = false;
bool tr = false;  // turning right?
bool tl = false;
bool trr = false; // turning right_reverse?
bool tlr = false;
bool reverse = false; // reverseline();
bool unforced = false; // unforced turn (no obstacle ahead, but turning)
bool obstaclesx[5][7];
bool obstaclesy[6][6];
uint8_t history[5][6];
bool start_obs = false;
float obstimecof = 1.8;
int lastobs = 0;
int distold = 0;
int dist = 0;
int distoldold = 0;
bool loopbreak = false;

// frontline() follows line and reports an intersection

void setup() {
  Serial.begin(9600);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
//  lcd.begin(16, 2);

  uint8_t i, j;
  
  for (i = 1; i <= destx; i++) {
    for (j = 1; j <= desty; j++) {
      obstaclesx[i][j] = false;
    }
  }
  for (i = 1; i <= destx; i++) {
    for (j = 1; j <= desty; j++) {
      obstaclesy[i][j] = false;
    }
  }
  for (i = 1; i <= desty; i++) {
    obstaclesx[0][i] = true;
    obstaclesx[destx][i] = true;
  }
  for (j = 1; j <= destx; j++) {
    obstaclesy[j][0] = true;
    obstaclesy[j][desty] = true;
  }
  obstaclesx[destx][desty] = false;
  obstaclesy[destx][desty] = false;
  
  for (i = 1; i <= destx; i++) {
    for (j = 1; j <= desty; j++) {
      history[i][j] = 4;
      //historyold[i][j] = 4;
      //historyoldold[i][j] = 4;
    }
  }
}

void loop() {
  countint++;
  distoldold = distold;
  distold = dist;
  dist = us.getDistance();
  
  Serial.println(dist);
  if ((posx == 1) && (posy == 1) && (dist < 12) && (dist > 0) && (distold < 12) && (distold > 0) &&
     (distoldold < 12) && (distoldold > 0) && (!start_obs) && (millis() < 200) && (millis() > 100)) {
    start_time = millis();
    while ((millis() - start_time) < 700) {
      right.reverse(rslow);
      left.reverse(lslow);
    }
    right.halt();
    left.halt();
    delay(100);
    
    posy = 0;
    start_obs = true;
    obstaclesy[1][2] = false;
  }

  if ((dist < 5) && (dist > 0) && (distold < 5) && (distold > 0) && (distoldold < 5) && (distoldold > 0)) {
    obstacle_detect = true; lastobs = countint;
  }
  
  if ((millis() - finish_time > 500) && (!t_state)) {lsens = lfast; rsens = rfast;}
//  lcd.setCursor(0,0); lcd.print(posx); lcd.print(" "); lcd.print(posy); lcd.print(" "); lcd.print(obstaclesy[2][2]); lcd.print(" "); lcd.print(obstaclesy[1][2]); lcd.print(" ");
//  if (t_state = 0) {
//    if ((state == 0) && (obstaclesy[posx][posy])) obstacle_detect = true;
//    if ((state == 1) && (obstaclesx[posx][posy])) obstacle_detect = true;
//    if ((state == 2) && (obstaclesy[posx][posy - 1])) obstacle_detect = true;
//    if ((state == 3) && (obstaclesx[posx - 1][posy])) obstacle_detect = true;
//  }

  intersect_old = intersect;
  old_state = state;

  if (t_state == 0) frontline();
  if ((intersect == 1) && (intersect_old == 0)) { // Intersected!
    //Serial.print(" Intersected! ");
    
    if ((countint - lastobs) < 5) {
      if (!t_state) {
        if ((!obstacle_right) && (!obstacle_left)){
          if (state == 0) obstaclesy[posx][posy + 1] = true;
          if (state == 1) obstaclesx[posx + 1][posy] = true;
          if (state == 2) obstaclesy[posx][posy - 2] = true;
          if (state == 3) obstaclesx[posx - 2][posy] = true;
        }
      }
    }
    if (state == 0) posy = posy + 1;
    if (state == 1) posx = posx + 1;
    if (state == 2) posy = posy - 1;
    if (state == 3) posx = posx - 1;
    
    logic();
    if (!(tr || tl || trr || tlr || reverse)) {
      history[posx][posy] = state;
    }
    else {
      lsens = lslow;
      rsens = rslow;
    }
    
  }
  
  if (tr)  turn_right();
  if (tl)  turn_left();
  if (trr) right_reverse();
  if (tlr) left_reverse();
  if (reverse) flip();

  if ((posx == destx) && (posy == desty)) state = 4;
  if ((state == 4) && (old_state != 4)) finish_time = millis();
  if ((millis() > (finish_time + 100)) && (state == 4)) {right.halt(); left.halt();}
}

void frontline() {
  int leftv = analogRead(4);
  int rightv = analogRead(5);
  // Serial.print(" leftv, rightv "); Serial.print(leftv); Serial.print(" "); Serial.println(rightv);
  if (leftv > lmid) ls = true;
  else ls = false;
  if (rightv > rmid) rs = true;
  else rs = false;

//  if (ls) digitalWrite(A1, HIGH); else digitalWrite(A1, LOW);
//  if (rs) digitalWrite(A0, HIGH); else digitalWrite(A0, LOW);
  
  if (ls && !rs) error = 50;
  else {if (rs && !ls) error = -50;
        else error = 0;}

  right.forward(rsens + error);
  left.forward(lsens - error);
  
  if (ls && rs) {
    if (intersect == 0) {intersect = 1; start_time = millis();}
  }
                else if((millis() - start_time) > 800) intersect = 0;
}

void turn_right() { // Obstacle detection included
  rsold = rs;
  lsold = ls;
  int rightv = analogRead(5);
  if (rightv > rmid) rs = true;
  else rs = false;
  int leftv = analogRead(4);
  if (leftv > rmid) ls = true;
  else ls = false;
  tr = true;
  left.forward(lsens);
  right.reverse(rsens + 15);
  if (t_state == 0) {start_time = millis(); t_state = 1;}
  turn_time_c = millis() - start_time;
  
  if (turn_time_c < turn_time_r * 4) {obstacle_detect = false;}
  if ((turn_time_c > turn_time_r) && (!ls) && (!rs) && (lsold) && (!rsold) && (t_state == 1)) {
//    lcd.setCursor(0,1); lcd.print(lastobs); lcd.print(" "); lcd.print(countint); lcd.print(" "); lcd.print(t_state);
    if ((countint - lastobs) < 5) {
      tr = false;
      obstacle_right = true;
      if (state == 0) obstaclesx[posx][posy] = true;
      if (state == 1) obstaclesy[posx][posy - 1] = true;
      if (state == 2) obstaclesx[posx - 1][posy] = true;
      if (state == 3) obstaclesy[posx][posy] = true;
      t_state = 0;
      right_reverse();
    }
    else t_state = 2;
  }
  if (t_state == 2) {
    
    right.forward(rsens * 1.2);
    left.forward(lsens);
    if (rs) {
      tr = false;
      obstacle_left = false;
      obstacle_detect = false;
      state = state + 1;
      if (state == 4) state = 0;
      t_state = 0;
      history[posx][posy] = state;
      finish_time = millis();
    }
  }
}

void turn_left() { // Obstacle detection included
  rsold = rs;
  lsold = ls;
  int leftv = analogRead(4);
  if (leftv > rmid) ls = true;
  else ls = false;
  int rightv = analogRead(5);
  if (rightv > rmid) rs = true;
  else rs = false;
  tl = true;
  right.forward(rsens);
  left.reverse(lsens + 15);
  if (t_state == 0) {start_time = millis(); t_state = 1;}
  turn_time_c = millis() - start_time;

  if (turn_time_c < turn_time_l * 4) {obstacle_detect = false;}
  if ((turn_time_c > turn_time_l) && (!rs) && (!ls) && (rsold) && (!lsold) && (t_state == 1)) {
    if ((countint - lastobs) < 5) {
      tl = false;
      obstacle_left = true;
      if (state == 0) obstaclesx[posx - 1][posy] = true;
      if (state == 1) obstaclesy[posx][posy] = true;
      if (state == 2) obstaclesx[posx][posy] = true;
      if (state == 3) obstaclesy[posx][posy - 1] = true;
      t_state = 0;
      left_reverse();
    }
    else t_state = 2;
  }
  if (t_state == 2) {
    right.forward(rsens);
    left.forward(lsens * 1.2);
    if (ls) {
      tl = false;
      obstacle_right = false;
      obstacle_detect = false;
      state = state - 1;
      if (state == -1) state = 3;
      t_state = 0;
      history[posx][posy] = state;
      finish_time = millis();
    }
  }
}


void right_reverse() {
  trr = true;
  left.reverse(lsens);
  right.forward(rsens + 15);
  if (t_state == 0) {start_time = millis(); t_state = 1;
    if (state == 0) posy = posy - 1;
    if (state == 1) posx = posx - 1;
    if (state == 2) posy = posy + 1;
    if (state == 3) posx = posx + 1;
  }
  if ((millis() - start_time) > (turn_time_c)) {
    trr = false;
    t_state = 0;
    obstacle_detect = false;
    obstacle_right = false;
    while ((millis() - start_time) < turn_time_c + 800) {
      right.reverse(rsens);
      left.reverse(lsens);
    }
    right.halt();
    left.halt();
    delay(100);

    finish_time = millis();
  }
}

void left_reverse() {
  tlr = true;
  right.reverse(rsens);
  left.forward(lsens + 15);
  if (t_state == 0) {start_time = millis(); t_state = 1;
  if (state == 0) posy = posy - 1;
  if (state == 1) posx = posx - 1;
  if (state == 2) posy = posy + 1;
  if (state == 3) posx = posx + 1;
  }  
  if ((millis() - start_time) > (turn_time_c + 100)) {
    tlr = false;
    t_state = 0;
    obstacle_detect = false;
    obstacle_left = false;
    while ((millis() - start_time) < turn_time_c + 800) {
      right.reverse(rsens);
      left.reverse(lsens);
    }
    right.halt();
    left.halt();
    delay(100);

    finish_time = millis();
  }
}

void flip() {
  int leftv = analogRead(4);
  if (leftv > lmid) ls = true;
  else ls = false;
  int rightv = analogRead(5);
  if (rightv > rmid) rs = true;
  else rs = false;
  uint8_t i, j;

  if (t_state == 0) start_time = millis();

  t_state = 1;
  
  if ((ls) && ((millis() - start_time) > turn_time_f)) {
    reverse = false;
    t_state = 0;
    obstacle_right = false;
    obstacle_left = false;
    obstacle_detect = false;
    if (state == 0) state = 2;
    else if (state == 2) state = 0;
    if (state == 1) state = 3;
    else if (state == 3) state = 1;
    if (!loopbreak) {
      if (state == 0) obstaclesy[posx][posy] = true;
      if (state == 1) obstaclesx[posx][posy] = true;
      if (state == 2) obstaclesy[posx][posy - 1] = true;
      if (state == 3) obstaclesx[posx - 1][posy] = true;
    }
    loopbreak = false;
    for (i = 1; i <= destx; i++) {
      for (j = 1; j <= desty; j++) {
        //historyoldold[i][j] = historyold[i][j];
        //historyold[i][j] = history[i][j];
        history[i][j] = 4;
      }
    }
    finish_time = millis();
//    lsens = lfast;
//    rsens = rfast;
  }
  else {
    right.forward(rsens);
    left.reverse(lsens);
  }
}

/*
  logic
*/

void logic() {
  if ((state == 0) && (obstaclesy[posx][posy])) {
    if      (!obstaclesx[posx][posy])      tr = true;
    else if (!obstaclesx[posx - 1][posy])  tl = true;
    else                                   reverse = true;
  }
  
  else if ((state == 1) && (obstaclesx[posx][posy])) {
    if      (!obstaclesy[posx][posy])      tl = true;
    else if (!obstaclesy[posx][posy - 1])  tr = true;
    else                                   reverse = true;
  }

  else if ((state == 2) && (obstaclesy[posx][posy - 1])) {
    if      (!obstaclesx[posx][posy])      tl = true;
    else if (!obstaclesx[posx - 1][posy])  reverse = true;
    else                                   reverse = true;
  }

  else if ((state == 2) && (!obstaclesy[posx][posy - 1])) {
    if (posx == 4)                          tr = true; 
    else                                    tl = true; 
  }

  else if ((state == 3) && (obstaclesx[posx - 1][posy])) {
    if      (!obstaclesy[posx][posy])      tr = true;
    else                                   reverse = true;
  }

    else if ((state == 3) && (!obstaclesx[posx - 1][posy])) {
    if (posy == 4)                        tl = true; 
    else                                  tr = true; 
  }
  if (history[posx][posy] == state) {
    
  }
  history[posx][posy] = state;
  
}

bool check_obstacle (int next_state) { //At current position of posx, posy and want to turn into next_state, whether obstacle.
    bool returnvalue;
    if (next_state == 0) returnvalue = obstaclesy[posx][posy];
    else if (next_state == 1) returnvalue = obstaclesx[posx][posy];
    else if (next_state == 2) returnvalue = obstaclesy[posx][posy - 1];
    else if (next_state == 3) returnvalue = obstaclesx[posx - 1][posy];
    return returnvalue;
}
