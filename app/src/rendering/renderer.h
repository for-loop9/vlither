#ifndef RENDERER_H
#define RENDERER_H

#include "bd_renderer.h"
#include "food_renderer.h"
#include "bp_renderer.h"
#include "sprite_renderer.h"
#include "text_renderer.h"
#include "mm_renderer.h"

typedef struct ImFont ImFont;

typedef enum {
	RENDERER_FONT_BIG = 0,
	RENDERER_FONT_MED,
	RENDERER_FONT_MED_BOLD,
	RENDERER_FONT_SMALL,
	RENDERER_FONT_SMALL_BOLD,
} renderer_font;

typedef struct renderer {
	ig_context* context;
	ig_buffer* quad_buffer;
	ig_buffer* global_buffer;
	bd_renderer* bd_renderer;
	sprite_renderer* bg_renderer;
	food_renderer* food_renderer;
	bp_renderer* bp_renderer;
	sprite_renderer* sprite_renderer;
	text_renderer* text_renderer;
	mm_renderer* mm_renderer;
	ImFont* fonts[5];
} renderer;

typedef struct game game;

renderer* renderer_create(game* g, ig_context* context, ig_window* window, const ig_texture* sprite_sheet, unsigned int max_circles, unsigned int max_foods, unsigned int max_sprites, ig_texture* font_sheet, ig_texture* bg_tex, unsigned int max_chars);
void renderer_start_imgui_frame(renderer* renderer);
void renderer_push_bg(renderer* renderer, const sprite_instance* bg_instance);
void renderer_push_bd(renderer* renderer, const bd_instance* bd_instance);
void renderer_push_food(renderer* renderer, const food_instance* food_instance);
void renderer_push_bp(renderer* renderer, const bp_instance* bp_instance);
void renderer_push_sprite(renderer* renderer, const sprite_instance* sprite_instance);
void renderer_set_map_data(renderer* renderer, const uint8_t* map_data);
void renderer_push_mm(renderer* renderer, const mm_instance* mm_instance, int size);
void renderer_push_text(renderer* renderer, const char* str, const ig_vec3* transform, const ig_vec4* color, ig_vec3* transform_out);
void renderer_flush(renderer* renderer);
void renderer_destroy(renderer* renderer);

#endif