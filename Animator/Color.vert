#version 330

layout(location = 1) in ivec2 vertex_pos;

void main() {
	gl_Position = vec4(vertex_pos, 1, 1);
}