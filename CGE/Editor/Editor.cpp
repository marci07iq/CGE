#include "Framework.h"

Shader Editor::_baseShader;
GLuint Editor::_baseShader_transform;
GLuint Editor::_baseShader_mix_color;
GLuint Editor::_baseShader_res_alpha;

Shader Editor::_edgeShader;
GLuint Editor::_edgeShader_transform;
//GLuint Editor::_edgeShader_transform2;
GLuint Editor::_edgeShader_cam_eye;
GLuint Editor::_edgeShader_color;

Shader Editor::_checkShader;
GLuint Editor::_checkShader_transform;

Shader Editor::_lineShader;
GLuint Editor::_lineShader_modview;
GLuint Editor::_lineShader_camview;
GLuint Editor::_lineShader_cam_eye;


GLuint Editor::_coordinate_vao;
GLuint Editor::_coordinate_vbo_pos;
GLuint Editor::_coordinate_vbo_col;
GLuint Editor::_coordinate_vbo_rad;

Texture Editor::_editorIcons;
//Plugin types
map<string, PluginCreator> Editor::_pluginTypes;

void Editor::registerPlugin(string name, PluginCreator creator) {
  _pluginTypes[name] = creator;
  creator(NULL, true);
}

Editor::Editor() {

}

void Editor::staticInit() {
  static bool first = true;
  if(first) {
    first = false;

    _baseShader.create("Renderer/Core");
    _baseShader_transform = glGetUniformLocation(_baseShader._pID, "transform");
    _baseShader_mix_color = glGetUniformLocation(_baseShader._pID, "mix_color");
    _baseShader_res_alpha = glGetUniformLocation(_baseShader._pID, "res_alpha");


    _edgeShader.create("Renderer/Edges", 7);
    _edgeShader_transform = glGetUniformLocation(_edgeShader._pID, "transform");
    //_edgeShader_transform2 = glGetUniformLocation(_edgeShader._pID, "transform2");
    _edgeShader_cam_eye = glGetUniformLocation(_edgeShader._pID, "cam_eye");
    _edgeShader_color = glGetUniformLocation(_edgeShader._pID, "color");

    _checkShader.create("Editor/Checkboard");
    _checkShader_transform = glGetUniformLocation(_checkShader._pID, "transform");

    _lineShader.create("Renderer/SimpleLine", 7);
    _lineShader_modview = glGetUniformLocation(_lineShader._pID, "modview");
    _lineShader_camview = glGetUniformLocation(_lineShader._pID, "camview");
    _lineShader_cam_eye = glGetUniformLocation(_lineShader._pID, "cam_eye");

    float pos[18] = {
      0, 0, 0,
      1, 0, 0,
      0, 0, 0,
      0, 1, 0,
      0, 0, 0,
      0, 0, 1,
    };
    float col[24] = {
      1, 0, 0, 1,
      1, 0, 0, 1,
      0, 1, 0, 1,
      0, 1, 0, 1,
      0, 0, 1, 1,
      0, 0, 1, 1,
    };

    float rad[6] = {
      0.02,
      0.02,
      0.02,
      0.02,
      0.02,
      0.02,
    };

    glGenBuffers(1, &_coordinate_vbo_pos);
    glBindBuffer(GL_ARRAY_BUFFER, _coordinate_vbo_pos);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), pos, GL_STATIC_DRAW);

    glGenBuffers(1, &_coordinate_vbo_col);
    glBindBuffer(GL_ARRAY_BUFFER, _coordinate_vbo_col);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), col, GL_STATIC_DRAW);

    glGenBuffers(1, &_coordinate_vbo_rad);
    glBindBuffer(GL_ARRAY_BUFFER, _coordinate_vbo_rad);
    glBufferData(GL_ARRAY_BUFFER,6 * sizeof(float), rad, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &_coordinate_vao);
    glBindVertexArray(_coordinate_vao);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _coordinate_vbo_pos);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, _coordinate_vbo_col);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, _coordinate_vbo_rad);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
  }
}

void Editor::init(Graphics::CanvasHwnd main, Graphics::TablerowHwnd toolribbon, Graphics::TableHwnd sidebar, Graphics::PanelHwnd config) {
  _main = main;
  _toolribbon = toolribbon;
  _toolbar = sidebar;
  _config = config;

  shared_ptr<Object> o1 = make_shared<Object>();
  o1->setCube({1,1,1},{0,0,0});
  o1->setColor(0xff808080);
  o1->upload();
  objs.push_back(o1);

  /*o1->upload();
  objs.push_back(o1);

  /*shared_ptr<Object> o2 = make_shared<Object>();
  o2->setCube({ 2,0.5,0.2 }, { 0,0,0 });
  o2->setColor(0xff00ff00);

  /*o2->upload();
  objs.push_back(o2);

  shared_ptr<Object> o4 = make_shared<Object>();
  Mesh::booleanSubtract({&o1->_mesh}, { &o2->_mesh }, o4->_mesh);
  o4->upload();
  objs.push_back(o4);

  shared_ptr<Object> o3 = make_shared<Object>();
  o3->setCube({ 0.2,1,2 }, { 0,1,0 });
  o3->setColor(0xff0000ff);

  o3->upload();
  objs.push_back(o3);

  shared_ptr<Object> o5 = make_shared<Object>();
  o5->_mesh.readPly("../_TRASH/Model.ply");
  o5->upload();
  objs.push_back(o5);*/

  staticInit();
}

EditorPlugin * Editor::findStaticPlugin(string name) {
  if (_staticPlugins.count(name)) {
    return _staticPlugins[name];
  }
  EditorPlugin* newPlugin = _staticPlugins[name] = _pluginTypes[name](this, false);
  newPlugin->onAdded();
  return newPlugin;
}

void Editor::activateStaticPlugin(EditorPlugin * plugin) {
  if(_currentPlugin != plugin) {
    if(_currentPlugin != NULL) {
      _currentPlugin->onDeactivated();
    }
    _currentPlugin = plugin;
    _config->elements.clear();
    plugin->onActivated();
    //Dont delete element(s), it is the owner plugins job.
  }
}

EditorPlugin * Editor::createDynamicPlugin(string name) {
  EditorPlugin* newPlugin = _pluginTypes[name](this, false);
  newPlugin->onAdded();
  return newPlugin;
}

void Editor::registerRibbon(Graphics::ElemHwnd elem) {
  Graphics::addElement(_toolribbon, elem);
}
void Editor::removeRibbon(Graphics::ElemHwnd elem) {
  _toolribbon->data.remove(elem);
  _toolribbon->getRect();
}

void Editor::registerSidebar(Graphics::ElemHwnd elem) {
  Graphics::addElement(_toolbar, elem);
}
void Editor::removeSidebar(Graphics::ElemHwnd elem) {
  _toolbar->data.remove(elem);
  _toolbar->getRect();
}

/*void Editor::pushPlugin(EditorPlugin * plugin) {
  _pluginStack.push_front(plugin);
}
bool Editor::popPlugin(EditorPlugin * plugin) {
  if (plugin == _pluginStack.front()) {
    _pluginStack.pop_front();
    return true;
  }
  return false;
}*/

int Editor::renderManager(int ax, int ay, int bx, int by, set<key_location>& down) {
  glDepthRange(0.01, 256);
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  view.cameraEye = viewCenter + viewOffset.toCartesian();
  view.viewport[0] = ax;
  view.viewport[1] = ay;
  view.viewport[2] = bx - ax;
  view.viewport[3] = by - ay;
  glViewport(ax, ay, bx-ax, by-ay);

  modview.createLook(view.cameraEye, -viewOffset.toCartesian().norm());
  //modview.transpose();
  modview.read(view.model_view);
  //modview.read(worldM);

  camview.setIdentity();
  camview.project(CONS_PI / 3, (bx - ax)*1.0f / (by - ay), viewOffset.r*256, viewOffset.r*0.01);
  //camview.ortho(ax, bx, ay, by, 256, 0.01);
  //camview.transpose();
  camview.read(view.projection);

  camview.matrix = camview.matrix * modview.matrix;

  //camview.read(cameraM);

  int res = 0;
  if (_currentPlugin) {
    res |= _currentPlugin->renderManager(ax, ay, bx, by, down);
  }

  if (!(res & 2)) {
    drawCoordinateSystem(ax, ay, bx, by);
  }


  Graphics::resetViewport();

  return res;

}
void Editor::drawCoordinateSystem(int ax, int ay, int bx, int by) {
  //Draw squares
  GLuint quadVbo_pos;
  GLuint quadVbo_uv;
  GLuint quadVao;

  float scale = getScale();

  float base[18] = {
    -50 * scale, -50 * scale, 0,
    -50 * scale,  50 * scale, 0,
    50 * scale,  50 * scale, 0,
    50 * scale,  50 * scale, 0,
    50 * scale, -50 * scale, 0,
    -50 * scale, -50 * scale, 0 };
  float uv[12] = {
    -50, -50,
    -50,  50,
    50,  50,
    50,  50,
    50, -50,
    -50, -50 };

  glGenBuffers(1, &quadVbo_pos);
  glBindBuffer(GL_ARRAY_BUFFER, quadVbo_pos);
  glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), base, GL_STATIC_DRAW);

  glGenBuffers(1, &quadVbo_uv);
  glBindBuffer(GL_ARRAY_BUFFER, quadVbo_uv);
  glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), uv, GL_STATIC_DRAW);

  glGenVertexArrays(1, &quadVao);
  glBindVertexArray(quadVao);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, quadVbo_pos);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, quadVbo_uv);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

  _checkShader.bind();

  if (_checkShader_transform != -1) {
    float readMatrix[16];
    camview.read(readMatrix);

    glUniformMatrix4fv(_checkShader_transform, 1, false, readMatrix);
  }

  glBindVertexArray(quadVao);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  _checkShader.unbind();

  glDeleteBuffers(1, &quadVbo_pos);
  glDeleteBuffers(1, &quadVbo_uv);
  glDeleteVertexArrays(1, &quadVao);

  //Draw mini XYZ system
  glDepthRange(0.01, 10);
  glClear(GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);

  glViewport(ax*0.2+bx*0.8, ay, (bx - ax)/5.0, (by - ay)/5.0);

  Transform miniView;

  fVec3 miniCamEye = viewOffset.toCartesian().norm() * 3;

  miniView.createLook(miniCamEye, -viewOffset.toCartesian().norm());
  miniView.project(CONS_PI / 3, (bx - ax)*1.0f / (by - ay), 10, 0.01);
  //miniView.ortho(-2, 2, 2*(by-ay)*1.0/(bx-ax), -2 * (by - ay)*1.0 / (bx - ax), 10, 0.01);

  Transform unit;
  drawXYZ(unit, miniView, miniCamEye);
}
void Editor::drawXYZ(Transform& modview, Transform& camview, fVec3& eye) {
  float mod_matrix_f[16];
  modview.read(mod_matrix_f);

  float cam_matrix_f[16];
  camview.read(cam_matrix_f);


  _lineShader.bind();

  glUniform3f(_lineShader_cam_eye, eye.x, eye.y, eye.z);
  glUniformMatrix4fv(_lineShader_modview, 1, false, mod_matrix_f);
  glUniformMatrix4fv(_lineShader_camview, 1, false, cam_matrix_f);


  glBindVertexArray(_coordinate_vao);
  glDrawArrays(GL_LINES, 0, 6);

  _lineShader.unbind();
}

void Editor::beginObjectDraw() {
  _baseShader.bind();
}
void Editor::drawObject(shared_ptr<Object> what, Matrix4f objectTransform, colorargb mix, float resAlpha) {
  glUniform4f(_baseShader_mix_color,
    ((mix >> 16) & 0xff) / 255.0,
    ((mix >> 8) & 0xff) / 255.0,
    ((mix >> 0) & 0xff) / 255.0,
    ((mix >> 24) & 0xff) / 255.0);

  glUniform1f(_baseShader_res_alpha,
    resAlpha);

  Matrix4f fullTransform = objectTransform;

  glUniformMatrix4fv(_baseShader_transform, 1, true, fullTransform._vals);

  what->draw();
}
void Editor::endObjectDraw() {
  _baseShader.unbind();
}

void Editor::beginEdgeDraw() {
  _edgeShader.bind();

  /*if (_edgeShader_transform2 != -1) {
    glUniformMatrix4fv(_edgeShader_transform2, 1, false, cameraM);
  }*/

  if (_edgeShader_cam_eye != -1) {
    glUniform3f(_edgeShader_cam_eye, view.cameraEye[0], view.cameraEye[1], view.cameraEye[2]);
  }

  /*if (_edgeShader_color != -1) {
  glUniform4f(_edgeShader_color, 0, 0, 0, 1);
  }*/
}
void Editor::drawEdge(shared_ptr<Object> what, Matrix4f objectTransform, colorargb edge) {
  glUniform4f(_edgeShader_color,
    ((edge >> 16) & 0xff) / 255.0,
    ((edge >> 8) & 0xff) / 255.0,
    ((edge >> 0) & 0xff) / 255.0,
    ((edge >> 24) & 0xff) / 255.0);

  Matrix4f fullTransform = camview.matrix * objectTransform;

  glUniformMatrix4fv(_edgeShader_transform, 1, true, fullTransform._vals);

  what->drawEdges();
}
void Editor::endEdgeDraw() {
  _edgeShader.unbind();
}

float Editor::getScale() {
  return pow(10, round(log10(viewOffset.r / 5)));
}

int Editor::resizeManager(int x, int y) {
  int res = 0;
  if (_currentPlugin) {
    res |= _currentPlugin->resizeManager(x, y);
    if (res & 2) {
      return res;
    }
  }
  return res;
}

int Editor::mouseEntryManager(int state) {
  int res = 0;
  if(_currentPlugin) {
    res |= _currentPlugin->mouseEntryManager(state);
    if (res & 2) {
      return res;
    }
  }
  return res;
}

int Editor::mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down, bool in) {
  int res = 0;
  if (_currentPlugin) {
    res |= _currentPlugin->mouseMoveManager(x, y, ox, oy, down, in);
    if (res & 2) {
      return res;
    }
  }

  if (drawDown) {
    polar_vec3 right = viewOffset;
    right.phi -= CONS_HALF_PI;
    right.theta = CONS_HALF_PI;
    polar_vec3 top = viewOffset;
    top.theta += CONS_HALF_PI;

    viewCenter += right.toCartesian() * (x - ox) * 0.003;
    viewCenter += top.toCartesian() * (y - oy) * 0.003;
    /*GLdouble pos3D_ax = 0, pos3D_ay = 0, pos3D_az = 0;

    // get 3D coordinates based on window coordinates

    gluUnProject(x, y, 0,
    view.model_view, view.projection, view.viewport,
    &pos3D_ax, &pos3D_ay, &pos3D_az);

    vec3<double> rayori = { pos3D_ax, pos3D_ay, pos3D_az };

    polyRays.push_back(rayori);*/
    res |= 1;

  }

  if (turnDown) {
    viewOffset.phi -= (x - ox)*0.005;
    viewOffset.theta += (y - oy)*0.005;
    viewOffset.reNormalize();
    // _editor->polyRays.clear();
    res |= 1;
  }

  return res;
}

int Editor::guiEventManager(gui_event& evt, int mx, int my, set<key_location>& down, bool in) {
  int res = 0;
  if (_currentPlugin) {
    res |= _currentPlugin->guiEventManager(evt, mx, my, down, in);
    if (res & 2) {
      return res;
    }
  }


  if (evt._key._type == evt._key.type_mouse) {
    if (!evt.captured && evt._type == evt.evt_down && in) {
      if (evt._key._keycode == 1) {
        turnDown = true;
        evt.captured = true;
        return 1;
      }
      if (evt._key._keycode == 0) {
        drawDown = true;
        //polyRays.clear();
        evt.captured = true;
        return 1;
      }
    }
    if (evt._type == evt.evt_up) {
      if (evt._key._keycode == 1) {
        turnDown = false;
        return 1;
      }
      if (evt._key._keycode == 0) {
        drawDown = false;
        //doCarve();
        return 1;
      }
    }
  }
  if (!evt.captured && evt._key._type == evt._key.type_wheel) {
    viewOffset.r *= 1 - 0.03*evt._key._keycode;
    viewOffset.r = min(max(viewOffset.r, 0.0001), 10000.0);
    evt.captured = true;
    return 1;
  }
  if (evt._key._type == evt._key.type_key) {
    if (!evt.captured && evt._type == evt.evt_pressed) {
      if (evt._key._keycode == 'c') {
        //doCarve();
        //polyRays.clear();
      }
    }
  }

  return res;
}
