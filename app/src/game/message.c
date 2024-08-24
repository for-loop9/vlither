#include "message.h"
#include <string.h>
#include "game.h"

message_queue message_queue_create() {
	return (message_queue) {
		.messages = ig_darray_create(message),
		.y = -60 - 14
	};
}

void message_queue_push(message_queue* msg_queue, message* message) {
	ig_darray_insert(&msg_queue->messages, 0, message);
}

void message_queue_destroy(message_queue* msg_queue) {
	ig_darray_destroy(msg_queue->messages);
}

void message_queue_render(message_queue* msg_queue, game* g, float dt) {
	int msgs_len = ig_darray_length(msg_queue->messages);
	if (msgs_len) {
		message* msg = msg_queue->messages + msgs_len - 1;

		if (msg->et >= msg->tt) {
			msg_queue->y -= dt * 330;
			if (msg_queue->y <= -60 - 14) {
				msg_queue->y = -60 - 14;
				ig_darray_pop(msg_queue->messages);
				// msg->et += dt;
				return;
			}
		} else {
			msg_queue->y += dt * 330;
			if (msg_queue->y >= 0) {
				msg_queue->y = 0;
				msg->et += dt;
			}
		}

		int msg_len = strlen(msg->message) * 16;
		int msg_x = g->icontext->default_frame.resolution.x / 2 - msg_len / 2;

		renderer_push_sprite(g->renderer, &(sprite_instance) {
			.rect = { .x = msg_x - 20, .y = (g->icontext->default_frame.resolution.y - 60) - msg_queue->y, .z = msg_len + 40, .w = 46  },
			.ratios = { .x = 0, .y = 1 },
			.uv_rect = { .x = 3 / 64.0f, .y = 3 / 64.0f, .z = 1 / 64.0f, .w = 1 / 64.0f },
			.color = { .x = 0.2f, .y = 0.2f, .z = 0.2f, .w = 1 },
		});
		renderer_push_text(g->renderer, msg->message,
			&(ig_vec3) {
				.x = msg_x,
				.y = (g->icontext->default_frame.resolution.y - 45) - msg_queue->y, .z = 14 },
			&msg->color,
			&(ig_vec3) {});
	}
}
