#pragma once

#include "Filter.h"

class FilterColor : public Filter {
  FrameBuffer _img;
  colorargb _col;

  static Shader _colshader;
  GLint _colshader_uniform_color;

  void staticInit() {
    static bool first = true;
    if (first) {
      first = false;

      _colshader.create("Color");
      _colshader_uniform_color = glGetUniformLocation(_colshader._pID, "uniform_color");
    }
  }

  FilterColor(iVec2 size) {
    _img = make_shared<Raw_FrameBuffer>(size);
    _img->attachColor(0, GL_RGB, GL_UNSIGNED_BYTE);
    assert(_img->valid());
  }

  void calculate(const GlobalFrameData& data) {
    float arr[12] = {
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

    float cols[4];//RGBA
    cols[3] = ((_col >> 24) & 0xff) / 255.0;
    cols[0] = ((_col >> 16) & 0xff) / 255.0;
    cols[1] = ((_col >>  8) & 0xff) / 255.0;
    cols[2] = ((_col      ) & 0xff) / 255.0;
    
    _colshader.bind();
    glUniform4f(_colshader_uniform_color, cols[0], cols[1], cols[2], cols[3]);

    _img->bind();
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    _colshader.unbind();
    glDeleteBuffers(1, &vbo_pos);
    glDeleteVertexArrays(1, &vao);
  }
};