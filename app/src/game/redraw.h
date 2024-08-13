#ifndef REDRAW_H
#define REDRAW_H

typedef struct game game;
typedef struct input_data input_data;

void redraw(game* g, int fps_display);

#endif