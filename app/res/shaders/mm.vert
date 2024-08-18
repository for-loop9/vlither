#version 460 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec3 transform;
layout (location = 2) in vec3 color;
layout (location = 3) in float usage;

layout (location = 0) out vec3 out_color;
layout (location = 1) out vec2 out_uv;
layout (location = 2) out float out_usage;

layout (set = 0, binding = 1) uniform Global {
	mat4 projection;
} global;

void main() {
	gl_Position = global.projection * vec4(position * transform.z + transform.xy, 0.0, 1.0);
	
	out_color = color;
	out_uv = position;
	out_usage = usage;
}