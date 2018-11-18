#version 420

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;

uniform mat4 transform;

out vec3 color;

void main() {
  color = vertex_color;
  gl_Position = transform * vec4(vertex_position, 1.0);
}