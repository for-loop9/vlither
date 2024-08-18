#version 460 core

layout (location = 0) out vec4 color;
layout (location = 0) in vec4 out_color;
layout (location = 1) in vec2 out_uv;
layout (location = 2) in float in_shadow;

void main() {
	float l = length(out_uv * 2.0 - 1.0);
	float f = 1.0 - l;
	float o = step(0.0, f);
	if (o == 0) discard;
    float s = exp(-l * 5) * 5;
	float n = 1 - l;
    float shadow_strength = n * n * n;
	vec4 shadow_col = vec4(0, 0, 0, out_color.a) * (f + shadow_strength);

	float v = pow(max(0, min(1, 1 - abs(out_uv.y - 0.5) / 0.5)), .35);
	float v2 = 1 - (l / 2);
	v += (v2 - v) * 0.375;
	v *= 1.22 - 0.44 * out_uv.x / 7;
	color = mix(vec4(out_color.rgb * v, out_color.a) * o, shadow_col, in_shadow);
}