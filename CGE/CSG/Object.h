#pragma once

#include "Geometry.h"

class Object {
public:
  bool _toDelete;

  Object* _up;

  //Constructor
  Object();
  //Object(list<shared_ptr<CSGPoly>> polygons);

  //Creating
 // virtual void fromPolygons(list<shared_ptr<CSGPoly>> polygons);
  //virtual Object clone();
  
  //virtual void inverse();
  //virtual void setShared(void* to);

  //virtual shared_ptr<Vertex> addVertex(fVec3 to, float EPSILON = 1e-4);
  //virtual shared_ptr<Edge> addEdge(shared_ptr<Vertex> a, shared_ptr<Vertex> b);

  //virtual shared_ptr<Tri> addTri(fVec3 a, fVec3 b, fVec3 c, void* data);
  //virtual shared_ptr<Tri> addTri(shared_ptr<Vertex> pt_a, shared_ptr<Vertex> pt_b, shared_ptr<Vertex> pt_c, void* data);
  //virtual void removeTri(shared_ptr<Tri> t);

  //virtual bool splitEdge(shared_ptr<Vertex> v, shared_ptr<Edge> e);
  //virtual void splitEdge(shared_ptr<Vertex> v, shared_ptr<Tri> t, int opp, shared_ptr<Tri> otherT, int otherOpp);

  //virtual void addToEdge(shared_ptr<Edge> e, int sidev, shared_ptr<Tri> sidet);
  //virtual void removeFromEdge(shared_ptr<Edge> e, shared_ptr<Tri> sidet);

  //VAO build
  virtual void compile();
  virtual void upload();
  virtual void clean();

  //Render
  virtual void draw();
  virtual void drawEdges();

  //Create
  //virtual void setCone(list<fVec3> points, fVec3 top, fVec3 dir, float nearDist, float farDist);

  virtual bool intersectRay(fVec3 from, fVec3 dir, float& at);
};

class Object_Raw : public Object {
public:
  GLuint _obj_vao = 0;
  GLuint _obj_pos_vbo = 0;
  GLuint _obj_col_vbo = 0;

  GLuint _edge_vao = 0;
  GLuint _edge_a_vbo = 0;
  GLuint _edge_b_vbo = 0;
  GLuint _edge_c_vbo = 0;
  GLuint _edge_d_vbo = 0;
  GLuint _edge_t_vbo = 0;

  //CSG::Object _o;

  Mesh _mesh;

  colorargb _color;

  Object_Raw();

  /*shared_ptr<Vertex> addVertex(fVec3 to, float EPSILON = 1e-4);
  shared_ptr<Edge> addEdge(shared_ptr<Vertex> a, shared_ptr<Vertex> b);
  shared_ptr<Tri> addTri(fVec3 a, fVec3 b, fVec3 c, void* data);
  shared_ptr<Tri> addTri(shared_ptr<Vertex> pt_a, shared_ptr<Vertex> pt_b, shared_ptr<Vertex> pt_c, void* data);

  void removeTri(shared_ptr<Tri> t);

  bool splitEdge(shared_ptr<Vertex> v, shared_ptr<Edge> e);
  void splitEdge(shared_ptr<Vertex> v, shared_ptr<Tri> t, int opp, shared_ptr<Tri> otherT, int otherOpp);

  void addToEdge(shared_ptr<Edge> e, int sidev, shared_ptr<Tri> sidet);
  void removeFromEdge(shared_ptr<Edge> e, shared_ptr<Tri> sidet);*/

  void compile();
  void upload();
  void clean();

  void draw();
  void drawEdges();

  void setCube(fVec3 radius, fVec3 center);
  //void setCone(list<fVec3> points, fVec3 top, fVec3 dir, float nearDist, float farDist);

  void setColor(colorargb to);

  bool intersectRay(fVec3 from, fVec3 dir, float& at);
};

class Object_CSG {
public:
  list<Object*> _parts;
  enum Mode {
    Mode_Union,
    Mode_Subtract,
    Mode_Intersect
  };

  Mode _mode;

  Mesh _compiled;
};



/*Object_Raw operator|(Object& lhs, Object& rhs);

Object_Raw operator+(Object& lhs, Object& rhs);

Object_Raw operator-(Object& lhs, Object& rhs);

Object_Raw operator*(Object& lhs, Object& rhs);
*/