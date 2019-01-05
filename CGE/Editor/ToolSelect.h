#pragma once

#include "../Framework.h"

class PluginSelect : public EditorPlugin {
  public:
    set<Object*> selectedObjects;
    Object* highlightedObject;

    Graphics::ElemHwnd _ribbonElement = NULL;
    Graphics::ElemHwnd _toolbarElement = NULL;

    Graphics::ElemHwnd _config = NULL;

    PluginSelect(Editor* e) : EditorPlugin(e) {
      //_ribbonElement = Graphics::createPanel("elementToolRibbonTemplate", LocationData(LinearScale(0,0), LinearScale(1, 0), LinearScale(0, 0), LinearScale(0, 10)), 0xffff0000);
    }

    int renderManager(int ax, int ay, int bx, int by, set<key_location>& down);
    int resizeManager(int x, int y);
    int mouseEntryManager(int state);
    int mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down);
    //void doCarve();
    int guiEventManager(gui_event evt, int mx, int my, set<key_location>& down);

    string getClassName() {
      return "EditorPlugin";
    }
  };

EditorPlugin* createPluginSelect(Editor* e);