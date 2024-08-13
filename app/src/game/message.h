#ifndef MSG_H
#define MSG_H

#include <ignite.h>

typedef struct game game;

typedef struct message {
	float et;
	float tt;
	char message[128];
	ig_vec4 color;
} message;

typedef struct message_queue {
	message* messages;
	float y;
} message_queue;

message_queue message_queue_create();
void message_queue_push(message_queue* msg_queue, message* message);
void message_queue_destroy(message_queue* msg_queue);
void message_queue_render(message_queue* msg_queue, game* g, float dt);

#endif