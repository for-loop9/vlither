#version 460 core

layout (location = 0) out vec4 color;

layout (set = 1, binding = 0) uniform sampler2D font_sheet;

layout (location = 0) in vec4 out_color;
layout (location = 1) in vec2 out_uv;

void main() {
	vec4 font_sheet_col = texture(font_sheet, out_uv);
	color = out_color * font_sheet_col;
}