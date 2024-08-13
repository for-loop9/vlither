#ifndef PREY_H
#define PREY_H

#include "game.h"

typedef struct prey {
	int id;
	float xx;
	float yy;
	float rad;
	float sz;
	int cv;
	int dir;
	float wang;
	float ang;
	float sp;
	float fr;
	float gfr;
	float gr;
	float fxs[RFC];
	float fys[RFC];
	int fpos;
	int ftg;
	float fx;
	float fy;
	int eaten;
	int eaten_by;
	float eaten_fr;
	float f;
	float f2;
	int blink_dir;
	float blink;
} prey;

#endif