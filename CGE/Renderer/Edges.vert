#version 420

layout(location = 0) in vec3 vertex_a;
layout(location = 1) in vec3 vertex_b;
layout(location = 2) in vec3 vertex_c;
layout(location = 3) in vec3 vertex_d;
layout(location = 4) in float line_radius;

uniform vec3 cam_eye;

out float radius;

void main() {
	vec3 planeNorm = cross(vertex_a-cam_eye, vertex_b-cam_eye);

	radius = (dot(vertex_c-cam_eye, planeNorm)*dot(vertex_d-cam_eye, planeNorm) > 0) ? line_radius : -1;

	gl_Position = vec4(vertex_a, 1);
}
