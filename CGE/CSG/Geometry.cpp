#include "Geometry.h"

//#include <igl/read_triangle_mesh.h>
//#include <igl/readOFF.h>
//#include <igl/readPLY.h>
//#include <igl/writePLY.h>
//#include <igl/readMESH.h>

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

bool Mesh::intersectRay(fVec3 from, fVec3 dir, float & at, int& faceId) {
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
      if (inter < at) {
        at = inter;
        faceId = i;
      }
    }

  }
  return intersectAny;
}

void Mesh::setColor(colorargb to) {
  _color.resize(_F.rows());
  std::fill(_color.begin(), _color.end(), to);
}

void mergeToCSG(list<Mesh*> &parts, igl::copyleft::cgal::CSGTree &out, map<size_t, Mesh*> &offsets, int &offset) {
  bool first_mesh = true;
  for (auto&& it : parts) {
    offsets[offset] = it;
    offset += it->faces();

    if (first_mesh) {
      out = igl::copyleft::cgal::CSGTree(it->_V, it->_F);
      first_mesh = false;
    } else {
      out = igl::copyleft::cgal::CSGTree(
        out,
        igl::copyleft::cgal::CSGTree(it->_V, it->_F),
        igl::MESH_BOOLEAN_TYPE_UNION
        );
    }
  }
}

void CSGtoMesh(igl::copyleft::cgal::CSGTree &csg, Mesh& res, map<size_t, Mesh*> &offsets, int &offset) {
  res._V = csg.cast_V<decltype(res._V)>();
  res._F = csg.F();

  res._color.resize(res._F.rows());

  Eigen::Matrix<Eigen::Index, -1, 1> J = csg.J();
  for (size_t jid = 0; jid < J.rows(); jid++) {
    auto it = offsets.upper_bound(J(jid));
    it--;

    res._color[jid] = it->second->_color[J(jid) - it->first];
  }
}

void booleanOperation(list<Mesh*> lhs, list<Mesh*> rhs, igl::MeshBooleanType type, Mesh& res) {
  igl::copyleft::cgal::CSGTree lhs_compact;
  igl::copyleft::cgal::CSGTree rhs_compact;

  map<size_t, Mesh*> offsets;
  int offset = 0;

  mergeToCSG(lhs, lhs_compact, offsets, offset);
  mergeToCSG(rhs, rhs_compact, offsets, offset);
   
  igl::copyleft::cgal::CSGTree result = igl::copyleft::cgal::CSGTree(lhs_compact, rhs_compact, type);

  CSGtoMesh(result, res, offsets, offset);

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
void compactOperation(list<Mesh*> parts, Mesh& res) {
  igl::copyleft::cgal::CSGTree compact;

  map<size_t, Mesh*> offsets;
  int offset = 0;

  mergeToCSG(parts, compact, offsets, offset);

  CSGtoMesh(compact, res, offsets, offset);
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

void Mesh::readPly(istream& f) {
  vector<ply_vertex_pos> _poss;
  vector<colorargb> _vcols;
  vector<ply_face_coords> _faces;

  read_ply(f, _poss, _vcols, _faces);

  _V.resize(_poss.size(), 3);
  _F.resize(_faces.size(), 3);
  _color.resize(_faces.size(), 0xffff00ff);

  for (size_t v = 0; v < _poss.size(); v++) {
    _V.block<1,3>(v, 0) << _poss[v].x, _poss[v].y, _poss[v].z;
  }

  for (size_t f = 0; f < _faces.size(); f++) {
    _F.block<1, 3>(f, 0) << _faces[f].i, _faces[f].j, _faces[f].k;
    if (_vcols.size()) {
      _color[f] = _vcols[_faces[f].i];
    }
  }

  /*if (FC.rows() == _F.rows()) {
    for (size_t f = 0; f < _F.rows(); f++) {
      _color[f] = 0xff000000 + 0x10000 * FC(f, 0) + 0x100 * FC(f, 1) + 0x1 * FC(f, 2);
    }
  } else if (VC.rows() == _V.rows()) {
    for (size_t f = 0; f < _F.rows(); f++) {
      _color[f] = 0xff000000 + 0x10000 * VC(_F(f, 0), 0) + 0x100 * VC(_F(f, 0), 1) + 0x1 * VC(_F(f, 0), 2);
    }
  }*/
}

void Mesh::writePly(ostream& f, bool saveFaceColor, bool ascii) {
  vector<ply_vertex_pos> _poss;
  vector<colorargb> _vcols;
  vector<ply_face_coords> _faces;

  _poss.resize(_F.rows() * 3);
  _vcols.resize(_F.rows() * 3);
    
  _faces.resize(_F.rows());
   

  for (size_t f = 0; f < _F.rows(); f++) {
    colorargb tmp = _color[f];

    _vcols[3 * f + 0] =
    _vcols[3 * f + 1] =
    _vcols[3 * f + 2] = tmp;

    _poss[3 * f + 0] = { float(_V(_F(f, 0), 0)), float(_V(_F(f, 0), 1)) , float(_V(_F(f, 0), 2)) };
    _poss[3 * f + 1] = { float(_V(_F(f, 1), 0)), float(_V(_F(f, 1), 1)) , float(_V(_F(f, 1), 2)) };
    _poss[3 * f + 2] = { float(_V(_F(f, 2), 0)), float(_V(_F(f, 2), 1)) , float(_V(_F(f, 2), 2)) };

    _faces[f] = {uint32_t(3*f), uint32_t(3*f+1), uint32_t(3*f+2)};
  }

  write_ply(f, _poss, _vcols, _faces, !ascii);
}