#ifndef CONSTANTS_H
#define CONSTANTS_H

#define CLIENT_VERSION 291

#define MAX_NICKNAME_LEN 32
#define MAX_IPV4_LEN 19
#define MAX_SKIN_CODE_LEN 256
#define NUM_COLOR_GROUPS 42
#define NUM_DEFAULT_SKINS 66
#define NUM_ACCESSORIES 32
#define NO_ACCESSORY 255
#define BLANK_UV 40
#define WORM_EFFECT_LEN 13
#define NUM_FOOD_SIZES 17
#define NUM_PREY_SIZES 22
#define MAX_MINIMAP_SIZE 512
#define TIMEOUT 5
#define PING_SAMPLE_COUNT 8
#define GOOD_PING 30
#define BAD_PING 60
#define NUM_LEADERBOARD_ENTRIES 10
#define MAX_ZOOM_IN 10
#define MAX_ZOOM_OUT 0.025f

#define PI2 6.2831853f
#define PI 3.1415926f

#define USER_SETTINGS_FILE "user.dat"

// game data constants:
#define PROTOCOL_VERSION 19
#define GD_FLXC 38
#define GD_EEZ 53
#define GD_AFC 26
#define GD_VFC 62
#define GD_SMUC 100
#define GD_SMUC_M3 (GD_SMUC - 3)
#define GD_A64K (65536.0f / PI2);
#define GD_K64A (PI2 / 65536.0f);
#define GD_NSEP 4.5f

// render constants:
#define MAX_BOOST_INSTANCES 131072
#define MAX_FOOD_INSTANCES 131072
#define MAX_SPRITE_INSTANCES 131072
#define MAX_PREYS 1024

// hotkeys:
#define RESTART_HKEY GLFW_KEY_R
#define QUIT_HKEY GLFW_KEY_Q
#define CROSSHAIR_HKEY GLFW_KEY_C
#define ZOOM_IN_HKEY GLFW_KEY_N
#define ZOOM_OUT_HKEY GLFW_KEY_M
#define BOOST_HKEY GLFW_KEY_SPACE
#define ASSIST_HKEY GLFW_KEY_K
#define HUD_HKEY GLFW_KEY_H
#define BIG_FOOD_HKEY GLFW_KEY_F
#define SHOW_NAMES_HKEY GLFW_KEY_P
#define HOTKEYS_HKEY GLFW_KEY_Z

typedef enum conn_status {
  DISCONNECTED = 0,
  CONNECTING = 1,
  CONNECTED = 2
} conn_status;

typedef enum screen {
  TITLE_SCREEN = 0,
  SKIN_EDITOR = 1,
  PLAYING = 2,
  SETTINGS = 3
} screen;

typedef enum font_size {
  FONT_SIZE_SMALL,
  FONT_SIZE_REGULAR,
  FONT_SIZE_LARGE,
  NUM_FONT_SIZES
} font_size;

#endif