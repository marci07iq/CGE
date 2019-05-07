#pragma once

#include "Geometry.h"

//#include "../Renderer/Transpose.h"

class Object {
public:
  bool _toDelete = false;

  GLuint _obj_vao = 0;
  GLuint _obj_pos_vbo = 0;
  GLuint _obj_col_vbo = 0;
  GLuint _obj_lig_vbo = 0;

  GLuint _edge_vao = 0;
  GLuint _edge_a_vbo = 0;
  GLuint _edge_b_vbo = 0;
  GLuint _edge_c_vbo = 0;
  GLuint _edge_d_vbo = 0;
  GLuint _edge_t_vbo = 0;

  //Object* _up;

  Mesh _mesh;
  Transform _offset;

  //Constructor
  Object();
  Object(Mesh& _from);
  ~Object();

  void setCube(fVec3 radius, fVec3 center);
  void setCylinder(fVec3 radius, fVec3 center);

  //VAO build
  void compile();
  void upload();
  void clean();

  //Render
  void draw();
  void drawEdges();

  //IO
  void set(const DataElement* from);
  const void get(DataElement* to);

  //Modify
  void setColor(colorargb to);
  void bakeTransform();
  void bakeCustomTransform(Transform& _what);

  //Query
  bool intersectRay(fVec3 from, fVec3 dir, float& at, int& faceId);

};

/*class Object : public Object {
public:
  virtual string getType() {
    return "Object";
  }

  GLuint _obj_vao = 0;
  GLuint _obj_pos_vbo = 0;
  GLuint _obj_col_vbo = 0;
  GLuint _obj_lig_vbo = 0;

  GLuint _edge_vao = 0;
  GLuint _edge_a_vbo = 0;
  GLuint _edge_b_vbo = 0;
  GLuint _edge_c_vbo = 0;
  GLuint _edge_d_vbo = 0;
  GLuint _edge_t_vbo = 0;

  Object();

  void compile();
  void upload();
  void clean();

  void draw();
  void drawEdges();

  void setCube(fVec3 radius, fVec3 center);
  //void setCone(list<fVec3> points, fVec3 top, fVec3 dir, float nearDist, float farDist);

  void setColor(colorargb to);

  bool intersectRay(fVec3 from, fVec3 dir, float& at, int& faceId);

  void applyTransform(Eigen::Matrix4d trans);
};*/

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