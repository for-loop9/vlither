#ifndef RENDERER_H
#define RENDERER_H

#include "bd_renderer.h"
#include "circle_renderer.h"
#include "sprite_renderer.h"
#include "text_renderer.h"
#include "mm_renderer.h"

typedef struct ImFont ImFont;

enum {
	RENDERER_FONT_BIG = 0,
	RENDERER_FONT_MED,
	RENDERER_FONT_MED_BOLD,
	RENDERER_FONT_SMALL,
	RENDERER_FONT_SMALL_BOLD,
};

typedef struct renderer {
	ig_context* context;
	ig_buffer* quad_buffer;
	ig_buffer* global_buffer;
	bd_renderer* bd_renderer;
	sprite_renderer* bg_renderer;
	circle_renderer* circle_renderer;
	sprite_renderer* sprite_renderer;
	text_renderer* text_renderer;
	mm_renderer* mm_renderer;
	ImFont* fonts[5];
} renderer;

typedef struct game game;

renderer* renderer_create(game* g, ig_context* context, ig_window* window, const ig_texture* sprite_sheet, unsigned int max_circles, unsigned int max_sprites, ig_texture* font_sheet, ig_texture* bg_tex, unsigned int max_chars);
void renderer_start_imgui_frame(renderer* renderer);
void renderer_push_bg(renderer* renderer, const sprite_instance* bg_instance);
void renderer_push_bd(renderer* renderer, const bd_instance* bd_instance);
void renderer_push_circle(renderer* renderer, const circle_instance* circle_instance);
void renderer_push_sprite(renderer* renderer, const sprite_instance* sprite_instance);
void renderer_set_map_data(renderer* renderer, const uint8_t* map_data);
void renderer_push_mm(renderer* renderer, const mm_instance* mm_instance);
void renderer_push_text(renderer* renderer, const char* str, const ig_vec3* transform, const ig_vec4* color, ig_vec3* transform_out);
void renderer_flush(renderer* renderer);
void renderer_destroy(renderer* renderer);

#endif