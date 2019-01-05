#pragma once

#include "../Defs.h"

typedef fVec3 Vertex;
struct Edge;
struct Tri;
struct Poly;
struct Object;

struct Edge {
  shared_ptr<Vertex> _vertexa;
  shared_ptr<Vertex> _vertexb;
  int _vertexc_id;
  int _vertexd_id;
  shared_ptr<Tri> _tric;
  shared_ptr<Tri> _trid;
};
struct Tri {
  shared_ptr<Edge> _edges[3]; //v2-v3-v1
  shared_ptr<Vertex> _vertexs[3]; //v1-v2-v3
  colorargb _color;

  bool isIn(fVec3 loc) {
    fVec3 n = crs((*_vertexs[1])- (*_vertexs[0]), (*_vertexs[2]) - (*_vertexs[0]));
    return (
      dot(crs((*_vertexs[1]) - (*_vertexs[0]), loc - (*_vertexs[0])), n) > 0 &&
      dot(crs((*_vertexs[2]) - (*_vertexs[1]), loc - (*_vertexs[1])), n) > 0 &&
      dot(crs((*_vertexs[0]) - (*_vertexs[2]), loc - (*_vertexs[2])), n) > 0
    );
  }
  double intersect(fVec3 rayOri, fVec3 rayDir) {
    fVec3 n = crs((*_vertexs[1]) - (*_vertexs[0]), (*_vertexs[2]) - (*_vertexs[0]));
    return dot((*_vertexs[0]) - rayOri,n)/dot(rayDir, n);
  }
};

struct CSGVertex;
struct CSGPlane;
struct CSGPoly;
struct CSGNode;

struct CSGVertex {
  shared_ptr<fVec3> _pos;
  shared_ptr<fVec3> _norm;

  CSGVertex();
  CSGVertex(fVec3 pos, fVec3 norm);

  CSGVertex clone();
  void flip();
  CSGVertex interpolate(CSGVertex& other, float t);
};

struct CSGPlane {
  static const float EPSILON;
  shared_ptr<fVec3> _norm;
  float _w;
  CSGPlane(shared_ptr<fVec3> norm, float w);
  static CSGPlane fromPoints(fVec3 a, fVec3 b, fVec3 c);

  CSGPlane clone();
  void flip();

  void splitPolygon(shared_ptr<CSGPoly> polygon, list<shared_ptr<CSGPoly>> &coplanarFront, list<shared_ptr<CSGPoly>> &coplanarBack, list<shared_ptr<CSGPoly>> &front, list<shared_ptr<CSGPoly>> &back);
};

struct CSGPoly {
  vector<shared_ptr<CSGVertex>> _vertices;
  shared_ptr<CSGPlane> _plane;
  void* _shared;
  CSGPoly();
  CSGPoly(vector<shared_ptr<CSGVertex>> vertices, void* shared = NULL);
  CSGPoly(list<shared_ptr<CSGVertex>> vertices, void* shared = NULL);
  CSGPoly clone();
  void split(list<shared_ptr<CSGPoly>>& addTo) {

  }
  void flip();

  bool intersectRay(fVec3 from, fVec3 dir, float& at);
};

struct CSGNode {
  shared_ptr<CSGPlane> _plane;
  shared_ptr<CSGNode> _front;
  shared_ptr<CSGNode> _back;
  list<shared_ptr<CSGPoly>> _polygons;

  void build(list<shared_ptr<CSGPoly>> polygons);

  CSGNode();
  CSGNode(list<shared_ptr<CSGPoly>> polygons);

  CSGNode clone();

  void invert();
  list<shared_ptr<CSGPoly>> clipPolygons(list<shared_ptr<CSGPoly>>& polygons);
  void clipTo(shared_ptr<CSGNode> bsp);
  list<shared_ptr<CSGPoly>> allPolygons();
};

struct Object {
  GLuint _obj_vao = 0;
  GLuint _obj_pos_vbo = 0;
  GLuint _obj_col_vbo = 0;

  GLuint _edge_vao = 0;
  GLuint _edge_a_vbo = 0;
  GLuint _edge_b_vbo = 0;
  GLuint _edge_c_vbo = 0;
  GLuint _edge_d_vbo = 0;
  GLuint _edge_t_vbo = 0;

  list<shared_ptr<CSGPoly>> _polygons;
  
  list<shared_ptr<Tri>> _tris;
  list<shared_ptr<Edge>> _edges;
  list<shared_ptr<Vertex>> _verts;

  void fromPolygons(list<shared_ptr<CSGPoly>> polygons);

  //void addPolygon(shared_ptr<Poly> poly);

  Object();
  Object(list<shared_ptr<CSGPoly>> polygons);

  Object clone();
  
  void inverse();

  shared_ptr<Vertex> addVertex(fVec3 to, float EPSILON = 1e-4);
  shared_ptr<Edge> addEdge(shared_ptr<Vertex> a, shared_ptr<Vertex> b);
  shared_ptr<Tri> addTri(fVec3 a, fVec3 b, fVec3 c, void* data);
  shared_ptr<Tri> addTri(shared_ptr<Vertex> pt_a, shared_ptr<Vertex> pt_b, shared_ptr<Vertex> pt_c, void* data);

  void removeTri(shared_ptr<Tri> t);

  bool splitEdge(shared_ptr<Vertex> v, shared_ptr<Edge> e);
  void splitEdge(shared_ptr<Vertex> v, shared_ptr<Tri> t, int opp, shared_ptr<Tri> otherT, int otherOpp);

  void addToEdge(shared_ptr<Edge> e, int sidev, shared_ptr<Tri> sidet);
  void removeFromEdge(shared_ptr<Edge> e, shared_ptr<Tri> sidet);

  void compile();
  void upload();
  void clean();

  void draw();
  void drawEdges();

  void setCube(fVec3 radius = { 1,1,1 }, fVec3 center = { 0,0,0 });
  void setCone(list<fVec3> points, fVec3 top, fVec3 dir, float nearDist, float farDist);

  void setShared(void* to);

  bool intersectRay(fVec3 from, fVec3 dir, float& at);
};

Object operator|(Object& lhs, Object& rhs);

Object operator+(Object& lhs, Object& rhs);

Object operator-(Object& lhs, Object& rhs);

Object operator*(Object& lhs, Object& rhs);

extern Graphics::WinHwnd objectMainWindowHwnd;
extern Graphics::CanvasHwnd objectMainCanvasHwnd;

extern Graphics::WinHwnd objectSettingsWindowHwnd;
