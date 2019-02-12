#version 420

in vec4 elem_color;

out vec4 frag_colour;

void main() {
  frag_colour = elem_color;
}