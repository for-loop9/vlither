#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <math.h>

#define PI 3.14159265358979323846
#define X2PI (2 * 3.14159265358979323846)

typedef struct game game;

void decode_secret(const uint8_t* packet, uint8_t* result);
uint8_t* make_nickname_skin_data(game* g, int* nickname_skin_data_len);
void set_mscps_fmlts_fpsls(game* g);
void reset_game(game* g);
void set_settings(game* g);
void save_settings(game* g);
void load_settings(game* g);

#endif