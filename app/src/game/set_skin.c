#include "set_skin.h"
#include "game.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "../external/cimgui/cimgui.h"
#include "../networking/util.h"

void set_skin(game* g) {
	renderer_push_bg(g->renderer, &(sprite_instance) {
		.rect = { 0, 0, g->icontext->default_frame.resolution.x, g->icontext->default_frame.resolution.y },
		.ratios = { .x = 0, .y = 1 },
		.uv_rect = { .x = glfwGetTime() * 0.15f, .y = 0, .z = (g->icontext->default_frame.resolution.x / g->config.bgw2), .w = (g->icontext->default_frame.resolution.y / g->config.bgh2) },
		.color = { .x = 1, .y = 1, .z = 1 }
	});

	ImGuiStyle* style = igGetStyle();
	int sx = (g->icontext->default_frame.resolution.x / 2 - 200) - 4;
	int sy = (g->icontext->default_frame.resolution.y / 2 - 165) - 40;

	uint8_t* skin_data = g->config.default_skins[g->settings_instance.cv] + 1;
	uint8_t skin_data_len = *(skin_data - 1);
	for (int i = 0; i < 76; i++) {
		if (g->settings_instance.cusk) {
			ig_vec4 col = { .w = 1 };
			
			if (g->settings_instance.cusk_skin_data_exp[(75 - i) % 255] != -1) {
				ig_vec3* cg_grp = g->config.color_groups + g->settings_instance.cusk_skin_data_exp[(75 - i) % 255];
				col.x = cg_grp->x;
				col.y = cg_grp->y;
				col.z = cg_grp->z;
			}
			renderer_push_circle(g->renderer, &(circle_instance) {
				.circ = { .x = sx + (i * 5), .y = sy, .z = 33 },
				.ratios = { .x = 0, .y = 1 },
				.color = col
			});
		} else {
			ig_vec3* col = g->config.color_groups + skin_data[(76 - i) % skin_data_len];

			renderer_push_circle(g->renderer, &(circle_instance) {
				.circ = { .x = sx + (i * 5), .y = sy, .z = 33 },
				.ratios = { .x = 0, .y = 1 },
				.color = { .x = col->x, .y = col->y, .z = col->z, .w = 1 }
			});
		}

		if (i == 75) {
			renderer_push_circle(g->renderer, &(circle_instance) {
				.circ = { .x = (sx + (i * 5)) + 17, .y = sy + 2, .z = 13 },
				.ratios = { .x = 0, .y = 1 },
				.color = { .x = 1, .y = 1, .z = 1, .w = 1 }
			});
			renderer_push_circle(g->renderer, &(circle_instance) {
				.circ = { .x = (sx + (i * 5)) + 17, .y = sy + 17, .z = 13 },
				.ratios = { .x = 0, .y = 1 },
				.color = { .x = 1, .y = 1, .z = 1, .w = 1 }
			});
			renderer_push_circle(g->renderer, &(circle_instance) {
				.circ = { .x = (sx + (i * 5)) + 22, .y = sy + 5, .z = 8 },
				.ratios = { .x = 0, .y = 1 },
				.color = { .x = 0, .y = 0, .z = 0, .w = 1 }
			});
			renderer_push_circle(g->renderer, &(circle_instance) {
				.circ = { .x = (sx + (i * 5)) + 22, .y = sy + 19, .z = 8 },
				.ratios = { .x = 0, .y = 1 },
				.color = { .x = 0, .y = 0, .z = 0, .w = 1 }
			});
		}
	}

	igSetNextWindowPos((ImVec2) { .x = sx + 4, .y = sy + 40 }, ImGuiCond_None, (ImVec2) {});
	igSetNextWindowSize((ImVec2) { .x = 400, .y = 330 }, ImGuiCond_None);
	igBegin("setskin", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
	if (igButton("Previous", (ImVec2) { igGetWindowWidth() / 2 - style->WindowPadding.x, 0 })) {
		g->settings_instance.cv--;
		if (g->settings_instance.cv < 0)
			g->settings_instance.cv = 65;
	}
	igSameLine(0, style->FramePadding.x);
	if (igButton("Next", (ImVec2) { -1, 0 })) {
		g->settings_instance.cv++;
		if (g->settings_instance.cv > 65)
			g->settings_instance.cv = 0;
	}
	igCheckbox("Custom", &g->settings_instance.cusk);

	if (g->settings_instance.cusk) {
		igColumns(7, "color_groups", false);
		ImVec2 buttonSize = { -1, 30.0f };

		for (int i = 0; i < 7; i++) {
			for (int j = 0; j < 7; j++) {
				int idx = j * 7 + i;
				if (idx == 40) {
					igPushStyleColor_Vec4(ImGuiCol_Button, (ImVec4) { 1, 0.5f, 0.5f, 1 });
					igPushStyleColor_Vec4(ImGuiCol_ButtonHovered, (ImVec4) { 1, 0.5f, 0.5f, 0.5f }); // Lighten color when hovered
					igPushStyleColor_Vec4(ImGuiCol_ButtonActive, (ImVec4) { 1, 0.5f, 0.5f, 0.3f }); // Lighten color when active
					igPushStyleColor_Vec4(ImGuiCol_Text, (ImVec4) { 1, 1, 1, 1 });

					if (igButton("C", buttonSize)) {
						for (int i = 0; i < 256; i++) g->settings_instance.cusk_skin_data_exp[i] = -1;
						memset(g->settings_instance.cusk_skin_data, 0, 256);
						g->settings_instance.cusk_ptr = 0;
						g->settings_instance.exp_ptr = 0;
						g->settings_instance.added_first = 0;
					}

					igPopStyleColor(4); // Pop the style colors
				}
				if (idx > 39)
					continue;
				ig_vec3* col_at_idx = g->config.color_groups + idx;
				igPushID_Int(idx);

				igPushStyleColor_Vec4(ImGuiCol_Button, (ImVec4) { col_at_idx->x, col_at_idx->y, col_at_idx->z, 1 });
				igPushStyleColor_Vec4(ImGuiCol_ButtonHovered, (ImVec4) { col_at_idx->x, col_at_idx->y, col_at_idx->z, 0.5f }); // Lighten color when hovered
				igPushStyleColor_Vec4(ImGuiCol_ButtonActive, (ImVec4) { col_at_idx->x, col_at_idx->y, col_at_idx->z, 0.3f }); // Lighten color when active
				igPushStyleColor_Vec4(ImGuiCol_Text, (ImVec4) { 1 - col_at_idx->x, 1 - col_at_idx->y, 1 - col_at_idx->z, 1 });

				char btn_label[3] = {};
				sprintf(btn_label, "%d", idx);
				if (igButton(btn_label, buttonSize)) {
					if (g->settings_instance.exp_ptr < 76) {
						g->settings_instance.cusk_skin_data_exp[g->settings_instance.exp_ptr++] = idx;

						if (!g->settings_instance.added_first) {
						g->settings_instance.cusk_skin_data[g->settings_instance.cusk_ptr + 1] = idx;
						g->settings_instance.added_first = 1;
						}

						if (g->settings_instance.cusk_skin_data[g->settings_instance.cusk_ptr + 1] == idx) {
							g->settings_instance.cusk_skin_data[g->settings_instance.cusk_ptr]++;
						}
						else {
							g->settings_instance.cusk_ptr += 2;
							g->settings_instance.cusk_skin_data[g->settings_instance.cusk_ptr + 1] = idx;
							g->settings_instance.cusk_skin_data[g->settings_instance.cusk_ptr]++;
						}
					} else {}
				}
				igPopStyleColor(4); // Pop the style colors
				igPopID();
			}

			igNextColumn();
		}
		
		igColumns(1, "color_groups", true);
	}
	if (igButton("Save", (ImVec2) { -1, 0 })) {
		if (g->settings_instance.cusk_skin_data[0] == 0)
			g->settings_instance.cusk = false;
		
		save_settings(g);
		g->setting_skin = 0;
	}
	igEnd();
}