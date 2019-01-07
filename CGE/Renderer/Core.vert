#version 420

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;

uniform mat4 transform;

uniform vec4 mix_color;
uniform float res_alpha;

out vec4 color;

void main() {
  color = vec4(mix(vertex_color, mix_color.rgb, mix_color.a), res_alpha);
  gl_Position = transform * vec4(vertex_position, 1.0);
}