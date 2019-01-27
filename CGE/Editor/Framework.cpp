#include "Framework.h"

void EditorPlugin::hideConfig() {
  Graphics::deleteElements(_editor->_config, false);
}
void EditorPlugin::showConfig() {
  _editor->_config->elements.clear();
  Graphics::addElement(_editor->_config, _config);
}
