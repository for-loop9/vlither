#ifndef TKEYBOARD_H
#define TKEYBOARD_H

typedef struct twindow twindow;

typedef struct tkeyboard {
  int* keys_pressed;
  int* keys_released;
  char char_pressed;
} tkeyboard;

tkeyboard* tkeyboard_create(twindow* window);
void tkeyboard_update(tkeyboard* keyboard);
int tkeyboard_key_pressed(tkeyboard* keyboard, int key);
int tkeyboard_key_released(tkeyboard* keyboard, int key);
void tkeyboard_destroy(tkeyboard* keyboard);

#endif