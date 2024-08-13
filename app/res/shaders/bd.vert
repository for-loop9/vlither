#version 460 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 circ;
layout (location = 2) in vec3 color;

layout (location = 0) out vec3 out_color;
layout (location = 1) out vec2 out_uv;

layout (set = 0, binding = 1) uniform Global {
	mat4 projection;
} global;

void main() {
	vec2 position_cpy = (position * 2.0 - 1.0) * circ.z + circ.xy;
	gl_Position = global.projection * vec4(position_cpy, 0.0, 1.0);
	
	out_color = color;
	out_uv = position;
}