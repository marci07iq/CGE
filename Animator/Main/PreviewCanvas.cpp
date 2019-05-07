#include "PreviewCanvas.h"

void PreviewCanvas::setViewport(float aspect) {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDisable(GL_DEPTH_TEST);
  if (aspect > (cbx - cay) * 1.0 / (cby - cay)) {
    glViewport(cax, (cay + cby)/(2.0) - (cbx - cax) / (2.0*aspect), cbx - cax, (cbx - cax) / aspect);
    glScissor(cax, (cay + cby) / (2.0) - (cbx - cax) / (2.0 * aspect), cbx - cax, (cbx - cax) / aspect);
  } else {
    glViewport((cax + cbx) / 2.0 - (cby - cay) * aspect / (2.0), cay, (cby - cay) * aspect, cby - cay);
    glScissor((cax + cbx) / 2.0 - (cby - cay) * aspect / (2.0), cay, (cby - cay) * aspect, cby - cay);
  }
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void PreviewCanvas::render(set<key_location>& down) {
  //_frameData->_time = _frameData->_frameCount * 1.0 / _frameData->_global->_frameRate;

  shared_ptr<Filter_Resource_RenderBuffer> buffer = _inputs["result"]->getAs<Filter_Resource_RenderBuffer>(_time);
  
  float arr[12] = {
      -1.0f, -1.0f,
      -1.0f, +1.0f,
      +1.0f, +1.0f,
      +1.0f, +1.0f,
      +1.0f, -1.0f,
      -1.0f, -1.0f
  };

  float uv[12] = {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f
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

  FilterTexture::_texshader->bind();

  glActiveTexture(GL_TEXTURE0 + 0);
  glBindTexture(GL_TEXTURE_2D, buffer->_source->_col_attachments[buffer->_col_att_ids[0]]);
  glUniform1i(FilterTexture::_texshader_uniform_texture, 0);

  //setViewport(_frameData->_global->_resolution.x * 1.0 / _frameData->_global->_resolution.y);
  setViewport(1920.0 / 1080);
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  FilterTexture::_texshader->unbind();

  glDeleteBuffers(1, &vbo_pos);
  glDeleteBuffers(1, &vbo_uv);
  glDeleteVertexArrays(1, &vao);
  
  NGin::Graphics::resetViewport();

  //_enc->addFrame();

  //++(_frameData->_frameCount);
}
int PreviewCanvas::mouseEnter(int state) {
  return 0;
}
int PreviewCanvas::mouseMoved(int x, int y, int ox, int oy, set<key_location>& down) {
  return 0;
}
int PreviewCanvas::guiEvent(gui_event& evt, int mx, int my, set<key_location>& down) {
  return 0;
}

void PreviewCanvas::finishRender() {
  //_enc->finish();
}
