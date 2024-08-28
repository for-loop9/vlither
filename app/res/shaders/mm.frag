#version 460 core

layout (location = 0) out vec4 color;
layout (location = 0) in vec3 out_color;
layout (location = 1) in vec2 out_uv;
layout (location = 2) in float in_usage;

layout (set = 1, binding = 0) uniform sampler2D minimap;

void main() {
	float minimap_col = texture(minimap, in_usage * out_uv).r;
	float l = length(out_uv * 2.0 - 1.0);
	float c = step(0.0, 1.0 - l);
	float b = smoothstep(0.0, 0.01, 1.0 - l) - smoothstep(0.0, 0.01, 0.985 - l);
	float o = minimap_col * 0.45;
	color = c * vec4(0.05 + o, 0.05 + o, 0.05 + o, 1) + b * vec4(0.3, 0.3, 0.3, 1);
	color.a *= 0.7;
}