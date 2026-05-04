#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <thermite.h>

#include "../external/mongoose.h"
#include "snake.h"
#include "food.h"
#include "prey.h"
#include "sbot.h"

typedef struct default_skin_data {
  vec3s ec;
  vec3s ppc;
  float pr;
} default_skin_data;

typedef struct leaderboard {
  struct {
    char nickname[MAX_NICKNAME_LEN + 1];
    int score;
    int cv;
  } entries[NUM_LEADERBOARD_ENTRIES];
} leaderboard;

typedef struct accessory_data {
  vec4s uv;
  float sc;
  float of;
} accessory_data;

typedef struct game_data {
  screen curr_screen;
  conn_status conn;
  
  uint8_t default_skins[NUM_DEFAULT_SKINS][64];
  char ntl_cg_map[NUM_COLOR_GROUPS];
  vec4s cg_uvs[NUM_COLOR_GROUPS];
  vec3s cg_colors[NUM_COLOR_GROUPS]; // original rgbs
  accessory_data accessories[NUM_ACCESSORIES];
  vec3s cg_glow_colors[NUM_COLOR_GROUPS]; // glow color rgbs
  bool cg_colors_ct[NUM_COLOR_GROUPS]; // original rgbs' contrasts
  float worm_effect[WORM_EFFECT_LEN]; // skin effect for default skins
  float fsz[NUM_FOOD_SIZES]; // food sizes
  float psz[NUM_PREY_SIZES]; // prey sizes
  default_skin_data dfs[NUM_DEFAULT_SKINS + 1]; // + 1 for custom skin
  int u_m[7];
  sbot bot;

  vec4s SHADOW_UV;
  vec4s CURSOR_UV;
  vec4s SKIN_LESS_UV;

  struct mg_mgr network_manager;
  struct mg_connection* connection;

  bool restart_req;
  bool closed;

  struct {
    float grd;
    float sector_size;
    float ssd256;
    float spangdv;
    float nsp1;
    float nsp2;
    float nsp3;
    float mamu;
    float mamu2;
    float cst;
    float default_msl;
    float ovxx;
    float ovyy;
    float flux_grd;
    float real_flux_grd;
    float view_xx;
    float view_yy;
    float lview_xx;
    float lview_yy;
    float lfsx;
    float lfsy;
    float lfcv;
    float lfvsx;
    float lfvsy;
    float gsc;
    float lag_mult;
    float etm;
    float ltm;
    float ctm;
    float vfr;
    float avfr;
    float last_ping_mtm;
    float last_accel_mtm;
    float last_e_mtm;
    float fps_etm;
    float fps_ltm;
    float fr;
    float fvx;
    float fvy;
    float bpx1;
    float bpy1;
    float bpx2;
    float bpy2;
    float fpx1;
    float fpy1;
    float fpx2;
    float fpy2;
    float ping_follow;
    float ms_zoom;
    float lkstm;

    double play_etm;

    int lsxm;
    int lsym;
    int mmsz;
    int fvpos;
    int fvtg;
    int flx_tg;
    int flux_grd_pos;
    int mscps;
    int protocol_version;
    int lfesid;
    int lsang;
    int vfrb;
    int frames;
    int fps;
    int cping;
    int ping;
    int score;
    int lb_pos;
    int rank;
    int slither_count;
    int kills;
    int snake_id;
    int kd_l_frb;
    int kd_r_frb;

    float* fmlts;
    float* fpsls;
    
    float pings[PING_SAMPLE_COUNT];
    float xfas[GD_EEZ];
    float afas[GD_AFC];
    float vfas[GD_VFC];
    float fvxs[GD_VFC];
    float fvys[GD_VFC];
    float flxas[GD_FLXC];
    float flux_grds[GD_EEZ];
    float smus[GD_SMUC];
    float p12[250];
    float pbx[32767];
    float pby[32767];
    float pba[32767];

    uint8_t pbu[32767];
    uint8_t mm_data[MAX_MINIMAP_SIZE * MAX_MINIMAP_SIZE];
    float mm_data_follow[MAX_MINIMAP_SIZE * MAX_MINIMAP_SIZE];

    snake* snakes;
    food* foods;
    prey* preys;
    body_part** pts_dp;
    gpt** gptz_dp;

    bool wfpr;
    bool lagging;
    bool md;
    bool wmd;
    bool dead;
    bool follow_view;
    bool mmgad;
    bool gotlb;

    leaderboard lb;
  } data;
} game_data;

void display_hotkeys(tuser_data* usr, float offset, font_size sz);

void recalc_sep_mults(game_data* gdata);
void set_mscps(game_data* gdata, int nmscps);

snake* get_snake(game_data* gdata, int id);

int get_cg_id(game_data* gdata, char skin_code_cg_id);

void game_data_init(tenv* env);
void game_data_reset(tenv* env);
void game_data_destroy(tenv* env);

#endif