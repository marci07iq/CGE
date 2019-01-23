#include "Transpose.h"

void Transpose::setIdentity() {
  matrix = glm::identity<glm::mat4x4>();
}

void Transpose::scale(fVec3 scale) {
  glm::mat4x4 with;

  with[0][0] = scale.x;
  with[1][1] = scale.y;
  with[2][2] = scale.z;
  with[3][3] = 1;
  matrix = with*matrix;
}
void Transpose::transform(fVec3 transform) {
  glm::mat4x4 with = glm::identity<glm::mat4x4>();

  with[3][0] = transform.x;
  with[3][1] = transform.y;
  with[3][2] = transform.z;

  matrix = with*matrix;
}
void Transpose::createLook(mVec3 from, sVec3 dir) {
  polar_vec3 p;
  p.fromCartesian(dir);

  float sinp = sin(p.phi);
  float cosp = cos(p.phi);
  float sint = sin(p.theta);
  float cost = cos(p.theta);

  setIdentity();
  transform(-from);
  glm::mat4x4 with;
  float withf[16] = {
    sinp, -cosp, 0, 0,
    -cosp*cost, -sinp*cost, sint, 0,
    -cosp * sint, -sinp*sint, -cost, 0,
    0, 0, 0, 1 };
  with = glm::make_mat4(withf);
  with = glm::transpose(with);
  matrix = with*matrix;
}

void Transpose::project(float fov, float aspectRatio, float zFar, float zNear) {
  float yScale = 1 / tan(fov / 2);
  float xScale = yScale / aspectRatio;
  glm::mat4x4 with;
  float withf[16] = {
    xScale, 0, 0, 0,
    0, yScale, 0, 0,
    0, 0, -(zFar + zNear) / (zFar - zNear), -2 * zNear*zFar / (zFar - zNear),
    0, 0, -1, 0 };
  with = glm::make_mat4(withf);
  with = glm::transpose(with);
  matrix = with * matrix;
}

// set the OpenGL orthographic projection matrix
void Transpose::ortho(float left, float right, float top, float bottom, float zFar, float zNear) {
  // set OpenGL perspective projection matrix
  glm::mat4x4 with;
  float withf[16] = {
    2 / (right - left),0,0,0,
    0,2 / (top - bottom),0,0,
    0,0,-2 / (zFar - zNear),0,
    -(right + left) / (right - left),-(top + bottom) / (top - bottom),-(zFar + zNear) / (zFar - zNear),1
  };
  with = glm::make_mat4(withf);
  with = glm::transpose(with);
  matrix = with*matrix;
}

void Transpose::read(float * to) {
  for (int i = 0; i < 16; i++) {
    to[i] = matrix[i / 4][i % 4];
  }
}
void Transpose::read(double * to) {
  for (int i = 0; i < 16; i++) {
    to[i] = matrix[i / 4][i % 4];
  }
}

void Transpose::transpose() {
  matrix = glm::transpose(matrix);
}

Transpose::Transpose() {
  setIdentity();
}
