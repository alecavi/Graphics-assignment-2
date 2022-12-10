#version 400 core
layout (location = 0) in vec3 position_L;

uniform mat4 model, view, projection;

void main() {
	gl_Position = projection * view * model * vec4(position_L, 1.0);
}