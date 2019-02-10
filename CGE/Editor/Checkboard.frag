#version 420

in vec2 loc;

out vec4 frag_colour;

void main() {
	int type = int(mod(floor(loc.x) + floor(loc.y), 2));
	frag_colour = vec4(mix(vec3(0.1, 0.1, 0.1), vec3(0.5, 0.5, 0.5), type), 0.4 * (1 - 0.02*length(loc)));
}