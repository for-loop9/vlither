#include "hud.h"
#include "game.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "../external/cimgui/cimgui.h"
#include <math.h>

void hud(game* g, const input_data* input_data) {
	float mww = g->icontext->default_frame.resolution.x;
	float mhh = g->icontext->default_frame.resolution.y;
	float mww2 = mww / 2;
	float mhh2 = mhh / 2;
	float mx = input_data->mouse_pos.x;
	float my = input_data->mouse_pos.y;

	if (!g->snake_null && g->config.assist) {
		// crosshair:
		float cx = mww2 + (g->os.snakes[0].sc * 29 * 0.5f * g->config.gsc) * cosf(g->os.snakes[0].ehang);
		float cy = mhh2 + (g->os.snakes[0].sc * 29 * 0.5f * g->config.gsc) * sinf(g->os.snakes[0].ehang);
		float extr = g->settings_instance.hp_size * g->config.gsc * g->os.snakes[0].sc * 1.3f;

		renderer_push_bp(g->renderer, &(bp_instance) {
			.circ = { .x = cx - extr, .y = cy - extr, .z = 1, .w = extr * 2 },
			.ratios = { .x = 0, .y = 1 },
			.color = { .x = 0, .y = 0, .z = 0, .w = 1 },
			.eye = 1
		});

		extr = g->settings_instance.hp_size * g->config.gsc * g->os.snakes[0].sc;

		renderer_push_bp(g->renderer, &(bp_instance) {
			.circ = { .x = cx - extr, .y = cy - extr, .z = 1, .w = extr * 2 },
			.ratios = { .x = 0, .y = 1 },
			.color = { .x = g->settings_instance.hp_color.x, .y = g->settings_instance.hp_color.y, .z = g->settings_instance.hp_color.z, .w = 1 },
			.eye = 1
		});

		float laser_length = sqrtf((mx - mww2) * (mx - mww2) + (my - mhh2) * (my - mhh2));

		float laser_ang = atan2f(my - mhh2, mx - mww2);
		float laser_sin = sinf(laser_ang);
		float laser_cos = cosf(laser_ang);

		renderer_push_sprite(g->renderer, &(sprite_instance) {
			.rect = { .x = (mww2 - laser_length / 2) + (0.5f * laser_length * laser_cos), .y = (mhh2 - g->settings_instance.laser_thickness / 2) + (0.5f * laser_length * laser_sin), .z = laser_length, .w = g->settings_instance.laser_thickness },
			.ratios = { .x = laser_sin, .y = laser_cos },
			.uv_rect = { .x = 3 / 64.0f, .y = 3 / 64.0f, .z = 1 / 64.0f, .w = 1 / 64.0f },
			.color = g->settings_instance.laser_color
		});
	}

	ImGuiStyle* style = igGetStyle();
	ImDrawList* draw_list = igGetWindowDrawList();
	ImVec2 fps_text_size;
	igCalcTextSize(&fps_text_size, "FPS: 000", NULL, 0, 0);

	char mm_labels[4][3] = { {}, {}, {}, {} };

	if (g->settings_instance.clk) {
		mm_labels[0][0] = '9';
		mm_labels[1][0] = '3';
		mm_labels[2][0] = '1'; mm_labels[2][1] = '2';
		mm_labels[3][0] = '6';
	} else {
		mm_labels[0][0] = 'W';
		mm_labels[1][0] = 'E';
		mm_labels[2][0] = 'N';
		mm_labels[3][0] = 'S';
	}

	if (g->config.mmsize) {
		igPushFont(g->renderer->fonts[RENDERER_FONT_SMALL]);
		ImVec2 ll_txt_size; igCalcTextSize(&ll_txt_size, mm_labels[0], NULL, false, 0);
		ImVec2 rl_txt_size; igCalcTextSize(&rl_txt_size, mm_labels[1], NULL, false, 0);
		ImVec2 tl_txt_size; igCalcTextSize(&tl_txt_size, mm_labels[2], NULL, false, 0);
		ImVec2 bl_txt_size; igCalcTextSize(&bl_txt_size, mm_labels[3], NULL, false, 0);

		float mm_scaled = g->config.mmsize * g->settings_instance.mm_scale;

		int mm_x = g->icontext->default_frame.resolution.x - ((mm_scaled + 6) + rl_txt_size.x + 2);
		int mm_y = g->icontext->default_frame.resolution.y - ((mm_scaled + 6) + bl_txt_size.y + 2);

		renderer_push_sprite(g->renderer, &(sprite_instance) {
			.rect = { .x = mm_x + mm_scaled / 2, .y = mm_y, .z = 1, .w = mm_scaled },
			.ratios = { .x = 0, .y = 1 },
			.uv_rect = { .x = 3 / 64.0f, .y = 3 / 64.0f, .z = 1 / 64.0f, .w = 1 / 64.0f },
			.color = { .x = 1, .y = 1, .z = 1, .w = 0.5f }
		});
		renderer_push_sprite(g->renderer, &(sprite_instance) {
			.rect = { .x = mm_x + mm_scaled / 2, .y = mm_y, .z = 1, .w = mm_scaled },
			.ratios = { .x = 1, .y = 0 },
			.uv_rect = { .x = 3 / 64.0f, .y = 3 / 64.0f, .z = 1 / 64.0f, .w = 1 / 64.0f },
			.color = { .x = 1, .y = 1, .z = 1, .w = 0.5f }
		});

		renderer_push_mm(g->renderer, &(mm_instance) {
			.transform = { .x = mm_x, .y = mm_y, .z = mm_scaled },
			.color = { .x = 0.4f, .y = 0.3f, .z = 0.3f },
			.usage = g->config.mmsize / 512.0f
		}, g->config.mmsize);
		renderer_set_map_data(g->renderer, g->config.mmdata);
		ImDrawList_AddText_Vec2(draw_list,
			(ImVec2) {
				.x = mm_x - ll_txt_size.x, .y = (mm_y + mm_scaled / 2) - ll_txt_size.y / 2
			}, igColorConvertFloat4ToU32((ImVec4) { 1, 1, 0, 1 }), mm_labels[0], NULL);
		ImDrawList_AddText_Vec2(draw_list,
			(ImVec2) {
				.x = mm_x + mm_scaled, .y = (mm_y + mm_scaled / 2) - ll_txt_size.y / 2
			}, igColorConvertFloat4ToU32((ImVec4) { 1, 1, 0, 1 }), mm_labels[1], NULL);
		ImDrawList_AddText_Vec2(draw_list,
			(ImVec2) {
				.x = (mm_x + mm_scaled / 2) - tl_txt_size.x / 2, .y = mm_y - tl_txt_size.y
			}, igColorConvertFloat4ToU32((ImVec4) { 1, 1, 0, 1 }), mm_labels[2], NULL);
		ImDrawList_AddText_Vec2(draw_list,
			(ImVec2) {
				.x = (mm_x + mm_scaled / 2) - bl_txt_size.x / 2, .y = mm_y + mm_scaled
			}, igColorConvertFloat4ToU32((ImVec4) { 1, 1, 0, 1 }), mm_labels[3], NULL);

		igPopFont();

		if (!g->snake_null) {
			g->config.length_display = floorf((g->config.fpsls[g->os.snakes[0].sct] + g->os.snakes[0].fam / g->config.fmlts[g->os.snakes[0].sct] - 1) * 15 - 5) / 1;

			ig_vec2 player_pos_map = { .x = mm_scaled * (g->os.snakes[0].xx / (g->config.grd * 2)), .y = mm_scaled * (g->os.snakes[0].yy / (g->config.grd * 2)) };
			renderer_push_sprite(g->renderer, &(sprite_instance) {
				.rect = { .x = player_pos_map.x + mm_x - 3 , player_pos_map.y + mm_y - 3, 6, 6 },
				.ratios = { .x = 0, .y = 1 },
				.uv_rect = { .x = 0, .y = 0, .z = 6 / 64.0f, .w = 6 / 64.0f },
				.color = { .x = 1, .y = 1, .z = 1, .w = 1 }
			});
		}
	}

	char stats_buff[64] = {};

	float orig_cx = igGetCursorPosX();
	float orig_cy = igGetCursorPosY();

	igSetCursorPosX(4);
	igSetCursorPosY(g->icontext->default_frame.resolution.y - fps_text_size.y);
	igPushFont(g->renderer->fonts[RENDERER_FONT_MED]);
	igTextColored((ImVec4) { 1, 1, 1, 0.5f }, "Rank:"); igSameLine(0, -1);
	igPopFont();
	igPushFont(g->renderer->fonts[RENDERER_FONT_MED_BOLD]);
	igTextColored((ImVec4) { 1, 1, 1, 0.6f }, "%d/%d", g->config.rank, g->config.total_players);
	igPopFont();

	igSetCursorPosX(4);
	igSetCursorPosY(g->icontext->default_frame.resolution.y - fps_text_size.y * 2);
	igPushFont(g->renderer->fonts[RENDERER_FONT_MED]);
	igTextColored((ImVec4) { 1, 1, 1, 0.5f }, "Length:"); igSameLine(0, -1);
	igPopFont();
	igPushFont(g->renderer->fonts[RENDERER_FONT_MED_BOLD]);
	igTextColored((ImVec4) { 1, 1, 1, 0.8f }, "%d", g->config.length_display);
	igPopFont();

	if (g->settings_instance.show_kill) {
		igSetCursorPosX(4);
		igSetCursorPosY(g->icontext->default_frame.resolution.y - fps_text_size.y * 3);
		igPushFont(g->renderer->fonts[RENDERER_FONT_MED]);
		igTextColored((ImVec4) { 1, 1, 1, 0.5f }, "Kills:"); igSameLine(0, -1);
		igPopFont();
		igPushFont(g->renderer->fonts[RENDERER_FONT_MED_BOLD]);
		igTextColored((ImVec4) { 1, 1, 1, 0.8f }, "%d", g->config.kills_display);
		igPopFont();
	}

	if (g->settings_instance.show_fps) {
		igSetCursorPosX(4);
		igSetCursorPosY(g->icontext->default_frame.resolution.y - fps_text_size.y * 4);
		igPushFont(g->renderer->fonts[RENDERER_FONT_MED]);
		igTextColored((ImVec4) { 1, 1, 1, 0.5f }, "FPS: "); igSameLine(0, -1);
		igPopFont();
		igPushFont(g->renderer->fonts[RENDERER_FONT_MED_BOLD]);
		igTextColored((ImVec4) { 1, 1, 0.6f, 0.8f }, "%d", input_data->fps_display);
		igPopFont();
	}

	igPushFont(g->renderer->fonts[RENDERER_FONT_SMALL]);

	igSetCursorPosX(orig_cx);
	igSetCursorPosY(orig_cy);

	float rank_size; ImVec2 clm_txt_size; igCalcTextSize(&clm_txt_size, "10", NULL, 0, 0); rank_size = clm_txt_size.x;
	float name_size; igCalcTextSize(&clm_txt_size, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", NULL, 0, 0); name_size = clm_txt_size.x;
	float score_size; igCalcTextSize(&clm_txt_size, "9999999", NULL, 0, 0); score_size = clm_txt_size.x;

	igSetCursorPosX(mww - (rank_size + name_size + score_size + style->FramePadding.x * 9));
	igBeginTable("LeaderboardTable", 3, ImGuiTableFlags_None, (ImVec2) {}, 0);

    igTableSetupColumn("Rank", ImGuiTableColumnFlags_WidthFixed, rank_size, 0);
    igTableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, name_size, 0);
    igTableSetupColumn("Score", ImGuiTableColumnFlags_None, 0, 0);
    // igTableHeadersRow();

    for (int i = 0; i < 10; i++) {
        leaderboard_entry* entry = g->leaderboard + i;
		ImVec4 textColor = (ImVec4){ g->config.color_groups[entry->cv].x, g->config.color_groups[entry->cv].y, g->config.color_groups[entry->cv].z, 0.8f };

		igPushStyleVar_Vec2(ImGuiStyleVar_CellPadding, (ImVec2) { style->CellPadding.x, style->CellPadding.y - 2 });
		igTableNextRow(ImGuiTableRowFlags_None, 0);
		igPopStyleVar(1);
		igTableSetColumnIndex(0);

		if (g->config.my_pos == i + 1 && !g->snake_null) {
			textColor.w = 1;
			igPushFont(g->renderer->fonts[RENDERER_FONT_SMALL_BOLD]);
			igText("%d", i + 1);

			igTableSetColumnIndex(1);
			if (strlen(entry->nk) != 0) {
				igTextColored(textColor, entry->nk);
			}
			igTableSetColumnIndex(2);
			igTextColored(textColor, "%d", entry->score);
			igPopFont();
		} else {
			igText("%d", i + 1);

			igTableSetColumnIndex(1);
			if (strlen(entry->nk) != 0) {
				igTextColored(textColor, entry->nk);
			}
			igTableSetColumnIndex(2);
			igTextColored(textColor, "%d", entry->score);
		}
    }

	float bp_mem = g->renderer->bp_renderer->instance_count / (float) MAX_BP_RENDER;
	float food_mem = g->renderer->food_renderer->instance_count / (float) MAX_FOOD_RENDER;

    igEndTable();
	igSetCursorPosX(4);
	igSetCursorPosY(4);
	ImVec4 bar_col = {}; igImLerp_Vec4(&bar_col, (ImVec4) { 0, 1, 0, 0.5f }, (ImVec4) { 1, 0, 0, 0.5f }, bp_mem);
	igPushStyleColor_Vec4(ImGuiCol_PlotHistogram, bar_col);
	igProgressBar(bp_mem, (ImVec2) { 100, 20 }, NULL);
	igPopStyleColor(1);
	igSetCursorPosX(4);
	igImLerp_Vec4(&bar_col, (ImVec4) { 0, 1, 0, 0.5f }, (ImVec4) { 1, 0, 0, 0.5f }, food_mem);
	igPushStyleColor_Vec4(ImGuiCol_PlotHistogram, bar_col);
	igProgressBar(food_mem, (ImVec2) { 100, 20 }, NULL);
	igPopStyleColor(1);
	igPopFont();

	if (bp_mem >= 0.8f || food_mem >= 0.8f) {
		message msg = {
			.message = "MEMORY WARNING",
			.tt = 1,
			.color = { .x = 1, .y = 0.2f, .z = 0.2f, .w = 1 }
		};

		pthread_mutex_lock(&g->msg_mutex);
		message_queue_push(&g->msg_queue, &msg);
		pthread_mutex_unlock(&g->msg_mutex);
	}
}