#include "title_screen.h"
#include "game.h"
#include "../networking/util.h"
#include "set_skin.h"
#include "settings_screen.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "../external/cimgui/cimgui.h"

void title_screen(game* g) {
	// renderer_start_imgui_frame(g->renderer);

	// renderer_push_eye(g->renderer, &(eye_instance) {
	// 	.circ = { 300, 100, 0, 250 },
	// 	.ratios = { 0, 1 },
	// 	.color = { 1, 1, 1, 1 }
	// });

	// renderer_push_eye(g->renderer, &(eye_instance) {
	// 	.circ = { 600, 100, 0, 10 },
	// 	.ratios = { 0, 1 },
	// 	.color = { 1, 1, 1, 1 }
	// });

	// renderer_flush(g->renderer);
	// return;
	// renderer_start_imgui_frame(g->renderer);
	ImGuiStyle* style = igGetStyle();

	if (g->respawn) {
		g->respawn = false;
		reset_game(g);
		message_queue_push(&g->msg_queue, (&(message) {
			.message = "Connecting",
				.tt = 1,
				.color = { .x = 1, .y = 0.5f, .z = 0.3f, .w = 1 }
		}));

		if (g->played) {
			pthread_join(g->load_thread, NULL);
			pthread_join(g->network_thread, NULL);
		}

		pthread_create(&g->network_thread, NULL, network_worker, g);
		pthread_create(&g->load_thread, NULL, load_worker, g);
		g->connecting = 1;
		g->played = 1;
	} else {
		if (g->setting_skin) {
			set_skin(g);
		} else if (g->show_settings) {
			settings_screen(g);
		} else {
			int logo_len = 7 * 16 * 4;
			int logo_pos = g->icontext->default_frame.resolution.x / 2 - (logo_len) / 2;

			renderer_push_text(g->renderer, "Vlither", &(ig_vec3) {.x = logo_pos, .y = 100, .z = 14 * 4 }, & (ig_vec4) { .x = 0, .y = 0.8f, .z = 0.5f, .w = 1 }, & (ig_vec3) {});

			igSetNextWindowPos((ImVec2) { .x = logo_pos + 30, .y = g->icontext->default_frame.resolution.y / 2 - 150 }, ImGuiCond_None, (ImVec2) {});
			igSetNextWindowSize((ImVec2) { .x = logo_len - 60, .y = 300 }, ImGuiCond_None);
			igBegin("login", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize);

			bool did_play = g->config.length_display != 0;
			
			if (did_play) {
				char buff[64] = {};
				sprintf(buff, "Length: %d", g->config.length_display);
				
				igPushFont(g->renderer->fonts[RENDERER_FONT_MED]);
				ImVec2 txt_size; igCalcTextSize(&txt_size, buff, NULL, false, 0);
				igSetCursorPosX(((logo_len - 60) - txt_size.x) / 2);
				// highscore
				igText("Length:"); igSameLine(0, -1);
				igPopFont();
				igPushFont(g->renderer->fonts[RENDERER_FONT_MED_BOLD]);
				igText("%d", g->config.length_display);
				igPopFont();

				igPushFont(g->renderer->fonts[RENDERER_FONT_MED]);
				sprintf(buff, "Kills: %d", g->config.kills_display);
				igCalcTextSize(&txt_size, buff, NULL, false, 0);
				igSetCursorPosX(((logo_len - 60) - txt_size.x) / 2);
				// highscore
				igText("Kills:"); igSameLine(0, -1);
				igPopFont();
				igPushFont(g->renderer->fonts[RENDERER_FONT_MED_BOLD]);
				igText("%d", g->config.kills_display);
				igPopFont();
			}
			
			igSetNextItemWidth(-1);
			igInputTextWithHint("##nick", "Nickname", g->settings_instance.nickname, sizeof(g->settings_instance.nickname), ImGuiInputTextFlags_None, NULL, NULL);
			igSetNextItemWidth(-1);
			igInputTextWithHint("##ip", "IP:port", g->settings_instance.ip, sizeof(g->settings_instance.ip), ImGuiInputTextFlags_None, NULL, NULL);

			// ImGuiStyle* style = igGetStyle();

			// ImVec2 btn_txt_size; igCalcTextSize(&btn_txt_size, "Play", NULL, NULL, 0);
			// ImVec2 avail; igGetContentRegionAvail(&avail);
			// float size = btn_txt_size.x + style->FramePadding.x * 2.0f;

			// float off = (avail.x - size) * 0.5f;
			// if (off > 0.0f)
			//     igSetCursorPosX(igGetCursorPosX() + off);
			if (igButton(did_play ? "Play again" : "Play", (ImVec2) { -1, 0 })) {
				reset_game(g);
				message_queue_push(&g->msg_queue, (&(message) {
					.message = "Connecting",
						.tt = 1,
						.color = { .x = 1, .y = 0.5f, .z = 0.3f, .w = 1 }
				}));

				if (g->played) {
					pthread_join(g->load_thread, NULL);
					pthread_join(g->network_thread, NULL);
				}

				pthread_create(&g->network_thread, NULL, network_worker, g);
				pthread_create(&g->load_thread, NULL, load_worker, g);
				g->connecting = 1;
				g->played = 1;
			}
			// igSetNextItemWidth(-1);
			if (igButton("Set skin", (ImVec2) { igGetWindowWidth() / 2 - style->WindowPadding.x, 0 })) {
				g->setting_skin = 1;
			}
			igSameLine(0, style->FramePadding.x);
			if (igButton("Settings", (ImVec2) { -1, 0 })) {
				g->show_settings = true;
			}
			igPushStyleColor_Vec4(ImGuiCol_Button, (ImVec4) { .x = 0.7f, .y = 0.3f, .z = 0.3f, .w = 1 });
			igPushStyleColor_Vec4(ImGuiCol_ButtonHovered, (ImVec4) { .x = 0.8f, .y = 0.4f, .z = 0.4f, .w = 1 });
			igPushStyleColor_Vec4(ImGuiCol_ButtonActive, (ImVec4) { .x = 0.9f, .y = 0.5f, .z = 0.5f, .w = 1 });
			if (igButton("Save & quit", (ImVec2) { -1, 0 })) {
				save_settings(g);
				g->game_quit = 1;
			}
			igPopStyleColor(3);
			igEnd();
		}
	}

	renderer_flush(g->renderer);
}