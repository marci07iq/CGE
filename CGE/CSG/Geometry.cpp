#include "Geometry.h"

#include <igl/read_triangle_mesh.h>
#include <igl/readOFF.h>

#include <igl/copyleft/cgal/CSGTree.h>

Graphics::WinHwnd objectMainWindowHwnd;
Graphics::CanvasHwnd objectMainCanvasHwnd;

Graphics::WinHwnd objectSettingsWindowHwnd;


Mesh::Mesh() {
  //_transform = Eigen::Matrix4d::Identity();
}

int Mesh::faces() {
  return _F.rows();
}

fVec3 Mesh::vertex(int face, int side) {
  int vertId = _F(face, side);
  Eigen::Vector3d pos_3;
  pos_3 << _V.block<1, 3>(vertId, 0).transpose().eval();
  /*Eigen::Vector4d pos_4;
  pos_4 << _V.block<1, 3>(vertId, 0).transpose().eval(), 1.0;
  pos_4 = _transform * pos_4;
  return fVec3(pos_4(0), pos_4(1) , pos_4(2));*/
  return fVec3(pos_3(0), pos_3(1), pos_3(2));
}

bool Mesh::intersectRay(fVec3 from, fVec3 dir, float & at) {
  bool intersectAny = false;
  for (int i = 0; i < faces(); i++) {
    fVec3 va = vertex(i, 0);
    fVec3 vb = vertex(i, 1);
    fVec3 vc = vertex(i, 2);

    fVec3 n = crs(vb - va, vc - va);
    float inter = dot(va - from, n) / dot(dir, n);

    fVec3 pos = from + dir * inter;

    if (
      dot(crs(vb - va, pos - va), n) > 0 &&
      dot(crs(vc - vb, pos - vb), n) > 0 &&
      dot(crs(va - vc, pos - vc), n) > 0) {
      intersectAny = true;
      at = min(at, inter);
    }

  }
  return intersectAny;
}

void Mesh::setColor(colorargb to) {
  _color.resize(_F.rows());
  std::fill(_color.begin(), _color.end(), to);
}

void booleanOperation(list<Mesh*> lhs, list<Mesh*> rhs, igl::MeshBooleanType type, Mesh& res) {
  //Eigen::VectorXi J;

  bool first_mesh = true;
  igl::copyleft::cgal::CSGTree lhs_compact;

  map<size_t, Mesh*> offsets;

  int offset = 0;
  for (auto&& it : lhs) {
    offsets[offset] = it;
    offset += it->faces();

    if (first_mesh) {
      lhs_compact = igl::copyleft::cgal::CSGTree(it->_V, it->_F);
      first_mesh = false;
    } else {
      lhs_compact = igl::copyleft::cgal::CSGTree(
        lhs_compact,
        igl::copyleft::cgal::CSGTree(it->_V, it->_F),
        igl::MESH_BOOLEAN_TYPE_UNION
      );
    }
  }

  first_mesh = true;
  igl::copyleft::cgal::CSGTree rhs_compact;
  for (auto&& it : rhs) {
    offsets[offset] = it;
    offset += it->faces();

    if (first_mesh) {
      rhs_compact = igl::copyleft::cgal::CSGTree(it->_V, it->_F);
      first_mesh = false;
    } else {
      rhs_compact = igl::copyleft::cgal::CSGTree(
        rhs_compact,
        igl::copyleft::cgal::CSGTree(it->_V, it->_F),
        igl::MESH_BOOLEAN_TYPE_UNION
        );
    }
  }

  igl::copyleft::cgal::CSGTree result = igl::copyleft::cgal::CSGTree(lhs_compact, rhs_compact, type);

  res._V = result.cast_V<decltype(res._V)>();
  res._F = result.F();

  res._color.resize(res._F.rows());

  Eigen::Matrix<Eigen::Index, -1, 1> J = result.J();
  for (size_t jid = 0; jid < J.rows(); jid++) {
    auto it = offsets.upper_bound(J(jid));
    it--;

    res._color[jid] = it->second->_color[J(jid) - it->first];
  }

  /*igl::copyleft::cgal::mesh_boolean(lhs._V, lhs._F, rhs._V, rhs._F, type, res._V, res._F, J);

  res._color.resize(J.rows());

  for (size_t f = 0; f<J.rows(); f++) {
    if (J(f)<lhs._F.rows()) {
      res._color[f] = lhs._color[f];
    } else {
      res._color[f] = lhs._color[f - lhs._F.rows()];
    }
  }*/
}

void Mesh::booleanUnion(list<Mesh*> lhs, list<Mesh*> rhs, Mesh& res) {
  booleanOperation(lhs, rhs, igl::MESH_BOOLEAN_TYPE_UNION, res);
}
void Mesh::booleanSubtract(list<Mesh*> lhs, list<Mesh*> rhs, Mesh& res) {
  booleanOperation(lhs, rhs, igl::MESH_BOOLEAN_TYPE_MINUS, res);
}
void Mesh::booleanIntersect(list<Mesh*> lhs, list<Mesh*> rhs, Mesh& res) {
  booleanOperation(lhs, rhs, igl::MESH_BOOLEAN_TYPE_INTERSECT, res);
}

void Mesh::applyTransform(Eigen::Matrix4d trans) {
  for (size_t row = 0; row < _V.rows(); ++row) {
    Eigen::Vector4d pos_4;

    pos_4 << _V.block<1, 3>(row, 0).transpose().eval(), 1.0;
    pos_4 = trans * pos_4;
    
    _V(row, 0) = pos_4(0);
    _V(row, 1) = pos_4(1);
    _V(row, 2) = pos_4(2);
  }
}
