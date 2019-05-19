#include "PluginBoolean.h"

void pluginBooleanUnionButton(NGin::Graphics::ElemHwnd sender) {
  ((PluginBoolean*)sender->data)->booleanUnion();
}
void pluginBooleanSubtractButton(NGin::Graphics::ElemHwnd sender) {
  ((PluginBoolean*)sender->data)->booleanSubtract();
}
void pluginBooleanIntersectButton(NGin::Graphics::ElemHwnd sender) {
  ((PluginBoolean*)sender->data)->booleanIntersect();
}

void pluginBooleanSelectLhs(NGin::Graphics::ElemHwnd sender) {
  ((PluginBoolean*)sender->data)->selectType(0);
}
void pluginBooleanSelectRhs(NGin::Graphics::ElemHwnd sender) {
  ((PluginBoolean*)sender->data)->selectType(1);
}

void pluginBooleanMainButton(NGin::Graphics::ElemHwnd elem) {
  //Editor* e = (Editor*)editor;
  ((Editor*)elem->data)->activateStaticPlugin(((Editor*)elem->data)->findStaticPlugin("PluginBoolean"));
}

int PluginBoolean::renderManager(int ax, int ay, int bx, int by, std::set<NGin::Graphics::key_location>& down) {
  _editor->beginObjectDraw();
  static const colorargb cols[3][3] = {
    { 0xbfffffff , 0xbf5f5fff , 0xbf3f3fff },
    { 0xbfff5f5f , 0xbfff5fff , 0xbf5f3fff },
    { 0xbfff3f3f , 0xbfff3f5f , 0xffffff00 }
  };
  for (auto&& it : _editor->objs) {
    _editor->drawObject(it, _editor->camview.matrix * it->_offset.matrix, cols
      [((_partSelectors[0]->highlightedObject == it && _activeSelector == 0) ? 2 : (_partSelectors[0]->selectedObjects.count(it) ? 1 : 0))]
      [((_partSelectors[1]->highlightedObject == it && _activeSelector == 1) ? 2 : (_partSelectors[1]->selectedObjects.count(it) ? 1 : 0))],
      1);
  }
  _editor->endObjectDraw();
  return 0;
}
int PluginBoolean::resizeManager(int x, int y) {
  return 0;
}
int PluginBoolean::mouseEntryManager(int state) {
  return 0;
}
int PluginBoolean::mouseMoveManager(int x, int y, int ox, int oy, std::set<NGin::Graphics::key_location>& down, bool in) {
  return _partSelectors[_activeSelector]->mouseMoveManager(x, y, ox, oy, down, in);
}
int PluginBoolean::guiEventManager(NGin::Graphics::gui_event& evt, int mx, int my, std::set<NGin::Graphics::key_location>& down, bool in) {
  return _partSelectors[_activeSelector]->guiEventManager(evt, mx, my, down, in);
}

void PluginBoolean::onAdded() {
  _config = NGin::Graphics::createGUI_T<NGin::Graphics::Panel>("objectPluginBooleanConfigPanel", NGin::Graphics::fullContainer, 0x00000000, nullptr);
  NGin::Graphics::setElements(_config, "html/booleanToolbar.xml");

  _config->getElementById("objectPluginBooleanSelectLhs")->data = this;
  _config->getElementById("objectPluginBooleanSelectRhs")->data = this;
  
  /*((NGin::Graphics::ButtonHwnd)_config->getElementById("objectPluginBooleanUnionNGin::Graphics::Button"))->data = this;
  ((NGin::Graphics::ButtonHwnd)_config->getElementById("objectPluginBooleanSubtractNGin::Graphics::Button"))->data = this;
  ((NGin::Graphics::ButtonHwnd)_config->getElementById("objectPluginBooleanIntersectNGin::Graphics::Button"))->data = this;*/

  _toolbarElement = NGin::Graphics::createGUI_T<NGin::Graphics::Button>("objectPluginBooleanMainButton", NGin::Graphics::LocationData(
    NGin::Graphics::LinearScale(0, 0),
    NGin::Graphics::LinearScale(0, 30),
    NGin::Graphics::LinearScale(0, 0),
    NGin::Graphics::LinearScale(0, 30)
  ),
    NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"),
    NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"),
    NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"),
    (void*)_editor, "*", -1, pluginBooleanMainButton);
  _editor->registerSidebar(_toolbarElement);

  _ribbonElement = NGin::Graphics::createGUI_T<NGin::Graphics::Panel>("objectPluginBooleanIcons", NGin::Graphics::LocationData(
    NGin::Graphics::LinearScale(0, 0),
    NGin::Graphics::LinearScale(0, 30),
    NGin::Graphics::LinearScale(0, 0),
    NGin::Graphics::LinearScale(0, 110)
  ), 0x00000000, nullptr);
  NGin::Graphics::addElement(static_pointer_cast<NGin::Graphics::Panel, NGin::Graphics::GUIElement>(_ribbonElement), NGin::Graphics::createGUI_T<NGin::Graphics::IconButton>("objectPluginBooleanUnionNGin::Graphics::Button", NGin::Graphics::LocationData(
    NGin::Graphics::LinearScale(0, 0),
    NGin::Graphics::LinearScale(0, 30),
    NGin::Graphics::LinearScale(0, 0),
    NGin::Graphics::LinearScale(0, 30)
  ),
    NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"),
    NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"),
    NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"),
    (void*)this, "+", -1, pluginBooleanUnionButton, "union", "html/icons.ilf"));

  NGin::Graphics::addElement(static_pointer_cast<NGin::Graphics::Panel, NGin::Graphics::GUIElement>(_ribbonElement), NGin::Graphics::createGUI_T<NGin::Graphics::IconButton>("objectPluginBooleanSubtractNGin::Graphics::Button", NGin::Graphics::LocationData(
    NGin::Graphics::LinearScale(0, 0),
    NGin::Graphics::LinearScale(0, 30),
    NGin::Graphics::LinearScale(0, 30),
    NGin::Graphics::LinearScale(0, 60)
  ),
    NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"),
    NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"),
    NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"),
    (void*)this, "-", -1, pluginBooleanSubtractButton, "subtract", "html/icons.ilf"));

  NGin::Graphics::addElement(static_pointer_cast<NGin::Graphics::Panel, NGin::Graphics::GUIElement>(_ribbonElement), NGin::Graphics::createGUI_T<NGin::Graphics::IconButton>("objectPluginBooleanIntersectNGin::Graphics::Button", NGin::Graphics::LocationData(
    NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 60), NGin::Graphics::LinearScale(0, 90)), NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"), (void*)this, "*", -1, pluginBooleanIntersectButton, "intersect", "html/icons.ilf"));
  /*NGin::Graphics::addElement((NGin::Graphics::PanelHwnd)_ribbonElement, NGin::Graphics::createGUI_T<IconButton>("objectPluginObjectCutIcon", LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 90), NGin::Graphics::LinearScale(0, 120)), NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"), "X", -1, pluginObjectCutIcon, (void*)this, "cut", "html/icons.ilf"));
  NGin::Graphics::addElement((NGin::Graphics::PanelHwnd)_ribbonElement, NGin::Graphics::createGUI_T<IconButton>("objectPluginObjectMoveIcon", LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 120), NGin::Graphics::LinearScale(0, 150)), NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"), "X", -1, pluginObjectMoveIcon, (void*)this, "move", "html/icons.ilf"));*/
}
void PluginBoolean::onActivated() {
  _editor->registerRibbon(_ribbonElement);

  _partSelectors[0] = (PluginSelect*)_editor->createDynamicPlugin("PluginSelect");
  _partSelectors[0]->onActivated();
  _activeSelector = 0;
  _partSelectors[1] = (PluginSelect*)_editor->createDynamicPlugin("PluginSelect");
  //_partSelectors[1]->onActivated();

  showConfig();
}
void PluginBoolean::onDeactivated() {
  _editor->removeRibbon(_ribbonElement);

  _partSelectors[_activeSelector]->onDeactivated();
  delete _partSelectors[0];
  delete _partSelectors[1];

  hideConfig();
}

void PluginBoolean::booleanUnion() {
  shared_ptr<Object> newObj = make_shared<Object>();
  
  list<Mesh*> lhs;
  for (auto&& it : _partSelectors[0]->selectedObjects) {
    lhs.push_back(&(it->_mesh));
  }

  list<Mesh*> rhs;
  for (auto&& it : _partSelectors[1]->selectedObjects) {
    rhs.push_back(&(it->_mesh));
  }
  Mesh::booleanUnion(lhs, rhs, newObj->_mesh);

  for (auto&& it : _partSelectors[0]->selectedObjects) {
    _editor->removeObject(it);
  }
  for (auto&& it : _partSelectors[1]->selectedObjects) {
    _editor->removeObject(it);
  }
  newObj->upload();
  _editor->objs.push_back(newObj);
}
void PluginBoolean::booleanSubtract() {
  shared_ptr<Object> newObj = make_shared<Object>();

  list<Mesh*> lhs;
  for (auto&& it : _partSelectors[0]->selectedObjects) {
    lhs.push_back(&(it->_mesh));
  }

  list<Mesh*> rhs;
  for (auto&& it : _partSelectors[1]->selectedObjects) {
    rhs.push_back(&(it->_mesh));
  }
  Mesh::booleanSubtract(lhs, rhs, newObj->_mesh);

  for (auto&& it : _partSelectors[0]->selectedObjects) {
    _editor->removeObject(it);
  }
  for (auto&& it : _partSelectors[1]->selectedObjects) {
    _editor->removeObject(it);
  }
  newObj->upload();
  _editor->objs.push_back(newObj);
}
void PluginBoolean::booleanIntersect() {
  shared_ptr<Object> newObj = make_shared<Object>();

  list<Mesh*> lhs;
  for (auto&& it : _partSelectors[0]->selectedObjects) {
    lhs.push_back(&(it->_mesh));
  }

  list<Mesh*> rhs;
  for (auto&& it : _partSelectors[1]->selectedObjects) {
    rhs.push_back(&(it->_mesh));
  }
  Mesh::booleanIntersect(lhs, rhs, newObj->_mesh);

  for (auto&& it : _partSelectors[0]->selectedObjects) {
    _editor->removeObject(it);
  }
  for (auto&& it : _partSelectors[1]->selectedObjects) {
    _editor->removeObject(it);
  }
  newObj->upload();
  _editor->objs.push_back(newObj);
}

void PluginBoolean::selectType(int group) {
  _partSelectors[_activeSelector]->onDeactivated();
  _partSelectors[group]->onActivated();
  _activeSelector = group;

  static_pointer_cast<NGin::Graphics::Button, NGin::Graphics::GUIElement>(_config->getElementById("objectPluginBooleanSelectLhs"))->stuck = (group == 0);
  static_pointer_cast<NGin::Graphics::Button, NGin::Graphics::GUIElement>(_config->getElementById("objectPluginBooleanSelectRhs"))->stuck = (group == 1);
}

EditorPlugin * createPluginBoolean(Editor * e, bool staticInit) {
  if (staticInit) {
    PluginBoolean::staticInit();
    return NULL;
  } else {
    return new PluginBoolean(e);
  }
}
