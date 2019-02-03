#pragma once

#include "../Defs.h"

struct ply_vertex_pos {
  float x, y, z;
};

struct ply_face_coords {
  uint32_t i, j, k;
};

void write_ply(ostream& _file, vector<ply_vertex_pos>& _pos, vector<colorargb>& _col, vector<ply_face_coords>& _tris, bool _binary);

void read_ply(istream& _file, vector<ply_vertex_pos>& _pos, vector<colorargb>& _col, vector<ply_face_coords>& _tris);