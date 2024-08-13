#include "snake_map.h"
#include "../game/snake.h"
#include <stdio.h>

snake_map snake_map_create() {
	return (snake_map) {
		.ids = ig_darray_create(int),
		.snakes = ig_darray_create(snake)
	};
}

void snake_map_destroy(snake_map* snake_map) {
	int keys_size = ig_darray_length(snake_map->ids);
	for (int i = 0; i < keys_size; i++) {
		ig_darray_destroy(snake_map->snakes[i].pts);
		if (snake_map->snakes[i].cusk) free(snake_map->snakes[i].skin_data);
	}
	ig_darray_destroy(snake_map->snakes);
	ig_darray_destroy(snake_map->ids);
}

int snake_map_get_total(snake_map* snake_map) {
	return ig_darray_length(snake_map->ids);
}

snake* snake_map_get(snake_map* snake_map, int id) {
	int keys_size = ig_darray_length(snake_map->ids);
	for (int i = 0; i < keys_size; i++) {
		if (snake_map->ids[i] == id)
			return snake_map->snakes + i;
	}
	return NULL;
}

snake* snake_map_put(snake_map* snake_map, int id, snake* snake) {
	ig_darray_push(&snake_map->ids, &id);
	ig_darray_push(&snake_map->snakes, snake);
	return snake_map->snakes + (snake_map_get_total(snake_map) - 1);
}

void snake_map_remove(snake_map* snake_map, int id) {
	int keys_length = ig_darray_length(snake_map->ids);
	for (int i = keys_length - 1; i >= 0; i--) {
		if (snake_map->ids[i] == id) {
			ig_darray_destroy(snake_map->snakes[i].pts);
			if (snake_map->snakes[i].cusk) free(snake_map->snakes[i].skin_data);
			ig_darray_remove(snake_map->ids, i);
			ig_darray_remove(snake_map->snakes, i);
			return;
		}
	}
}

void snake_map_remove_idx(snake_map* snake_map, int i) {
	ig_darray_destroy(snake_map->snakes[i].pts);
	if (snake_map->snakes[i].cusk) free(snake_map->snakes[i].skin_data);
	ig_darray_remove(snake_map->ids, i);
	ig_darray_remove(snake_map->snakes, i);
}

void snake_map_clear(snake_map* snake_map) {
	int keys_length = ig_darray_length(snake_map->ids);
	for (int i = 0; i < keys_length; i++) {
		ig_darray_destroy(snake_map->snakes[i].pts);
		if (snake_map->snakes[i].cusk) free(snake_map->snakes[i].skin_data);
	}

	ig_darray_clear(snake_map->ids);
	ig_darray_clear(snake_map->snakes);
}
