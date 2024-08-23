#include "set_skin.h"
#include "game.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "../external/cimgui/cimgui.h"
#include "../networking/util.h"

int skin_code_filter(ImGuiInputTextCallbackData* data) { 
    return !((data->EventChar >= 'a' && data->EventChar <= 'z') || (data->EventChar == '-') || (data->EventChar == ',') || (data->EventChar >= '0' && data->EventChar <= '9'));
}

int get_cg_id(game* g, char skin_code_cg_id) {
	for (int i = 0; i < 40; i++) {
		if (g->config.ntl_cg_map[i] == skin_code_cg_id) return i;
	}
	return -1;
}

void set_skin(game* g) {
	renderer_push_bg(g->renderer, &(sprite_instance) {
		.rect = { 0, 0, g->icontext->default_frame.resolution.x, g->icontext->default_frame.resolution.y },
		.ratios = { .x = 0, .y = 1 },
		.uv_rect = { .x = -1 * glfwGetTime() * 0.15f, .y = 0, .z = (g->icontext->default_frame.resolution.x / g->config.bgw2), .w = (g->icontext->default_frame.resolution.y / g->config.bgh2) },
		.color = { .x = 1, .y = 1, .z = 1 }
	});

	ImGuiStyle* style = igGetStyle();
	int sx = (g->icontext->default_frame.resolution.x / 2 - 200) - 4;
	int sy = (g->icontext->default_frame.resolution.y / 2 - 260) - 40;

	uint8_t* skin_data = g->config.default_skins[g->settings_instance.cv] + 1;
	uint8_t skin_data_len = *(skin_data - 1);
	int tot_segments = 0;
	for (int x = g->icontext->default_frame.resolution.x - 133 - 8; (x >= 96) && (tot_segments < 192); x -= 8, tot_segments++) {
		renderer_push_bp(g->renderer, &(bp_instance) {
			.circ = { .x = x - 8, .y = sy - 8, .z = 0, .w = 49 },
			.ratios = { .x = 0, .y = 1 },
			.color = { .x = 0, .y = 0, .z = 0, .w = 1 },
			.shadow = 1
		});
	}

	for (int x = g->icontext->default_frame.resolution.x - 133 - 8, seg_ct = 0; (x >= 96) && (seg_ct < 192); x -= 8, seg_ct++) {
		if (g->settings_instance.cusk) {
			if (g->settings_instance.cusk_skin_data_exp[(tot_segments - 1 - seg_ct)] != -1) {
				ig_vec3* cg_grp = g->config.color_groups + g->settings_instance.cusk_skin_data_exp[(tot_segments - 1 - seg_ct)];

				renderer_push_bp(g->renderer, &(bp_instance) {
					.circ = { .x = x, .y = sy, .z = 0, .w = 33 },
					.ratios = { .x = 0, .y = 1 },
					.color = { .x = cg_grp->x, .y = cg_grp->y, .z = cg_grp->z, .w = 1 }
				});
			}
		} else {
			ig_vec3* col = g->config.color_groups + skin_data[(tot_segments - 1 - seg_ct) % skin_data_len];
			renderer_push_bp(g->renderer, &(bp_instance) {
				.circ = { .x = x, .y = sy, .z = 0, .w = 33 },
				.ratios = { .x = 0, .y = 1 },
				.color = { .x = col->x, .y = col->y, .z = col->z, .w = 1 }
			});
		}
	}

	renderer_push_bp(g->renderer, &(bp_instance) {
		.circ = { .x = 100 + 3, .y = sy + 2, .z = 1, .w = 13 },
		.ratios = { .x = 0, .y = 1 },
		.color = { .x = 1, .y = 1, .z = 1, .w = 1 },
		.eye = 1
	});
	renderer_push_bp(g->renderer, &(bp_instance) {
		.circ = { .x = 100 + 3, .y = sy + 17, .z = 1, .w = 13 },
		.ratios = { .x = 0, .y = 1 },
		.color = { .x = 1, .y = 1, .z = 1, .w = 1 },
		.eye = 1
	});
	renderer_push_bp(g->renderer, &(bp_instance) {
		.circ = { .x = 100 + 3, .y = sy + 5, .z = 1, .w = 8 },
		.ratios = { .x = 0, .y = 1 },
		.color = { .x = 0, .y = 0, .z = 0, .w = 1 },
		.eye = 1
	});
	renderer_push_bp(g->renderer, &(bp_instance) {
		.circ = { .x = 100 + 3, .y = sy + 19, .z = 1, .w = 8 },
		.ratios = { .x = 0, .y = 1 },
		.color = { .x = 0, .y = 0, .z = 0, .w = 1 },
		.eye = 1
	});

	igSetNextWindowPos((ImVec2) { .x = sx + 4, .y = sy + 40 }, ImGuiCond_None, (ImVec2) {});
	igSetNextWindowSize((ImVec2) { .x = 400, .y = 860 }, ImGuiCond_None);
	igBegin("setskin", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar);

	igSetCursorPosX(igGetWindowWidth() / 2 - igGetFrameHeight());
	if (igArrowButton("##prev_skin", ImGuiDir_Left)) {
		g->settings_instance.cv--;
		if (g->settings_instance.cv < 0)
			g->settings_instance.cv = 65;
	}
	igSameLine(0, style->FramePadding.x);
	if (igArrowButton("##next_skin", ImGuiDir_Right)) {
		g->settings_instance.cv++;
		if (g->settings_instance.cv > 65)
			g->settings_instance.cv = 0;
	}
	igCheckbox("Custom", &g->settings_instance.cusk);

	if (g->settings_instance.cusk) {
		igPushFont(g->renderer->fonts[RENDERER_FONT_MED]);
		if (igBeginTable("cg_buttons", 11, ImGuiTableFlags_None, (ImVec2) {}, 0)) {
			igTableNextRow(ImGuiTableRowFlags_None, 0);
			int column = 0;
			for (int i = 27; i < 40; i++) {
				igTableSetColumnIndex(column);
				// igText("%d%d", 0, column);
				if (i == 36 || i == 38) continue;
				ig_vec3* col_at_idx = g->config.color_groups + i;
				ig_vec3* opp_col_at_idx = g->config.color_groups_opp + i;

				char btn_label[3] = {};
				sprintf(btn_label, "%c", g->config.ntl_cg_map[i]);
				igPushStyleColor_Vec4(ImGuiCol_Button, (ImVec4) { col_at_idx->x, col_at_idx->y, col_at_idx->z, 1 });
				igPushStyleColor_Vec4(ImGuiCol_ButtonHovered, (ImVec4) { col_at_idx->x, col_at_idx->y, col_at_idx->z, 0.5f }); // Lighten color when hovered
				igPushStyleColor_Vec4(ImGuiCol_ButtonActive, (ImVec4) { col_at_idx->x, col_at_idx->y, col_at_idx->z, 0.3f }); // Lighten color when active
				igPushStyleColor_Vec4(ImGuiCol_Text, (ImVec4) { opp_col_at_idx->x,opp_col_at_idx->y,opp_col_at_idx->z, 1});
				if (igButton(btn_label, (ImVec2) { -1, 0 })) {
					if (g->settings_instance.exp_ptr < tot_segments) {
						g->settings_instance.cusk_skin_data_exp[g->settings_instance.exp_ptr++] = i;
					} else {
						message_queue_push(&g->msg_queue, (&(message) {
							.message = "Use skin code to make longer skin",
							.tt = 2,
							.color = { .x = 1, .y = 0.3f, .z = 0.3f, .w = 1 }
						}));
					}
				}
				igPopStyleColor(4);
				column++;
			}
			igTableNextRow(ImGuiTableRowFlags_None, 0);
			column = 0;
			for (int i = 17; i < 27; i++) {
				igTableSetColumnIndex(column);
				ig_vec3* col_at_idx = g->config.color_groups + i;
				ig_vec3* opp_col_at_idx = g->config.color_groups_opp + i;

				char btn_label[3] = {};
				sprintf(btn_label, "%c", g->config.ntl_cg_map[i]);
				igPushStyleColor_Vec4(ImGuiCol_Button, (ImVec4) { col_at_idx->x, col_at_idx->y, col_at_idx->z, 1 });
				igPushStyleColor_Vec4(ImGuiCol_ButtonHovered, (ImVec4) { col_at_idx->x, col_at_idx->y, col_at_idx->z, 0.5f }); // Lighten color when hovered
				igPushStyleColor_Vec4(ImGuiCol_ButtonActive, (ImVec4) { col_at_idx->x, col_at_idx->y, col_at_idx->z, 0.3f }); // Lighten color when active
				igPushStyleColor_Vec4(ImGuiCol_Text, (ImVec4) { opp_col_at_idx->x,opp_col_at_idx->y,opp_col_at_idx->z, 1});
				if (igButton(btn_label, (ImVec2) { -1, 0 })) {
					if (g->settings_instance.exp_ptr < tot_segments) {
						g->settings_instance.cusk_skin_data_exp[g->settings_instance.exp_ptr++] = i;
					} else {
						message_queue_push(&g->msg_queue, (&(message) {
							.message = "Use skin code to make longer skin",
							.tt = 2,
							.color = { .x = 1, .y = 0.3f, .z = 0.3f, .w = 1 }
						}));
					}
				}
				igPopStyleColor(4);
				column++;
			}
			igTableNextRow(ImGuiTableRowFlags_None, 0);
			column = 0;
			for (int i = 8; i < 17; i++) {
				igTableSetColumnIndex(column);
				ig_vec3* col_at_idx = g->config.color_groups + i;
				ig_vec3* opp_col_at_idx = g->config.color_groups_opp + i;

				char btn_label[3] = {};
				sprintf(btn_label, "%c", g->config.ntl_cg_map[i]);
				igPushStyleColor_Vec4(ImGuiCol_Button, (ImVec4) { col_at_idx->x, col_at_idx->y, col_at_idx->z, 1 });
				igPushStyleColor_Vec4(ImGuiCol_ButtonHovered, (ImVec4) { col_at_idx->x, col_at_idx->y, col_at_idx->z, 0.5f }); // Lighten color when hovered
				igPushStyleColor_Vec4(ImGuiCol_ButtonActive, (ImVec4) { col_at_idx->x, col_at_idx->y, col_at_idx->z, 0.3f }); // Lighten color when active
				igPushStyleColor_Vec4(ImGuiCol_Text, (ImVec4) { opp_col_at_idx->x,opp_col_at_idx->y,opp_col_at_idx->z, 1});
				if (igButton(btn_label, (ImVec2) { -1, 0 })) {
					if (g->settings_instance.exp_ptr < tot_segments) {
						g->settings_instance.cusk_skin_data_exp[g->settings_instance.exp_ptr++] = i;
					} else {
						message_queue_push(&g->msg_queue, (&(message) {
							.message = "Use skin code to make longer skin",
							.tt = 2,
							.color = { .x = 1, .y = 0.3f, .z = 0.3f, .w = 1 }
						}));
					}
				}
				igPopStyleColor(4);
				column++;
			}
			igTableNextRow(ImGuiTableRowFlags_None, 0);
			column = 0;
			for (int i = 0; i < 8; i++) {
				igTableSetColumnIndex(column);
				ig_vec3* col_at_idx = g->config.color_groups + i;
				ig_vec3* opp_col_at_idx = g->config.color_groups_opp + i;

				char btn_label[3] = {};
				sprintf(btn_label, "%c", g->config.ntl_cg_map[i]);
				igPushStyleColor_Vec4(ImGuiCol_Button, (ImVec4) { col_at_idx->x, col_at_idx->y, col_at_idx->z, 1 });
				igPushStyleColor_Vec4(ImGuiCol_ButtonHovered, (ImVec4) { col_at_idx->x, col_at_idx->y, col_at_idx->z, 0.5f }); // Lighten color when hovered
				igPushStyleColor_Vec4(ImGuiCol_ButtonActive, (ImVec4) { col_at_idx->x, col_at_idx->y, col_at_idx->z, 0.3f }); // Lighten color when active
				igPushStyleColor_Vec4(ImGuiCol_Text, (ImVec4) { opp_col_at_idx->x,opp_col_at_idx->y,opp_col_at_idx->z, 1 });
				if (igButton(btn_label, (ImVec2) { -1, 0 })) {
					if (g->settings_instance.exp_ptr < tot_segments) {
						g->settings_instance.cusk_skin_data_exp[g->settings_instance.exp_ptr++] = i;
					} else {
						message_queue_push(&g->msg_queue, (&(message) {
							.message = "Use skin code to make longer skin",
							.tt = 2,
							.color = { .x = 1, .y = 0.3f, .z = 0.3f, .w = 1 }
						}));
					}
				}
				igPopStyleColor(4);
				column++;
			}
			igEndTable();
		}
		igPopFont();

		igSetNextItemWidth(igGetWindowWidth() / 1.25f - style->WindowPadding.x);
		igInputTextWithHint("##skin_code", "Skin code", g->settings_instance.skin_code, sizeof(g->settings_instance.skin_code), ImGuiInputTextFlags_CallbackCharFilter | ImGuiInputTextFlags_EnterReturnsTrue, skin_code_filter, NULL);
		igSameLine(0, style->FramePadding.x);
		igSetNextItemWidth(-1);
		if (igButton("Set", (ImVec2) { -1, 0 })) {
			int skin_code_length = strlen(g->settings_instance.skin_code);
			if (skin_code_length) {
				for (int i = 0; i < 256; i++) g->settings_instance.cusk_skin_data_exp[i] = -1;
				int i = 0;
				for (i = 0; i < skin_code_length; i++) {
					g->settings_instance.cusk_skin_data_exp[i] = get_cg_id(g, g->settings_instance.skin_code[i]);
				}
				g->settings_instance.exp_ptr = i;
			}
		}
		if (igButton("Reset", (ImVec2) { igGetWindowWidth() / 1.25f - style->WindowPadding.x, 0 })) {
			for (int i = 0; i < 256; i++) g->settings_instance.cusk_skin_data_exp[i] = -1;
			g->settings_instance.exp_ptr = 0;
		}
		igSameLine(0, style->FramePadding.x);
		igSetNextItemWidth(-1);
		if (igButton("C", (ImVec2) { -1, 0 }) && g->settings_instance.exp_ptr > 0) {
			g->settings_instance.cusk_skin_data_exp[--g->settings_instance.exp_ptr] = -1;
		}
	}
	if (igButton("Save", (ImVec2) { -1, 0 })) {
		if (g->settings_instance.cusk_skin_data_exp[0] == -1)
			g->settings_instance.cusk = false;
		
		save_settings(g);
		g->setting_skin = 0;
	}
	igEnd();
}