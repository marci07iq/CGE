#version 420

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_loc;

uniform mat4 transform;

out vec2 loc;

void main() {
  loc = vertex_loc;
  gl_Position = transform * vec4(vertex_position, 1.0);
}