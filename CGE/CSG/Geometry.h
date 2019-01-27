#pragma once

#include "../Defs.h"

struct Mesh {
  Eigen::MatrixXd _V;
  Eigen::MatrixXi _F;

  vector<colorargb> _color;

  Mesh();

  int faces();

  fVec3 vertex(int face, int side);

  bool intersectRay(fVec3 from, fVec3 dir, float & at);

  void setColor(colorargb to);

  static void booleanUnion(list<Mesh*> lhs, list<Mesh*> rhs, Mesh& res);
  static void booleanSubtract(list<Mesh*> lhs, list<Mesh*> rhs, Mesh& res);
  static void booleanIntersect(list<Mesh*> lhs, list<Mesh*> rhs, Mesh& res);

  void applyTransform(Eigen::Matrix4d trans);
  
};

extern Graphics::WinHwnd objectMainWindowHwnd;
extern Graphics::CanvasHwnd objectMainCanvasHwnd;

extern Graphics::WinHwnd objectSettingsWindowHwnd;
