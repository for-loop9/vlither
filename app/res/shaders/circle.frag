#version 460 core

layout (location = 0) out vec4 color;
layout (location = 0) in vec4 out_color;
layout (location = 1) in vec2 out_uv;

void main() {
	float l = length(out_uv * 2.0 - 1.0);
	//float g = abs(out_uv * 2.0 - 1.0).y;
	float o = step(0.0, 1.0 - l);
	color = out_color * o;
}