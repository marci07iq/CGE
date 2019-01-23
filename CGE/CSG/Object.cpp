#include "Object.h"

Object::Object() {

}

void Object::compile() {

}
void Object::upload() {

}

void Object::draw() {

}
void Object::drawEdges() {

}

/*void Object::setCone(list<fVec3> points, fVec3 top, fVec3 dir, float nearDist, float farDist) {

}*/


bool Object::intersectRay(fVec3 from, fVec3 dir, float & at) {
  return false;
}

void Object::clean() {

}



Object_Raw::Object_Raw() {

}

void Object_Raw::setColor(colorargb to) {
  _color = to;
}

void Object_Raw::compile() {
  
}
void Object_Raw::upload() {
  clean();
  compile();

  float* vert = new float[3 * 3 * _mesh.faces()];
  float* col = new float[3 * 3 * _mesh.faces()];

  for (int i = 0; i < _mesh.faces(); i++) {
    insertVector(vert, 9 * i + 0, _mesh.vertex(i, 0));
    insertVector(vert, 9 * i + 3, _mesh.vertex(i, 1));
    insertVector(vert, 9 * i + 6, _mesh.vertex(i, 2));

    fVec3 light = fVec3(0.7 + 0.3*dot(crs(
       _mesh.vertex(i, 0) -  _mesh.vertex(i, 1),
       _mesh.vertex(i, 0) -  _mesh.vertex(i, 2)).norm(),
      vec3<double>(1, 2, 3).norm()));

    insertColor(col, 9 * i + 0, _color, light);
    insertColor(col, 9 * i + 3, _color, light);
    insertColor(col, 9 * i + 6, _color, light);
  }

  glGenBuffers(1, &_obj_pos_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _obj_pos_vbo);
  glBufferData(GL_ARRAY_BUFFER, _mesh.faces() * 3 * 3 * sizeof(float), vert, GL_STATIC_DRAW);

  glGenBuffers(1, &_obj_col_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _obj_col_vbo);
  glBufferData(GL_ARRAY_BUFFER, _mesh.faces() * 3 * 3 * sizeof(float), col, GL_STATIC_DRAW);

  glGenVertexArrays(1, &_obj_vao);
  glBindVertexArray(_obj_vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _obj_pos_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, _obj_col_vbo);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  delete[_mesh.faces() * 3 * 3] vert;
  delete[_mesh.faces() * 3 * 3] col;

  //Edges
  /*
  float* vert_a = new float[_o._edges.size() * 2 * 3];
  float* vert_b = new float[_o._edges.size() * 2 * 3];
  float* vert_c = new float[_o._edges.size() * 2 * 3];
  float* vert_d = new float[_o._edges.size() * 2 * 3];
  float* vert_r = new float[_o._edges.size() * 2 * 1];

  i = 0;
  for (auto&& it : _o._edges) {
    vert_r[i * 2 + 0] = 0.05;
    vert_r[i * 2 + 1] = 0.05;

    insertVector(vert_a, i * 6 + 0, *(it->_vertexs[0]));
    insertVector(vert_b, i * 6 + 0, *(it->_vertexs[1]));
    insertVector(vert_c, i * 6 + 0, *(it->_tris[0]->_vertexs[it->_vertex_ids[0]]));
    insertVector(vert_d, i * 6 + 0, *(it->_tris[1]->_vertexs[it->_vertex_ids[1]]));

    insertVector(vert_a, i * 6 + 3, *(it->_vertexs[1]));
    insertVector(vert_b, i * 6 + 3, *(it->_vertexs[0]));
    insertVector(vert_c, i * 6 + 3, *(it->_tris[1]->_vertexs[it->_vertex_ids[1]]));
    ++i;
  }

  glGenBuffers(1, &_edge_a_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _edge_a_vbo);
  glBufferData(GL_ARRAY_BUFFER, _o._edges.size() * 2 * 3 * sizeof(float), vert_a, GL_STATIC_DRAW);

  glGenBuffers(1, &_edge_b_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _edge_b_vbo);
  glBufferData(GL_ARRAY_BUFFER, _o._edges.size() * 2 * 3 * sizeof(float), vert_b, GL_STATIC_DRAW);

  glGenBuffers(1, &_edge_c_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _edge_c_vbo);
  glBufferData(GL_ARRAY_BUFFER, _o._edges.size() * 2 * 3 * sizeof(float), vert_c, GL_STATIC_DRAW);

  glGenBuffers(1, &_edge_d_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _edge_d_vbo);
  glBufferData(GL_ARRAY_BUFFER, _o._edges.size() * 2 * 3 * sizeof(float), vert_d, GL_STATIC_DRAW);

  glGenBuffers(1, &_edge_t_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _edge_t_vbo);
  glBufferData(GL_ARRAY_BUFFER, _o._edges.size() * 2 * 1 * sizeof(float), vert_r, GL_STATIC_DRAW);


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


  delete[_o._edges.size() * 2 * 3] vert_a;
  delete[_o._edges.size() * 2 * 3] vert_b;
  delete[_o._edges.size() * 2 * 3] vert_c;
  delete[_o._edges.size() * 2 * 3] vert_d;
  delete[_o._edges.size() * 2 * 1] vert_r;*/
}

void Object_Raw::draw() {
  glBindVertexArray(_obj_vao);
  glDrawArrays(GL_TRIANGLES, 0, _mesh.faces() * 3);
}
void Object_Raw::drawEdges() {
  /*glBindVertexArray(_edge_vao);
  glDrawArrays(GL_LINES, 0, _o._edges.size() * 2);*/
}

void Object_Raw::setCube(fVec3 radius, fVec3 center) {
  _mesh._V = Eigen::Matrix<double, 8,3>();
  _mesh._V <<
    center.x - radius.x, center.y - radius.y, center.z - radius.z,
    center.x - radius.x, center.y - radius.y, center.z + radius.z,
    center.x - radius.x, center.y + radius.y, center.z - radius.z,
    center.x - radius.x, center.y + radius.y, center.z + radius.z,
    center.x + radius.x, center.y - radius.y, center.z - radius.z,
    center.x + radius.x, center.y - radius.y, center.z + radius.z,
    center.x + radius.x, center.y + radius.y, center.z - radius.z,
    center.x + radius.x, center.y + radius.y, center.z + radius.z;

  _mesh._F = Eigen::Matrix<int, 12, 3>();
  _mesh._F <<
    0,1,3,
    0,3,2,
    0,4,5,
    0,5,1,
    0,2,6,
    0,6,4,
    2,3,7,
    2,7,6,
    4,6,7,
    4,7,5,
    1,5,7,
    1,7,3;

}

bool Object_Raw::intersectRay(fVec3 from, fVec3 dir, float & at) {
  return _mesh.intersectRay(from, dir, at);
}

void Object_Raw::clean() {
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


/*
Object_Raw operator|(Object& lhs, Object& rhs) {
  shared_ptr<CSGNode> a = make_shared<CSGNode>(lhs.clone()._polygons);
  shared_ptr<CSGNode> b = make_shared<CSGNode>(rhs.clone()._polygons);

  a->clipTo(b);
  b->clipTo(a);
  b->invert();
  b->clipTo(a);
  b->invert();
  a->build(b->allPolygons());
  return Object_Raw(a->allPolygons());
}

Object_Raw operator+(Object& lhs, Object& rhs) {
  shared_ptr<CSGNode> a = make_shared<CSGNode>(lhs.clone()._polygons);
  shared_ptr<CSGNode> b = make_shared<CSGNode>(rhs.clone()._polygons);

  a->clipTo(b);
  b->clipTo(a);
  b->invert();
  b->clipTo(a);
  b->invert();
  a->build(b->allPolygons());
  return Object_Raw(a->allPolygons());
}

Object_Raw operator-(Object& lhs, Object& rhs) {
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

Object_Raw operator*(Object& lhs, Object& rhs) {
  shared_ptr<CSGNode> a = make_shared<CSGNode>(lhs.clone()._polygons);
  shared_ptr<CSGNode> b = make_shared<CSGNode>(rhs.clone()._polygons);

  a->invert();
  b->clipTo(a);
  b->invert();
  a->clipTo(b);
  b->clipTo(a);
  a->build(b->allPolygons());
  a->invert();
  return Object_Raw(a->allPolygons());
}
*/