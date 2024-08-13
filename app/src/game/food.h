#ifndef FOOD_H
#define FOOD_H

#define FOOD_SIZES 17

typedef struct food {
	int id;
	int rsp;
	int sx;
	int sy;
	int eaten;
	float xx;
	float yy;
	float rx;
	float ry;
	float sz;
	float rad;
	int cv;
	float lrrad;
	float f;
	float f2;
	int eaten_by;
	float eaten_fr;
	float fr;
	float wsp;
	float gfr;
	float gr;
} food;

#endif