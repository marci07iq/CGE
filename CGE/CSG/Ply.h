#pragma once

#include "../Defs.h"

/*enum Ply_Modes {
  Ply_Mode_ASCII,
  Ply_Mode_LE,
  Ply_Mode_BE,
};

uint8_t Ply_Convert_UI8_UI8(char* data, int offset) {
  return 
}

void loadPly(const string filename) {
  struct Vertex {
    double x, y, z;
    uint8_t r, g, b;
  };

  ifstream in(filename);
  string line;

  size_t vertices;
  size_t faces;

  int vertex_parts[6] = {
    offsetof(Vertex, x), offsetof(Vertex, y), offsetof(Vertex, z),
    offsetof(Vertex, r), offsetof(Vertex, g), offsetof(Vertex, b)};

  struct vertex_def {
    int part_id;
    enum types {
      Vertex_Prop_Char,
      Vertex_Prop_UChar,
      Vertex_Prop_Short,
      Vertex_Prop_UShort,
      Vertex_Prop_Int,
      Vertex_Prop_UInt,

      Vertex_Prop_Float,
      Vertex_Prop_Double,

      Vertex_Prop_Int8,
      Vertex_Prop_UInt8,
      Vertex_Prop_Int16,
      Vertex_Prop_UInt16,
      Vertex_Prop_Int32,
      Vertex_Prop_UInt32,

      Vertex_Prop_Float32,
      Vertex_Prop_Float64
    };
  };

  vector<pair<int, vertex_def>> vertex_componenets; //part ID, binary length

  Ply_Modes mode;

  bool done = false;
  while (!done) {
    getline(in, line);
    if (line.size() && line.find("comment", 0) != 0) { //Not empty/comment
      vector<string> tokens = tokenize(line);
      
      if (tokens[0] == "format") {
        if (tokens[1] == "ascii ") {
          mode = Ply_Mode_ASCII;
        } else if (tokens[1] == "binary_little_endian") {
          mode = Ply_Mode_LE;
        } else if (tokens[1] == "binary_big_endian ") {
          mode = Ply_Mode_BE;
        }

        if (tokens[2] != "1.0") {
          cerr << "This file has an unsupported version. We will still try!" << endl;
        }
      
      } else if (tokens[0] == "element") {
        if (tokens[1] == "vertex ") {
          vertices = strTo<size_t>(tokens[2]);
        } else if (tokens[1] == "face ") {
          faces = strTo<size_t>(tokens[2]);
        }

      }
    }
  }
}*/