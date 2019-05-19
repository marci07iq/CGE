#include "FilterVAO.h"
#include "TinyPly.cpp"

struct ply_vertex_pos {
  float x, y, z;
};

struct ply_face_coords {
  uint32_t i, j, k;
};

void read_ply(istream& _file, vector<ply_vertex_pos>& _pos, vector<colorargb>& _col, vector<ply_face_coords>& _tris) {
  using namespace tinyply;

  try {
    PlyFile file;
    file.parse_header(_file);

    // Tinyply treats parsed data as untyped byte buffers. See below for examples.
    std::shared_ptr<PlyData> positions, colors, alphas, faces;

    // The header information can be used to programmatically extract properties on elements
    // known to exist in the header prior to reading the data. For brevity of this sample, properties 
    // like vertex position are hard-coded: 
    try { positions = file.request_properties_from_element("vertex", { "x", "y", "z" }); } catch (const std::exception& e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

    try { colors = file.request_properties_from_element("vertex", { "red", "green", "blue" }); } catch (const std::exception& e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }
    try { alphas = file.request_properties_from_element("vertex", { "alpha" }); } catch (const std::exception& e) {};

    // Providing a list size hint (the last argument) is a 2x performance improvement. If you have 
    // arbitrary ply files, it is best to leave this 0. 
    try { faces = file.request_properties_from_element("face", { "vertex_indices" }, 3); } catch (const std::exception& e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

    file.read(_file);

    if (positions) {
      _pos.resize(positions->count);
      if (positions->t == tinyply::Type::FLOAT32) {
        float* data = reinterpret_cast<float*>(positions->buffer.get());
        for (size_t i = 0; i < positions->count; i++) {
          _pos[i] = {
          float(data[3 * i + 0]),
            float(data[3 * i + 1]),
            float(data[3 * i + 2]) };
        }
      } else if (positions->t == tinyply::Type::FLOAT64) {
        double* data = reinterpret_cast<double*>(positions->buffer.get());
        for (size_t i = 0; i < positions->count; i++) {
          _pos[i] = {
            float(data[3 * i + 0]),
            float(data[3 * i + 1]),
            float(data[3 * i + 2]) };
        }
      } else {
        cerr << "This position type is not supported" << endl;
      }
    } else {
      cerr << "No position information!" << endl;
    }


    if (colors && colors->count == positions->count) {
      _col.resize(colors->count, 0xff000000);
      if (alphas && colors->count == alphas->count) {
        if (alphas->t == tinyply::Type::UINT8) {
          uint8_t* data = reinterpret_cast<uint8_t*>(alphas->buffer.get());
          for (size_t i = 0; i < alphas->count; i++) {
            _col[i] = int(data[i]) << 24;
          }
        } else if (colors->t == tinyply::Type::FLOAT32) {
          float* data = reinterpret_cast<float*>(alphas->buffer.get());
          for (size_t i = 0; i < alphas->count / 3; i++) {
            _col[i] = int(data[i] * 255) << 24;
          }
        } else {
          cerr << "This alpha type is not supported" << endl;
        }
      }

      if (colors->t == tinyply::Type::UINT8) {
        uint8_t* data = reinterpret_cast<uint8_t*>(colors->buffer.get());
        for (size_t i = 0; i < colors->count; i++) {
          _col[i] |= (int(data[3 * i]) << 16) | (int(data[3 * i + 1]) << 8) | int(data[3 * i + 2]);
        }
      } else if (colors->t == tinyply::Type::FLOAT32) {
        float* data = reinterpret_cast<float*>(colors->buffer.get());
        for (size_t i = 0; i < colors->count / 3; i++) {
          _col[i] |= (int(data[3 * i] * 255) << 16) | (int(data[3 * i + 1] * 255) << 8) | int(data[3 * i + 2] * 255);
        }
      } else {
        cerr << "This color type is not supported" << endl;
      }
    }

    if (faces->count) {
      _tris.resize(faces->count);
      if (faces->t == tinyply::Type::UINT8) {
        uint8_t* data = reinterpret_cast<uint8_t*>(faces->buffer.get());
        for (size_t i = 0; i < faces->count; i++) {
          _tris[i] = {
            uint32_t(data[3 * i + 0]),
            uint32_t(data[3 * i + 1]),
            uint32_t(data[3 * i + 2]) };
        }
      } else if (faces->t == tinyply::Type::UINT16) {
        uint16_t* data = reinterpret_cast<uint16_t*>(faces->buffer.get());
        for (size_t i = 0; i < faces->count; i++) {
          _tris[i] = {
            uint32_t(data[3 * i + 0]),
            uint32_t(data[3 * i + 1]),
            uint32_t(data[3 * i + 2]) };
        }
      } else if (faces->t == tinyply::Type::UINT32) {
        uint32_t* data = reinterpret_cast<uint32_t*>(faces->buffer.get());
        for (size_t i = 0; i < faces->count; i++) {
          _tris[i] = {
            uint32_t(data[3 * i + 0]),
            uint32_t(data[3 * i + 1]),
            uint32_t(data[3 * i + 2]) };
        }
      } else  if (faces->t == tinyply::Type::INT8) {
        int8_t* data = reinterpret_cast<int8_t*>(faces->buffer.get());
        for (size_t i = 0; i < faces->count; i++) {
          _tris[i] = {
            uint32_t(data[3 * i + 0]),
            uint32_t(data[3 * i + 1]),
            uint32_t(data[3 * i + 2]) };
        }
      } else if (faces->t == tinyply::Type::INT16) {
        int16_t* data = reinterpret_cast<int16_t*>(faces->buffer.get());
        for (size_t i = 0; i < faces->count; i++) {
          _tris[i] = {
            uint32_t(data[3 * i + 0]),
            uint32_t(data[3 * i + 1]),
            uint32_t(data[3 * i + 2]) };
        }
      } else if (faces->t == tinyply::Type::INT32) {
        int32_t* data = reinterpret_cast<int32_t*>(faces->buffer.get());
        for (size_t i = 0; i < faces->count; i++) {
          _tris[i] = {
            uint32_t(data[3 * i + 0]),
            uint32_t(data[3 * i + 1]),
            uint32_t(data[3 * i + 2]) };
        }
      } else {
        cerr << "This face id type is not supported" << endl;
      }
    } else {
      cerr << "No faces. What is this file?" << endl;
    }
  } catch (const std::exception& e) {
    std::cerr << "Caught tinyply exception: " << e.what() << std::endl;
  }
}


void FilterVAO::configure() {
  string path = _params["path"]->get_string(0);

  ifstream in(path, ios::binary);

  vector<ply_vertex_pos> pos;
  vector<colorargb> col;
  vector<ply_face_coords> tris;

  read_ply(in, pos, col, tris);

  float* b_vert = new float[3 * 3 * tris.size()];
  float* b_col = new float[3 * 4 * tris.size()];
  float* b_light = new float[3 * tris.size()];

  int i = 0;
  for (auto&& it : tris) {
    NGin::Graphics::insertVector(b_vert, 9 * i + 0, fVec3(pos[it.i].x, pos[it.i].y, pos[it.i].z));
    NGin::Graphics::insertVector(b_vert, 9 * i + 3, fVec3(pos[it.j].x, pos[it.j].y, pos[it.j].z));
    NGin::Graphics::insertVector(b_vert, 9 * i + 6, fVec3(pos[it.k].x, pos[it.k].y, pos[it.k].z));

    float light = 0.7 + 0.3 * abs(dot(crs(
      fVec3(pos[it.i].x, pos[it.i].y, pos[it.i].z) - fVec3(pos[it.j].x, pos[it.j].y, pos[it.j].z),
      fVec3(pos[it.i].x, pos[it.i].y, pos[it.i].z) - fVec3(pos[it.k].x, pos[it.k].y, pos[it.k].z)).norm(),
      vec3<double>(1, 2, 3).norm()));

    NGin::Graphics::insertColor(b_col, 12 * i + 0, col[it.i], fVec3(light));
    NGin::Graphics::insertColor(b_col, 12 * i + 4, col[it.j], fVec3(light));
    NGin::Graphics::insertColor(b_col, 12 * i + 8, col[it.k], fVec3(light));
    ++i;
  }

  _vao = make_shared<NGin::Gll::PolyVao_Raw>();
  _vao->size = tris.size() * 3;

  glfwMakeContextCurrent(objectPreviewWindowHwnd->rawHwnd);

  glGenBuffers(1, &_vao->vbo_pos);
  glBindBuffer(GL_ARRAY_BUFFER, _vao->vbo_pos);
  glBufferData(GL_ARRAY_BUFFER, tris.size() * 3 * 3 * sizeof(float), b_vert, GL_STATIC_DRAW);

  glGenBuffers(1, &_vao->vbo_col);
  glBindBuffer(GL_ARRAY_BUFFER, _vao->vbo_col);
  glBufferData(GL_ARRAY_BUFFER, tris.size() * 4 * 3 * sizeof(float), b_col, GL_STATIC_DRAW);

  /*glGenBuffers(1, &_obj_lig_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, _obj_lig_vbo);
  glBufferData(GL_ARRAY_BUFFER, _mesh.faces() * 3 * sizeof(float), light, GL_STATIC_DRAW);*/

  glGenVertexArrays(1, &_vao->vao);
  glBindVertexArray(_vao->vao);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, _vao->vbo_pos);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, _vao->vbo_col);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
  /*glEnableVertexAttribArray(2);
  glBindBuffer(GL_ARRAY_BUFFER, _obj_lig_vbo);
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);*/

  glfwMakeContextCurrent(NGin::Graphics::current->rawHwnd);

  delete[] b_vert;
  delete[] b_col;

  _outputs["out"]->castTo<Filter_Resource_VAO>()->_vao = _vao;

}
