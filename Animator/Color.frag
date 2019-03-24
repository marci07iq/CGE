#version 330

uniform vec4 uniform_color;

out vec4 col;

void main() {
	col = uniform_color;
}