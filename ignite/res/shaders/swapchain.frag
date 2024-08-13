#version 460 core

layout (location = 0) out vec4 color;
layout (set = 0, binding = 0) uniform sampler2D scene_tex;

layout (location = 0) in vec2 out_pos;

void main() {
	vec3 scene_col = texture(scene_tex, out_pos * 0.5 + 0.5).rgb;
	color = vec4(scene_col, 1.0);
}