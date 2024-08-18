#version 460 core

layout (location = 0) out vec4 color;
layout (location = 0) in vec4 out_color;
layout (location = 1) in vec2 out_uv;

void main() {
    float l = length(out_uv * 2.0 - 1.0);
    float o = exp(-l * 5);
	float n = 1 - l;
    float glow = n * n * n;
    color = out_color * (o + glow);
}
