#ifndef FOOD_H
#define FOOD_H

#include <stdint.h>
#include <stdbool.h>

typedef struct food {
  int id;
  int cv;
  int cv2;
  int ebid;
  int sx;
  int sy;

  float xx;
  float yy;
  float rx;
  float ry;
  float rsp;
  float rad;
  float sz;
  float lrrad;
  float fr;
  float gfr;
  float gr;
  float wsp;
  float eaten_fr;

  bool eaten;
} food;

#endif