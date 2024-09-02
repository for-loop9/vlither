#version 460 core

layout (location = 0) out vec4 color;
layout (location = 0) in vec4 out_color;
layout (location = 1) in vec2 out_uv;
layout (location = 2) in float in_shadow;
layout (location = 3) in float in_eye;

void main() {
	float l = length(out_uv * 2.0 - 1.0);
	float f = (1.0 - l);
	float o = smoothstep(0, 0.07, f);
	if (o == 0) discard;
    float shadow_strength = f * f * f;
	vec4 shadow_col = out_color * (f + shadow_strength);

	float v = pow(max(0, min(1, 1 - abs(out_uv.y - 0.5) / 0.5)), .35);
	float v2 = 1 - (l / 2);
	v += (v2 - v) * 0.375;
	v *= 1.22 - 0.44 * out_uv.x / 7;

	vec4 eye_col = out_color;
	eye_col.a *= o;

	color = mix(vec4(out_color.rgb * v, out_color.a * o), shadow_col, in_shadow);
	color = mix(color, eye_col, in_eye);
}