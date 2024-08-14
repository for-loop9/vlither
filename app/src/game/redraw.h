#ifndef REDRAW_H
#define REDRAW_H

typedef struct game game;
typedef struct input_data input_data;

void redraw(game* g, const input_data* input_data);

#endif