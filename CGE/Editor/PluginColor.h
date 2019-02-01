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

    PluginColor(Editor* e) : EditorPlugin(e) {
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
      return "PluginColor";
    }

    void paintColor();
    void fillColor();
    void sampleColor();

    void onAdded();
    void onActivated();
    void onDeactivated();

    //This would be really nice in an internal namespace. But that cant be done...
    int HSV__renderManager    (Canvas* me, int ax, int ay, int bx, int by, set<key_location>& down);
    int HSV__resizeManager    (Canvas* me, int x, int y);
    int HSV__mouseEntryManager(Canvas* me, int state);
    int HSV__mouseMoveManager (Canvas* me, int x, int y, int ox, int oy, set<key_location>& down, bool in);
    int HSV__guiEventManager(Canvas* me, gui_event evt, int mx, int my, set<key_location>& down, bool in);
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