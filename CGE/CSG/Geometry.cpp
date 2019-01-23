#include "Geometry.h"

Graphics::WinHwnd objectMainWindowHwnd;
Graphics::CanvasHwnd objectMainCanvasHwnd;

Graphics::WinHwnd objectSettingsWindowHwnd;

/*CSGPlane::CSGPlane(CSGVector norm, CSGScalar w) : _w(w), _norm(norm) {
  
}

CSGPlane CSGPlane::fromPoints(CSGVector a, CSGVector b, CSGVector c) {
  CSGVector n = crs(b - a, c - a).norm();
  return CSGPlane(n, dot(n, a));
}

void CSGPlane::flip() {
  _norm *= -1;
  _w *= -1;
}

void CSGPlane::splitPolygon(CSGPoly &polygon, list<CSGPoly> &coplanarFront, list<CSGPoly> &coplanarBack, list<CSGPoly> &front, list<CSGPoly> &back) {
  static const int COPLANAR = 0;
  static const int FRONT = 1;
  static const int BACK = 2;
  static const int SPANNING = 3;

  // Classify each point as well as the entire polygon into one of the above
  // four classes.
  int polygonType = 0;
  vector<int> types(polygon._vertices.size());
  int i = 0;
  for (auto&& it : polygon._vertices) {
    CSGScalar t = dot(_norm, it._pos) - _w;
    int type = (t < 0) ? BACK : (t > 0) ? FRONT : COPLANAR;
    polygonType |= type;
    types[i] = type;
    ++i;
  }

  // Put the polygon in the correct list, splitting it when necessary.
  switch (polygonType) {
    case COPLANAR:
      (dot(_norm, polygon._plane._norm) > 0 ? coplanarFront : coplanarBack).push_back(polygon);
      break;
    case FRONT:
      front.push_back(polygon);
      break;
    case BACK:
      back.push_back(polygon);
      break;
    case SPANNING:
      list<CSGVertex> f;
      list<CSGVertex> b;
      int i = 0;
      for (auto&& it : polygon._vertices) {
        int j = (i + 1) % polygon._vertices.size();
        int ti = types[i];
        int tj = types[j];
        CSGVertex vi = polygon._vertices[i];
        CSGVertex vj = polygon._vertices[j];
        if (ti != BACK) f.push_back(vi);
        if (ti != FRONT) b.push_back(vi);
        if ((ti | tj) == SPANNING) {
          CSGScalar t = (_w - dot(_norm, vi._pos)) / dot(_norm, vj._pos - vi._pos);
          CSGVertex v = vi.interpolate(vj, t);
          f.push_back(v);
          b.push_back(v);
        }
        ++i;
      }
      if (f.size() >= 3) front.push_back(CSGPoly(f, polygon._shared));
      if (b.size() >= 3)  back.push_back(CSGPoly(b, polygon._shared));
      break;
  }
}

void CSGNode::build(list<shared_ptr<CSGPoly>> polygons) {
  if (!polygons.size()) {
    return;
  }
  if (!_plane) {
    _plane = make_shared<CSGPlane>(polygons.front()->_plane->clone());
  }
  list<shared_ptr<CSGPoly>> front;
  list<shared_ptr<CSGPoly>> back;
  for (auto&& it : polygons) {
    _plane->splitPolygon(it, _polygons, _polygons, front, back);
  }
  if (front.size()) {
    if (!_front) _front = make_shared<CSGNode>();
    _front->build(front);
  }
  if (back.size()) {
    if (!_back) _back = make_shared<CSGNode>();
    _back->build(back);
  }
}

CSGNode::CSGNode() {
}

CSGNode::CSGNode(list<shared_ptr<CSGPoly>> polygons) {
  build(polygons);
}

CSGNode CSGNode::clone() {
  CSGNode res;
  if (_plane) res._plane = make_shared<CSGPlane>(_plane->clone());
  if (_front) res._front = make_shared<CSGNode>(_front->clone());
  if (_back) res._back = make_shared<CSGNode>(_back->clone());
  for (auto&& it : _polygons) {
    res._polygons.push_back(make_shared<CSGPoly>(it->clone()));
  }
  return res;
}

void CSGNode::invert() {
  for (auto&& it : _polygons) {
    it->flip();
  }
  _plane->flip();
  if (_front) _front->invert();
  if (_back) _back->invert();
  shared_ptr<CSGNode> temp = _front;
  _front = _back;
  _back = temp;
}

list<shared_ptr<CSGPoly>> CSGNode::clipPolygons(list<shared_ptr<CSGPoly>>& polygons) {
  if(!_plane) return polygons;
  list<shared_ptr<CSGPoly>> front;
  list<shared_ptr<CSGPoly>> back;
  for (auto&& it : polygons) {
    _plane->splitPolygon(it, front, back, front, back);
  }
  if (_front) front = _front->clipPolygons(front);
  if (_back) {
    back = _back->clipPolygons(back);
  } else {
    back.clear();
  }
  front.splice(front.end(), back);
  return front;
}

void CSGNode::clipTo(shared_ptr<CSGNode> bsp) {
  _polygons = bsp->clipPolygons(_polygons);
  if (_front) _front->clipTo(bsp);
  if (_back) _back->clipTo(bsp);
}

list<shared_ptr<CSGPoly>> CSGNode::allPolygons() {
  list<shared_ptr<CSGPoly>> res;
  res.splice(res.end(), _polygons);
  if (_front) res.splice(res.end(), _front->allPolygons());
  if (_back) res.splice(res.end(), _back->allPolygons());
  return res;
}

CSGPoly::CSGPoly() {

}

CSGPoly::CSGPoly(vector<shared_ptr<CSGVertex>> vertices, void* shared) {
  _vertices = vertices;
  _shared = shared;
  _plane = make_shared<CSGPlane>(CSGPlane::fromPoints(
  _vertices[0]->_pos,
  _vertices[1]->_pos,
  _vertices[2]->_pos));
}

CSGPoly::CSGPoly(list<shared_ptr<CSGVertex>> vertices, void* shared) {
  _vertices = vector<shared_ptr<CSGVertex>>(begin(vertices), end(vertices));
  _shared = shared;
  _plane = make_shared<CSGPlane>(CSGPlane::fromPoints(
  _vertices[0]->_pos,
  _vertices[1]->_pos,
  _vertices[2]->_pos));
}

CSGPoly CSGPoly::clone() {
  list<shared_ptr<CSGVertex>> vertices;
  for (auto&& it : _vertices) {
    vertices.push_back(make_shared<CSGVertex>(it->clone()));
  }
  return CSGPoly(vertices, _shared);
}

void CSGPoly::flip() {
  for (auto&& it : _vertices) {
    it->flip();
  }
  reverse(_vertices.begin(), _vertices.end());
  _plane->flip();
}

bool CSGPoly::intersectRay(fVec3 from, fVec3 dir, float & at) {
  bool isIn = true;
  float loc = dot(_vertices[0]->_pos - from, _plane->_norm) / dot(dir, _plane->_norm);
  fVec3 pt = from + dir * loc;
  for (int i = 0; i < _vertices.size() && isIn; i++) {
    if (dot(
      crs(
        _vertices[(i + 1) % _vertices.size()]->_pos - _vertices[i]->_pos,
        pt - _vertices[i]->_pos
      ).norm(), 
      _plane->_norm.norm()
    ) < CSGPlane::EPSILON) {
      isIn = false;
    }
  }
  if (isIn) {
    at = min(at, loc);
  }
  return isIn;
}

CSGVertex::CSGVertex() {

}

CSGVertex::CSGVertex(fVec3 pos, fVec3 norm) {
  _pos = pos;
  _norm = norm;
}

CSGVertex CSGVertex::clone() {
  return CSGVertex(_pos, _norm);
}

void CSGVertex::flip() {
  _norm *= -1;
}

CSGVertex CSGVertex::interpolate(CSGVertex & other, CSGScalar t) {
  return CSGVertex(
  _pos * (1 - t) + other._pos * t,
  _norm * (1 - t) + other._norm * t);
}*/