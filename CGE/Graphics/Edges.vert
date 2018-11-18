#version 420

layout(location = 0) in vec3 vertex_a;
layout(location = 1) in vec3 vertex_b;
layout(location = 2) in vec3 vertex_c;
layout(location = 3) in vec3 vertex_d;
layout(location = 4) in float line_radius;

uniform mat4 transform;

out float radius;

void main() {
	vec3 o_vertex_a = (transform * vec4(vertex_a,1.0)).xyz;
	vec3 o_vertex_b = (transform * vec4(vertex_b,1.0)).xyz;
	vec3 o_vertex_c = (transform * vec4(vertex_c,1.0)).xyz;
	vec3 o_vertex_d = (transform * vec4(vertex_d,1.0)).xyz;

	vec3 mcrs = cross(o_vertex_a, o_vertex_b);

	radius = (dot(mcrs, o_vertex_c) * dot(mcrs, o_vertex_d) > 0) ? -line_radius*o_vertex_a.z : -1;

	gl_Position = vec4(vertex_a, 1);
}