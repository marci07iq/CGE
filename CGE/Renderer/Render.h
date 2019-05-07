#pragma once

#include "../Editor/PluginSelect.h"
#include "../Editor/PluginBoolean.h"
#include "../Editor/PluginObject.h"
#include "../Editor/PluginCreate.h"
#include "../Editor/PluginColor.h"
#include "../Editor/PluginBuilding.h"

namespace MainGameCanvas {
  extern int mxold;
  extern int myold;

  extern OpenGLData view;

  extern bool lockMouse;

  //void normalizeAngles();
  void renderManager(NGin::Graphics::CanvasHwnd me, int ax, int ay, int bx, int by, set<key_location>& down);
  int resizeManager(NGin::Graphics::CanvasHwnd me, int x, int y);
  int mouseEntryManager(NGin::Graphics::CanvasHwnd me, int state);
  int mouseMoveManager(NGin::Graphics::CanvasHwnd me, int x, int y, int ox, int oy, set<key_location>& down);
  int guiEventManager(NGin::Graphics::CanvasHwnd me, gui_event& evt, int mx, int my, set<key_location>& down);
}