#version 460 core

layout (location = 0) out vec4 color;

layout (set = 1, binding = 0) uniform sampler2D sprite_sheet;

layout (location = 0) in vec4 out_color;
layout (location = 1) in vec2 out_uv;

void main() {
	vec4 sprite_sheet_col = texture(sprite_sheet, out_uv);
	color = out_color * sprite_sheet_col;
}