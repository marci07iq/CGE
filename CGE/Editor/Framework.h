#pragma once

#include "Editor.h"

class Editor;

class EditorPlugin {
public:
  Editor* _editor;

  Graphics::ElemHwnd _ribbonElement = NULL;
  Graphics::ElemHwnd _toolbarElement = NULL;
  
  Graphics::PanelHwnd _config = NULL;

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
  virtual int mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down, bool in) {
    return 0;
  }
  //virtual void doCarve();
  virtual int guiEventManager(gui_event& evt, int mx, int my, set<key_location>& down, bool in) {
    return 0;
  }

  virtual string getClassName() {
    return "EditorPlugin";
  }

  void hideConfig();
  void showConfig();

  virtual void onAdded() {
  }
  virtual void onActivated() {

  }
  virtual void onDeactivated() {

  }
};

extern Editor mainEditor;
