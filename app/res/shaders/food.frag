#version 460 core

layout (location = 0) out vec4 color;
layout (location = 0) in vec4 out_color;
layout (location = 1) in vec2 out_uv;

void main() {
    float l = length(out_uv * 2.0 - 1.0);
	float n = 1 - l;
	float n4 = 1 - l * 4;
	float f = smoothstep(0.0, 0.2, n4);
	float v = step(0.0, n) - f;
    color = (out_color * f) + (v * out_color * n * 0.65);
}
