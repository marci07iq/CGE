#pragma once

#include "Editor.h"

class Editor;

class EditorPlugin {
public:
  Editor* _editor;

  NGin::Graphics::ElemHwnd _ribbonElement = NULL;
  NGin::Graphics::ElemHwnd _toolbarElement = NULL;
  
  NGin::Graphics::PanelHwnd _config = NULL;

  EditorPlugin(Editor* e) {
    _editor = e;
    //_ribbonElement = NGin::Graphics::createGUI_T<Panel>("elementToolRibbonTemplate", LocationData(NGin::Graphics::LinearScale(0,0), NGin::Graphics::LinearScale(1, 0), NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 10)), 0xffff0000);
  }

  virtual int renderManager(int ax, int ay, int bx, int by, std::set<NGin::Graphics::key_location>& down) {
    return 0;
  }
  virtual int resizeManager(int x, int y) {
    return 0;
  }
  virtual int mouseEntryManager(int state) {
    return 0;
  }
  virtual int mouseMoveManager(int x, int y, int ox, int oy, std::set<NGin::Graphics::key_location>& down, bool in) {
    return 0;
  }
  //virtual void doCarve();
  virtual int guiEventManager(NGin::Graphics::gui_event& evt, int mx, int my, std::set<NGin::Graphics::key_location>& down, bool in) {
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
