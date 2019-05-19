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
      //_ribbonElement = NGin::Graphics::createGUI_T<Panel>("elementToolRibbonTemplate", LocationData(NGin::Graphics::LinearScale(0,0), NGin::Graphics::LinearScale(1, 0), NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 10)), 0xffff0000);
    }

    static void staticInit() {
      NGin::Graphics::setName<NGin::Graphics::ClickCallback>("pluginBooleanSelectLhs", pluginBooleanSelectLhs);
      NGin::Graphics::setName<NGin::Graphics::ClickCallback>("pluginBooleanSelectRhs", pluginBooleanSelectRhs);

      NGin::Graphics::setName<NGin::Graphics::ClickCallback>("pluginBooleanUnionNGin::Graphics::Button", pluginBooleanUnionButton);
      NGin::Graphics::setName<NGin::Graphics::ClickCallback>("pluginBooleanSubtractNGin::Graphics::Button", pluginBooleanSubtractButton);
      NGin::Graphics::setName<NGin::Graphics::ClickCallback>("pluginBooleanIntersectNGin::Graphics::Button", pluginBooleanIntersectButton);
    }

    int renderManager(int ax, int ay, int bx, int by, std::set<NGin::Graphics::key_location>& down);
    int resizeManager(int x, int y);
    int mouseEntryManager(int state);
    int mouseMoveManager(int x, int y, int ox, int oy, std::set<NGin::Graphics::key_location>& down, bool in);
    int guiEventManager(NGin::Graphics::gui_event& evt, int mx, int my, std::set<NGin::Graphics::key_location>& down, bool in);

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