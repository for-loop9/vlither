#ifndef TENTRY_H
#define TENTRY_H

#include "tenv.h"

#define TDEF_USER_DATA(data) typedef struct tuser_data data tuser_data

int tentry(int argc, char** argv, size_t usr_size);

#define TDEF_ENTRY()\
  int main(int argc, char** argv) {\
    return tentry(argc, argv, sizeof(tuser_data));\
  }\

#endif