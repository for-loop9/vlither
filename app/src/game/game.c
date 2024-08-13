#include "game.h"
#include "../networking/callback.h"
#include "../networking/util.h"
#include "prey.h"
#include "title_screen.h"

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
			.notify_kills = true,
			.clk = true,
			.vsync = true,
			.enable_zoom = 1,
			.cv = rand() % 9,
			.cusk = 0,
			.nickname = "",
			.ip = "15.204.212.200:444",
			.hq = true,
		},
		.config = {
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
			.qsm = 1, // 1 = high quality, 1.7 = low quality
			.fmlts = ig_darray_create(float),
			.fpsls = ig_darray_create(float),
			.lfsx = -1,
			.lfsy = -1,
			.lfvsx = -1,
			.lfvsy = -1,
			.lfcv = -1,
			.zoom = 0.9f * 18.0f / 14.0f,
			.gsc = 0.9f * 18.0f / 14.0f,
			.bgw2 = 599,
			.bgh2 = 519,
			.lag_mult = 1,

			.color_groups = {
				[0] = { .x = 0.90f, .y = 0.60f, .z = 1.00f },
				[1] = { .x = 0.67f, .y = 0.71f, .z = 1.00f },
				[2] = { .x = 0.60f, .y = 0.97f, .z = 0.97f },
				[3] = { .x = 0.60f, .y = 1.00f, .z = 0.60f },
				[4] = { .x = 1.00f, .y = 1.00f, .z = 0.53f },
				[5] = { .x = 1.00f, .y = 0.75f, .z = 0.45f },
				[6] = { .x = 1.00f, .y = 0.67f, .z = 0.67f },
				[7] = { .x = 1.00f, .y = 0.30f, .z = 0.30f },
				[8] = { .x = 1.00f, .y = 0.22f, .z = 1.00f },
				[9] = { .x = 1.00f, .y = 1.00f, .z = 1.00f },
				[10] = { .x = 0.66f, .y = 0.71f, .z = 1.00f },
				[11] = { .x = 0.37f, .y = 0.37f, .z = 0.37f },
				[12] = { .x = 1.00f, .y = 0.89f, .z = 0.37f },
				[13] = { .x = 0.18f, .y = 0.63f, .z = 0.45f },
				[14] = { .x = 0.46f, .y = 0.55f, .z = 1.00f },
				[15] = { .x = 0.56f, .y = 0.62f, .z = 1.00f },
				[16] = { .x = 0.33f, .y = 0.39f, .z = 1.00f },
				[17] = { .x = 0.75f, .y = 0.37f, .z = 1.00f },
				[18] = { .x = 1.00f, .y = 1.00f, .z = 0.29f },
				[19] = { .x = 0.26f, .y = 0.31f, .z = 1.00f },
				[20] = { .x = 0.26f, .y = 0.31f, .z = 1.00f },
				[21] = { .x = 0.36f, .y = 0.16f, .z = 0.89f },
				[22] = { .x = 1.00f, .y = 0.40f, .z = 0.04f },
				[23] = { .x = 0.45f, .y = 1.00f, .z = 1.00f },
				[24] = { .x = 0.52f, .y = 0.46f, .z = 0.35f },
				[25] = { .x = 0.24f, .y = 0.76f, .z = 0.28f },
				[26] = { .x = 0.00f, .y = 1.00f, .z = 0.37f },
				[27] = { .x = 0.99f, .y = 0.31f, .z = 0.31f },
				[28] = { .x = 1.00f, .y = 0.25f, .z = 0.25f },
				[29] = { .x = 0.55f, .y = 0.55f, .z = 0.55f },
				[30] = { .x = 0.03f, .y = 0.03f, .z = 0.56f },
				[31] = { .x = 0.56f, .y = 0.03f, .z = 0.03f },
				[32] = { .x = 0.58f, .y = 0.58f, .z = 0.23f },
				[33] = { .x = 0.58f, .y = 0.40f, .z = 0.23f },
				[34] = { .x = 0.58f, .y = 0.23f, .z = 0.58f },
				[35] = { .x = 0.23f, .y = 0.58f, .z = 0.23f },
				[36] = { .x = 0.80f, .y = 0.15f, .z = 0.34f }, // britisher
				[37] = { .x = 0.48f, .y = 0.60f, .z = 1.00f },
				[38] = { .x = 0.00f, .y = 0.00f, .z = 0.52f },
				[39] = { .x = 0.48f, .y = 0.18f, .z = 0.80f },
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
			}
		}
	};

	for (int i = 0; i < 256; i++) g.settings_instance.cusk_skin_data_exp[i] = -1;
	g.settings_instance.cusk_ptr = 0;
	g.settings_instance.exp_ptr = 0;
	g.settings_instance.added_first = 0;

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
	for (int yy = 0; yy < 256; yy++) 
		for (int xx = 0; xx < 256; xx++) g.config.at2lt[yy << 8 | xx] = atan2f(yy - 128, xx - 128);
	
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

	g.window = g.settings_instance.fullscreen ? ig_window_create(&(ig_ivec2) { .x = 1280, .y = 832 }, "vlither", 1, 0) : ig_window_create_asp(16.0f / 9.0f, "vlither");
	g.keyboard = ig_keyboard_create(g.window);
	g.mouse = ig_mouse_create(g.window);
	g.icontext = ig_context_create(g.window, &(ig_ivec2) { .x = g.window->dim.x, .y = g.window->dim.y }, g.settings_instance.vsync);

	ig_texture* sprite_sheet = ig_context_texture_create_from_file(g.icontext, "app/res/textures/sprite_sheet.png");
	ig_texture* font_sheet = ig_context_texture_create_from_file(g.icontext, "app/res/textures/font_sheet.png");
	ig_texture* bg_tex = ig_context_texture_create_from_file(g.icontext, "app/res/textures/bg54.jpg");

	g.renderer = renderer_create(&g, g.icontext, g.window, sprite_sheet, 8192, 128, font_sheet, bg_tex, 512);

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

		input_data.mouse_pos = g.mouse->pos;
		input_data.mouse_delta = g.mouse->delta;
		input_data.mouse_dwheel = g.mouse->dwheel;
		input_data.btn_down = ig_window_mouse_button_down(g.window, GLFW_MOUSE_BUTTON_LEFT);
		// input_data.z_pressed = ig_keyboard_key_pressed(g.keyboard, GLFW_KEY_Z);
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

		pthread_mutex_lock(&g.msg_mutex);
		message_queue_render(&g.msg_queue, &g, dt);
		pthread_mutex_unlock(&g.msg_mutex);

		ig_context_begin(g.icontext);
		pthread_mutex_lock(&g.connecting_mutex);
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
			renderer_start_imgui_frame(g.renderer);
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
	ig_context_texture_destroy(g.icontext, bg_tex);
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
