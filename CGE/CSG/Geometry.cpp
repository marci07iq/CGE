#include "Geometry.h"

Graphics::WinHwnd objectMainWindowHwnd;
Graphics::CanvasHwnd objectMainCanvasHwnd;

Graphics::WinHwnd objectSettingsWindowHwnd;

//Insert to array
void insertColor(float* arr, size_t to, colorargb col, fVec3 light = {1,1,1}) {
  arr[to] = ((col >> 16) & 255) / 255.0 * light.x;
  arr[to + 1] = ((col >> 8) & 255) / 255.0 * light.y;
  arr[to + 2] = ((col >> 0) & 255) / 255.0 * light.z;
}

void insertVector(float* arr, size_t to, fVec3 vec) {
  arr[to] = vec.x;
  arr[to + 1] = vec.y;
  arr[to + 2] = vec.z;
}

Object::Object() {

}

void Object::fromPolygons(list<shared_ptr<CSGPoly>> polygons) {
  _polygons = polygons;
}

/*void Object::addPolygon(shared_ptr<Poly> poly) {
  list<shared_ptr<Poly>> toProc;
  toProc.push_back(poly);

  list<shared_ptr<Poly>> tempBack;
  list<shared_ptr<Poly>> tempFront;

  while (toProc.size()) {
    shared_ptr<Poly> now;
    now = toProc.front();
    toProc.pop_front();
    auto it = now->_vertices.begin();
    auto it2 = it;
    ++it2;
    while ((tempBack.size() == 0 || tempFront.size() == 0) && it != now->_vertices.end()) {
      tempBack.clear();
      tempFront.clear();
      fVec3 newNorm = crs(*((*it2)->_pos) - *((*it)->_pos), *(poly->_plane->_norm)).norm();
      Plane p(make_shared<fVec3>(newNorm), dot(*(*it)->_pos, newNorm));

      p.splitPolygon(now, tempFront, tempBack, tempFront, tempBack);

      ++it2;
      if (it2 == now->_vertices.end()) {
        it2 = now->_vertices.begin();
      }
      ++it;
    }
    if ((tempBack.size() == 0 || tempFront.size() == 0)) { //passed splitting
      _polygons.insert(_polygons.end(), tempBack.begin(), tempBack.end());
      _polygons.insert(_polygons.end(), tempFront.begin(), tempFront.end());
    } else { //cut up, reprocess all
      toProc.insert(toProc.end(), tempBack.begin(), tempBack.end());
      toProc.insert(toProc.end(), tempFront.begin(), tempFront.end());
    }
  }
}*/

Object::Object(list<shared_ptr<CSGPoly>> polygons) {
  fromPolygons(polygons);
}

Object Object::clone() {
  Object res;
  for (auto&& it : _polygons) {
    res._polygons.push_back(make_shared<CSGPoly>(it->clone()));
  }
  return res;
}

void Object::inverse() {
  for (auto&& it : _polygons) {
    it->flip();
  }
}

shared_ptr<Vertex> Object::addVertex(fVec3 to, float EPSILON) {
  for (auto&& it : _verts) {
    if ((*it - to).sqrlen() < EPSILON*EPSILON) {
      return it;
    }
  }
  shared_ptr<Vertex> newVert = make_shared<Vertex>(to);
  _verts.push_back(newVert);
  
  return newVert;
}

shared_ptr<Edge> Object::addEdge(shared_ptr<Vertex> a, shared_ptr<Vertex> b) {
  shared_ptr<Edge> what;
  if (a == b) {
    throw 1;
  }
  for (auto&& it : _edges) {
    if (
      (it->_vertexa == a && it->_vertexb == b) ||
      (it->_vertexa == b && it->_vertexb == a)) {
      what = it;
      break;
    }
  }
  if (!what) {
    what = make_shared<Edge>();
    what->_vertexa = a;
    what->_vertexb = b;
    _edges.push_back(what);
  }
  return what;
}

shared_ptr<Tri> Object::addTri(fVec3 a, fVec3 b, fVec3 c, void * data) {
  return addTri(addVertex(a), addVertex(b), addVertex(c), data);
}

shared_ptr<Tri> Object::addTri(shared_ptr<Vertex> pt_a, shared_ptr<Vertex> pt_b, shared_ptr<Vertex> pt_c, void * data) {
  if (pt_a == pt_b || pt_b == pt_c || pt_c == pt_a) {
    return make_shared<Tri>();
  }
  shared_ptr<Tri> res = make_shared<Tri>();
  res->_vertexs[0] = pt_a;
  res->_vertexs[1] = pt_b;
  res->_vertexs[2] = pt_c;

  res->_edges[2] = addEdge(pt_a, pt_b);
  addToEdge(res->_edges[2], 2, res);
  res->_edges[0] = addEdge(pt_b, pt_c);
  addToEdge(res->_edges[0], 0, res);
  res->_edges[1] = addEdge(pt_c, pt_a);
  addToEdge(res->_edges[1], 1, res);

  res->_color = reinterpret_cast<colorargb>(data);

  _tris.push_back(res);

  return res;
}

void Object::removeTri(shared_ptr<Tri> t) {
  removeFromEdge(t->_edges[0], t);
  removeFromEdge(t->_edges[1], t);
  removeFromEdge(t->_edges[2], t);

  _tris.remove(t);
}

void Object::addToEdge(shared_ptr<Edge> e, int sidev, shared_ptr<Tri> sidet) {
  if(sidet && e->_tric != sidet &&e->_trid != sidet) {
    if (!e->_tric) {
      e->_vertexc_id = sidev;
      e->_tric = sidet;
    } else {
      if (!e->_trid) {
        e->_vertexd_id = sidev;
        e->_trid = sidet;
      } else {
        LOG LERROR "Compile failed, 3+ faces on edge" << endl;
      }
    }
  }
}

void Object::removeFromEdge(shared_ptr<Edge> e, shared_ptr<Tri> sidet) {
  if (e->_tric == sidet) {
    e->_tric = e->_trid;
    e->_vertexc_id = e->_vertexd_id;
    e->_trid = NULL;
    e->_vertexd_id = 0;
  } else if (e->_trid == sidet) {
    e->_trid = NULL;
    e->_vertexd_id = 0;
  } else {
    cout << "FUK" << endl;
  }
}

void Object::compile() {
  _tris.clear();
  _edges.clear();
  _verts.clear();

  //Convert

  for (auto&& it : _polygons) {
    if (it->_vertices.size() >= 3) {
      list<shared_ptr<Vertex>> t_verts;
      for (auto&& vit : it->_vertices) {
        shared_ptr<Vertex> newVert = addVertex(*(vit->_pos));
        if (t_verts.size() >= 2) {
          addTri(
            t_verts.front(),
            t_verts.back(),
            newVert,
            it->_shared
          );
        }
        t_verts.push_back(newVert);
      }
    }
  }

  //Split

  //*
  auto it = _edges.begin();
  while (it != _edges.end()) {
    bool b = false;
    for (auto && vit : _verts) {
      b = splitEdge(vit, *it);
      if (b) {
        auto it2 = it;
        ++it;
        _edges.erase(it2);
        break;
      }
    }
    if(!b) {
      ++it;
    }
  }//*/
}

void Object::upload() {
  clean();
  compile();

  float* vert = new float[3*3*_tris.size()];
  float* col = new float[3*3*_tris.size()];

  int i;
  i = 0;
  for (auto&& it : _tris) {
    insertVector(vert, 9 * i + 0, *(it->_vertexs[0]));
    insertVector(vert, 9 * i + 3, *(it->_vertexs[1]));
    insertVector(vert, 9 * i + 6, *(it->_vertexs[2]));

    fVec3 light = fVec3(0.7+0.3*dot(crs(
    *(it->_vertexs[0]) - *(it->_vertexs[1]),
    *(it->_vertexs[0]) - *(it->_vertexs[2])).norm(),
    vec3<double>(1,2,3).norm()));

    insertColor(col, 9 * i + 0, it->_color, light);
    insertColor(col, 9 * i + 3, it->_color, light);
    insertColor(col, 9 * i + 6, it->_color, light);
    ++i;
  }

  glGenBuffers(1, &_obj_pos_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _obj_pos_vbo);
  glBufferData(GL_ARRAY_BUFFER, _tris.size() * 3 * 3 * sizeof(float), vert, GL_STATIC_DRAW);

  glGenBuffers(1, &_obj_col_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _obj_col_vbo);
  glBufferData(GL_ARRAY_BUFFER, _tris.size() * 3 * 3 * sizeof(float), col, GL_STATIC_DRAW);

  glGenVertexArrays(1, &_obj_vao);
  glBindVertexArray(_obj_vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _obj_pos_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, _obj_col_vbo);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  delete[_tris.size() * 3 * 3] vert;
  delete[_tris.size() * 3 * 3] col;

  //Edges

  float* vert_a = new float[_edges.size() * 2 * 3];
  float* vert_b = new float[_edges.size() * 2 * 3];
  float* vert_c = new float[_edges.size() * 2 * 3];
  float* vert_d = new float[_edges.size() * 2 * 3];
  float* vert_r = new float[_edges.size() * 2 * 1];

  i = 0;
  for (auto&& it : _edges) {
    vert_r[i * 2 + 0] = 0.01;
    vert_r[i * 2 + 1] = 0.01;

    insertVector(vert_a, i * 6 + 0, *(it->_vertexa));
    insertVector(vert_b, i * 6 + 0, *(it->_vertexb));
    insertVector(vert_c, i * 6 + 0, *(it->_tric->_vertexs[it->_vertexc_id]));
    insertVector(vert_d, i * 6 + 0, *(
    it->_trid ?
    it->_trid->_vertexs[it->_vertexd_id] :
    it->_tric->_vertexs[it->_vertexc_id]));

    insertVector(vert_a, i * 6 + 3, *(it->_vertexb));
    insertVector(vert_b, i * 6 + 3, *(it->_vertexa));
    insertVector(vert_c, i * 6 + 3, *(
      it->_trid ?
      it->_trid->_vertexs[it->_vertexd_id] :
      it->_tric->_vertexs[it->_vertexc_id]));
    insertVector(vert_d, i * 6 + 3, *(it->_tric->_vertexs[it->_vertexc_id]));
    ++i;
  }

  glGenBuffers(1, &_edge_a_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _edge_a_vbo);
  glBufferData(GL_ARRAY_BUFFER, _edges.size() * 2 * 3 * sizeof(float), vert_a, GL_STATIC_DRAW);

  glGenBuffers(1, &_edge_b_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _edge_b_vbo);
  glBufferData(GL_ARRAY_BUFFER, _edges.size() * 2 * 3 * sizeof(float), vert_b, GL_STATIC_DRAW);

  glGenBuffers(1, &_edge_c_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _edge_c_vbo);
  glBufferData(GL_ARRAY_BUFFER, _edges.size() * 2 * 3 * sizeof(float), vert_c, GL_STATIC_DRAW);

  glGenBuffers(1, &_edge_d_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _edge_d_vbo);
  glBufferData(GL_ARRAY_BUFFER, _edges.size() * 2 * 3 * sizeof(float), vert_d, GL_STATIC_DRAW);

  glGenBuffers(1, &_edge_t_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _edge_t_vbo);
  glBufferData(GL_ARRAY_BUFFER, _edges.size() * 2 * 1 * sizeof(float), vert_r, GL_STATIC_DRAW);


  glGenVertexArrays(1, &_edge_vao);
  glBindVertexArray(_edge_vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _edge_a_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, _edge_b_vbo);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, _edge_c_vbo);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(3);
  glBindBuffer(GL_ARRAY_BUFFER, _edge_d_vbo);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(4);
  glBindBuffer(GL_ARRAY_BUFFER, _edge_t_vbo);
  glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 0, NULL);


  delete[_edges.size() * 2 * 3] vert_a;
  delete[_edges.size() * 2 * 3] vert_b;
  delete[_edges.size() * 2 * 3] vert_c;
  delete[_edges.size() * 2 * 3] vert_d;
  delete[_edges.size() * 2 * 1] vert_r;
}

void Object::draw() {
  glBindVertexArray(_obj_vao);
  glDrawArrays(GL_TRIANGLES, 0, _tris.size() * 3);
}
void Object::drawEdges() {
  glBindVertexArray(_edge_vao);
  glDrawArrays(GL_LINES, 0, _edges.size() * 2);
}

void Object::setCube(fVec3 radius, fVec3 center) {
  vector<pair<vector<int>, fVec3>> data = {
    { { 0, 4, 6, 2 }, fVec3(-1, 0, 0) },
    { { 1, 3, 7, 5 }, fVec3(+1, 0, 0) },
    { { 0, 1, 5, 4 }, fVec3(0, -1, 0) },
    { { 2, 6, 7, 3 }, fVec3(0, +1, 0) },
    { { 0, 2, 3, 1 }, fVec3(0, 0, -1) },
    { { 4, 5, 7, 6 }, fVec3(0, 0, +1) }
  };
  for (auto&& info : data) {
    list<shared_ptr<CSGVertex>> vertices;
    for (auto&& i : info.first) {
      vertices.push_back(make_shared<CSGVertex>(
        center + radius * vecSwitch(bVec3(i & 1, i & 2, i & 4), fVec3(1), fVec3(-1)),
        info.second
        ));
    }
    shared_ptr<CSGPoly> newPoly = make_shared<CSGPoly>(vertices);
    _polygons.push_back(newPoly);
  }
}

void Object::setCone(list<fVec3> points, fVec3 top, fVec3 dir, float nearDist, float farDist) {
  dir.norm();
  for (auto&& it : points) {
    it -= top;
    it /= dot(dir, it);
  }
  fVec3 areaVec;
  auto it2 = points.begin();
  for (auto&& it : points) {
    ++it2;
    if (it2 == points.end()) {
      it2 = points.begin();
    }
    areaVec += crs(*it2 - points.front(), it - points.front());
    fVec3 norm = crs(*it2 - it, it).norm();
    list<shared_ptr<CSGVertex>> vertices;
    vertices.push_back(make_shared<CSGVertex>(
      top + it*nearDist,
      norm
      ));
    vertices.push_back(make_shared<CSGVertex>(
      top + it*farDist,
      norm
      ));
    vertices.push_back(make_shared<CSGVertex>(
      top + (*it2)*farDist,
      norm
      ));
    vertices.push_back(make_shared<CSGVertex>(
      top + (*it2)*nearDist,
      norm
      ));
    shared_ptr<CSGPoly> newPoly = make_shared<CSGPoly>(vertices);
    _polygons.push_back(newPoly);
  }
  list<shared_ptr<CSGVertex>> fvertices;
  list<shared_ptr<CSGVertex>> bvertices;
  for (auto&& it : points) {
    fvertices.push_back(make_shared<CSGVertex>(
      top + it*nearDist,
      dir * sgn(dot(areaVec, dir))
      ));
    bvertices.push_front(make_shared<CSGVertex>(
      top + it*farDist,
      dir * sgn(-dot(areaVec, dir))
      ));
  }
  shared_ptr<CSGPoly> newPoly = make_shared<CSGPoly>(fvertices);
  _polygons.push_back(newPoly);
  newPoly = make_shared<CSGPoly>(bvertices);
  _polygons.push_back(newPoly);
}

void Object::setShared(void* to) {
  for (auto&& it : _polygons) {
    it->_shared = to;
  }
}

bool Object::intersectRay(fVec3 from, fVec3 dir, float & at) {
  bool intersectAny = false;
  for (auto&& it : _polygons) {
    intersectAny |= it->intersectRay(from, dir, at);
  }
  return intersectAny;
}

void Object::clean() {
  if (_obj_pos_vbo) {
    glDeleteBuffers(1, &_obj_pos_vbo);
    _obj_pos_vbo = 0;
  }
  if (_obj_col_vbo) {
    glDeleteBuffers(1, &_obj_col_vbo);
    _obj_col_vbo = 0;
  }
  if (_obj_vao) {
    glDeleteVertexArrays(1, &_obj_vao);
    _obj_vao = 0;
  }

  if (_edge_a_vbo) {
    glDeleteBuffers(1, &_edge_a_vbo);
    _edge_a_vbo = 0;
  }
  if (_edge_b_vbo) {
    glDeleteBuffers(1, &_edge_b_vbo);
    _edge_b_vbo = 0;
  }
  if (_edge_c_vbo) {
    glDeleteBuffers(1, &_edge_c_vbo);
    _edge_c_vbo = 0;
  }
  if (_edge_d_vbo) {
    glDeleteBuffers(1, &_edge_d_vbo);
    _edge_d_vbo = 0;
  }
  if (_edge_t_vbo) {
    glDeleteBuffers(1, &_edge_t_vbo);
    _edge_t_vbo = 0;
  }
  if (_edge_vao) {
    glDeleteVertexArrays(1, &_edge_vao);
    _edge_vao = 0;
  }
}

Object operator|(Object& lhs, Object& rhs) {
  shared_ptr<CSGNode> a = make_shared<CSGNode>(lhs.clone()._polygons);
  shared_ptr<CSGNode> b = make_shared<CSGNode>(rhs.clone()._polygons);

  a->clipTo(b);
  b->clipTo(a);
  b->invert();
  b->clipTo(a);
  b->invert();
  a->build(b->allPolygons());
  return Object(a->allPolygons());
}

Object operator+(Object& lhs, Object& rhs) {
  shared_ptr<CSGNode> a = make_shared<CSGNode>(lhs.clone()._polygons);
  shared_ptr<CSGNode> b = make_shared<CSGNode>(rhs.clone()._polygons);

  a->clipTo(b);
  b->clipTo(a);
  b->invert();
  b->clipTo(a);
  b->invert();
  a->build(b->allPolygons());
  return Object(a->allPolygons());
}

Object operator-(Object& lhs, Object& rhs) {
  shared_ptr<CSGNode> a = make_shared<CSGNode>(lhs.clone()._polygons);
  shared_ptr<CSGNode> b = make_shared<CSGNode>(rhs.clone()._polygons);

  a->invert();
  a->clipTo(b);
  b->clipTo(a);
  b->invert();
  b->clipTo(a);
  b->invert();
  a->build(b->allPolygons());
  a->invert();
  return Object(a->allPolygons());
}

Object operator*(Object& lhs, Object& rhs) {
  shared_ptr<CSGNode> a = make_shared<CSGNode>(lhs.clone()._polygons);
  shared_ptr<CSGNode> b = make_shared<CSGNode>(rhs.clone()._polygons);

  a->invert();
  b->clipTo(a);
  b->invert();
  a->clipTo(b);
  b->clipTo(a);
  a->build(b->allPolygons());
  a->invert();
  return Object(a->allPolygons());
}

const float CSGPlane::EPSILON = 1e-5;

CSGPlane::CSGPlane(shared_ptr<fVec3> norm, float w) : _w(w) {
  _norm = norm;
}

CSGPlane CSGPlane::fromPoints(fVec3 a, fVec3 b, fVec3 c) {
  fVec3 n = crs(b - a, c - a).norm();
  return CSGPlane(make_shared<fVec3>(n), dot(n, a));
}

CSGPlane CSGPlane::clone() {
  return CSGPlane(make_shared<fVec3>(*_norm), _w);
}

void CSGPlane::flip() {
  *_norm *= -1;
  _w *= -1;
}

void CSGPlane::splitPolygon(shared_ptr<CSGPoly> polygon, list<shared_ptr<CSGPoly>> &coplanarFront, list<shared_ptr<CSGPoly>> &coplanarBack, list<shared_ptr<CSGPoly>> &front, list<shared_ptr<CSGPoly>> &back) {
  static const int COPLANAR = 0;
  static const int FRONT = 1;
  static const int BACK = 2;
  static const int SPANNING = 3;

  // Classify each point as well as the entire polygon into one of the above
  // four classes.
  int polygonType = 0;
  vector<int> types(polygon->_vertices.size());
  int i = 0;
  for (auto&& it : polygon->_vertices) {
    float t = dot(*_norm, *(it->_pos)) - _w;
    int type = (t < -EPSILON) ? BACK : (t > EPSILON) ? FRONT : COPLANAR;
    polygonType |= type;
    types[i] = type;
    ++i;
  }

  // Put the polygon in the correct list, splitting it when necessary.
  switch (polygonType) {
    case COPLANAR:
      (dot(*_norm, *(polygon->_plane->_norm)) > 0 ? coplanarFront : coplanarBack).push_back(polygon);
      break;
    case FRONT:
      front.push_back(polygon);
      break;
    case BACK:
      back.push_back(polygon);
      break;
    case SPANNING:
      list<shared_ptr<CSGVertex>> f;
      list<shared_ptr<CSGVertex>> b;
      int i = 0;
      for (auto&& it : polygon->_vertices) {
        int j = (i + 1) % polygon->_vertices.size();
        int ti = types[i];
        int tj = types[j];
        shared_ptr<CSGVertex> vi = polygon->_vertices[i];
        shared_ptr<CSGVertex> vj = polygon->_vertices[j];
        if (ti != BACK) f.push_back(vi);
        if (ti != FRONT) b.push_back(ti != BACK ? make_shared<CSGVertex>(vi->clone()) : vi);
        if ((ti | tj) == SPANNING) {
          float t = (_w - dot(*_norm, *(vi->_pos))) / dot(*_norm, *(vj->_pos) - *(vi->_pos));
          CSGVertex v = vi->interpolate(*vj, t);
          f.push_back(make_shared<CSGVertex>(v));
          b.push_back(make_shared<CSGVertex>(v.clone()));
        }
        ++i;
      }
      if (f.size() >= 3) front.push_back(make_shared<CSGPoly>(f, polygon->_shared));
      if (b.size() >= 3)  back.push_back(make_shared<CSGPoly>(b, polygon->_shared));
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
  _plane = make_shared<CSGPlane>(CSGPlane::fromPoints(*(_vertices[0]->_pos), *(_vertices[1]->_pos), *(_vertices[2]->_pos)));
}

CSGPoly::CSGPoly(list<shared_ptr<CSGVertex>> vertices, void* shared) {
  _vertices = vector<shared_ptr<CSGVertex>>(begin(vertices), end(vertices));
  _shared = shared;
  _plane = make_shared<CSGPlane>(CSGPlane::fromPoints(*(_vertices[0]->_pos), *(_vertices[1]->_pos), *(_vertices[2]->_pos)));
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
  float loc = dot(*(_vertices[0]->_pos) - from, *(_plane->_norm)) / dot(dir, *(_plane->_norm));
  fVec3 pt = from + dir * loc;
  for (int i = 0; i < _vertices.size() && isIn; i++) {
    if (dot(
      crs(
        *(_vertices[(i + 1) % _vertices.size()]->_pos) - *(_vertices[i]->_pos),
        pt - *(_vertices[i]->_pos)
      ).norm(), 
      (_plane->_norm)->norm()
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
  _pos = make_shared<fVec3>(pos);
  _norm = make_shared<fVec3>(norm);
}

CSGVertex CSGVertex::clone() {
  return CSGVertex(*_pos, *_norm);
}

void CSGVertex::flip() {
  (*_norm) *= -1;
}

CSGVertex CSGVertex::interpolate(CSGVertex & other, float t) {
  return CSGVertex((*_pos) * (1 - t) + (*other._pos) * t, (*_norm) * (1 - t) + (*other._norm) * t);
}

bool Object::splitEdge(shared_ptr<Vertex> v, shared_ptr<Edge> e) {
  if (e->_vertexa == v ||
  e->_vertexb == v) {
    return false;
  }
  fVec3 edge = (*(e->_vertexb) - *(e->_vertexa));
  fVec3 edgeNorm = edge;
  edgeNorm.norm();
  fVec3 vecTo = (*(e->_vertexb) - *(v));

  float vecCross = crs(edgeNorm, vecTo).length();
  float vecDot = dot(edgeNorm, vecTo);

  static const float EPSILON = 0.0001;

  //cout << edge << endl;
  //cout << vecTo << endl;
  //cout << vecCross << " " << vecDot << endl;
  //cout << endl;

  //Check if close to edge
  if((vecCross < EPSILON) &&
    (EPSILON < vecDot && vecDot < edge.length() - EPSILON)) {
    while (e->_tric) {
      if(e->_tric->_vertexs[e->_vertexc_id] == v) {//Split by same triangle
        removeTri(e->_tric);
      } else {
        splitEdge(v, e->_tric, e->_vertexc_id, e->_trid, e->_vertexd_id);
      }
      e->_tric = e->_trid;
      e->_vertexc_id = e->_vertexd_id;
      e->_trid = NULL;
      e->_vertexd_id = 0;
      if (e->_tric) {
        LOG LERROR "This is wrong" END;
      }
    }
    //Delete edge;
    return true;
  }
  return false;
}

void Object::splitEdge(shared_ptr<Vertex> v, shared_ptr<Tri> t, int opp, shared_ptr<Tri> otherT, int otherOpp) {
  shared_ptr<Tri> newTri = make_shared<Tri>(*t);
  _tris.push_back(newTri);

  //Build split edges
  shared_ptr<Edge> splitEo = addEdge(t->_vertexs[(opp + 2) % 3],v);
  addToEdge(splitEo, opp, t);
  addToEdge(splitEo, otherOpp, otherT);

  shared_ptr<Edge> splitEn = addEdge(t->_vertexs[(opp + 1) % 3], v);
  addToEdge(splitEn, opp, newTri);
  addToEdge(splitEn, otherOpp, otherT);

  //Build cutting edge
  shared_ptr<Edge> cutE = addEdge(t->_vertexs[(opp + 0) % 3], v);
  addToEdge(cutE, (opp + 2) % 3, t);
  addToEdge(cutE, (opp + 1) % 3, newTri);

  //Move tri edges
  t->_edges[opp] = splitEo;
  newTri->_edges[opp] = splitEn;

  t->_edges[(opp+2)%3] = cutE;
  newTri->_edges[(opp+1)%3] = cutE;

  //Move tri vertices
  t->_vertexs[(opp + 1)%3] = v;
  newTri->_vertexs[(opp + 2) % 3] = v;
}
