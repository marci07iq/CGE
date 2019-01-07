#include "ToolBoolean.h"

int PluginBoolean::renderManager(int ax, int ay, int bx, int by, set<key_location>& down) {
  return 0;
}
int PluginBoolean::resizeManager(int x, int y) {
  return 0;
}
int PluginBoolean::mouseEntryManager(int state) {
  return 0;
}
int PluginBoolean::mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down) {
  return 0;
}
int PluginBoolean::guiEventManager(gui_event evt, int mx, int my, set<key_location>& down) {
  return 0;
}

EditorPlugin * createPluginBoolean(Editor * e) {
  return new PluginBoolean(e);
}
