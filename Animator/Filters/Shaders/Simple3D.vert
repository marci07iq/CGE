#version 420

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec4 vertex_color;

uniform mat4 u_camera;

out vec4 color;

void main() {
  color = vertex_color;
  gl_Position = u_camera * vec4(vertex_position, 1);
}