#include "Object.h"

Object::Object() {

}

Object::Object(Mesh & _from) {
  _mesh = _from;
}

Object::~Object() {
  clean();
}

void Object::setColor(colorargb to) {
  _mesh.setColor(to);
}

void Object::compile() {
  
}
void Object::upload() {
  clean();
  compile();

  float* vert = new float[3 * 3 * _mesh.faces()];
  float* col = new float[3 * 4 * _mesh.faces()];
  float* light = new float[3 * _mesh.faces()];

  for (int i = 0; i < _mesh.faces(); i++) {
    insertVector(vert, 9 * i + 0, _mesh.vertex(i, 0));
    insertVector(vert, 9 * i + 3, _mesh.vertex(i, 1));
    insertVector(vert, 9 * i + 6, _mesh.vertex(i, 2));

    light[3* i] = light[3 * i + 1] = light[3 * i + 2] = 0.7 + 0.3*dot(crs(
       _mesh.vertex(i, 0) -  _mesh.vertex(i, 1),
       _mesh.vertex(i, 0) -  _mesh.vertex(i, 2)).norm(),
      vec3<double>(1, 2, 3).norm());

    insertColor(col, 12 * i + 0, _mesh._color[i]);
    insertColor(col, 12 * i + 4, _mesh._color[i]);
    insertColor(col, 12 * i + 8, _mesh._color[i]);
  }

  glGenBuffers(1, &_obj_pos_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _obj_pos_vbo);
  glBufferData(GL_ARRAY_BUFFER, _mesh.faces() * 3 * 3 * sizeof(float), vert, GL_STATIC_DRAW);

  glGenBuffers(1, &_obj_col_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _obj_col_vbo);
  glBufferData(GL_ARRAY_BUFFER, _mesh.faces() * 4 * 3 * sizeof(float), col, GL_STATIC_DRAW);
  
  glGenBuffers(1, &_obj_lig_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _obj_lig_vbo);
  glBufferData(GL_ARRAY_BUFFER, _mesh.faces() * 3 * sizeof(float), light, GL_STATIC_DRAW);

  glGenVertexArrays(1, &_obj_vao);
  glBindVertexArray(_obj_vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _obj_pos_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, _obj_col_vbo);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, _obj_lig_vbo);
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);

  delete[_mesh.faces() * 3 * 3] vert;
  delete[_mesh.faces() * 4 * 3] col;
  delete[_mesh.faces() * 3] light;

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

void Object::draw() {
  if (_obj_vao != 0) {
    glBindVertexArray(_obj_vao);
    glDrawArrays(GL_TRIANGLES, 0, _mesh.faces() * 3);
  }
}
void Object::drawEdges() {
  /*glBindVertexArray(_edge_vao);
  glDrawArrays(GL_LINES, 0, _o._edges.size() * 2);*/
}

//IO

void Object::set(const DataElement * from) {
  std::stringbuf ss;
  ss.sputn(reinterpret_cast<const char*>(from->_core->_data), from->_core->_len);
  
  assert(from->_children.size() > 0);
  assert(from->_children[0]->_core->_len == 16*sizeof(float));
  memcpy(_offset.matrix._vals, from->_children[0]->_core->_data, 16 * sizeof(float));
    
  istream inPart(&ss);
  _mesh.readPly(inPart);
}

const void Object::get(DataElement * to) {
  std::stringbuf ss;
  ostream outPart(&ss);
  _mesh.writePly(outPart, false, false);

  string str = ss.str();

  delete to->_core;
  to->_core = new DataPair(str.length());
  memcpy(to->_core->_data, str.c_str(), str.length());

  DataElement* mat = new DataElement();
  delete mat->_core;
  mat->_core = new DataPair(16 * sizeof(float));
  memcpy(mat->_core->_data, _offset.matrix._vals, 16 * sizeof(float));
  
  to->addChild(mat);
}

void Object::setCube(fVec3 radius, fVec3 center) {
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

    _mesh.setColor(0xffff00ff);
}
void Object::setCylinder(fVec3 radius, fVec3 center) {
  const static int parts = 20;
  
  _mesh._V = Eigen::Matrix<double, 2 * parts + 2, 3>();
  _mesh._F = Eigen::Matrix<int, 4 * parts, 3>();

  _mesh._V.block<1, 3>(2 * parts, 0) << center.x, center.y, center.z - radius.z;
  _mesh._V.block<1, 3>(2 * parts + 1, 0) << center.x, center.y, center.z + radius.z;

  for (int i = 0; i < parts; i++) {
    fVec3 lowPos = center + radius * fVec3( cos(i * CONS_TWO_PI / parts), sin(i * CONS_TWO_PI / parts),-1 );
    _mesh._V.block<1, 3>(i, 0) << lowPos.x, lowPos.y, lowPos.z;
    fVec3 highPos = center + radius * fVec3(cos(i * CONS_TWO_PI / parts), sin(i * CONS_TWO_PI / parts), 1 );
    _mesh._V.block<1, 3>(i + parts, 0) << highPos.x, highPos.y, highPos.z;

    _mesh._F.block<1, 3>(i + 0 * parts, 0) << i, 2 * parts, (i + 1) % parts;
    _mesh._F.block<1, 3>(i + 1 * parts, 0) << (i + 1) % parts, i + parts, i;
    _mesh._F.block<1, 3>(i + 2 * parts, 0) << i + parts, (i + 1) % parts, (i + 1) % parts + parts;
    _mesh._F.block<1, 3>(i + 3 * parts, 0) << (i + 1) % parts + parts, 2 * parts + 1, i + parts;
  }

  _mesh.setColor(0xffff00ff);
}

bool Object::intersectRay(fVec3 from, fVec3 dir, float & at, int& faceId) {
  Eigen::Matrix4f inv = Eigen::Map<Eigen::Matrix<float, 4, 4, Eigen::RowMajor>>(_offset.matrix._vals).inverse();
  float oDirLen = dir.length();

  fVec3 from_t = fVec3 (
    from.x * inv(0,0) + from.y * inv(0,1) + from.z * inv(0,2) + inv(0,3),
    from.x * inv(1,0) + from.y * inv(1,1) + from.z * inv(1,2) + inv(1,3),
    from.x * inv(2,0) + from.y * inv(2,1) + from.z * inv(2,2) + inv(2,3)
  );

  fVec3 dir_t = fVec3(
    dir.x * inv(0,0) + dir.y * inv(0,1) + dir.z * inv(0,2),
    dir.x * inv(1,0) + dir.y * inv(1,1) + dir.z * inv(1,2),
    dir.x * inv(2,0) + dir.y * inv(2,1) + dir.z * inv(2,2)
  );

  float rat = oDirLen / dir.length();

  float lat = at * rat;
  int res = _mesh.intersectRay(from_t, dir_t, lat, faceId);
  float nat = lat / rat;
  if (nat < at) {
    at = nat;
  }

  return res;
}

void Object::bakeTransform() {
  _mesh.applyTransform(Eigen::Map<Eigen::Matrix<float, 4, 4, Eigen::RowMajor>>(_offset.matrix._vals).cast<double>());
  _offset.setIdentity();
  upload();
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
  if (_obj_lig_vbo) {
    glDeleteBuffers(1, &_obj_lig_vbo);
    _obj_lig_vbo = 0;
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

/*void Object_CSG::compile() {
  
}*/
