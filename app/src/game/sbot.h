#ifndef SBOT_H
#define SBOT_H

#include <thermite.h>

// [Unimplemented] Implementation of Saya's bot: https://github.com/saya-0x0efe/Slither.io-bot

typedef struct sbot {
  struct {
    float xm;
    float ym;
    bool accel;
  } output;
} sbot;

void sbot_init(tenv* env);
void sbot_go(tenv* env);
void sbot_destroy(tenv* env);

#endif