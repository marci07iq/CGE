#pragma once

#include "Framework.h"
#include "PluginSelect.h"

class PluginObject : public EditorPlugin {
  public:
    enum OffsetLockMode {
      OffsetModeLockUndef,
      OffsetModeLockRelative,
      OffsetModeLockLocal,
      OffsetModeLockAbs,
    };
    OffsetLockMode _offsetLockMode = OffsetModeLockUndef;

    enum OffsetRefMode {
      OffsetModeRefUndef,
      OffsetModeRefRelative,
      OffsetModeRefAbsolute,
    };
    OffsetRefMode _offsetRefMode = OffsetModeRefUndef;

    PluginSelect* selectorPlugin;

    Transform _temp_movement;

    list<shared_ptr<Object>> _clipboard;

    PluginObject(Editor* e) : EditorPlugin(e) {
      //_ribbonElement = Graphics::createPanel("elementToolRibbonTemplate", LocationData(LinearScale(0,0), LinearScale(1, 0), LinearScale(0, 0), LinearScale(0, 10)), 0xffff0000);
    }

    static void staticInit();

    void drawVirtualObject(shared_ptr<Object>& it);

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

    void onDeleteIcon();
    void onCopyIcon();
    void onPasteIcon();
    void onCutIcon();
    void onMoveIcon();

    void cycleMode();
    void cycleRef();
    void onDone();

    void onMoveInput(float value, int axis);

    fVec3 _scale = fVec3(1, 1, 1);
    fVec3 _rot = fVec3(0, 0, 0);
    void onResizeInput(float value, int axis);
    void onRotateInput(float value, int axis);
    void recalcUpperBlock();
  };

EditorPlugin* createPluginObject(Editor* e, bool staticInit);