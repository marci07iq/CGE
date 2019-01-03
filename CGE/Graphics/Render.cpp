#include "Render.h"

Shader baseShader;
Shader edgeShader;


GLuint textures;
GLuint texSampler;

list<Object*> objs;
set<Object*> selectedObjects;

namespace MainGameCanvas {
  int mxold;
  int myold;

  OpenGLData view;

  bool drawDown;
  bool turnDown;
  bool dragDown;

  list<fVec3> polyRays;
  polar_vec3 viewOffset = {1,0,0};
  fVec3 viewCenter;

  void normalizeAngles() {

  }
  int renderManager(int ax, int ay, int bx, int by, set<key_location>& down) {
    glDepthRange(0.01, 256);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glScissor(ax, ay, bx-ax, by-ay);
    glViewport(ax, ay, bx - ax, by - ay);

    
    view.cameraEye = viewCenter + viewOffset.toCartesian();
    view.viewport[0] = ax;
    view.viewport[1] = ay;
    view.viewport[2] = bx - ax;
    view.viewport[3] = by - ay;

    float worldM[16];
    float cameraM[16];

    Transpose modview;
    modview.createLook(view.cameraEye,-viewOffset.toCartesian().norm());
    modview.transpose();
    modview.read(view.model_view);
    modview.read(worldM);

    Transpose camview;
    camview.setIdentity();
    camview.project(PI / 2, (bx - ax)*1.0f / (by - ay), 256, 0.01);
    //camview.ortho(ax, bx, ay, by, 256, 0.01);
    camview.transpose();
    camview.read(view.projection);

    camview.matrix = modview.matrix * camview.matrix;

    camview.read(cameraM);

    baseShader.bind();

    GLint loc = glGetUniformLocation(baseShader._pID, "transform");
    if (loc != -1) {
      glUniformMatrix4fv(loc, 1, false, cameraM);
    }

    for (auto&& it : objs) {
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
      glUniform3f(loc, view.cameraEye[0], view.cameraEye[1], view.cameraEye[2]);
    }

    loc = glGetUniformLocation(edgeShader._pID, "color");
    if (loc != -1) {
      glUniform4f(loc, 0, 0, 0, 1);
    }

    for (auto&& it : objs) {
      if (loc != -1) {
        if(selectedObjects.count(it)) {
          glUniform4f(loc, 1, 0, 0, 1);
        } else {
          glUniform4f(loc, 0, 0, 0, 1);
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
  int resizeManager(int x, int y) {
    return 0;
  }
  int mouseEntryManager(int state) {
    return 0;
  }
  int mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down) {
    if (turnDown) {
      viewOffset.phi -= (x-mxold)*0.005;
      viewOffset.theta += (y - myold)*0.005;
      viewOffset.reNormalize();
      polyRays.clear();
    }

    /*if (drawDown) {
      GLdouble pos3D_ax = 0, pos3D_ay = 0, pos3D_az = 0;

      // get 3D coordinates based on window coordinates

      gluUnProject(x, y, 0,
        view.model_view, view.projection, view.viewport,
        &pos3D_ax, &pos3D_ay, &pos3D_az);

      vec3<double> rayori = { pos3D_ax, pos3D_ay, pos3D_az };
 
      polyRays.push_back(rayori);
    }*/

    mxold = x;
    myold = y;
    
    return 1;
  }
  void doCarve() {
    if(polyRays.size() >= 3) {
      Object obj;
      obj.setCone(polyRays, view.cameraEye, -viewOffset.toCartesian(), 0.1, 100);
      obj.setShared(reinterpret_cast<void*>(0xff0000));
      auto it = objs.begin();
      while(it != objs.end()) {
        Object* res = new Object(**it - obj); //cut
        (*it)->clean();
        Object* toWipe = *it;
        *it = res;
        delete toWipe;
        res->upload();
        ++it;
      }
    }
    //Object* res = new Object(obj);
    //res->upload();
    //objs.push_back(res);
  }
  int guiEventManager(gui_event evt, int mx, int my, set<key_location>& down, Canvas* me) {
    if (evt._key._type == evt._key.type_mouse) {
      if (evt._type == evt.evt_down) {
        if (evt._key._keycode == 1) {
          turnDown = true;
          return 1;
        }
        if (evt._key._keycode == 0) {
          /*drawDown = true;
          polyRays.clear();*/
          GLdouble pos3D_ax = 0, pos3D_ay = 0, pos3D_az = 0;

          // get 3D coordinates based on window coordinates
          gluUnProject(mx, my, 0,
            view.model_view, view.projection, view.viewport,
            &pos3D_ax, &pos3D_ay, &pos3D_az);

          vec3<double> raydir = vec3<double>{ pos3D_ax, pos3D_ay, pos3D_az } -view.cameraEye;

          float dist = INFINITY;
          Object* obj = NULL;

          for (auto&& it : objs) {
            float nDist = INFINITY;
            if(it->intersectRay(view.cameraEye, raydir, nDist)) {
              if (nDist < dist) {
                dist = nDist;
                obj = it;
              }
            }
          }

         
          if (!down.count(key_location(key(0, evt._key.type_special)))) {
            selectedObjects.clear();
          }
          if (obj) {
            selectedObjects.insert(obj);
          }
          
          /*polyRays.push_front(rayori);*/
          return 1;
        }
      }
      if (evt._type == evt.evt_up) {
        if (evt._key._keycode == 1) {
          turnDown = false;
          return 1;
        }
        /*if (evt._key._keycode == 0) {
          drawDown = false;
          doCarve();
          return 1;
        }*/
      }
    }
    if (evt._key._type == evt._key.type_wheel) {
      viewOffset.r *= 1 - 0.03*evt._key._keycode;
      return 1;
    }
    if(evt._key._type == evt._key.type_key) {
      if (evt._type == evt.evt_pressed) {
        if (evt._key._keycode == 'c') {
          //doCarve();
          polyRays.clear();
        }
      }
    }
  }
}
