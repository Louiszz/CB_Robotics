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
