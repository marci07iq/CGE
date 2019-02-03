#pragma once

#include "Ply.h"

struct Mesh {
  Eigen::MatrixXd _V;
  Eigen::MatrixXi _F;

  vector<colorargb> _color;

  Mesh();

  int faces();

  fVec3 vertex(int face, int side);

  bool intersectRay(fVec3 from, fVec3 dir, float & at, int& faceId);

  void setColor(colorargb to);

  static void booleanUnion(list<Mesh*> lhs, list<Mesh*> rhs, Mesh& res);
  static void booleanSubtract(list<Mesh*> lhs, list<Mesh*> rhs, Mesh& res);
  static void booleanIntersect(list<Mesh*> lhs, list<Mesh*> rhs, Mesh& res);

  void applyTransform(Eigen::Matrix4d trans);
 
  void readPly(istream& f);
  void writePly(ostream& f, bool saveFaceColor, bool ascii);
};

void compactOperation(list<Mesh*> parts, Mesh& res);

extern Graphics::WinHwnd objectMainWindowHwnd;
extern Graphics::CanvasHwnd objectMainCanvasHwnd;

extern Graphics::WinHwnd objectSettingsWindowHwnd;
