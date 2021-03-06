#pragma once

#include "Framework.h"
#include "PluginSelect.h"

class PluginColor : public EditorPlugin {
  public:
    enum DrawMode {
      DrawMode_None,
      DrawMode_Paint,
      DrawMode_Fill,
      DrawMode_Pick
    };

    DrawMode _mode;
    colorargb _color;

    shared_ptr<Object> highlightedObject;
    int selectedFace = -1;

    bool painting = false;

    PluginColor(Editor* e) : EditorPlugin(e) {
      //_ribbonElement = NGin::Graphics::createGUI_T<Panel>("elementToolRibbonTemplate", LocationData(NGin::Graphics::LinearScale(0,0), NGin::Graphics::LinearScale(1, 0), NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 10)), 0xffff0000);
    }

    static void staticInit() {

    }

    int renderManager(int ax, int ay, int bx, int by, std::set<NGin::Graphics::key_location>& down);
    int resizeManager(int x, int y);
    int mouseEntryManager(int state);
    int mouseMoveManager(int x, int y, int ox, int oy, std::set<NGin::Graphics::key_location>& down, bool in);
    int guiEventManager(NGin::Graphics::gui_event& evt, int mx, int my, std::set<NGin::Graphics::key_location>& down, bool in);

    string getClassName() {
      return "PluginColor";
    }

    void paintColor();
    void fillColor();
    void sampleColor();

    void onAdded();
    void onActivated();
    void onDeactivated();

    //This would be really nice in an internal namespace. But that cant be done...
    int HSV__renderManager    (NGin::Graphics::CanvasHwnd me, int ax, int ay, int bx, int by, std::set<NGin::Graphics::key_location>& down);
    int HSV__resizeManager    (NGin::Graphics::CanvasHwnd me, int x, int y);
    int HSV__mouseEntryManager(NGin::Graphics::CanvasHwnd me, int state);
    int HSV__mouseMoveManager (NGin::Graphics::CanvasHwnd me, int x, int y, int ox, int oy, std::set<NGin::Graphics::key_location>& down, bool in);
    int HSV__guiEventManager(NGin::Graphics::CanvasHwnd me, NGin::Graphics::gui_event& evt, int mx, int my, std::set<NGin::Graphics::key_location>& down, bool in);
    fVec3 HSV__col;

    bool HSV__rotating;
    bool HSV__in_tri;
    bool HSV__alpha;

    //Modified from https://stackoverflow.com/a/6930407/5987489
    fVec3 HSV__rgb2hsv(colorargb val);
    colorargb HSV__hsv2rgb(fVec3 val, uint8_t alpha);

    static const float HSV__radiusMult;
    static const float HSV__innerMult;
    static const float HSV__triMult;
  };

EditorPlugin* createPluginColor(Editor* e, bool staticInit);