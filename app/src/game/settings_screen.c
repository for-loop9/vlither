#include "settings_screen.h"
#include "game.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "../external/cimgui/cimgui.h"
#include "../networking/util.h"

#define HOTKEY_INFO(key, description)\
igAlignTextToFramePadding();\
igPushFont(g->renderer->fonts[RENDERER_FONT_SMALL_BOLD]);\
igText(key); igSameLine(0, -1);\
igPopFont();\
igText(description)\

void settings_screen(game* g) {
	ImGuiStyle* style = igGetStyle();
	int sx = g->icontext->default_frame.resolution.x / 2 - 200;
	int win_height = 522;
	igSetNextWindowPos((ImVec2) { .x = 0, .y = 0 }, ImGuiCond_None, (ImVec2) {});
	igSetNextWindowSize((ImVec2) { .x = g->icontext->default_frame.resolution.x / 4, .y = g->icontext->default_frame.resolution.y }, ImGuiCond_None);
	igPushFont(g->renderer->fonts[RENDERER_FONT_SMALL]);
	igBegin("settings", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
	igSeparatorText("General");
	igColumns(2, "settings_clmn", 0);
	igAlignTextToFramePadding();
	igText("Quality");
	igAlignTextToFramePadding();
	igText("Background");
	igAlignTextToFramePadding();
	igText("VSync");
	igAlignTextToFramePadding();
	igText("FPS");
	igAlignTextToFramePadding();
	igText("Notify kills");
	igAlignTextToFramePadding();
	igText("Kill count");
	igAlignTextToFramePadding();
	igText("Zoom");
	igAlignTextToFramePadding();
	igText("Instant gameover");
	igNextColumn();
	
	igSetNextItemWidth(igGetColumnWidth(1) - style->WindowPadding.x - 5);
	if (igBeginCombo("##hq", g->settings_instance.hq ? "High" : "Low", ImGuiComboFlags_None)) {
		if (igSelectable_Bool("Low", !g->settings_instance.hq, ImGuiSelectableFlags_None, (ImVec2) { 0, 0 })) {
			g->settings_instance.hq = false;
		}
		if (igSelectable_Bool("High", g->settings_instance.hq, ImGuiSelectableFlags_None, (ImVec2) { 0, 0 })) {
			g->settings_instance.hq = true;
		}
		igEndCombo();
	}
	igSetNextItemWidth(igGetColumnWidth(1) - style->WindowPadding.x - 5);
	if (igBeginCombo("##background", g->settings_instance.black_bg ? "Black" : "Default", ImGuiComboFlags_None)) {
		if (igSelectable_Bool("Default", !g->settings_instance.black_bg, ImGuiSelectableFlags_None, (ImVec2) { 0, 0 })) {
			g->settings_instance.black_bg = false;
		}
		if (igSelectable_Bool("Black", g->settings_instance.black_bg, ImGuiSelectableFlags_None, (ImVec2) { 0, 0 })) {
			g->settings_instance.black_bg = true;
		}
		igEndCombo();
	}

	float cx = igGetCursorPosX();
	igSetCursorPosX(cx + (igGetColumnWidth(1) - style->WindowPadding.x - 5) - igGetFrameHeight());
	if (igCheckbox("##vsync", &g->settings_instance.vsync)) {
		g->window->resize_requested = true;
	}
	cx = igGetCursorPosX();
	igSetCursorPosX(cx + (igGetColumnWidth(1) - style->WindowPadding.x - 5) - igGetFrameHeight());
	igCheckbox("##fps", &g->settings_instance.show_fps);
	cx = igGetCursorPosX();
	igSetCursorPosX(cx + (igGetColumnWidth(1) - style->WindowPadding.x - 5) - igGetFrameHeight());
	igCheckbox("##notify_kills", &g->settings_instance.notify_kills);
	cx = igGetCursorPosX();
	igSetCursorPosX(cx + (igGetColumnWidth(1) - style->WindowPadding.x - 5) - igGetFrameHeight());
	igCheckbox("##kill_count", &g->settings_instance.show_kill);
	cx = igGetCursorPosX();
	igSetCursorPosX(cx + (igGetColumnWidth(1) - style->WindowPadding.x - 5) - igGetFrameHeight());
	if (igCheckbox("##zoom", &g->settings_instance.enable_zoom)) {
		if (!g->settings_instance.enable_zoom) {
			g->config.gsc = 0.9f * 18.0f / 14.0f;
		} else {
			g->config.gsc = g->config.zoom;
		}
	}
	cx = igGetCursorPosX();
	igSetCursorPosX(cx + (igGetColumnWidth(1) - style->WindowPadding.x - 5) - igGetFrameHeight());
	igCheckbox("##instant_gameover", &g->settings_instance.instant_gameover);

	igColumns(1, NULL, false);
	igSeparatorText("Player names");
	igColumns(2, NULL, false);
	igAlignTextToFramePadding();
	igText("Font");
	igAlignTextToFramePadding();
	igText("Show lengths");
	igAlignTextToFramePadding();
	igText("Color");
	igNextColumn();

	const char* font_size_strs[] = {
		"Large",
		"Medium",
		"Medium bold",
		"Small",
		"Small bold",
	};

	igSetNextItemWidth(igGetColumnWidth(1) - style->WindowPadding.x - 5);
	if (igBeginCombo("##names_font", font_size_strs[g->settings_instance.names_font], ImGuiComboFlags_None)) {
		if (igSelectable_Bool("Small", g->settings_instance.names_font == RENDERER_FONT_SMALL, ImGuiSelectableFlags_None, (ImVec2) { 0, 0 })) {
			g->settings_instance.names_font = RENDERER_FONT_SMALL;
		}
		if (igSelectable_Bool("Medium", g->settings_instance.names_font == RENDERER_FONT_MED, ImGuiSelectableFlags_None, (ImVec2) { 0, 0 })) {
			g->settings_instance.names_font = RENDERER_FONT_MED;
		}
		if (igSelectable_Bool("Large", g->settings_instance.names_font == RENDERER_FONT_BIG, ImGuiSelectableFlags_None, (ImVec2) { 0, 0 })) {
			g->settings_instance.names_font = RENDERER_FONT_BIG;
		}
		igEndCombo();
	}
	cx = igGetCursorPosX();
	igSetCursorPosX(cx + (igGetColumnWidth(1) - style->WindowPadding.x - 5) - igGetFrameHeight());
	igCheckbox("##show_lengths", &g->settings_instance.show_lengths);
	igSetNextItemWidth(igGetColumnWidth(1) - style->WindowPadding.x - 5);
	float names_color_ref[3] = { g->settings_instance.names_color.x, g->settings_instance.names_color.y, g->settings_instance.names_color.z };
	if (igColorEdit3("##names_color", names_color_ref, ImGuiColorEditFlags_None)) {
		g->settings_instance.names_color.x = names_color_ref[0];
		g->settings_instance.names_color.y = names_color_ref[1];
		g->settings_instance.names_color.z = names_color_ref[2];
	}
	igColumns(1, NULL, false);

	igSeparatorText("Assist");
	igColumns(2, NULL, false);
	igAlignTextToFramePadding();
	igText("Laser thickness");
	igAlignTextToFramePadding();
	igText("Laser color");
	igAlignTextToFramePadding();
	igText("Hurt point size");
	igAlignTextToFramePadding();
	igText("Hurt point color");
	igNextColumn();
	igSetNextItemWidth(igGetColumnWidth(1) - style->WindowPadding.x - 5);
	igSliderInt("##laser_thickness", &g->settings_instance.laser_thickness, 1, 6, NULL, ImGuiSliderFlags_None);
	igSetNextItemWidth(igGetColumnWidth(1) - style->WindowPadding.x - 5);
	float laser_color_ref[4] = { g->settings_instance.laser_color.x, g->settings_instance.laser_color.y, g->settings_instance.laser_color.z, g->settings_instance.laser_color.w };
	if (igColorEdit4("##laser_color", laser_color_ref, ImGuiColorEditFlags_None)) {
		g->settings_instance.laser_color.x = laser_color_ref[0];
		g->settings_instance.laser_color.y = laser_color_ref[1];
		g->settings_instance.laser_color.z = laser_color_ref[2];
		g->settings_instance.laser_color.w = laser_color_ref[3];
	}
	igSetNextItemWidth(igGetColumnWidth(1) - style->WindowPadding.x - 5);
	igSliderInt("##hp_thickness", &g->settings_instance.hp_size, 1, 6, NULL, ImGuiSliderFlags_None);
	igSetNextItemWidth(igGetColumnWidth(1) - style->WindowPadding.x - 5);
	float hp_color_ref[3] = { g->settings_instance.hp_color.x, g->settings_instance.hp_color.y, g->settings_instance.hp_color.z };
	if (igColorEdit3("##hp_color", hp_color_ref, ImGuiColorEditFlags_None)) {
		g->settings_instance.hp_color.x = hp_color_ref[0];
		g->settings_instance.hp_color.y = hp_color_ref[1];
		g->settings_instance.hp_color.z = hp_color_ref[2];
	}
	igColumns(1, NULL, false);
	igSeparatorText("Food");
	igColumns(2, NULL, false);
	igAlignTextToFramePadding();
	igText("Food scale");
	igNextColumn();
	igSetNextItemWidth(igGetColumnWidth(1) - style->WindowPadding.x - 5);
	igSliderFloat("##food_scale", &g->settings_instance.food_scale, 0.5f, 2, "%.2f", ImGuiSliderFlags_None);
	igColumns(1, NULL, false);
	igSeparatorText("Minimap");
	igColumns(2, NULL, false);
	igAlignTextToFramePadding();
	igText("Label");
	igAlignTextToFramePadding();
	igText("Scale");
	igNextColumn();
	igSetNextItemWidth(igGetColumnWidth(1) - style->WindowPadding.x - 5);
	if (igBeginCombo("##minimap", g->settings_instance.clk ? "Clock" : "Compass", ImGuiComboFlags_None)) {
		if (igSelectable_Bool("Compass", !g->settings_instance.clk, ImGuiSelectableFlags_None, (ImVec2) { 0, 0 })) {
			g->settings_instance.clk = false;
		}
		if (igSelectable_Bool("Clock", g->settings_instance.clk, ImGuiSelectableFlags_None, (ImVec2) { 0, 0 })) {
			g->settings_instance.clk = true;
		}
		igEndCombo();
	}
	igSetNextItemWidth(igGetColumnWidth(1) - style->WindowPadding.x - 5);
	igSliderFloat("##mm_scale", &g->settings_instance.mm_scale, 1, 3, "%.2f", ImGuiSliderFlags_None);
	igEnd();

	igSetNextWindowPos((ImVec2) { .x = g->icontext->default_frame.resolution.x / 4, .y = 0 }, ImGuiCond_None, (ImVec2) {});
	igSetNextWindowSize((ImVec2) { .x = g->icontext->default_frame.resolution.x / 4, .y = g->icontext->default_frame.resolution.y }, ImGuiCond_None);
	igBegin("hotkeys", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
	igSeparatorText("Hotkeys");
	
	HOTKEY_INFO("(M)", "Zoom out");
	HOTKEY_INFO("(N)", "Zoom in");
	HOTKEY_INFO("(K)", "Toggle assist");
	HOTKEY_INFO("(F11)", "Toggle fullscreen");
	HOTKEY_INFO("(B)", "Toggle big food");
	HOTKEY_INFO("(S)", "Toggle shadows");
	HOTKEY_INFO("(H)", "Toggle HUD");
	HOTKEY_INFO("(G)", "Toggle boost effect");
	HOTKEY_INFO("(P)", "Toggle player names");
	HOTKEY_INFO("(0)", "Respawn");
	HOTKEY_INFO("(9)", "Quit");
	
	igEnd();

	igSetNextWindowPos((ImVec2) { .x = g->icontext->default_frame.resolution.x - 150, .y = g->icontext->default_frame.resolution.y - 52 }, ImGuiCond_None, (ImVec2) {});
	igSetNextWindowSize((ImVec2) { .x = 150, .y = 52 }, ImGuiCond_None);
	igBegin("dd", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
	if (igButton("Save", (ImVec2) { -1, -1 })) {
		save_settings(g);
		g->show_settings = false;
	}
	igEnd();
	igPopFont();
}