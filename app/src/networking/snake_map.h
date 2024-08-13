#ifndef SNAKE_MAP_H
#define SNAKE_MAP_H

#include <ignite.h>
#include "../game/food.h"

typedef struct snake snake;

typedef struct snake_map {
	int* ids;	
	snake* snakes;
} snake_map;

snake_map snake_map_create();
void snake_map_destroy(snake_map* snake_map);

int snake_map_get_total(snake_map* snake_map);
snake* snake_map_get(snake_map* snake_map, int id);
snake* snake_map_put(snake_map* snake_map, int id, snake* snake);
void snake_map_remove(snake_map* snake_map, int id);
void snake_map_remove_idx(snake_map* snake_map, int i);
void snake_map_clear(snake_map* snake_map);

#endif