#pragma once

#include "Framework.h"

class PluginSelect : public EditorPlugin {
  public:
    set<shared_ptr<Object>> selectedObjects;
    shared_ptr<Object> highlightedObject;

    Graphics::ElemHwnd _ribbonElement = NULL;
    Graphics::ElemHwnd _toolbarElement = NULL;

    Graphics::ElemHwnd _config = NULL;

    PluginSelect(Editor* e) : EditorPlugin(e) {
      //_ribbonElement = Graphics::createPanel("elementToolRibbonTemplate", LocationData(LinearScale(0,0), LinearScale(1, 0), LinearScale(0, 0), LinearScale(0, 10)), 0xffff0000);
    }

    static void staticInit() {

    }

    int renderManager(int ax, int ay, int bx, int by, set<key_location>& down);
    int renderSelection(int ax, int ay, int bx, int by, set<key_location>& down);
    int resizeManager(int x, int y);
    int mouseEntryManager(int state);
    int mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down);
    int guiEventManager(gui_event evt, int mx, int my, set<key_location>& down);   

    string getClassName() {
      return "PluginSelect";
    }

    void onAdded();
    void onActivated();
    void onDeactivated();

    void selectAll() {
      selectedObjects.insert(_editor->objs.begin(), _editor->objs.end());
    }
    void deselectAll() {
      selectedObjects.clear();
    }
  };

EditorPlugin* createPluginSelect(Editor* e, bool staticInit);