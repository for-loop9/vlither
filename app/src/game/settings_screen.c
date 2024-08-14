#include "settings_screen.h"
#include "game.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "../external/cimgui/cimgui.h"
#include "../networking/util.h"

void settings_screen(game* g) {
	ImGuiStyle* style = igGetStyle();
	int sx = g->icontext->default_frame.resolution.x / 2 - 200;
	int win_height = 522;
	igSetNextWindowPos((ImVec2) { .x = sx, .y = g->icontext->default_frame.resolution.y / 2 - win_height / 2 }, ImGuiCond_None, (ImVec2) {});
	igSetNextWindowSize((ImVec2) { .x = 400, .y = win_height }, ImGuiCond_None);
	igPushFont(g->renderer->fonts[RENDERER_FONT_MED]);
	igBegin("settings", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
	igColumns(2, "settings_clmn", 0);
	igAlignTextToFramePadding();
	igText("Quality");
	igAlignTextToFramePadding();
	igText("Minimap");
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
	igText("Fullscreen");
	igAlignTextToFramePadding();
	igText("Player lengths");
	igAlignTextToFramePadding();
	igText("Instant gameover");
	igNextColumn();

	igSetNextItemWidth(igGetColumnWidth(1) - style->WindowPadding.x - 5);
	if (igBeginCombo("##quality", g->settings_instance.hq ? "High" : "Low", ImGuiComboFlags_None)) {
		if (igSelectable_Bool("High", g->settings_instance.hq, ImGuiSelectableFlags_None, (ImVec2) { 0, 0 })) {
			g->settings_instance.hq = true;
			g->config.qsm = 1.0f;
		}
		if (igSelectable_Bool("Low", !g->settings_instance.hq, ImGuiSelectableFlags_None, (ImVec2) { 0, 0 })) {
			g->settings_instance.hq = false;
			g->config.qsm = 1.7f;
		}
		igEndCombo();
	}
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
		message_queue_push(&g->msg_queue, (&(message) {
			.message = "Restart program to apply changes.",
			.tt = 2,
			.color = { .x = 1, .y = 0.5f, .z = 0.3f, .w = 1 }
		}));
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
		if (!&g->settings_instance.enable_zoom) {
			g->config.gsc = 0.9f * 18.0f / 14.0f;
		} else {
			g->config.gsc = g->config.zoom;
		}
	}
	cx = igGetCursorPosX();
	igSetCursorPosX(cx + (igGetColumnWidth(1) - style->WindowPadding.x - 5) - igGetFrameHeight());
	if (igCheckbox("##fullscreen", &g->settings_instance.fullscreen)) {
		message_queue_push(&g->msg_queue, (&(message) {
			.message = "Restart program to apply changes.",
			.tt = 2,
			.color = { .x = 1, .y = 0.5f, .z = 0.3f, .w = 1 }
		}));
	}
	cx = igGetCursorPosX();
	igSetCursorPosX(cx + (igGetColumnWidth(1) - style->WindowPadding.x - 5) - igGetFrameHeight());
	igCheckbox("##show_lengths", &g->settings_instance.show_lengths);
	cx = igGetCursorPosX();
	igSetCursorPosX(cx + (igGetColumnWidth(1) - style->WindowPadding.x - 5) - igGetFrameHeight());
	igCheckbox("##instant_gameover", &g->settings_instance.instant_gameover);

	igColumns(1, NULL, false);
	igSeparatorText("Laser");
	igColumns(2, NULL, false);
	igAlignTextToFramePadding();
	igText("Length");
	igAlignTextToFramePadding();
	igText("Thickness");
	igAlignTextToFramePadding();
	igText("Color");
	igNextColumn();
	igSetNextItemWidth(igGetColumnWidth(1) - style->WindowPadding.x - 5);
	igSliderInt("##laser_length", &g->settings_instance.laser_length, 50, 300, NULL, ImGuiSliderFlags_None);
	igSetNextItemWidth(igGetColumnWidth(1) - style->WindowPadding.x - 5);
	igSliderInt("##laser_thickness", &g->settings_instance.laser_thickness, 1, 6, NULL, ImGuiSliderFlags_None);
	igSetNextItemWidth(igGetColumnWidth(1) - style->WindowPadding.x - 5);
	float laser_color_ref[3] = { g->settings_instance.laser_color.x, g->settings_instance.laser_color.y, g->settings_instance.laser_color.z };
	if (igColorEdit3("##laser_color", laser_color_ref, ImGuiColorEditFlags_None)) {
		g->settings_instance.laser_color.x = laser_color_ref[0];
		g->settings_instance.laser_color.y = laser_color_ref[1];
		g->settings_instance.laser_color.z = laser_color_ref[2];
	}
	igColumns(1, NULL, false);
	igEnd();

	igSetNextWindowPos((ImVec2) { .x = sx, .y = (g->icontext->default_frame.resolution.y / 2 - win_height / 2) + win_height }, ImGuiCond_None, (ImVec2) {});
	igSetNextWindowSize((ImVec2) { .x = 400, .y = 200 }, ImGuiCond_None);
	igBegin("dd", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
	if (igButton("Save", (ImVec2) { -1, 0 })) {
		save_settings(g);
		g->show_settings = false;
	}
	igEnd();
	igPopFont();
}