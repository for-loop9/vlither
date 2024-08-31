#include "game.h"
#include "../networking/callback.h"
#include "../networking/util.h"
#include "prey.h"
#include "title_screen.h"
#include "external/stb/stb_image.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "../external/cimgui/cimgui.h"

bool igSpinner(const char* label, float radius, int thickness, float time, ImU32 color) {
	ImGuiWindow* window = igGetCurrentWindow();
	if (window->SkipItems)
		return false;
	
	ImGuiStyle* style = igGetStyle();
	ImGuiContext* ctx = igGetCurrentContext();
	const ImGuiID id = ImGuiWindow_GetID_Str(window, label, NULL);
	
	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = { (radius) * 2, (radius + style->FramePadding.y) * 2 };
	
	ImRect bb = { pos, (ImVec2) { pos.x + size.x, pos.y + size.y }};
	igItemSize_Rect(bb, style->FramePadding.y);
	if (!igItemAdd(bb, id, NULL, ImGuiItemFlags_None))
		return false;
	
	ImDrawList_PathClear(window->DrawList);
	
	int num_segments = 30;
	int start = abs(sinf(time * 1.8f) * (num_segments - 5));
	
	float a_min = PI * 2.0f * ((float) start)  / (float) num_segments;
	float a_max = PI * 2.0f * ((float) num_segments - 3) / (float) num_segments;

	ImVec2 centre = { pos.x + radius, pos.y + radius + style->FramePadding.y };
	
	for (int i = 0; i < num_segments; i++) {
		const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
		ImDrawList_PathLineTo(window->DrawList, (ImVec2) { centre.x + cosf(a + time * 8) * radius, centre.y + sinf(a + time * 8) * radius });
	}

	ImDrawList_PathStroke(window->DrawList, color, ImDrawFlags_None, thickness);
}

float luminance(ig_vec3 color) {
	float r = (color.x <= 0.03928f) ? color.x / 12.92f : powf((color.x + 0.055f) / 1.055f, 2.4f);
	float g = (color.y <= 0.03928f) ? color.y / 12.92f : powf((color.y + 0.055f) / 1.055f, 2.4f);
	float b = (color.z <= 0.03928f) ? color.z / 12.92f : powf((color.z + 0.055f) / 1.055f, 2.4f);

	return 0.2126f * r + 0.7152f * g + 0.0722f * b;
}

float contrast_ratio(ig_vec3 color1, ig_vec3 color2) {
	float L1 = luminance(color1);
	float L2 = luminance(color2);
	if (L1 < L2) {
		float temp = L1;
		L1 = L2;
		L2 = temp;
	}
	return (L1 + 0.05f) / (L2 + 0.05f);
}

ig_vec3 get_cg_opp_color(ig_vec3 cg_color) {
	ig_vec3 black = { 0.0f, 0.0f, 0.0f };
	ig_vec3 white = { 1.0f, 1.0f, 1.0f };

	float contrast_with_black = contrast_ratio(cg_color, black);
	float contrast_with_white = contrast_ratio(cg_color, white);

	if (contrast_with_black > contrast_with_white) {
		return black;
	}
	else {
		return white;
	}
}

void* network_worker(void* args) {
	game* g = (game*) args;
	struct mg_mgr mgr;
	struct mg_connection* c;
	mg_mgr_init(&mgr);
	g->mgr_ptr = &mgr;
	mg_log_set(MG_LL_NONE);
	char url[256] = {};
	sprintf(url, "ws://%s/slither", g->settings_instance.ip);
	c = mg_ws_connect(&mgr, url, client_callback, g,
		"%s:%s\r\n%s:%s\r\n%s:%s\r\n%s:%s\r\n%s:%s\r\n%s:%s\r\n",
		"Accept-Encoding", "gzip, deflate",
		"Accept-Language", "en-US,en;q=0.5",
		"Cache-Control", "no-cache",
		"DNT", "1",
		"Origin", "https://slither.com",
		"Pragma", "no-cache");
	mg_wakeup_init(&mgr);
	while (c && !g->network_done) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);
	return NULL;
}

void* load_worker(void* args) {
	game* g = (game*) args;

	pthread_mutex_lock(&g->connecting_mutex);
	while (!g->connected && g->should_load) {
		pthread_cond_wait(&g->connecting_cond, &g->connecting_mutex);
	}
	pthread_mutex_unlock(&g->connecting_mutex);
	return NULL;
}

void create_game(int argc, char** argv) {
	// return;

	srand(time(NULL));
	game g = {
		.game_quit = 0,
		.network_done = 0,
		.os = snake_map_create(),
		.foods = ig_darray_create(food),
		.preys = ig_darray_create(prey),
		.leaderboard = {
			[0] = { .nk = "LOADING...", .cv = 11 },
			[1] = { .nk = "LOADING...", .cv = 11 },
			[2] = { .nk = "LOADING...", .cv = 11 },
			[3] = { .nk = "LOADING...", .cv = 11 },
			[4] = { .nk = "LOADING...", .cv = 11 },
			[5] = { .nk = "LOADING...", .cv = 11 },
			[6] = { .nk = "LOADING...", .cv = 11 },
			[7] = { .nk = "LOADING...", .cv = 11 },
			[8] = { .nk = "LOADING...", .cv = 11 },
			[9] = { .nk = "LOADING...", .cv = 11 },
		},
		.connected = 0,
		.snake_null = 1,
		.should_load = 1,
		.msg_queue = message_queue_create(),
		.settings_instance = {
			.show_fps = false,
			.notify_kills = false,
			.show_kill = true,
			.clk = true,
			.vsync = true,
			.enable_zoom = 1,
			.cv = rand() % 9,
			.cusk = 0,
			.hq = false,
			.nickname = "",
			.ip = "15.204.212.200:444",
			.laser_thickness = 1,
			.hp_size = 2,
			.laser_color = { .x = 0, .y = 1, .z = 0, .w = 0.5f },
			.names_color = { .x = 0.9f, .y = 0.9f, .z = 0.9f },
			.hp_color = { .x = 1, .y = 1, .z =1 },
			.food_scale = 1,
			.mm_scale = 1,
			.names_font = RENDERER_FONT_SMALL
		},
		.config = {
			.player_names = true,
			.shadow = true,
			.show_hud = true,

			.grd = -1,
			.mscps = -1,
			.ssd256 = -1,
			.spangdv = -1,
			.nsp1 = -1,
			.nsp2 = -1,
			.nsp3 = -1,
			.mamu = -1,
			.mamu2 = -1,
			.cst = -1,
			.qsm = 1.7f, // 1 = high quality, 1.7 = low quality
			.fmlts = ig_darray_create(float),
			.fpsls = ig_darray_create(float),
			.lfsx = -1,
			.lfsy = -1,
			.lfvsx = -1,
			.lfvsy = -1,
			.lfcv = -1,
			.zoom = 0.9f * 18.0f / 14.0f,
			.gsc = 0.9f * 18.0f / 14.0f,
			.lag_mult = 1,

			.color_groups = {
				[0] = { .x = 0.75, .y = 0.50, .z = 1.00 },
				[1] = { .x = 0.56, .y = 0.60, .z = 1.00 },
				[2] = { .x = 0.50, .y = 0.82, .z = 0.82 },
				[3] = { .x = 0.50, .y = 1.00, .z = 0.50 },
				[4] = { .x = 0.93, .y = 0.93, .z = 0.44 },
				[5] = { .x = 1.00, .y = 0.63, .z = 0.38 },
				[6] = { .x = 1.00, .y = 0.56, .z = 0.56 },
				[7] = { .x = 1.00, .y = 0.25, .z = 0.25 },
				[8] = { .x = 0.88, .y = 0.19, .z = 0.88 },
				[9] = { .x = 1.00, .y = 1.00, .z = 1.00 },
				[10] = { .x = 0.56, .y = 0.60, .z = 1.00 },
				[11] = { .x = 0.31, .y = 0.31, .z = 0.31 },
				[12] = { .x = 1.00, .y = 0.75, .z = 0.31 },
				[13] = { .x = 0.16, .y = 0.53, .z = 0.38 },
				[14] = { .x = 0.39, .y = 0.46, .z = 1.00 },
				[15] = { .x = 0.47, .y = 0.53, .z = 1.00 },
				[16] = { .x = 0.28, .y = 0.33, .z = 1.00 },
				[17] = { .x = 0.63, .y = 0.31, .z = 1.00 },
				[18] = { .x = 1.00, .y = 0.88, .z = 0.25 },
				[19] = { .x = 0.22, .y = 0.27, .z = 1.00 },
				[20] = { .x = 0.22, .y = 0.27, .z = 1.00 },
				[21] = { .x = 0.31, .y = 0.14, .z = 0.75 },
				[22] = { .x = 1.00, .y = 0.34, .z = 0.04 },
				[23] = { .x = 0.40, .y = 0.78, .z = 0.91 },
				[24] = { .x = 0.50, .y = 0.52, .z = 0.56 },
				[25] = { .x = 0.24, .y = 0.75, .z = 0.28 },
				[26] = { .x = 0.00, .y = 1.00, .z = 0.33 },
				[27] = { .x = 0.85, .y = 0.27, .z = 0.27 },
				[28] = { .x = 1.00, .y = 0.25, .z = 0.25 },
				[29] = { .x = 0.56, .y = 0.56, .z = 0.56 },
				[30] = { .x = 0.13, .y = 0.13, .z = 0.94 },
				[31] = { .x = 0.94, .y = 0.13, .z = 0.13 },
				[32] = { .x = 0.94, .y = 0.94, .z = 0.13 },
				[33] = { .x = 0.94, .y = 0.56, .z = 0.13 },
				[34] = { .x = 0.94, .y = 0.13, .z = 0.94 },
				[35] = { .x = 0.13, .y = 0.94, .z = 0.13 },
				[36] = { .x = 0.16, .y = 0.24, .z = 0.68 }, // britisher color group
				[37] = { .x = 0.41, .y = 0.50, .z = 1.00 },
				[38] = { .x = 0.00, .y = 0.00, .z = 0.44 },
				[39] = { .x = 0.41, .y = 0.16, .z = 0.67 }
			},

			.default_skins = {
				[0] = { 1, 0 },
				[1] = { 1, 1 },
				[2] = { 1, 2 },
				[3] = { 1, 3 },
				[4] = { 1, 4 },
				[5] = { 1, 5 },
				[6] = { 1, 6 },
				[7] = { 1, 7 },
				[8] = { 1, 8 },
				[9] = { 20, 7, 9, 7, 9, 7, 9, 7, 9, 7, 9, 7, 10, 10, 10, 10, 10, 10, 10, 10, 10 },
				[10] = { 15, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 7, 7, 7, 7, 7 },
				[11] = { 15, 11, 11, 11, 11, 11, 7, 7, 7, 7, 7, 12, 12, 12, 12, 12 },
				[12] = { 15, 7, 7, 7, 7, 7, 9, 9, 9, 9, 9, 13, 13, 13, 13, 13 },
				[13] = { 15, 14, 14, 14, 14, 14, 9, 9, 9, 9, 9, 7, 7, 7, 7, 7 },
				[14] = { 14, 9, 9, 9, 9, 9, 9, 9, 7, 7, 7, 7, 7, 7, 7 },
				[15] = { 9, 0, 1, 2, 3, 4, 5, 6, 7, 8 },
				[16] = { 14, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4 },
				[17] = { 14, 9, 9, 9, 9, 9, 9, 9, 16, 16, 16, 16, 16, 16, 16 },
				[18] = { 14, 7, 7, 7, 7, 7, 7, 7, 9, 9, 9, 9, 9, 9, 9 },
				[19] = { 1, 9 },
				[20] = { 10, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0 },
				[21] = { 26, 3, 3, 3, 3, 3, 3, 3, 18, 18, 18, 18, 18, 18, 20, 19, 20, 19, 20, 19, 20, 18, 18, 18, 18, 18, 18 },
				[22] = { 21, 5, 5, 5, 5, 5, 5, 5, 9, 9, 9, 9, 9, 9, 9, 13, 13, 13, 13, 13, 13, 13 },
				[23] = { 21, 16, 16, 16, 16, 16, 16, 16, 18, 18, 18, 18, 18, 18, 18, 7, 7, 7, 7, 7, 7, 7 },
				[24] = { 18, 23, 23, 23, 23, 23, 23, 23, 23, 23, 18, 18, 18, 18, 18, 18, 18, 18, 18 },
				[25] = { 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22 },
				[26] = { 1, 24 },
				[27] = { 1, 25 },
				[28] = { 21, 18, 18, 18, 18, 18, 18, 18, 25, 25, 25, 25, 25, 25, 25, 7, 7, 7, 7, 7, 7, 7 },
				[29] = { 10, 11, 11, 4, 11, 11, 11, 11, 4, 11, 11 },
				[30] = { 8, 10, 10, 19, 20, 10, 10, 20, 19 },
				[31] = { 2, 10, 10 },
				[32] = { 2, 20, 20 },
				[33] = { 3, 12, 11, 11 },
				[34] = { 18, 7, 7, 9, 13, 13, 9, 16, 16, 9, 12, 12, 9, 7, 7, 9, 16, 16, 9 },
				[35] = { 8, 7, 7, 9, 9, 6, 6, 9, 9 },
				[36] = { 8, 16, 16, 9, 9, 15, 15, 9, 9 },
				[37] = { 1, 22 },
				[38] = { 1, 18 },
				[39] = { 1, 23 },
				[40] = { 1, 26 },
				[41] = { 1, 27 },
				[42] = { 32, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 7 },
				[43] = { 1, 28 },
				[44] = { 1, 29 },
				[45] = { 14, 7, 7, 7, 9, 9, 9, 9, 9, 9, 9, 9, 7, 7, 7 },
				[46] = { 1, 7 },
				[47] = { 24, 16, 16, 16, 18, 18, 18, 18, 18, 18, 18, 18, 18, 7, 7, 7, 7, 7, 7, 7, 7, 16, 16, 16, 16 },
				[48] = { 1, 7 },
				[49] = { 15, 23, 23, 23, 23, 23, 9, 9, 9, 9, 9, 9, 9, 9, 23, 23 },
				[50] = { 30, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 16, 16, 16, 16, 16, 16, 16, 16, 7, 7, 7, 7, 7, 7, 7, 7 },
				[51] = { 13, 7, 7, 7, 9, 9, 16, 16, 16, 16, 16, 16, 9, 9 },
				[52] = { 18, 7, 7, 7, 7, 18, 18, 18, 18, 18, 18, 18, 18, 18, 7, 7, 7, 7, 7 },
				[53] = { 1, 30 },
				[54] = { 1, 31 },
				[55] = { 1, 32 },
				[56] = { 1, 33 },
				[57] = { 1, 34 },
				[58] = { 1, 35 },
				[59] = { 1, 18 },
				[60] = { 1, 36 },
				[61] = { 36, 30, 30, 30, 30, 30, 30, 35, 35, 35, 35, 35, 35, 33, 33, 33, 33, 33, 33, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 34, 34, 34, 34, 34, 34 },
				[62] = { 10, 17, 17, 17, 17, 17, 39, 39, 39, 39, 39 },
				[63] = { 6, 7, 7, 7, 11, 11, 11 },
				[64] = { 4, 16, 16, 11, 11 },
				[65] = { 8, 4, 4, 4, 4, 9, 9, 9, 9 },
			},

			.ntl_cg_map = {
				[0] = 'z',
				[1] = 'x',
				[2] = 'c',
				[3] = 'v',
				[4] = 'b',
				[5] = 'n',
				[6] = 'm',
				[7] = ',',
				[8] = 'a',
				[9] = 's',
				[10] = 'd',
				[11] = 'f',
				[12] = 'g',
				[13] = 'h',
				[14] = 'j',
				[15] = 'k',
				[16] = 'l',
				[17] = 'q',
				[18] = 'w',
				[19] = 'e',
				[20] = 'r',
				[21] = 't',
				[22] = 'y',
				[23] = 'u',
				[24] = 'i',
				[25] = 'o',
				[26] = 'p',
				[27] = '1',
				[28] = '2',
				[29] = '3',
				[30] = '4',
				[31] = '5',
				[32] = '6',
				[33] = '7',
				[34] = '8',
				[35] = '9',
				[36] = '*', // britisher
				[37] = '0',
				[38] = '*', // idk
				[39] = '-'
			}
		}
	};

	for (int i = 0; i < 40; i++) {
		g.config.color_groups_opp[i] = get_cg_opp_color(g.config.color_groups[i]);
	}

	for (int i = 0; i < 256; i++) g.settings_instance.cusk_skin_data_exp[i] = -1;
	g.settings_instance.exp_ptr = 0;

	load_settings(&g);

	pthread_mutex_init(&g.render_mutex, NULL);
	pthread_cond_init(&g.render_cond, NULL);
	pthread_mutex_init(&g.connecting_mutex, NULL);
	pthread_cond_init(&g.connecting_cond, NULL);
	pthread_mutex_init(&g.msg_mutex, NULL);

	for (int i = 0; i < LFC; i++) {
		g.config.lfas[i] = 0.5f * (1 - cosf(PI * (LFC - 1 - i) / (LFC - 1)));
	}
	for (int i = 0; i < RFC; i++) {
		g.config.rfas[i] = 0.5f * (1 - cosf(PI * (RFC - 1 - i) / (RFC - 1)));
	}
	for (int i = 0; i < HFC; i++) {
		g.config.hfas[i] = 0.5f * (1 - cosf(PI * (HFC - 1 - i) / (HFC - 1)));
	}
	for (int i = 0; i < AFC; i++) {
		g.config.afas[i] = 0.5f * (1 - cosf(PI * (AFC - 1 - i) / (AFC - 1)));
	}
	for (int i = 0; i < VFC; i++) {
		float j = 0.5 * (1 - cosf(PI * (VFC - 1 - i) / (VFC - 1)));
		j += (.5 * (1 - cosf(PI * j)) - j) * .5;
		g.config.vfas[i] = j;
	}
	
	int i = 0;
	for (float j = 2.8f; j <= 18.8f; j += 1) {
		float sz = ceilf(j * 2.5f + 28);
		g.config.fs[i] = sz;
		g.config.f2s[i] = sz / 2;
		i++;
	}

	i = 0;
	for (float j = 3; j <= 24; j += 1) {
		float sz = ceilf(j * 2 + 38);
		g.config.pr_fs[i] = sz;
		g.config.pr_f2s[i] = sz / 2;
		i++;
	}

	{
		GLFWimage icons[3];
		int channels;

		icons[0].pixels = stbi_load("app/res/textures/icon_s.png", &icons[0].width, &icons[0].height, &channels, 4);
		icons[1].pixels = stbi_load("app/res/textures/icon_m.png", &icons[1].width, &icons[1].height, &channels, 4);
		icons[2].pixels = stbi_load("app/res/textures/icon_l.png", &icons[2].width, &icons[2].height, &channels, 4);
		
		if (g.settings_instance.fullscreen) {
			g.window = ig_window_create(&(ig_ivec2) { .x = 1280, .y = 832 }, "vlither", 1, 0, icons, 3);
		} else {
			if (g.settings_instance.window_size.x == 0 || g.settings_instance.window_size.y == 0) {
				g.window = ig_window_create_asp(16.0f / 9.0f, "vlither", icons, 3);
			} else {
				g.window = ig_window_create(&g.settings_instance.window_size, "vlither", 0, 0, icons, 3);
			}
		}

		stbi_image_free(icons[0].pixels);
		stbi_image_free(icons[1].pixels);
		stbi_image_free(icons[2].pixels);
	}

	g.window->last_dim.x = g.settings_instance.window_size.x;
	g.window->last_dim.y = g.settings_instance.window_size.y;

	g.keyboard = ig_keyboard_create(g.window);
	g.mouse = ig_mouse_create(g.window);
	g.icontext = ig_context_create(g.window, 1, g.settings_instance.vsync);

	ig_texture* sprite_sheet = ig_context_texture_create_from_file(g.icontext, "app/res/textures/sprite_sheet.png");
	ig_texture* font_sheet = ig_context_texture_create_from_file(g.icontext, "app/res/textures/font_sheet.png");
	g.bg_tex = ig_context_texture_create_from_file(g.icontext, "app/res/textures/background.jpg");
	ig_texture* logo_tex = ig_context_texture_create_from_file(g.icontext, "app/res/textures/logo.png");

	g.renderer = renderer_create(&g, g.icontext, g.window, sprite_sheet, MAX_BP_RENDER, MAX_FOOD_RENDER, 128, font_sheet, g.bg_tex, 64);
	g.logo_tex_ptr = igImplVulkan_AddTexture(g.icontext->nearest_sampler, logo_tex->view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	input_data input_data = {};

	g.config.ltm = glfwGetTime() * 1000;
	int fps_display;
	int fps = 0;
	float dt = 0;
	float pdt = glfwGetTime();
	float et = 0;
	float time_out = 4.0f;
	float load_et = 0.0f;

	while (!ig_window_closed(g.window) && !g.game_quit) {
		ig_window_input(g.window);
		renderer_start_imgui_frame(g.renderer);

		input_data.mouse_pos = g.mouse->pos;
		input_data.mouse_delta = g.mouse->delta;
		input_data.mouse_dwheel = g.mouse->dwheel;
		input_data.btn_down = ig_window_mouse_button_down(g.window, GLFW_MOUSE_BUTTON_LEFT) || ig_window_keyboard_key_down(g.window, GLFW_KEY_SPACE);
		input_data.k_pressed = ig_keyboard_key_pressed(g.keyboard, GLFW_KEY_K);
		input_data.m_pressed = ig_keyboard_key_pressed(g.keyboard, GLFW_KEY_M);
		input_data.n_pressed = ig_keyboard_key_pressed(g.keyboard, GLFW_KEY_N);
		input_data.b_pressed = ig_keyboard_key_pressed(g.keyboard, GLFW_KEY_B);
		input_data.p_pressed = ig_keyboard_key_pressed(g.keyboard, GLFW_KEY_P);
		input_data.s_pressed = ig_keyboard_key_pressed(g.keyboard, GLFW_KEY_S);
		input_data.h_pressed = ig_keyboard_key_pressed(g.keyboard, GLFW_KEY_H);
		input_data.nine_pressed = ig_keyboard_key_pressed(g.keyboard, GLFW_KEY_9);
		input_data.zero_pressed = ig_keyboard_key_pressed(g.keyboard, GLFW_KEY_0);
		bool fullscreen_toggle = ig_keyboard_key_pressed(g.keyboard, GLFW_KEY_F11);
		input_data.ctm = glfwGetTime() * 1000;
		float ct = glfwGetTime();
		dt = ct - pdt;
		pdt = ct;
		et += dt;

		if (et >= 1.0f) {
			et = 0;
			fps_display = fps;
			input_data.fps_display = fps_display;
			fps = 0;
		}

		ig_context_begin(g.icontext);
		pthread_mutex_lock(&g.connecting_mutex);
		pthread_mutex_lock(&g.msg_mutex);
		message_queue_render(&g.msg_queue, &g, dt);
		pthread_mutex_unlock(&g.msg_mutex);

		if (g.connected) {
			if (g.connecting) {
				pthread_mutex_lock(&g.msg_mutex);
				message_queue_push(&g.msg_queue, (&(message) {
					.message = "Connected",
					.tt = 1,
					.color = { .x = 0.5f, .y = 1, .z = 0.5f, .w = 1 }
				}));
				pthread_mutex_unlock(&g.msg_mutex);
				g.config.ltm = input_data.ctm;
				g.connecting = 0;
			}

			g.woke_up = false;
			mg_wakeup(g.mgr_ptr, 1, &input_data, sizeof(input_data));
			pthread_mutex_lock(&g.render_mutex);
			while (!g.frame_write) {
				pthread_cond_wait(&g.render_cond, &g.render_mutex);
			}
			renderer_flush(g.renderer);
			g.frame_write = 0;
			pthread_mutex_unlock(&g.render_mutex);
		} else if (g.connecting) {
			igSetNextWindowPos((ImVec2) { .x = g.icontext->default_frame.resolution.x / 2 - 40, .y = g.icontext->default_frame.resolution.y / 2 - 40 }, ImGuiCond_None, (ImVec2) {});
			igSetNextWindowSize((ImVec2) { .x = 80, .y = 80 }, ImGuiCond_None);
			igBegin("loading_scr", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
			igSpinner("##loader", 26, 6, glfwGetTime(), igColorConvertFloat4ToU32((ImVec4) { .x = 0, .y = 0.8f, .z = 0.5f, .w = 1 }));
			igEnd();
			renderer_flush(g.renderer);

			if (load_et >= time_out) {
				load_et = 0;
				pthread_mutex_lock(&g.msg_mutex);
				message_queue_push(&g.msg_queue, (&(message) {
					.message = "Connection timed out",
					.tt = 2,
					.color = { .x = 1, .y = 0.3f, .z = 0.3f, .w = 1 }
				}));
				pthread_mutex_unlock(&g.msg_mutex);
				g.connecting = 0;
				g.should_load = 0;
				g.network_done = 1;
				pthread_cond_signal(&g.connecting_cond);
			}
			load_et += dt;
		} else {			
			title_screen(&g);
		}
		pthread_mutex_unlock(&g.connecting_mutex);

		ig_keyboard_update(g.keyboard);
		ig_mouse_update(g.mouse);
		ig_context_end(g.icontext);

		if (fullscreen_toggle) {
			g.settings_instance.fullscreen = !g.settings_instance.fullscreen;
			ig_window_set_fullscreen(g.window, g.settings_instance.fullscreen);
		}

		if (g.window->resize_requested) {
			g.window->resize_requested = false;
			g.icontext->frame_idx = (g.icontext->frame_idx - 1 + g.icontext->fif) % g.icontext->fif;
			ig_context_resize(g.icontext, g.window, 1, g.settings_instance.vsync);
			g.icontext->frame_idx = (g.icontext->frame_idx + 1) % g.icontext->fif;
			g.settings_instance.window_size.x = g.window->last_dim.x;
			g.settings_instance.window_size.y = g.window->last_dim.y;
		}

		fps++;
	}

	printf("window exit\n");
	g.network_done = 1;

	pthread_join(g.load_thread, NULL);
	pthread_join(g.network_thread, NULL);

	pthread_mutex_destroy(&g.msg_mutex);
	pthread_cond_destroy(&g.connecting_cond);
	pthread_mutex_destroy(&g.connecting_mutex);
	pthread_cond_destroy(&g.render_cond);
	pthread_mutex_destroy(&g.render_mutex);

	ig_context_finish(g.icontext);

	renderer_destroy(g.renderer);
	ig_context_texture_destroy(g.icontext, logo_tex);
	ig_context_texture_destroy(g.icontext, g.bg_tex);
	ig_context_texture_destroy(g.icontext, font_sheet);
	ig_context_texture_destroy(g.icontext, sprite_sheet);
	message_queue_destroy(&g.msg_queue);

	ig_context_destroy(g.icontext);
	ig_mouse_destroy(g.mouse);
	ig_keyboard_destroy(g.keyboard);
	ig_window_destroy(g.window);

	ig_darray_destroy(g.preys);
	ig_darray_destroy(g.foods);
	snake_map_destroy(&g.os);
	ig_darray_destroy(g.config.fpsls);
	ig_darray_destroy(g.config.fmlts);
}
