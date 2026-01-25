#ifndef PREY_H
#define PREY_H

#include <stdint.h>
#include <stdbool.h>

#include "../constants.h"

typedef struct prey {
  int id;
  int cv;
  int cv2;
  int dir;
  int fpos;
  int ftg;
  int ebid;

  float xx;
  float yy;
  float fx;
  float fy;
  float rad;
  float sz;
  float wang;
  float ang;
  float sp;
  float fr;
  float gfr;
  float gr;
  float eaten_fr;

  float fxs[GD_EEZ];
  float fys[GD_EEZ];

  bool eaten;
} prey;

#endif