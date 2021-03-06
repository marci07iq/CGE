#pragma once

#include "../Filter.h"

class FilterColor : public Filter {
public:
  FrameBuffer _img;
  colorargb _col;

  static NGin::Graphics::Shader _colshader;
  static GLint _colshader_uniform_color;

  static void staticInit() {
    static bool first = true;
    if (first) {
      first = false;

      _colshader = make_shared<NGin::Graphics::Shader_Raw>("Filters/Shaders/FilterColor");
      _colshader_uniform_color = glGetUniformLocation(_colshader->_pID, "uniform_color");
    }
  }

  FilterColor(weak_ptr<EditorContext> ctx) : Filter(ctx) {
    staticInit();

    _col = 0xffff00ff;

  }

  void init() {
    weak_ptr<Filter> me = weak_from_this();
    addParam("resolution", "Resolution", "Resolution of output stream", Filter_Resource::Type_Object, false, NGin::Graphics::getIcon("resolution", ilfPath));

    addOutput("out", "Out", "Output image", Filter_Resource_IO_Base::Restriction_Dynamic, make_shared<Filter_Resource_RenderBuffer>(nullptr), NGin::Graphics::getIcon("out", ilfPath));

    updateSize();
  }

  void configure() {
    //cout << _params["resolution"]->get_fVec2(0);
    _img = make_shared<Raw_FrameBuffer>(_params["resolution"]->get_fVec2(0));
    _img->attachColor(0, GL_RGBA, GL_UNSIGNED_BYTE);
    _img->attachDepth();
    assert(_img->valid());

    _outputs["out"]->castTo<Filter_Resource_RenderBuffer>()->_source = _img;
    _outputs["out"]->castTo<Filter_Resource_RenderBuffer>()->_col_att_ids = { 0 };
  }

  void calculate(float t) {
    if (_img == nullptr) return;

    float cols[4];//RGBA
    cols[3] = ((_col >> 24) & 0xff) / 255.0;
    cols[0] = 0.5 + 0.5 * sin((t + 0.0 / 3) * CONS_TWO_PI);//((_col >> 16) & 0xff) / 255.0;
    cols[1] = 0.5 + 0.5 * sin((t + 1.0 / 3) * CONS_TWO_PI);//((_col >>  8) & 0xff) / 255.0;
    cols[2] = 0.5 + 0.5 * sin((t + 2.0 / 3) * CONS_TWO_PI);//((_col      ) & 0xff) / 255.0;

    /*float arr[12] = {
      -1, -1,
      -1, 1,
      1, 1,
      1, 1,
      1, -1,
      -1, -1
    };

    GLuint vbo_pos;
    GLuint vao;

    glGenBuffers(1, &vbo_pos);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), arr, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    
    _colshader->bind();
    glUniform4f(_colshader_uniform_color, cols[0], cols[1], cols[2], cols[3]);*/

    _img->bind();
    glClearColor(cols[0], cols[1], cols[2], cols[3]);
    _img->clear();

    /*glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    _colshader->unbind();
    glDeleteBuffers(1, &vbo_pos);
    glDeleteVertexArrays(1, &vao);*/
  }
};