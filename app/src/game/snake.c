#include "snake.h"
#include "game.h"
#include "../networking/util.h"
#include "food.h"

void snake_update_length(snake* snake, game* g) {
	float orl = snake->tl;
	snake->tl = snake->sct + snake->fam;
	float d = snake->tl - orl;
	int k = snake->flpos;
	for (int j = 0; j < LFC; j++) {
		snake->fls[k] -= d * g->config.lfas[j];
		k++;
		if (k >= LFC) k = 0;
	}
	snake->fl = snake->fls[snake->flpos];
	snake->fltg = LFC;
}