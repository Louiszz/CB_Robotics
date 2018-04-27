const int dim = 9;
const int MAXDIST = 1000;

String rawData = "WWWWWWWWW"
                 "W...O...W"
                 "W.W.W.WOW"
                 "W.O.....W"
                 "W.W.W.W.W"
                 "W...O...W"
                 "WOW.W.W.W"
                 "WS..O...W"
                 "WWWWWWWWW";

String data[dim];
int nodes[dim][dim];
int nodeslen = dim * dim;
int prev[dim][dim][2];
int visited[dim * dim][3];
int flat[dim * dim];
int starti = 0;
int startj = 0;
int ci = 0;
int cj = 0;

bool quit = false;
bool quitold = false;

void setup() {
  Serial.begin(9600);
  for (int n = 0; n < (dim * dim); n++) {
    int ni = n / dim;
    int nj = n % dim;
    if (rawData[n] == '.') {
      nodes[ni][nj] = -1;
    }
    else if (rawData[n] == 'S') {
      nodes[ni][nj] = 0;
      starti = ni; startj = nj;
    }
    else {
      nodes[ni][nj] = -2;
    }
  }
  ci = starti;
  cj = startj;
  
}

void loop() {
  //Serial.println("Start:" + (String)starti + ", " + (String)startj);
  int vc = 0;
  while ((nodeslen > 0) && (!quit)) {
    int dist = nodes[ci][cj];
    int tentd = dist + 1;
    int cd = 0;
    //Serial.println("i, j:" + (String)ci + ", " + (String)cj);
    if (nodes[ci + 1][cj] != -2) {
      cd = nodes[ci + 1][cj];
      if ((tentd < cd) || (cd == -1)) {
        nodes[ci + 1][cj] = tentd;
        prev[ci + 1][cj][0] = ci;
        prev[ci + 1][cj][1] = cj;
      }
    }
    if (nodes[ci - 1][cj] != -2) {
      cd = nodes[ci - 1][cj];
      if ((tentd < cd) || (cd == -1)) {
        nodes[ci - 1][cj] = tentd;
        prev[ci - 1][cj][0] = ci;
        prev[ci - 1][cj][1] = cj;
      }
    }
    if (nodes[ci][cj + 1] != -2) {
      cd = nodes[ci][cj + 1];
      if ((tentd < cd) || (cd == -1)) {
        nodes[ci][cj + 1] = tentd;
        prev[ci][cj + 1][0] = ci;
        prev[ci][cj + 1][1] = cj;
      }
    }
    if (nodes[ci][cj - 1] != -2) {
      cd = nodes[ci][cj - 1];
      if ((tentd < cd) || (cd == -1)) {
        nodes[ci][cj - 1] = tentd;
        prev[ci][cj - 1][0] = ci;
        prev[ci][cj - 1][1] = cj;
      }
    }

    nodes[ci][cj] = -2;
    visited[vc][0] = ci;
    visited[vc][1] = cj;
    visited[vc][2] = dist;
    flatten2d(nodes, flat);
    //for (int i = 0; i < (dim * dim); i ++ ) {
    //  Serial.println("F[" + (String)i + "]: " + (String)flat[i]);
    //}
    int minindex = 0;
    int mindist = MAXDIST;
    for (int x = 0; x < (dim * dim); x++) {
      if (flat[x] != -2) {
        if ((flat[x] < mindist) && (flat[x] != -1)) {
          minindex = x;
          mindist = flat[x];
        }
      }
    }
    if (mindist == MAXDIST) quit = true;

    ci = minindex / dim;
    cj = minindex % dim;

    vc ++;
  }
  if (quit && !quitold) {
    Serial.println("Path to [1, 7]:");
    int path[40][2] = {};
    int t[2] = {1, 7};
    int plen = 0;
    findPath(t, path, plen);
    for (int n = plen; n >= 0; n--) {
      Serial.print("[" + (String)(path[n][0]) + "," + (String)(path[n][1]) + "], ");
    }
    Serial.println(".");
  }
  quitold = quit;
}

void flatten2d(int list[dim][dim], int out[dim * dim]) {
  int c = 0;
  for (int a = 0; a < dim; a++) {
    for (int b = 0; b < dim; b++) {
      out[c] = list[a][b];
      c ++;
    }
  }
}
void findPath(int target[2], int out[40][2], int &len) { // (dim * dim / 2 = 40) is arbitrary
  int u[2] = {target[0], target[1]};
  int temp[2] = {u[0], u[1]};
  int c = 0;
  while (!((u[0] == starti) && (u[1] == startj))) {
    //Serial.println("u: [" + (String)u[0] + ", " + (String)(u[1]) + "]");
    out[c][0] = u[0];
    out[c][1] = u[1];
    temp[0] = prev[u[0]][u[1]][0];
    temp[1] = prev[u[0]][u[1]][1];
    u[0] = temp[0];
    u[1] = temp[1];
    c ++;
  }
  len = c;
}
