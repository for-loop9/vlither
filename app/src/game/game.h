#ifndef GAME_H
#define GAME_H

#include "snake.h"
#include <pthread.h>
#include "../external/mg/mongoose.h"
#include "../networking/snake_map.h"
#include "leaderboard_entry.h"
#include "../rendering/renderer.h"
#include "message.h"

#define PROTOCOL_VERSION 14

typedef struct prey prey;

#define PREY_SIZES 22
#define MAX_BP_RENDER (32768 + 512)
#define MAX_FOOD_RENDER 16384

typedef struct input_data {
	ig_vec2 mouse_pos;
	ig_vec2 mouse_delta;
	float mouse_dwheel;
	bool btn_down;
	bool k_pressed;
	bool m_pressed;
	bool n_pressed;
	bool b_pressed;
	bool p_pressed;
	bool s_pressed;
	bool h_pressed;
	bool nine_pressed;
	bool zero_pressed;
	float ctm;
	int fps_display;
} input_data;

void* network_worker(void* args);
void* load_worker(void* args);

typedef struct settings {
	char nickname[25];
	char ip[60];
	int cv;
	bool cusk;
	int cusk_skin_data_exp[256];
	int exp_ptr;
	char skin_code[256];
	bool clk;
	bool vsync;
	bool fullscreen;
	bool show_fps;
	bool show_kill;
	bool notify_kills;
	bool enable_zoom;
	bool show_lengths;
	bool hq;
	renderer_font names_font;
	bool instant_gameover;
	bool black_bg;
	bool big_food;
	int laser_thickness;
	int hp_size;
	ig_vec4 laser_color;
	ig_vec3 names_color;
	ig_vec3 hp_color;
	float food_scale;
	float mm_scale;
} settings;

typedef struct game {
	// network:
	struct mg_mgr* mgr_ptr;
	int network_done;
	int played;
	int connecting;
	pthread_t network_thread;
	pthread_t load_thread;

	// shared
	int game_quit;
	leaderboard_entry leaderboard[10];
	snake_map os;
	food* foods;
	prey* preys;
	int connected;
	int frame_write;
	int snake_null;
	message_queue msg_queue;
	int should_load;
	bool respawn;

	pthread_mutex_t render_mutex;
	pthread_cond_t render_cond;
	
	pthread_mutex_t connecting_mutex;
	pthread_cond_t connecting_cond;
	pthread_mutex_t msg_mutex;

	bool woke_up;

	// ig_vec2 scale_factor;
	
	struct {
		int grd;
		int mscps;
		bool md;
		bool wmd;

		float sector_size;
		float ssd256;
		float spangdv;
		float nsp1;
		float nsp2;
		float nsp3;
		float mamu;
		float mamu2;
		float cst;
		float qsm;

		int mmsize;
		uint8_t mmdata[512 * 512];

		float ltm;
		float last_ping_mtm;
		float last_e_mtm;
		float last_accel_mtm;
		float vfr;
		float avfr;
		float afr;
		float lag_mult;
		float fr;
		float lfr;
		float vfrb;
		float vfrb2;
		float lfr2;
		float fr2;

		bool lagging;
		int lfsx;
		int lfsy;
		int lfvsx;
		int lfvsy;
		int lfcv;
		int wfpr;
		int lsang;

		float* fmlts;
		float* fpsls;

		float lfas[LFC];
		float rfas[RFC];
		float hfas[HFC];
		float afas[AFC];
		float vfas[VFC];
		float pbx[32767 * 2];
		float pby[32767 * 2];
		float pba[32767 * 2];
		float pbu[32767 * 2];

		float view_xx;
		float view_yy;
		int my_pos;

		float fvxs[VFC];
		float fvys[VFC];
		float fs[FOOD_SIZES];
		float f2s[FOOD_SIZES];
		float pr_fs[PREY_SIZES];
		float pr_f2s[PREY_SIZES];
		float fvx;
		float fvy;
		int fvpos;
		int fvtg;
		int lfesid;

		float zoom;
		float gsc;
		float bpx1;
		float bpy1;
		float bpx2;
		float bpy2;
		float fpx1;
		float fpy1;
		float fpx2;
		float fpy2;
		float apx1;
		float apy1;
		float apx2;
		float apy2;
		float bgx2;
		float bgy2;
		float bgw2;
		float bgh2;

		// skin stuff:
		ig_vec3 color_groups[40];
		ig_vec3 color_groups_opp[40];
		uint8_t default_skins[66][64];
		char ntl_cg_map[40];

		int length_display;
		int kills_display;
		int rank;
		int total_players;
		bool assist;
		bool player_names;
		bool shadow;
		bool show_hud;
	} config;
	
	settings settings_instance;

	// game:
	ig_window* window;
	ig_keyboard* keyboard;
	ig_mouse* mouse;
	ig_context* icontext;
	renderer* renderer;

	int setting_skin;
	int show_settings;
} game;

void create_game(int argc, char** argv);

#endif