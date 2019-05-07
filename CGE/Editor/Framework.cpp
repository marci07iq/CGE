#include "Framework.h"

void EditorPlugin::hideConfig() {
  NGin::Graphics::deleteElements(_editor->_config);
}
void EditorPlugin::showConfig() {
  _editor->_config->elements.clear();
  NGin::Graphics::addElement(_editor->_config, _config);
}
