#pragma once

#include "Framework.h"
#include "PluginSelect.h"

class PluginBoolean;

void pluginBooleanUnionButton(Graphics::ElemHwnd sender, void * plugin);
void pluginBooleanSubtractButton(Graphics::ElemHwnd sender, void * plugin);
void pluginBooleanIntersectButton(Graphics::ElemHwnd sender, void * plugin);

void pluginBooleanSelectLhs(Graphics::ElemHwnd sender, PluginBoolean * plugin);
void pluginBooleanSelectRhs(Graphics::ElemHwnd sender, PluginBoolean * plugin);

class PluginBoolean : public EditorPlugin {
  public:
    PluginSelect* _partSelectors[2];
    int _activeSelector;

    PluginBoolean(Editor* e) : EditorPlugin(e) {
      //_ribbonElement = Graphics::createPanel("elementToolRibbonTemplate", LocationData(LinearScale(0,0), LinearScale(1, 0), LinearScale(0, 0), LinearScale(0, 10)), 0xffff0000);
    }

    static void staticInit() {
      Graphics::setName("pluginBooleanSelectLhs", pluginBooleanSelectLhs);
      Graphics::setName("pluginBooleanSelectRhs", pluginBooleanSelectRhs);

      Graphics::setName("pluginBooleanUnionButton", pluginBooleanUnionButton);
      Graphics::setName("pluginBooleanSubtractButton", pluginBooleanSubtractButton);
      Graphics::setName("pluginBooleanIntersectButton", pluginBooleanIntersectButton);
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

    void selectType(int group);

    void booleanIntersect();
    void booleanSubtract();
    void booleanUnion();
  };

EditorPlugin* createPluginBoolean(Editor* e, bool staticInit);