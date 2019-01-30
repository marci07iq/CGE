#pragma once

#include "Framework.h"

class PluginCreate;

void pluginCreatePositionXInput(Graphics::ElemHwnd sender, PluginCreate* plugin);
void pluginCreatePositionYInput(Graphics::ElemHwnd sender, PluginCreate* plugin);
void pluginCreatePositionZInput(Graphics::ElemHwnd sender, PluginCreate* plugin);

void pluginCreateSizeXInput(Graphics::ElemHwnd sender, PluginCreate* plugin);
void pluginCreateSizeYInput(Graphics::ElemHwnd sender, PluginCreate* plugin);
void pluginCreateSizeZInput(Graphics::ElemHwnd sender, PluginCreate* plugin);


void pluginCreateDoneButton(Graphics::ElemHwnd sender, PluginCreate* plugin);

class PluginCreate : public EditorPlugin {
  public:

    shared_ptr<Object_Raw> _temp;

    Transpose _temp_movement;

    PluginCreate(Editor* e) : EditorPlugin(e) {
      //_ribbonElement = Graphics::createPanel("elementToolRibbonTemplate", LocationData(LinearScale(0,0), LinearScale(1, 0), LinearScale(0, 0), LinearScale(0, 10)), 0xffff0000);
      _temp = NULL;
    }

    static void staticInit() {
      Graphics::setName("pluginCreatePositionXInput", pluginCreatePositionXInput);
      Graphics::setName("pluginCreatePositionYInput", pluginCreatePositionYInput);
      Graphics::setName("pluginCreatePositionZInput", pluginCreatePositionZInput);

      Graphics::setName("pluginCreateSizeXInput", pluginCreateSizeXInput);
      Graphics::setName("pluginCreateSizeYInput", pluginCreateSizeYInput);
      Graphics::setName("pluginCreateSizeZInput", pluginCreateSizeZInput);

      Graphics::setName("pluginCreateDoneButton", pluginCreateDoneButton);
    }

    int renderManager(int ax, int ay, int bx, int by, set<key_location>& down);
    int resizeManager(int x, int y);
    int mouseEntryManager(int state);
    int mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down, bool in);
    int guiEventManager(gui_event evt, int mx, int my, set<key_location>& down, bool in);

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
    void createCylinder() {
      _editor->_config->elements.clear();
    }

    void onMoveInput(float value, int axis) {
      //_temp->_mesh._transform(axis, 3) = value;
      //_temp->upload(); //The extra matrix should be sent to the videocard. Oh well
      _temp_movement.matrix[3][axis] = value;
    }
    void onResizeInput(float value, int axis) {
      //_temp->_mesh._transform(axis, 3) = value;
      //_temp->upload(); //The extra matrix should be sent to the videocard. Oh well
      _temp_movement.matrix[axis][axis] = value;
    }
  };

EditorPlugin* createPluginCreate(Editor* e, bool staticInit);