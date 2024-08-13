#version 460 core

layout (location = 0) in vec2 position;

layout (location = 0) out vec2 out_pos;

void main() {
	gl_Position = vec4(position * 1, 0.0, 1.0);
	out_pos = position;
}