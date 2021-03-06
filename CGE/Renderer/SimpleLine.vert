#version 420

layout(location = 0) in vec3 vertex_pos;
layout(location = 1) in vec4 vertex_color;
layout(location = 2) in float vertex_radius;
//layout(location = 1) in vec3 vertex_b;
//layout(location = 2) in float line_radius;

uniform mat4 modview;

out float radius;
out vec4 color;

void main() {
	radius = vertex_radius;//line_radius;
	color = vertex_color;
	gl_Position = modview * vec4(vertex_pos, 1);
}
