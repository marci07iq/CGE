#pragma once

#include "../Filter.h"

class FilterTexture : public Filter {
  FrameBuffer _img;
  NGin::Graphics::Texture _tex;

public:
  static NGin::Graphics::Shader _texshader;
  static GLint _texshader_uniform_texture;

  static void staticInit() {
    static bool first = true;
    if (first) {
      first = false;

      _texshader = make_shared<NGin::Graphics::Shader_Raw>("Filters/Shaders/FilterTexture");
      _texshader_uniform_texture = glGetUniformLocation(_texshader->_pID, "uniform_texture");
    }
  }

  FilterTexture(weak_ptr<EditorContext> ctx) : Filter(ctx) {
    staticInit();
  }

  void init() {
    weak_ptr<Filter> me = weak_from_this();
    addParam("resolution", "Resolution", "Resolution of output stream", Filter_Resource::Type_Object, false, NGin::Graphics::getIcon("resolution", ilfPath));

    addOutput("out", "Out", "Output image", Filter_Resource_IO_Base::Restriction_Dynamic, make_shared<Filter_Resource_RenderBuffer>(nullptr), NGin::Graphics::getIcon("texture", ilfPath));

    updateSize();
  }

  void configure() {
    shared_ptr<Filter_Resource_Object> res = _params["resolution"]->castTo<Filter_Resource_Object>();

    _img = make_shared<Raw_FrameBuffer>(res->get_fVec2(fVec2(0, 0)));
    _img->attachColor(0, GL_RGBA, GL_UNSIGNED_BYTE);
    assert(_img->valid());

    _outputs["out"]->castTo<Filter_Resource_RenderBuffer>()->_source = _img;
    _outputs["out"]->castTo<Filter_Resource_RenderBuffer>()->_col_att_ids = { 0 };
  }

  void calculate(float t) {
    float arr[12] = {
      -1, -1,
      -1, 1,
      1, 1,
      1, 1,
      1, -1,
      -1, -1
    };

    float uv[12] = {
      0, 0,
      0, 1,
      1, 1,
      1, 1,
      1, 0,
      0, 0
    };

    GLuint vbo_pos;
    GLuint vbo_uv;
    GLuint vao;

    glGenBuffers(1, &vbo_pos);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), arr, GL_STATIC_DRAW);

    glGenBuffers(1, &vbo_uv);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_uv);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), uv, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_uv);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    _texshader->bind();
    _tex.bind(_texshader_uniform_texture, 0);

    _img->bind();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    _texshader->unbind();
    glDeleteBuffers(1, &vbo_pos);
    glDeleteBuffers(1, &vbo_uv);
    glDeleteVertexArrays(1, &vao);
  }
};