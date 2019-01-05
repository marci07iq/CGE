#pragma once

#include "Renderer/Transpose.h"

extern Shader baseShader;
extern Shader edgeShader;


class Editor;

class EditorPlugin {
public:
  Editor* _editor;

  Graphics::ElemHwnd _ribbonElement = NULL;
  Graphics::ElemHwnd _toolbarElement = NULL;
  
  Graphics::ElemHwnd _config = NULL;

  EditorPlugin(Editor* e) {
    _editor = e;
    //_ribbonElement = Graphics::createPanel("elementToolRibbonTemplate", LocationData(LinearScale(0,0), LinearScale(1, 0), LinearScale(0, 0), LinearScale(0, 10)), 0xffff0000);
  }

  virtual int renderManager(int ax, int ay, int bx, int by, set<key_location>& down) {
    return 0;
  }
  virtual int resizeManager(int x, int y) {
    return 0;
  }
  virtual int mouseEntryManager(int state) {
    return 0;
  }
  virtual int mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down) {
    return 0;
  }
  //virtual void doCarve();
  virtual int guiEventManager(gui_event evt, int mx, int my, set<key_location>& down) {
    return 0;
  }

  virtual string getClassName() {
    return "EditorPlugin";
  }

  virtual void onAdded() {
  }
};

typedef EditorPlugin*(*PluginCreator)(Editor*);

class Editor {
public:
  static Texture _editorIcons;
  //Plugin types
  static map<string, PluginCreator> _pluginTypes;
  static void registerPlugin(string name, PluginCreator creator) {
    _pluginTypes[name] = creator;
  }


  map<string, EditorPlugin*> _plugins;

  list<EditorPlugin*> _pluginStack;

  Graphics::CanvasHwnd _main;

  Graphics::TablerowHwnd _toolribbon; //Ribbon bar up top; for small button
  Graphics::TableHwnd _toolbar; //Ribbon bar up top; for small button
  Graphics::PanelHwnd _config; //Sidebar

  list<Object*> objs;

  OpenGLData view;

  bool drawDown;
  bool turnDown;

  //list<fVec3> polyRays;
  polar_vec3 viewOffset = { 5,1,1 };
  fVec3 viewCenter;

  Editor() {

  }

  void init(Graphics::CanvasHwnd main, Graphics::TablerowHwnd toolribbon, Graphics::PanelHwnd config) {
    _main = main;
    _toolribbon = toolribbon;
    _config = config;

    Object* o1 = new Object();
    o1->setCube();
    o1->setShared(reinterpret_cast<void*>(0xffff0000));

    o1->upload();
    objs.push_back(o1);

    Object* o2 = new Object();
    o2->setCube({ 2,0.5,0.2 }, { 0,0,0 });
    o2->setShared(reinterpret_cast<void*>(0xff00ff00));

    o2->upload();
    objs.push_back(o2);

    Object* o3 = new Object();
    o3->setCube({ 0.2,1,2 }, { 0,1,0 });
    o3->setShared(reinterpret_cast<void*>(0xff0000ff));

    o3->upload();
    objs.push_back(o3);

    if (_editorIcons.id == 0) {
      _editorIcons.load("html/icons.png");
    }
  }

  EditorPlugin* findPlugin(string name) {
    if (_plugins.count(name)) {
      return _plugins[name];
    }
    EditorPlugin* newPlugin = _plugins[name] = _pluginTypes[name](this);
    newPlugin->onAdded();
    return newPlugin;
  }

  void registerRibbon(Graphics::ElemHwnd elem) {
    Graphics::addElement(_toolribbon, elem);
  }
  void removeRibbon(Graphics::ElemHwnd elem) {
    _toolribbon->data.remove(elem);
    _toolribbon->getRect();
  }

  void registerSidebar(Graphics::ElemHwnd elem) {
    Graphics::addElement(_toolribbon, elem);
  }
  void removeSidebar(Graphics::ElemHwnd elem) {
    _toolbar->data.remove(elem);
    _toolbar->getRect();
  }

  void pushPlugin(EditorPlugin* plugin) {
    _pluginStack.push_front(plugin);
  }
  bool popPlugin(EditorPlugin* plugin) {
    if(plugin == _pluginStack.front()) {
      _pluginStack.pop_front();
      return true;
    }
    return false;
  }


  void normalizeAngles() {

  }
  int renderManager(int ax, int ay, int bx, int by, set<key_location>& down) {
    int res = 0;
    for (auto&& it : _pluginStack) {
      res |= it->renderManager(ax, ay, bx, by, down);
      if (res & 2) {
        return res;
      }
    }
    return res;
  }
  int resizeManager(int x, int y) {
    int res = 0;
    for (auto&& it : _pluginStack) {
      res |= it->resizeManager(x, y);
      if (res & 2) {
        return res;
      }
    }
    return res;
  }
  int mouseEntryManager(int state) {
    int res = 0;
    for (auto&& it : _pluginStack) {
      res |= it->mouseEntryManager(state);
      if (res & 2) {
        return res;
      }
    }
    return res;
  }
  int mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down) {
    int res = 0;
    for (auto&& it : _pluginStack) {
      res |= it->mouseMoveManager(x, y, ox, oy, down);
      if (res & 2) {
        return res;
      }
    }
    return res;
  }
  int guiEventManager(gui_event evt, int mx, int my, set<key_location>& down) {
    int res = 0;
    for (auto&& it : _pluginStack) {
      res |= it->guiEventManager(evt, mx, my, down);
      if (res & 2) {
        return res;
      }
    }
    return res;
  }
};

enum Icons {
  //Global
  Icon_New = 0,
  Icon_Open = 1,
  Icon_Save = 2,
  Icon_Close = 3,
  Icon_Resize = 4,

};