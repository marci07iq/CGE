#pragma once

#include "Geometry.h"

class Object {
public:
  virtual string getType() {
    return "Object";
  }

  bool _toDelete;

  //Object* _up;

  Mesh _mesh;

  //Constructor
  Object();

  //VAO build
  virtual void compile();
  virtual void upload();
  virtual void clean();

  //Render
  virtual void draw();
  virtual void drawEdges();

  virtual bool intersectRay(fVec3 from, fVec3 dir, float& at);
};

class Object_Raw : public Object {
public:
  virtual string getType() {
    return "Object_Raw";
  }

  GLuint _obj_vao = 0;
  GLuint _obj_pos_vbo = 0;
  GLuint _obj_col_vbo = 0;

  GLuint _edge_vao = 0;
  GLuint _edge_a_vbo = 0;
  GLuint _edge_b_vbo = 0;
  GLuint _edge_c_vbo = 0;
  GLuint _edge_d_vbo = 0;
  GLuint _edge_t_vbo = 0;

  Object_Raw();

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

/*class Object_CSG {
public:
  virtual string getType() {
    return "Object_CSG";
  }

  list<Object*> _lhs;
  list<Object*> _rhs;

  enum Mode {
    Mode_Union,
    Mode_Subtract,
    Mode_Intersect
  };

  Mode _mode;

  void compile();
};
*/