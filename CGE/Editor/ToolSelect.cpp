#include "ToolSelect.h"

int PluginSelect::renderManager(int ax, int ay, int bx, int by, set<key_location>& down) {
  glDepthRange(0.01, 256);
  glClear(GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  glScissor(ax, ay, bx - ax, by - ay);
  glViewport(ax, ay, bx - ax, by - ay);


  _editor->view.cameraEye = _editor->viewCenter + _editor->viewOffset.toCartesian();
  _editor->view.viewport[0] = ax;
  _editor->view.viewport[1] = ay;
  _editor->view.viewport[2] = bx - ax;
  _editor->view.viewport[3] = by - ay;

  float worldM[16];
  float cameraM[16];

  Transpose modview;
  modview.createLook(_editor->view.cameraEye, -_editor->viewOffset.toCartesian().norm());
  modview.transpose();
  modview.read(_editor->view.model_view);
  modview.read(worldM);

  Transpose camview;
  camview.setIdentity();
  camview.project(PI / 2, (bx - ax)*1.0f / (by - ay), 256, 0.01);
  //camview.ortho(ax, bx, ay, by, 256, 0.01);
  camview.transpose();
  camview.read(_editor->view.projection);

  camview.matrix = modview.matrix * camview.matrix;

  camview.read(cameraM);

  baseShader.bind();

  GLint loc = glGetUniformLocation(baseShader._pID, "transform");
  if (loc != -1) {
    glUniformMatrix4fv(loc, 1, false, cameraM);
  }

  for (auto&& it : _editor->objs) {
    it->draw();
  }

  baseShader.unbind();

  edgeShader.bind();

  loc = glGetUniformLocation(edgeShader._pID, "transform");
  if (loc != -1) {
    glUniformMatrix4fv(loc, 1, false, worldM);
  }

  loc = glGetUniformLocation(edgeShader._pID, "transform2");
  if (loc != -1) {
    glUniformMatrix4fv(loc, 1, false, cameraM);
  }

  loc = glGetUniformLocation(edgeShader._pID, "cam_eye");
  if (loc != -1) {
    glUniform3f(loc, _editor->view.cameraEye[0], _editor->view.cameraEye[1], _editor->view.cameraEye[2]);
  }

  loc = glGetUniformLocation(edgeShader._pID, "color");
  if (loc != -1) {
    glUniform4f(loc, 0, 0, 0, 1);
  }

  for (auto&& it : _editor->objs) {
    if (loc != -1) {
      if (selectedObjects.count(it)) {
        if (it == highlightedObject) {
          glUniform4f(loc, 0.8, 0.2, 0.2, 1);
        } else {
          glUniform4f(loc, 1, 0, 0, 1);
        }
      } else {
        if (it == highlightedObject) {
          glUniform4f(loc, 0.2, 0.2, 0.2, 1);
        } else {
          glUniform4f(loc, 0, 0, 0, 1);
        }
      }
    }
    it->drawEdges();
  }

  edgeShader.unbind();

  /*if(polyRays.size()) {
  setColor(0x0000ff00);
  glBegin(GL_LINE_STRIP);
  for (auto&& it : polyRays) {
  glVertex3f(it.x, it.y, it.z);
  }
  glVertex3f(polyRays.front().x, polyRays.front().y, polyRays.front().z);
  Gll::gllEnd();
  }*/
  Graphics::resetViewport();

  //Graphics::requestRedraw();
  return 0;
}
int PluginSelect::resizeManager(int x, int y) {
  return 0;
}
int PluginSelect::mouseEntryManager(int state) {
  return 0;
}
int PluginSelect::mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down) {
  GLdouble pos3D_ax = 0, pos3D_ay = 0, pos3D_az = 0;

  // get 3D coordinates based on window coordinates
  gluUnProject(x, y, 0,
    _editor->view.model_view, _editor->view.projection, _editor->view.viewport,
    &pos3D_ax, &pos3D_ay, &pos3D_az);

  vec3<double> raydir = vec3<double>{ pos3D_ax, pos3D_ay, pos3D_az } -_editor->view.cameraEye;

  float dist = INFINITY;
  Object* oldhighlight = highlightedObject;
  highlightedObject = NULL;

  for (auto&& it : _editor->objs) {
    float nDist = INFINITY;
    if (it->intersectRay(_editor->view.cameraEye, raydir, nDist)) {
      if (nDist < dist) {
        dist = nDist;
        highlightedObject = it;
      }
    }
  }

  if (_editor->drawDown) {
    polar_vec3 right = _editor->viewOffset;
    right.phi -= HALF_PI;
    right.theta = HALF_PI;
    polar_vec3 top = _editor->viewOffset;
    top.theta += HALF_PI;

    _editor->viewCenter += right.toCartesian() * (x - ox) * 0.003;
    _editor->viewCenter += top.toCartesian() * (y - oy) * 0.003;
    /*GLdouble pos3D_ax = 0, pos3D_ay = 0, pos3D_az = 0;

    // get 3D coordinates based on window coordinates

    gluUnProject(x, y, 0,
    view.model_view, view.projection, view.viewport,
    &pos3D_ax, &pos3D_ay, &pos3D_az);

    vec3<double> rayori = { pos3D_ax, pos3D_ay, pos3D_az };

    polyRays.push_back(rayori);*/
  }

  if (_editor->turnDown) {
    _editor->viewOffset.phi -= (x - ox)*0.005;
    _editor->viewOffset.theta += (y - oy)*0.005;
    _editor->viewOffset.reNormalize();
   // _editor->polyRays.clear();
  }

  if (oldhighlight != highlightedObject || _editor->turnDown || _editor->drawDown) {
    return 1;
  }
}
int PluginSelect::guiEventManager(gui_event evt, int mx, int my, set<key_location>& down) {

  if (evt._key._type == evt._key.type_mouse) {
    if (evt._type == evt.evt_down) {
      if (evt._key._keycode == 1) {
        _editor->turnDown = true;
        return 1;
      }
      if (evt._key._keycode == 0) {
        _editor->drawDown = true;
        //polyRays.clear();

        if (!down.count(key_location(key(GLFW_KEY_LEFT_SHIFT, evt._key.type_key)))) {
          selectedObjects.clear();
        }
        if (highlightedObject) {
          selectedObjects.insert(highlightedObject);
        }

        /*polyRays.push_front(rayori);*/
        return 1;
      }
    }
    if (evt._type == evt.evt_up) {
      if (evt._key._keycode == 1) {
        _editor->turnDown = false;
        return 1;
      }
      if (evt._key._keycode == 0) {
        _editor->drawDown = false;
        //doCarve();
        return 1;
      }
    }
  }
  if (evt._key._type == evt._key.type_wheel) {
    _editor->viewOffset.r *= 1 - 0.03*evt._key._keycode;
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
  return 0;
}

void doCarve() {
  /*if (polyRays.size() >= 3) {
    Object obj;
    obj.setCone(polyRays, view.cameraEye, -viewOffset.toCartesian(), 0.1, 100);
    obj.setShared(reinterpret_cast<void*>(0xff0000));
    auto it = objs.begin();
    while (it != objs.end()) {
      Object* res = new Object(**it - obj); //cut
      (*it)->clean();
      Object* toWipe = *it;
      *it = res;
      delete toWipe;
      res->upload();
      ++it;
    }
  }*/
  //Object* res = new Object(obj);
  //res->upload();
  //objs.push_back(res);
}

EditorPlugin * createPluginSelect(Editor * e) {
  return new PluginSelect(e);
}
