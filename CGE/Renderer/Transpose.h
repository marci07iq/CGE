#pragma once

#include "../CSG/Object.h"

class Transpose {
public:
  glm::mat4x4 matrix;

  void setIdentity();
  void scale(fVec3 scale);
  void transform(fVec3 transform);

  void createLook(mVec3 from, sVec3 dir);
  void project(float fov, float aspectRatio, float zFar, float zNear);
  // set the OpenGL orthographic projection matrix
  void ortho(float left, float right, float top, float bottom, float zFar, float zNear);

  void read(float* to);
  void read(double* to);
  void transpose();
  Transpose();
};