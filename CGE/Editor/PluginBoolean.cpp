#include "PluginBoolean.h"

void pluginBooleanUnionButton(Graphics::ElemHwnd sender, PluginBoolean * plugin) {

}
void pluginBooleanSubtractButton(Graphics::ElemHwnd sender, PluginBoolean * plugin) {

}
void pluginBooleanIntersectButton(Graphics::ElemHwnd sender, PluginBoolean * plugin) {

}

void pluginBooleanSelectLhs(Graphics::ElemHwnd sender, PluginBoolean * plugin) {
  plugin->selectType(0);
}
void pluginBooleanSelectRhs(Graphics::ElemHwnd sender, PluginBoolean * plugin) {
  plugin->selectType(1);
}

int PluginBoolean::renderManager(int ax, int ay, int bx, int by, set<key_location>& down) {
  return 0;
}
int PluginBoolean::resizeManager(int x, int y) {
  return 0;
}
int PluginBoolean::mouseEntryManager(int state) {
  return 0;
}
int PluginBoolean::mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down, bool in) {
  return 0;
}
int PluginBoolean::guiEventManager(gui_event evt, int mx, int my, set<key_location>& down, bool in) {
  return 0;
}


void PluginBoolean::booleanUnion() {
  shared_ptr<Object_Raw> newObj = make_shared<Object_Raw>();
  
  list<Mesh*> lhs;
  for (auto&& it : _partSelectors[0]->selectedObjects) {
    lhs.push_back(&(it->_mesh));
  }

  list<Mesh*> rhs;
  for (auto&& it : _partSelectors[1]->selectedObjects) {
    rhs.push_back(&(it->_mesh));
  }
  Mesh::booleanUnion(lhs, rhs, newObj->_mesh);
}
void PluginBoolean::booleanSubtract() {
  shared_ptr<Object_Raw> newObj = make_shared<Object_Raw>();

  list<Mesh*> lhs;
  for (auto&& it : _partSelectors[0]->selectedObjects) {
    lhs.push_back(&(it->_mesh));
  }

  list<Mesh*> rhs;
  for (auto&& it : _partSelectors[1]->selectedObjects) {
    rhs.push_back(&(it->_mesh));
  }
  Mesh::booleanSubtract(lhs, rhs, newObj->_mesh);
}
void PluginBoolean::booleanIntersect() {
  shared_ptr<Object_Raw> newObj = make_shared<Object_Raw>();

  list<Mesh*> lhs;
  for (auto&& it : _partSelectors[0]->selectedObjects) {
    lhs.push_back(&(it->_mesh));
  }

  list<Mesh*> rhs;
  for (auto&& it : _partSelectors[1]->selectedObjects) {
    rhs.push_back(&(it->_mesh));
  }
  Mesh::booleanIntersect(lhs, rhs, newObj->_mesh);
}
void PluginBoolean::selectType(int group) {
  _partSelectors[_activeSelector]->onDeactivated();
  _partSelectors[group]->onActivated();
  _activeSelector = group;
}

EditorPlugin * createPluginBoolean(Editor * e, bool staticInit) {
  if (staticInit) {
    PluginBoolean::staticInit();
    return NULL;
  } else {
    return new PluginBoolean(e);
  }
}
