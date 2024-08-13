#ifndef SNAKE_H
#define SNAKE_H

#include <ignite.h>
#include <stdbool.h>

#define LFC 128
#define RFC 43
#define HFC 92
#define AFC 26
#define VFC 62

typedef struct game game;

typedef struct body_part {
	float xx;
	float yy;
	float fx;
	float fy;
	float ebx;
	float eby;
	int dying;
	float da;
	int eiu;
	int efs[128];
	float exs[128];
	float eys[128];
	float ems[128];
} body_part;

typedef struct snake {
	int id;
	int sct;
	bool iiv;

	int skin_data_len;
	uint8_t* skin_data;
	bool cusk;

	float ang;
	float dir;
	float wang;
	float sp;
	float fam;
	float xx;
	float yy;
	float fx;
	float fy;
	float rex;
	float rey;
	float eang;
	float spang;
	float sc;
	float scang;
	float ssp;
	float fsp;
	float tsp;
	float sep;
	float wsep;
	float tl;
	float chl;
	float ehl;
	float wehang;
	float ehang;
	float fchl;
	body_part* pts;
	float fxs[RFC];
	float fys[RFC];
	float fchls[RFC];
	float fls[LFC];
	float fas[AFC];
	float cfl;
	float fl;
	float fa;
	int flpos;
	int fltg;
	int fpos;
	int fapos;
	int fatg;
	int ftg;
	char nk[25];

	bool dead;
	float dead_amt;
	float alive_amt;
} snake;

void snake_update_length(snake* snake, game* g);

#endif