#ifndef TENV_H
#define TENV_H

#include "../framework/tkeyboard.h"
#include "../framework/tmouse.h"
#include "../graphics/tcontext.h"

typedef struct tuser_data tuser_data;

typedef struct tenv {
  twindow* wnd;
  tkeyboard* kb;
  tmouse* ms;
  tcontext* ctx;
  tuser_data* usr;

  struct {
    int argc;
    char** argv;
    bool running;
    bool vsync;
    bool fullscreen;
    bool resizable;
    float aspect_ratio;
    const char* title;
    int fif;
  } config;
} tenv;

void tlaunch(tenv* env);
void tresize(tenv* env);
void tinit(tenv* env);
void trender(tenv* env);
void tinput(tenv* env);
void tdestroy(tenv* env);

#endif