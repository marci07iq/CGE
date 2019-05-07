#pragma once

#include "Framework.h"
#include "PluginSelect.h"

class PluginBoolean;

void pluginBooleanUnionButton(NGin::Graphics::ElemHwnd sender);
void pluginBooleanSubtractButton(NGin::Graphics::ElemHwnd sender);
void pluginBooleanIntersectButton(NGin::Graphics::ElemHwnd sender);

void pluginBooleanSelectLhs(NGin::Graphics::ElemHwnd sender);
void pluginBooleanSelectRhs(NGin::Graphics::ElemHwnd sender);

class PluginBoolean : public EditorPlugin {
  public:
    PluginSelect* _partSelectors[2];
    int _activeSelector;

    PluginBoolean(Editor* e) : EditorPlugin(e) {
      //_ribbonElement = NGin::Graphics::createGUI_T<Panel>("elementToolRibbonTemplate", LocationData(LinearScale(0,0), LinearScale(1, 0), LinearScale(0, 0), LinearScale(0, 10)), 0xffff0000);
    }

    static void staticInit() {
      NGin::Graphics::setName<ClickCallback>("pluginBooleanSelectLhs", pluginBooleanSelectLhs);
      NGin::Graphics::setName<ClickCallback>("pluginBooleanSelectRhs", pluginBooleanSelectRhs);

      NGin::Graphics::setName<ClickCallback>("pluginBooleanUnionButton", pluginBooleanUnionButton);
      NGin::Graphics::setName<ClickCallback>("pluginBooleanSubtractButton", pluginBooleanSubtractButton);
      NGin::Graphics::setName<ClickCallback>("pluginBooleanIntersectButton", pluginBooleanIntersectButton);
    }

    int renderManager(int ax, int ay, int bx, int by, set<key_location>& down);
    int resizeManager(int x, int y);
    int mouseEntryManager(int state);
    int mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down, bool in);
    int guiEventManager(gui_event& evt, int mx, int my, set<key_location>& down, bool in);

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