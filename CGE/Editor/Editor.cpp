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
  }
}

void Editor::init(Graphics::CanvasHwnd main, Graphics::TablerowHwnd toolribbon, Graphics::TableHwnd sidebar, Graphics::PanelHwnd config) {
  _main = main;
  _toolribbon = toolribbon;
  _toolbar = sidebar;
  _config = config;

  shared_ptr<Object_Raw> o1 = make_shared<Object_Raw>();
  o1->setCube({1,1,1},{0,0,0});
  o1->setColor(0xffff0000);

  /*o1->upload();
  objs.push_back(o1);*/

  shared_ptr<Object_Raw> o2 = make_shared<Object_Raw>();
  o2->setCube({ 2,0.5,0.2 }, { 0,0,0 });
  o2->setColor(0xff00ff00);

  /*o2->upload();
  objs.push_back(o2);*/

  shared_ptr<Object_Raw> o4 = make_shared<Object_Raw>();
  Mesh::booleanSubtract({&o1->_mesh}, { &o2->_mesh }, o4->_mesh);
  o4->upload();
  objs.push_back(o4);

  shared_ptr<Object_Raw> o3 = make_shared<Object_Raw>();
  o3->setCube({ 0.2,1,2 }, { 0,1,0 });
  o3->setColor(0xff0000ff);

  o3->upload();
  objs.push_back(o3);

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
  camview.project(CONS_PI / 2, (bx - ax)*1.0f / (by - ay), 256, 0.01);
  //camview.ortho(ax, bx, ay, by, 256, 0.01);
  //camview.transpose();
  camview.read(view.projection);

  camview.matrix = camview.matrix * modview.matrix;

  //camview.read(cameraM);

  int res = 0;
  if (_currentPlugin) {
    res |= _currentPlugin->renderManager(ax, ay, bx, by, down);
  }

  Graphics::resetViewport();

  return res;

}

void Editor::beginObjectDraw(Transpose objectTransform) {
  _baseShader.bind();

  Transpose objview;
  objview.matrix = camview.matrix * objectTransform.matrix;
  float readMatrix[16];
  objview.read(readMatrix);

  if (_baseShader_transform != -1) {
    glUniformMatrix4fv(_baseShader_transform, 1, false, readMatrix);
  }

}
void Editor::drawObject(shared_ptr<Object> what, colorargb mix, float resAlpha) {
  glUniform4f(_baseShader_mix_color,
    ((mix >> 16) & 0xff) / 255.0,
    ((mix >> 8) & 0xff) / 255.0,
    ((mix >> 0) & 0xff) / 255.0,
    ((mix >> 24) & 0xff) / 255.0);

  glUniform1f(_baseShader_res_alpha,
    resAlpha);

  what->draw();
}
void Editor::endObjectDraw() {
  _baseShader.unbind();
}

void Editor::beginEdgeDraw() {
  _edgeShader.bind();

  if (_edgeShader_transform != -1) {
    float readcamera[16];
    camview.read(readcamera);
    glUniformMatrix4fv(_edgeShader_transform, 1, false, readcamera);
  }

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
void Editor::drawEdge(shared_ptr<Object> what, colorargb edge) {
  glUniform4f(_edgeShader_color,
    ((edge >> 16) & 0xff) / 255.0,
    ((edge >> 8) & 0xff) / 255.0,
    ((edge >> 0) & 0xff) / 255.0,
    ((edge >> 24) & 0xff) / 255.0);

  what->drawEdges();
}
void Editor::endEdgeDraw() {
  _edgeShader.unbind();
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

int Editor::guiEventManager(gui_event evt, int mx, int my, set<key_location>& down, bool in) {
  int res = 0;
  if (_currentPlugin) {
    res |= _currentPlugin->guiEventManager(evt, mx, my, down, in);
    if (res & 2) {
      return res;
    }
  }


  if (evt._key._type == evt._key.type_mouse) {
    if (evt._type == evt.evt_down && in) {
      if (evt._key._keycode == 1) {
        turnDown = true;
        return 1;
      }
      if (evt._key._keycode == 0) {
        drawDown = true;
        //polyRays.clear();

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
  if (evt._key._type == evt._key.type_wheel) {
    viewOffset.r *= 1 - 0.03*evt._key._keycode;
    return 1;
  }
  if (evt._key._type == evt._key.type_key) {
    if (evt._type == evt.evt_pressed) {
      if (evt._key._keycode == 'c') {
        //doCarve();
        //polyRays.clear();
      }
    }
  }

  return res;
}
