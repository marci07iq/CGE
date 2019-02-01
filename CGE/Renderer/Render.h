#pragma once

#include "../Editor/PluginSelect.h"
#include "../Editor/PluginBoolean.h"
#include "../Editor/PluginObject.h"
#include "../Editor/PluginCreate.h"
#include "../Editor/PluginColor.h"

namespace MainGameCanvas {
  extern int mxold;
  extern int myold;

  extern OpenGLData view;

  extern bool lockMouse;

  void normalizeAngles();
  int renderManager(Canvas* me, int ax, int ay, int bx, int by, set<key_location>& down);
  int resizeManager(Canvas* me, int x, int y);
  int mouseEntryManager(Canvas* me, int state);
  int mouseMoveManager(Canvas* me, int x, int y, int ox, int oy, set<key_location>& down);
  int guiEventManager(Canvas* me, gui_event evt, int mx, int my, set<key_location>& down);
}