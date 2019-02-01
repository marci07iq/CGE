#pragma once

#include "Framework.h"
#include "PluginSelect.h"

class PluginObject : public EditorPlugin {
  public:
    PluginSelect* selectorPlugin;

    PluginObject(Editor* e) : EditorPlugin(e) {
      //_ribbonElement = Graphics::createPanel("elementToolRibbonTemplate", LocationData(LinearScale(0,0), LinearScale(1, 0), LinearScale(0, 0), LinearScale(0, 10)), 0xffff0000);
    }

    static void staticInit() {

    }

    int renderManager(int ax, int ay, int bx, int by, set<key_location>& down);
    int resizeManager(int x, int y);
    int mouseEntryManager(int state);
    int mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down, bool in);
    int guiEventManager(gui_event evt, int mx, int my, set<key_location>& down, bool in);

    string getClassName() {
      return "PluginBoolean";
    }

    void onAdded();
    void onActivated();
    void onDeactivated();
  };

EditorPlugin* createPluginObject(Editor* e, bool staticInit);