#version 330

uniform sampler2D uniform_texture;
in vec2 uv;

out vec4 col;

void main() {
	col = texture(uniform_texture, uv);
	//col.a = 1;
	//col = vec4(col.g, col.b, col.r, col.a);
}