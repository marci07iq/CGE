#version 420

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec2 vertex_loc;

uniform vec2 scale;
uniform vec2 offset;

out vec2 loc;

void main() {
  loc = vertex_loc;
  gl_Position = vec4(((vertex_position - offset)/scale-1), 0.0, 1.0);
}