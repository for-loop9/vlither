#include "message.h"
#include <string.h>
#include "game.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "../external/cimgui/cimgui.h"

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

		ImGuiStyle* style = igGetStyle();
		ImVec2 msg_txt_sz; igCalcTextSize(&msg_txt_sz, msg->message, NULL, false, 0);
		int msg_len = msg_txt_sz.x;
		int msg_x = g->icontext->default_frame.resolution.x / 2 - msg_len / 2;

		if (g->connected)
			igSetNextWindowFocus();
		
		igSetNextWindowPos((ImVec2) { msg_x - 10, (g->icontext->default_frame.resolution.y - 60) - msg_queue->y }, ImGuiCond_None, (ImVec2) {});
		igSetNextWindowSize((ImVec2) { msg_len + 20, 46 }, ImGuiCond_None);
		igPushStyleColor_Vec4(ImGuiCol_WindowBg, (ImVec4) { .x = 0.2f, .y = 0.2f, .z = 0.2f, .w = 1 });
		igBegin("message_win", NULL, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		igTextColored((ImVec4) { msg->color.x, msg->color.y, msg->color.z, msg->color.w }, msg->message);
		igEnd();
		igPopStyleColor(1);
	}
}
