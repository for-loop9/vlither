#ifndef BODY_PART_H
#define BODY_PART_H

#include "../constants.h"

#include <stdbool.h>

typedef struct body_part {
  float fxs[GD_EEZ];
  float fys[GD_EEZ];
  float fltns[GD_EEZ];
  float fsmus[GD_EEZ];
  
  int fpos;
  int ftg;

  float xx;
  float yy;
  float fx;
  float fy;
  float ebx;
  float eby;
  float fltn;
  float fsmu;
  float ltn;
  float iang;
  float da;
  float smu;

  bool iiv;
  bool dying;
} body_part;

#endif