#include "game_data.h"

#include "../user.h"

void show_hot_key(tuser_data* usr, int hotkey, vec3 color, const char* info,
                  float offset, font_size sz) {
  user_settings* usrs = &usr->usrs;

  char hotkey_str[3] = {(char) hotkey, ' ', 0};
  igPushFont(usr->imgui_data.mono_font_bold[sz],
             usr->imgui_data.mono_font_bold[sz]->LegacySize);
  igSetCursorPosX(igGetCursorPosX() + offset);
  igTextColored((ImVec4){color[0], color[1], color[2], 0.6f}, hotkey_str);
  igPopFont();
  igSameLine(0, -1);
  igTextColored((ImVec4){color[0], color[1], color[2], 0.5f}, info);
}

void show_hot_key_str(tuser_data* usr, const char* str, vec3 color,
                      const char* info, float offset, font_size sz) {
  user_settings* usrs = &usr->usrs;

  igPushFont(usr->imgui_data.mono_font_bold[sz],
             usr->imgui_data.mono_font_bold[sz]->LegacySize);
  igSetCursorPosX(igGetCursorPosX() + offset);
  igTextColored((ImVec4){color[0], color[1], color[2], 0.6f}, str);
  igPopFont();
  igSameLine(0, -1);
  igTextColored((ImVec4){color[0], color[1], color[2], 0.5f}, info);
}

void display_hotkeys(tuser_data* usr, float offset, font_size sz) {
  user_settings* usrs = &usr->usrs;
  igPushFont(usr->imgui_data.mono_font[sz],
             usr->imgui_data.mono_font[sz]->LegacySize);

  show_hot_key_str(usr, "F11", (vec3){1, 0.7f, 0.7f}, " Fullscreen", offset,
                   sz);
  show_hot_key(usr, usrs->hotkeys[HOTKEY_RESTART].key, (vec3){1, 0.7f, 0.7f}, usrs->hotkeys[HOTKEY_RESTART].description, offset, sz);
  show_hot_key(usr, usrs->hotkeys[HOTKEY_QUIT].key, (vec3){1, 0.7f, 0.7f}, usrs->hotkeys[HOTKEY_QUIT].description, offset, sz);

  show_hot_key(usr, GLFW_KEY_N, (vec3){0.7f, 1, 0.7f}, "Zoom in", offset, sz);
  show_hot_key(usr, GLFW_KEY_M, (vec3){0.7f, 1, 0.7f}, "Zoom out", offset,
               sz);
  show_hot_key_str(usr, "\ueaed", (vec3){0.7f, 1, 0.7f}, "Boost", 0, sz);

  for (int i = 0; i < NUM_HOTKEYS - 2; i++) {
    hotkey* hk = usrs->hotkeys + i;
    show_hot_key(usr, hk->key, (vec3){1, 1, 0.7f}, hk->description, offset, sz);
  }

  igPopFont();
}

void recalc_sep_mults(game_data* gdata) {
  int n = 0;
  int k = 3;
  float mv = 0;
  for (int i = 0; i < GD_SMUC; i++)
    if (i < k)
      gdata->data.smus[i] = 1;
    else {
      n++;
      if (n <= 4) mv = gdata->data.cst * n / 4.0f;
      gdata->data.smus[i] = 1.0f - mv;
    }
}

void set_mscps(game_data* gdata, int nmscps) {
  if (nmscps != gdata->data.mscps) {
    gdata->data.mscps = nmscps;

    tdarray_clear(gdata->data.fpsls);
    tdarray_clear(gdata->data.fmlts);

    for (int i = 0; i <= gdata->data.mscps; i++) {
      if (i >= gdata->data.mscps)
        tdarray_push(&gdata->data.fmlts, (&(float){gdata->data.fmlts[i - 1]}));
      else
        tdarray_push(&gdata->data.fmlts,
                     (&(float){powf(1 - i / (float)gdata->data.mscps, 2.25f)}));
      if (i == 0)
        tdarray_push(&gdata->data.fpsls, (&(float){0}));
      else
        tdarray_push(&gdata->data.fpsls,
                     (&(float){gdata->data.fpsls[i - 1] +
                               1 / gdata->data.fmlts[i - 1]}));
    }
    float t_fmlt = gdata->data.fmlts[tdarray_length(gdata->data.fmlts) - 1];
    float t_fpsl = gdata->data.fpsls[tdarray_length(gdata->data.fpsls) - 1];
    for (int i = 0; i < 2048; i++) {
      tdarray_push(&gdata->data.fmlts, &t_fmlt);
      tdarray_push(&gdata->data.fpsls, &t_fpsl);
    }
  }
}

int get_cg_id(game_data* gdata, char skin_code_cg_id) {
  if (skin_code_cg_id) {
    for (int i = 0; i < NUM_COLOR_GROUPS; i++) {
      if (gdata->ntl_cg_map[i] == skin_code_cg_id) return i;
    }
  }
  return -1;
}

void calc_cg_uvs(vec4s* cg_uvs, tuser_data* usr) {
  int cg_id = 0;
  for (int i = 0; i < 6; i++) {
    for (int j = 0; j < 7; j++) {
      int col = cg_id % 7;
      int row = cg_id / 7;

      cg_uvs[cg_id].x = col * (1 / 7.0f);
      cg_uvs[cg_id].y = row * (1 / 9.0f);
      cg_uvs[cg_id].z = 1 / 7.0f;
      cg_uvs[cg_id].w = 1 / 9.0f;

      cg_id++;
    }
  }
}

void setup_accessories(accessory_data* accessories, tuser_data* usr) {
  int aid = 0;

  vec2 uv_start = {5 / 7.0f, 8 / 9.0f};

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 8; j++) {
      int col = aid % 8;
      int row = aid / 8;

      accessories[aid].uv.x = uv_start[0] + col * (1 / 28.0f);
      accessories[aid].uv.y = uv_start[1] + row * (1 / 36.0f);
      accessories[aid].uv.z = 1 / 28.0f;
      accessories[aid].uv.w = 1 / 36.0f;

      aid++;
    }
  }

  accessories[0].sc = 1.8f;
  accessories[0].of = 0.6f;

  accessories[1].sc = 1.8f;
  accessories[1].of = -3;

  accessories[2].sc = 1.8f;
  accessories[2].of = 0.5f;

  accessories[3].sc = 2.25f;
  accessories[3].of = -1.25f;

  accessories[4].sc = 2.2f;
  accessories[4].of = -0.25f;

  accessories[5].sc = 2.5f;
  accessories[5].of = -1.6f;

  accessories[6].sc = 2;
  accessories[6].of = 1.5f;

  accessories[7].sc = 3;
  accessories[7].of = -5;

  accessories[8].sc = 1.7f;
  accessories[8].of = -2.5f;

  accessories[9].sc = 3;
  accessories[9].of = -2.5f;

  accessories[10].sc = 2.9f;
  accessories[10].of = -4.1f;

  accessories[11].sc = 3;
  accessories[11].of = -4;

  accessories[12].sc = 2.6f;
  accessories[12].of = -2.5f;

  accessories[13].sc = 2.6f;
  accessories[13].of = -3;

  accessories[14].sc = 1.5f;
  accessories[14].of = -1.5f;

  accessories[15].sc = 1.8f;
  accessories[15].of = -2.5f;

  accessories[16].sc = 1.6f;
  accessories[16].of = -1.5f;

  accessories[17].sc = 2.4f;
  accessories[17].of = -1.15f;

  accessories[18].sc = 2.1f;
  accessories[18].of = -1.8f;

  accessories[19].sc = 2;
  accessories[19].of = -1.8f;

  accessories[20].sc = 1.8f;
  accessories[20].of = -1.8f;

  accessories[21].sc = 2.1f;
  accessories[21].of = -3;

  accessories[22].sc = 1.63f;
  accessories[22].of = 0.55f;

  accessories[23].sc = 1.6f;
  accessories[23].of = 0.8f;

  accessories[24].sc = 1.6f;
  accessories[24].of = 0.5f;

  accessories[25].sc = 1.4f;
  accessories[25].of = 0.4f;

  accessories[26].sc = 1.4f;
  accessories[26].of = 1;

  accessories[27].sc = 1.5f;
  accessories[27].of = 1;

  accessories[28].sc = 1.7f;
  accessories[28].of = -2.5f;

  accessories[29].sc = 1.8f;
  accessories[29].of = 0.5f;

  accessories[30].sc = 1.7f;
  accessories[30].of = -2.5f;

  accessories[31].sc = 1.5f;
  accessories[31].of = -2.6f;
}

void calc_g_glow_colors(game_data* gdata) {
  for (int i = 0; i < NUM_COLOR_GROUPS; i++) {
    int fi = i;
    if (i == 26)
      fi = 3;
    else if (i == 29)
      fi = 9;
    else if (i == 30)
      fi = 15;
    else if (i == 31)
      fi = 7;
    else if (i == 32)
      fi = 4;
    else if (i == 33)
      fi = 5;
    else if (i == 34)
      fi = 0;
    else if (i == 35)
      fi = 3;
    else if (i == 36)
      fi = 7;
    else if (i == 41)
      fi = 15;

    float rr = gdata->cg_colors[fi].r;
    float gg = gdata->cg_colors[fi].g;
    float bb = gdata->cg_colors[fi].b;

    float t1 = (rr + gg + bb) / 3.0f;
    if (t1 <= (24 / 255.0f))
      rr = gg = bb = (90 / 255.0f);
    else {
      float t2 = (120 / 255.0f) / t1;
      rr = fminf(1, rr * t2);
      gg = fminf(1, gg * t2);
      bb = fminf(1, bb * t2);
    }

    gdata->cg_glow_colors[i].r = rr;
    gdata->cg_glow_colors[i].g = gg;
    gdata->cg_glow_colors[i].b = bb;
  }
}

snake* get_snake(game_data* gdata, int id) {
  int snakes_len = tdarray_length(gdata->data.snakes);
  for (int i = 0; i < snakes_len; i++) {
    if (gdata->data.snakes[i].id == id) {
      return gdata->data.snakes + i;
    }
  }
  return NULL;
}

void set_default_skin_data(default_skin_data* dfs) {
  for (int i = 0; i < NUM_DEFAULT_SKINS + 1; i++) {
    dfs[i].ec = (vec3s){1, 1, 1};
    dfs[i].ppc = (vec3s){0, 0, 0};
    dfs[i].pr = 3.5f;
  }

  // black eyes (cv = 63):
  dfs[1 + 63].ec = (vec3s){0, 0, 0};
  dfs[1 + 63].ppc = (vec3s){0.8f, 0.8f, 0.8f};
  dfs[1 + 63].pr = 2.5f;

  // yellow eyes (cv = 64):
  dfs[1 + 64].ec = (vec3s){1, 1, 0.50196f};

  // orange eyes (cv = 25):
  dfs[1 + 25].ec = (vec3s){1, 0.3373f, 0.0353f};

  // grey eyes (cv = 44):
  dfs[1 + 44].ec = (vec3s){0.8314f, 0.8314f, 0.8314f};
}

void game_data_init(tenv* env) {
  tuser_data* usr = env->usr;
  game_data* gdata = &usr->gdata;

  *gdata = (game_data){
      .curr_screen = TITLE_SCREEN,
      .conn = DISCONNECTED,

      .cg_colors_ct =
          {[0] = 0,  [1] = 0,  [2] = 0,  [3] = 0,  [4] = 0,  [5] = 0,  [6] = 0,
           [8] = 0,  [9] = 0,  [10] = 0, [11] = 1, [12] = 0, [13] = 1, [14] = 1,
           [15] = 0, [16] = 1, [17] = 0, [18] = 0, [19] = 1, [20] = 1, [21] = 1,
           [22] = 0, [23] = 0, [24] = 0, [25] = 0, [26] = 0, [27] = 1, [28] = 1,
           [29] = 1, [30] = 1, [31] = 1, [32] = 1, [33] = 1, [34] = 1, [35] = 1,
           [36] = 1, [37] = 0, [38] = 1, [39] = 1, [40] = 0, [41] = 1},

      .cg_colors = {[0] = {0.75f, 0.5f, 0.99609375f},
                    [1] = {0.5625f, 0.59765625f, 0.99609375f},
                    [2] = {0.5f, 0.8125f, 0.8125f},
                    [3] = {0.5f, 0.99609375f, 0.5f},
                    [4] = {0.9296875f, 0.9296875f, 0.4375f},
                    [5] = {0.99609375f, 0.625f, 0.375f},
                    [6] = {0.99609375f, 0.5625f, 0.5625f},
                    [7] = {0.99609375f, 0.25f, 0.25f},
                    [8] = {0.875f, 0.1875f, 0.875f},
                    [9] = {0.99609375f, 0.99609375f, 0.99609375f},
                    [10] = {0.5625f, 0.59765625f, 0.99609375f},
                    [11] = {0.3125f, 0.3125f, 0.3125f},
                    [12] = {0.99609375f, 0.75f, 0.3125f},
                    [13] = {0.15625f, 0.53125f, 0.375f},
                    [14] = {0.390625f, 0.45703125f, 0.99609375f},
                    [15] = {0.46875f, 0.5234375f, 0.99609375f},
                    [16] = {0.28125f, 0.328125f, 0.99609375f},
                    [17] = {0.625f, 0.3125f, 0.99609375f},
                    [18] = {0.99609375f, 0.875f, 0.25f},
                    [19] = {0.21875f, 0.265625f, 0.99609375f},
                    [20] = {0.21875f, 0.265625f, 0.99609375f},
                    [21] = {0.3046875f, 0.13671875f, 0.75f},
                    [22] = {0.99609375f, 0.3359375f, 0.03515625f},
                    [23] = {0.39453125f, 0.78125f, 0.90625f},
                    [24] = {0.5f, 0.515625f, 0.5625f},
                    [25] = {0.234375f, 0.75f, 0.28125f},
                    [26] = {0, 0.99609375f, 0.32421875f},
                    [27] = {0.84765625f, 0.26953125f, 0.26953125f},
                    [28] = {0.99609375f, 0.25f, 0.25f},
                    [29] = {0.5625f, 0.5625f, 0.5625f},
                    [30] = {0.125f, 0.125f, 0.9375f},
                    [31] = {0.9375f, 0.125f, 0.125f},
                    [32] = {0.9375f, 0.9375f, 0.125f},
                    [33] = {0.9375f, 0.5625f, 0.125f},
                    [34] = {0.9375f, 0.125f, 0.9375f},
                    [35] = {0.125f, 0.9375f, 0.125f},
                    [36] = {0.15625f, 0.234375f, 0.67578125f},
                    [37] = {0.40625f, 0.5f, 0.99609375f},
                    [38] = {0, 0, 0.4375f},
                    [39] = {0.40625f, 0.15625f, 0.6640625f},
                    [40] = {1, 1, 1},  // (changed from 0 to 1)
                    [41] = {0.5f, 0.5f, 0.99609375f}},

      .default_skins =
          {[0] = {1, 0},
           [1] = {1, 1},
           [2] = {1, 2},
           [3] = {1, 3},
           [4] = {1, 4},
           [5] = {1, 5},
           [6] = {1, 6},
           [7] = {1, 7},
           [8] = {1, 8},
           [9] = {20, 7,  9,  7,  9,  7,  9,  7,  9,  7, 9,
                  7,  10, 10, 10, 10, 10, 10, 10, 10, 10},
           [10] = {15, 9, 9, 9, 9, 9, 1, 1, 1, 1, 1, 7, 7, 7, 7, 7},
           [11] = {15, 11, 11, 11, 11, 11, 7, 7, 7, 7, 7, 12, 12, 12, 12, 12},
           [12] = {15, 7, 7, 7, 7, 7, 9, 9, 9, 9, 9, 13, 13, 13, 13, 13},
           [13] = {15, 14, 14, 14, 14, 14, 9, 9, 9, 9, 9, 7, 7, 7, 7, 7},
           [14] = {14, 9, 9, 9, 9, 9, 9, 9, 7, 7, 7, 7, 7, 7, 7},
           [15] = {9, 0, 1, 2, 3, 4, 5, 6, 7, 8},
           [16] = {14, 15, 15, 15, 15, 15, 15, 15, 4, 4, 4, 4, 4, 4, 4},
           [17] = {14, 9, 9, 9, 9, 9, 9, 9, 16, 16, 16, 16, 16, 16, 16},
           [18] = {14, 7, 7, 7, 7, 7, 7, 7, 9, 9, 9, 9, 9, 9, 9},
           [19] = {1, 9},
           [20] = {10, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0},
           [21] = {26, 3,  3,  3,  3,  3,  3,  3,  18, 18, 18, 18, 18, 18,
                   20, 19, 20, 19, 20, 19, 20, 18, 18, 18, 18, 18, 18},
           [22] = {21, 5, 5, 5, 5,  5,  5,  5,  9,  9,  9,
                   9,  9, 9, 9, 13, 13, 13, 13, 13, 13, 13},
           [23] = {21, 16, 16, 16, 16, 16, 16, 16, 18, 18, 18,
                   18, 18, 18, 18, 7,  7,  7,  7,  7,  7,  7},
           [24] = {18, 23, 23, 23, 23, 23, 23, 23, 23, 23, 18, 18, 18, 18, 18,
                   18, 18, 18, 18},
           [25] = {21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
                   21, 21, 22, 22, 22, 22, 22, 22, 22, 22, 22},
           [26] = {1, 24},
           [27] = {1, 25},
           [28] = {21, 18, 18, 18, 18, 18, 18, 18, 25, 25, 25,
                   25, 25, 25, 25, 7,  7,  7,  7,  7,  7,  7},
           [29] = {10, 11, 11, 4, 11, 11, 11, 11, 4, 11, 11},
           [30] = {8, 10, 10, 19, 20, 10, 10, 20, 19},
           [31] = {2, 10, 10},
           [32] = {2, 20, 20},
           [33] = {3, 12, 11, 11},
           [34] = {18, 7, 7, 9, 13, 13, 9, 16, 16, 9, 12, 12, 9, 7, 7, 9, 16,
                   16, 9},
           [35] = {8, 7, 7, 9, 9, 6, 6, 9, 9},
           [36] = {8, 16, 16, 9, 9, 15, 15, 9, 9},
           [37] = {1, 22},
           [38] = {1, 18},
           [39] = {1, 23},
           [40] = {1, 26},
           [41] = {1, 27},
           [42] = {32, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
                   5,  5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 7},
           [43] = {1, 28},
           [44] = {1, 29},
           [45] = {14, 7, 7, 7, 9, 9, 9, 9, 9, 9, 9, 9, 7, 7, 7},
           [46] = {1, 7},
           [47] = {24, 16, 16, 16, 18, 18, 18, 18, 18, 18, 18, 18, 18,
                   7,  7,  7,  7,  7,  7,  7,  7,  16, 16, 16, 16},
           [48] = {1, 7},
           [49] = {15, 23, 23, 23, 23, 23, 9, 9, 9, 9, 9, 9, 9, 9, 23, 23},
           [50] = {30, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
                   18, 18, 18, 18, 16, 16, 16, 16, 16, 16, 16,
                   16, 7,  7,  7,  7,  7,  7,  7,  7},
           [51] = {13, 7, 7, 7, 9, 9, 16, 16, 16, 16, 16, 16, 9, 9},
           [52] = {18, 7, 7, 7, 7, 18, 18, 18, 18, 18, 18, 18, 18, 18, 7, 7, 7,
                   7, 7},
           [53] = {1, 30},
           [54] = {1, 31},
           [55] = {1, 32},
           [56] = {1, 33},
           [57] = {1, 34},
           [58] = {1, 35},
           [59] = {1, 18},
           [60] = {1, 36},
           [61] = {36, 30, 30, 30, 30, 30, 30, 35, 35, 35, 35, 35, 35,
                   33, 33, 33, 33, 33, 33, 31, 31, 31, 31, 31, 31, 32,
                   32, 32, 32, 32, 32, 34, 34, 34, 34, 34, 34},
           [62] = {10, 17, 17, 17, 17, 17, 39, 39, 39, 39, 39},
           [63] = {6, 7, 7, 7, 11, 11, 11},
           [64] = {4, 16, 16, 11, 11},
           [65] = {8, 4, 4, 4, 4, 9, 9, 9, 9}},

      .ntl_cg_map =
          {
              [0] = 'z',  [1] = 'x',  [2] = 'c',  [3] = 'v',  [4] = 'b',
              [5] = 'n',  [6] = 'm',  [7] = ',',  [8] = 'a',  [9] = 's',
              [10] = 'd', [11] = 'f', [12] = 'g', [13] = 'h', [14] = 'j',
              [15] = 'k', [16] = 'l', [17] = 'q', [18] = 'w', [19] = 'e',
              [20] = 'r', [21] = 't', [22] = 'y', [23] = 'u', [24] = 'i',
              [25] = 'o', [26] = 'p', [27] = '1', [28] = '2', [29] = '3',
              [30] = '4', [31] = '5', [32] = '6', [33] = '7', [34] = '8',
              [35] = '9',
              [36] = '*',  // disabled
              [37] = '0',
              [38] = '*',  // disabled
              [39] = '-',
              [40] = '*',  // disabled
              [41] = '*'   // disabled
          },
      .u_m = {64, 32, 16, 8, 4, 2, 1},
      .SHADOW_UV = {0, 6 / 9.0f, 1.5f / 7, 1.5f / 9},
      .CURSOR_UV = {2 / 7.0, 7 / 9.0, 1 / 7.0, 1 / 9.0},
      .SKIN_LESS_UV = {6 / 7.0, 6 / 9.0, 1 / 7.0, 1 / 9.0}};

  calc_g_glow_colors(gdata);
  set_default_skin_data(gdata->dfs);

  for (int i = 0; i < NUM_FOOD_SIZES; i++) {
    float sz = ceilf(2 * ((i + 2.8f) * 0.65f + 0 * 6));
    gdata->fsz[i] = sz;
  }

  for (int i = 0; i < NUM_PREY_SIZES; i++) {
    gdata->psz[i] = i + 3;
  }

  gdata->data.lview_xx = -1;
  gdata->data.lview_yy = -1;
  gdata->data.grd = 16384;
  gdata->data.sector_size = 480;
  gdata->data.ssd256 = gdata->data.sector_size / 256.0f;
  gdata->data.spangdv = 4.8;
  gdata->data.nsp1 = 4.25;
  gdata->data.nsp2 = .5;
  gdata->data.nsp3 = 12;
  gdata->data.mamu = .033;
  gdata->data.mamu2 = .028;
  gdata->data.cst = .43;
  gdata->data.default_msl = 42;
  gdata->data.ms_zoom = gdata->data.gsc = 1.5f;
  gdata->data.mmsz = -1;

  gdata->data.snakes = tdarray_create(snake);
  gdata->data.foods = tdarray_create(food);
  gdata->data.preys = tdarray_create(prey);
  gdata->data.pts_dp = tdarray_create(body_part*);
  gdata->data.gptz_dp = tdarray_create(gpt*);

  gdata->data.fmlts = tdarray_create(float);
  gdata->data.fpsls = tdarray_create(float);

  for (int i = 0; i < GD_FLXC; i++) {
    float d = .5 * (1 - cosf(PI * i / (float)(GD_FLXC - 1)));
    gdata->data.flxas[i] = d;
  }

  for (int i = 0; i < GD_EEZ; i++) {
    float j = .5 * (1 - cosf(PI * (GD_EEZ - 1 - i) / (float)(GD_EEZ - 1)));
    gdata->data.xfas[i] = j;
  }

  for (int i = 0; i < GD_AFC; i++) {
    float j = .5 * (1 - cosf(PI * (GD_AFC - 1 - i) / (float)(GD_AFC - 1)));
    gdata->data.afas[i] = j;
  }

  for (int i = 0; i < GD_VFC; i++) {
    float j = .5 * (1 - cosf(PI * (GD_VFC - 1 - i) / (float)(GD_VFC - 1)));
    j += (.5 * (1 - cosf(PI * j)) - j) * .5;
    gdata->data.vfas[i] = j;
    gdata->data.fvxs[i] = 0;
    gdata->data.fvys[i] = 0;
  }

  float j = 0;
  for (int i = 0; i < 250; i++) {
    gdata->data.p12[i] = j;
    j += (1 - j) * .12;
  }

  gdata->worm_effect[0] = 1;
  gdata->worm_effect[1] = 0.95f;
  gdata->worm_effect[2] = 0.9f;
  gdata->worm_effect[3] = 0.85f;
  gdata->worm_effect[4] = 0.8f;
  gdata->worm_effect[5] = 0.75f;
  gdata->worm_effect[6] = 0.7f;
  gdata->worm_effect[7] = 0.75f;
  gdata->worm_effect[8] = 0.8f;
  gdata->worm_effect[9] = 0.85f;
  gdata->worm_effect[10] = 0.9f;
  gdata->worm_effect[11] = 0.95f;
  gdata->worm_effect[12] = 1.0f;

  recalc_sep_mults(gdata);
  calc_cg_uvs(gdata->cg_uvs, usr);
  setup_accessories(gdata->accessories, usr);
  sbot_init(env);
  game_data_reset(env);
}

void game_data_reset(tenv* env) {
  tuser_data* usr = env->usr;
  user_settings* usrs = &env->usr->usrs;
  game_data* gdata = &usr->gdata;

  gdata->data.wfpr = false;
  gdata->data.lagging = false;
  gdata->data.lag_mult = 1;
  gdata->data.ltm = 0;
  gdata->data.last_ping_mtm = 0;
  gdata->data.last_accel_mtm = 0;
  gdata->data.last_e_mtm = 0;
  gdata->data.play_etm = 0;
  gdata->data.fps_etm = 0;
  gdata->data.fps_ltm = 0;
  gdata->data.dead = true;
  gdata->data.follow_view = false;
  gdata->data.mmgad = false;
  gdata->data.gotlb = false;
  gdata->data.lsxm = 0;
  gdata->data.lsym = 0;
  gdata->data.fvx = 0;
  gdata->data.fvy = 0;
  gdata->data.view_xx = 0;
  gdata->data.view_yy = 0;
  gdata->data.fr = 0;
  gdata->data.frames = 0;
  gdata->data.fps = 0;
  gdata->data.cping = 0;
  gdata->data.ping = 0;
  gdata->data.lb_pos = 0;
  gdata->data.score = 0;
  gdata->data.rank = 0;
  gdata->data.kills = 0;
  gdata->data.slither_count = 0;
  gdata->data.snake_id = -1;
  gdata->data.ping_follow = GOOD_PING;
  gdata->data.kd_l_frb = 0;
  gdata->data.kd_r_frb = 0;
  gdata->data.lkstm = 0;
  gdata->data.gsc = gdata->data.ms_zoom;

  memset(gdata->data.lb.entries, 0,
         NUM_LEADERBOARD_ENTRIES * sizeof(gdata->data.lb.entries[0]));
  memset(gdata->data.pings, 0, sizeof(int) * PING_SAMPLE_COUNT);

  for (int row = 0; row < gdata->data.mmsz; row++) {
    memset(gdata->data.mm_data + row * MAX_MINIMAP_SIZE, 0, gdata->data.mmsz);
    memset(gdata->data.mm_data_follow + row * MAX_MINIMAP_SIZE, 0,
           gdata->data.mmsz * sizeof(float));
    memset(usr->r->mmr->minimap + row * MAX_MINIMAP_SIZE, 0, gdata->data.mmsz);
  }

  gdata->data.mmsz = -1;

  for (int j = GD_VFC - 1; j >= 0; j--) {
    gdata->data.fvxs[j] = 0;
    gdata->data.fvys[j] = 0;
  }

  int snakes_len = tdarray_length(gdata->data.snakes);
  for (int i = 0; i < snakes_len; i++) {
    tdarray_push(&gdata->data.pts_dp, &gdata->data.snakes[i].pts);
    tdarray_push(&gdata->data.gptz_dp, &gdata->data.snakes[i].gptz);
  }
  tdarray_clear(gdata->data.snakes);
  tdarray_clear(gdata->data.foods);
  tdarray_clear(gdata->data.preys);
}

void game_data_destroy(tenv* env) {
  tuser_data* usr = env->usr;
  game_data* gdata = &usr->gdata;

  sbot_destroy(env);
  
  tdarray_destroy(gdata->data.fpsls);
  tdarray_destroy(gdata->data.fmlts);
  int gptz_dp_len = tdarray_length(gdata->data.gptz_dp);
  for (int i = 0; i < gptz_dp_len; i++) {
    tdarray_destroy(gdata->data.gptz_dp[i]);
  }
  int pts_dp_len = tdarray_length(gdata->data.pts_dp);
  for (int i = 0; i < pts_dp_len; i++) {
    tdarray_destroy(gdata->data.pts_dp[i]);
  }
  tdarray_destroy(gdata->data.gptz_dp);
  tdarray_destroy(gdata->data.pts_dp);
  tdarray_destroy(gdata->data.preys);
  tdarray_destroy(gdata->data.foods);
  tdarray_destroy(gdata->data.snakes);
}