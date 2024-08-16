#version 460 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec4 circ;
layout (location = 2) in vec2 ratios;
layout (location = 3) in vec4 color;

layout (location = 0) out vec4 out_color;
layout (location = 1) out vec2 out_uv;

layout (set = 0, binding = 1) uniform Global {
	mat4 projection;
} global;

void main() {
	vec2 position_cpy = (position * circ.w) - circ.w * 0.5;
	vec2 point_rot = position_cpy;
	point_rot.x = position_cpy.x * ratios.y - position_cpy.y * ratios.x;
	point_rot.y = position_cpy.x * ratios.x + position_cpy.y * ratios.y;
	point_rot += circ.w * 0.5;

	gl_Position = global.projection * vec4(point_rot + circ.xy, circ.z, 1.0);
	
	out_color = color;
	out_uv = position;
}