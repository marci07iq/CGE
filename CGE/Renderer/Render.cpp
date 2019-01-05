#include "Render.h"

Editor mainEditor;

GLuint textures;
GLuint texSampler;

list<Object*> objs;
set<Object*> selectedObjects;
Object* highlightedObject;

namespace MainGameCanvas {
  
  int renderManager(int ax, int ay, int bx, int by, set<key_location>& down) {
    return mainEditor.renderManager(ax, ay, bx, by, down);
  }
  int resizeManager(int x, int y) {
    return mainEditor.resizeManager(x, y);
  }
  int mouseEntryManager(int state) {
    return mainEditor.mouseEntryManager(state);
  }
  int mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down) {
    return mainEditor.mouseMoveManager(x, y, ox, oy, down);
  }
  int guiEventManager(gui_event evt, int mx, int my, set<key_location>& down, Canvas* me) {
    return mainEditor.guiEventManager(evt, mx, my, down);
  }
  /*void doCarve() {
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
  }*/
}