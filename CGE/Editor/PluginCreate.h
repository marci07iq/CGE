#pragma once

#include "Framework.h"

class PluginCreate;

/*void pluginCreatePositionXInput(NGin::Graphics::ElemHwnd sender, void * plugin, string& val);
void pluginCreatePositionYInput(NGin::Graphics::ElemHwnd sender, void * plugin, string& val);
void pluginCreatePositionZInput(NGin::Graphics::ElemHwnd sender, void * plugin, string& val);

void pluginCreateSizeXInput(NGin::Graphics::ElemHwnd sender, void * plugin, string& val);
void pluginCreateSizeYInput(NGin::Graphics::ElemHwnd sender, void * plugin, string& val);
void pluginCreateSizeZInput(NGin::Graphics::ElemHwnd sender, void * plugin, string& val);


void pluginCreateDoneButton(NGin::Graphics::ElemHwnd sender, void* plugin);*/

class PluginCreate : public EditorPlugin {
  public:

    shared_ptr<Object> _temp;

    Transform _temp_movement;

    PluginCreate(Editor* e) : EditorPlugin(e) {
      //_ribbonElement = NGin::Graphics::createGUI_T<Panel>("elementToolRibbonTemplate", LocationData(LinearScale(0,0), LinearScale(1, 0), LinearScale(0, 0), LinearScale(0, 10)), 0xffff0000);
      _temp = NULL;
    }

    static void staticInit();

    int renderManager(int ax, int ay, int bx, int by, set<key_location>& down);
    int resizeManager(int x, int y);
    int mouseEntryManager(int state);
    int mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down, bool in);
    int guiEventManager(gui_event& evt, int mx, int my, set<key_location>& down, bool in);

    string getClassName() {
      return "PluginCreate";
    }

    void newObject(bool keep = true);

    void onAdded();
    void onActivated();
    void onDeactivated();

    void onDone();

    void createCube();
    void createSphere();
    void createCylinder();

    void onMoveInput(float value, int axis);
    void onResizeInput(float value, int axis);
  };

EditorPlugin* createPluginCreate(Editor* e, bool staticInit);