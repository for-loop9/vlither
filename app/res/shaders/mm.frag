#version 460 core

layout (location = 0) out vec4 color;
layout (location = 0) in vec3 out_color;
layout (location = 1) in vec2 out_uv;

layout (set = 1, binding = 0) uniform sampler2D minimap;

void main() {
	float minimap_col = texture(minimap, out_uv).r;
	float l = length(out_uv * 2.0 - 1.0);
	float c = step(0.0, 1.0 - l);
	float o = (1 - c) + minimap_col;
	color = vec4(out_color * o, 1) * c;
}