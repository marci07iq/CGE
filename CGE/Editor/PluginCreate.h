#pragma once

#include "Framework.h"

void objectPluginCreatePositionXInput(Graphics::ElemHwnd sender, void* plugin);

void objectPluginCreatePositionYInput(Graphics::ElemHwnd sender, void* plugin);

void objectPluginCreatePositionZInput(Graphics::ElemHwnd sender, void* plugin);


class PluginCreate : public EditorPlugin {
  public:
    Graphics::ElemHwnd _ribbonElement = NULL;
    Graphics::ElemHwnd _toolbarElement = NULL;

    Graphics::ElemHwnd _config = NULL;

    Object_Raw _temp;

    PluginCreate(Editor* e) : EditorPlugin(e) {
      //_ribbonElement = Graphics::createPanel("elementToolRibbonTemplate", LocationData(LinearScale(0,0), LinearScale(1, 0), LinearScale(0, 0), LinearScale(0, 10)), 0xffff0000);
    }

    static void staticInit() {
      Graphics::setName("objectPluginCreatePositionXInput", objectPluginCreatePositionXInput);
      Graphics::setName("objectPluginCreatePositionYInput", objectPluginCreatePositionYInput);
      Graphics::setName("objectPluginCreatePositionZInput", objectPluginCreatePositionZInput);
    }

    int renderManager(int ax, int ay, int bx, int by, set<key_location>& down);
    int resizeManager(int x, int y);
    int mouseEntryManager(int state);
    int mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down);
    int guiEventManager(gui_event evt, int mx, int my, set<key_location>& down);

    string getClassName() {
      return "PluginCreate";
    }

    void onAdded();
    void onActivated();
    void onDeactivated();

    void createCube() {
      _temp.setCube({1,1,1},{0,0,0});
    }
    void createSphere() {

    }
    void createCylinder() {

    }

    void onMoveInput(float value, int axis) {

    }
  };

EditorPlugin* createPluginCreate(Editor* e, bool staticInit);