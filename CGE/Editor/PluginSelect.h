#pragma once

#include "Framework.h"

class PluginSelect : public EditorPlugin {
  public:
    set<shared_ptr<Object>> selectedObjects;
    shared_ptr<Object> highlightedObject;

    PluginSelect(Editor* e) : EditorPlugin(e) {
      //_ribbonElement = NGin::Graphics::createPanel("elementToolRibbonTemplate", LocationData(NGin::Graphics::LinearScale(0,0), NGin::Graphics::LinearScale(1, 0), NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 10)), 0xffff0000);
    }

    static void staticInit() {

    }

    int renderManager(int ax, int ay, int bx, int by, std::set<NGin::Graphics::key_location>& down);
    int renderSelection(int ax, int ay, int bx, int by, std::set<NGin::Graphics::key_location>& down);
    int resizeManager(int x, int y);
    int mouseEntryManager(int state);
    int mouseMoveManager(int x, int y, int ox, int oy, std::set<NGin::Graphics::key_location>& down, bool in);
    int guiEventManager(NGin::Graphics::gui_event& evt, int mx, int my, std::set<NGin::Graphics::key_location>& down, bool in);

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