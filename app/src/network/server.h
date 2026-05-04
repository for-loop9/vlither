#ifndef SERVER_H
#define SERVER_H

#include <thermite.h>

void server_init(tenv* env);
void server_connect(tenv* env);
void server_poll(tenv* env);
void server_destroy(tenv* env);

#endif