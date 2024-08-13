#ifndef OEF_H
#define OEF_H

typedef struct game game;
#include "../external/mg/mongoose.h"

typedef struct input_data input_data;

void oef(game* g, struct mg_connection* c, const input_data* input_data);

#endif