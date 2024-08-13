#include "hud.h"
#include "game.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "../external/cimgui/cimgui.h"
#include <math.h>

void hud(game* g, int fps_display) {
	float mww = g->icontext->default_frame.resolution.x;
	float mhh = g->icontext->default_frame.resolution.y;
	float mww2 = mww / 2;
	float mhh2 = mhh / 2;

	ImGuiStyle* style = igGetStyle();
	ImDrawList* draw_list = igGetWindowDrawList();
	ImVec2 fps_text_size;
	igCalcTextSize(&fps_text_size, "FPS: 000", NULL, 0, 0);

	if (g->settings_instance.show_fps) {
		char fps_str[32] = {};
		sprintf(fps_str, "FPS: %d", fps_display);
		ImDrawList_AddText_Vec2(draw_list,
			(ImVec2) {
				.x = g->icontext->default_frame.resolution.x / 2 - fps_text_size.x / 2, .y = 10
			}, igColorConvertFloat4ToU32((ImVec4) { 1, 1, 0, 1 }), fps_str, NULL);
	}

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

	igPushFont(g->renderer->small_font);
	ImVec2 ll_txt_size; igCalcTextSize(&ll_txt_size, mm_labels[0], NULL, false, 0);
	ImVec2 rl_txt_size; igCalcTextSize(&rl_txt_size, mm_labels[1], NULL, false, 0);
	ImVec2 tl_txt_size; igCalcTextSize(&tl_txt_size, mm_labels[2], NULL, false, 0);
	ImVec2 bl_txt_size; igCalcTextSize(&bl_txt_size, mm_labels[3], NULL, false, 0);

	int mm_x = g->icontext->default_frame.resolution.x - (142 + rl_txt_size.x + 2);
	int mm_y = g->icontext->default_frame.resolution.y - (142 + bl_txt_size.y + 2);

	renderer_set_map_data(g->renderer, g->config.mmdata);
	renderer_push_mm(g->renderer, &(mm_instance) {
		.transform = { .x = mm_x, mm_y, 136 },
		.color = { .x = 0.4f, .y = 0.3f, .z = 0.3f }
	});
	ImDrawList_AddText_Vec2(draw_list,
		(ImVec2) {
			.x = mm_x - ll_txt_size.x, .y = (mm_y + 136 / 2) - ll_txt_size.y / 2
		}, igColorConvertFloat4ToU32((ImVec4) { 1, 1, 0, 1 }), mm_labels[0], NULL);
	ImDrawList_AddText_Vec2(draw_list,
		(ImVec2) {
			.x = mm_x + 136, .y = (mm_y + 136 / 2) - ll_txt_size.y / 2
		}, igColorConvertFloat4ToU32((ImVec4) { 1, 1, 0, 1 }), mm_labels[1], NULL);
	ImDrawList_AddText_Vec2(draw_list,
		(ImVec2) {
			.x = (mm_x + 136 / 2) - tl_txt_size.x / 2, .y = mm_y - tl_txt_size.y
		}, igColorConvertFloat4ToU32((ImVec4) { 1, 1, 0, 1 }), mm_labels[2], NULL);
	ImDrawList_AddText_Vec2(draw_list,
		(ImVec2) {
			.x = (mm_x + 136 / 2) - bl_txt_size.x / 2, .y = mm_y + 136
		}, igColorConvertFloat4ToU32((ImVec4) { 1, 1, 0, 1 }), mm_labels[3], NULL);

	igPopFont();

	if (!g->snake_null) {
		// crosshair:
		// float cx = mww2;// + (g->os.snakes[0].sc * 29 * 0.5f * g->config.gsc) * cosf(g->os.snakes[0].ehang);
		// float cy = mhh2;// + (g->os.snakes[0].sc * 29 * 0.5f * g->config.gsc) * sinf(g->os.snakes[0].ehang);
		// ig_vec3* head_col = g->config.color_groups + g->os.snakes[0].skin_data[0];

		// renderer_push_circle(g->renderer, &(circle_instance) {
		// 	.circ = { .x = cx - 3, .y = cy - 3, .z = 6 },
		// 	.ratios = { .x = 0, .y = 1 },
		// 	.color = { .x = 1 - head_col->x, .y = 1 - head_col->y, .z = 1 - head_col->z, .w = 1 }
		// });

		g->config.length_display = floorf((g->config.fpsls[g->os.snakes[0].sct] + g->os.snakes[0].fam / g->config.fmlts[g->os.snakes[0].sct] - 1) * 15 - 5) / 1;

		ig_vec2 player_pos_map = { .x = 136 * (g->os.snakes[0].xx / (g->config.grd * 2)), .y = 136 * (g->os.snakes[0].yy / (g->config.grd * 2)) };
		renderer_push_sprite(g->renderer, &(sprite_instance) {
			.rect = { .x = player_pos_map.x + mm_x - 3 , player_pos_map.y + mm_y - 3, 6, 6 },
			.ratios = { .x = 0, .y = 1 },
			.uv_rect = { .x = 0, .y = 0, .z = 6 / 64.0f, .w = 6 / 64.0f },
			.color = { .x = 1, .y = 1, .z = 1 }
		});
	}

	char stats_buff[64] = {};
	sprintf(stats_buff, "Length = %d", g->config.length_display);
	ImDrawList_AddText_Vec2(draw_list,
	(ImVec2) {
		.x = 4, .y = g->icontext->default_frame.resolution.y - fps_text_size.y 
	}, igColorConvertFloat4ToU32((ImVec4) { .x = 1, .y = 0.5f, .z = 0.5f, .w = 1 }), stats_buff, NULL);

	if (g->settings_instance.show_kill) {
		sprintf(stats_buff, "Kills = %d", g->config.kills_display);
		ImDrawList_AddText_Vec2(draw_list,
		(ImVec2) {
			.x = 4, .y = g->icontext->default_frame.resolution.y - fps_text_size.y * 2 
		}, igColorConvertFloat4ToU32((ImVec4) { .x = 1, .y = 0.5f, .z = 0.5f, .w = 1 }), stats_buff, NULL);
	}

	igPushFont(g->renderer->small_font);

	float rank_size; ImVec2 clm_txt_size; igCalcTextSize(&clm_txt_size, "10", NULL, 0, 0); rank_size = clm_txt_size.x;
	float name_size; igCalcTextSize(&clm_txt_size, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", NULL, 0, 0); name_size = clm_txt_size.x;
	float score_size; igCalcTextSize(&clm_txt_size, "9999999", NULL, 0, 0); score_size = clm_txt_size.x;

	igSetCursorPosX(mww - (rank_size + name_size + score_size + style->FramePadding.x * 9));
	igBeginTable("LeaderboardTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, (ImVec2) {}, 0);

    igTableSetupColumn("Rank", ImGuiTableColumnFlags_WidthFixed, rank_size, 0);
    igTableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, name_size, 0);
    igTableSetupColumn("Score", ImGuiTableColumnFlags_None, 0, 0);
    // igTableHeadersRow();

    for (int i = 0; i < 10; i++) {
        leaderboard_entry* entry = g->leaderboard + i;
		ImVec4 textColor = (ImVec4){ g->config.color_groups[entry->cv].x, g->config.color_groups[entry->cv].y, g->config.color_groups[entry->cv].z, 0.6f };

		igPushStyleVar_Vec2(ImGuiStyleVar_CellPadding, (ImVec2) { style->CellPadding.x, style->CellPadding.y - 2 });
		igTableNextRow(ImGuiTableRowFlags_None, 0);
		igPopStyleVar(1);
		igTableSetColumnIndex(0);

		if (g->config.my_pos == i + 1 && !g->snake_null) {
			textColor.w = 1;
			igPushFont(g->renderer->small_bold_font);
			igText("%d", i + 1);

			igTableSetColumnIndex(1);
			if (strlen(entry->nk) == 0) {
				igTextColored(textColor, "<NO NAME>");
			}
			else {
				igTextColored(textColor, entry->nk);
			}
			igTableSetColumnIndex(2);
			igTextColored(textColor, "%d", entry->score);
			igPopFont();
		} else {
			igText("%d", i + 1);

			igTableSetColumnIndex(1);
			if (strlen(entry->nk) == 0) {
				igTextColored(textColor, "<NO NAME>");
			}
			else {
				igTextColored(textColor, entry->nk);
			}
			igTableSetColumnIndex(2);
			igTextColored(textColor, "%d", entry->score);
		}
    }

    igEndTable();
	igPopFont();
}