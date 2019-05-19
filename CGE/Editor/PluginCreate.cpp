#include "PluginCreate.h"

void pluginCreatePositionXInput(NGin::Graphics::ElemHwnd sender, string& val) {
  ((PluginCreate*)sender->data)->onMoveInput(strTo<float>(val), 0);
}
void pluginCreatePositionYInput(NGin::Graphics::ElemHwnd sender, string& val) {
  ((PluginCreate*)sender->data)->onMoveInput(strTo<float>(val), 1);
}
void pluginCreatePositionZInput(NGin::Graphics::ElemHwnd sender, string& val) {
  ((PluginCreate*)sender->data)->onMoveInput(strTo<float>(val), 2);
}

void pluginCreateSizeXInput(NGin::Graphics::ElemHwnd sender, string& val) {
  ((PluginCreate*)sender->data)->onResizeInput(strTo<float>(val), 0);
}
void pluginCreateSizeYInput(NGin::Graphics::ElemHwnd sender, string& val) {
  ((PluginCreate*)sender->data)->onResizeInput(strTo<float>(val), 1);
}
void pluginCreateSizeZInput(NGin::Graphics::ElemHwnd sender, string& val) {
  ((PluginCreate*)sender->data)->onResizeInput(strTo<float>(val), 2);
}

void pluginCreateDoneButton(NGin::Graphics::ElemHwnd sender) {
  ((PluginCreate*)sender->data)->onDone();
}

void pluginCreateMainButton(NGin::Graphics::ElemHwnd elem) {
  ((Editor*)elem->data)->activateStaticPlugin(((Editor*)elem->data)->findStaticPlugin("PluginCreate"));
}

void pluginCreateCubeIcon(NGin::Graphics::ElemHwnd elem) {
  ((PluginCreate*)elem->data)->createCube();
}
void pluginCreateSphereIcon(NGin::Graphics::ElemHwnd elem) {
  ((PluginCreate*)elem->data)->createSphere();
}
void pluginCreateCylinderIcon(NGin::Graphics::ElemHwnd elem) {
  ((PluginCreate*)elem->data)->createCylinder();
}


void PluginCreate::staticInit() {
  NGin::Graphics::setName<NGin::Graphics::TextInputFunc>("pluginCreatePositionXInput", pluginCreatePositionXInput);
  NGin::Graphics::setName<NGin::Graphics::TextInputFunc>("pluginCreatePositionYInput", pluginCreatePositionYInput);
  NGin::Graphics::setName<NGin::Graphics::TextInputFunc>("pluginCreatePositionZInput", pluginCreatePositionZInput);

  NGin::Graphics::setName<NGin::Graphics::TextInputFunc>("pluginCreateSizeXInput", pluginCreateSizeXInput);
  NGin::Graphics::setName<NGin::Graphics::TextInputFunc>("pluginCreateSizeYInput", pluginCreateSizeYInput);
  NGin::Graphics::setName<NGin::Graphics::TextInputFunc>("pluginCreateSizeZInput", pluginCreateSizeZInput);

  NGin::Graphics::setName<NGin::Graphics::ClickCallback>("pluginCreateDoneNGin::Graphics::Button", pluginCreateDoneButton);
}

int PluginCreate::renderManager(int ax, int ay, int bx, int by, std::set<NGin::Graphics::key_location>& down) {
  _editor->beginObjectDraw();
  for (auto&& it : _editor->objs) {
    _editor->drawObject(it, _editor->camview.matrix * it->_offset.matrix, 0x7fffffff);
  }
  if (_temp != NULL) {
    //_editor->setObjectTransform(_temp_movement);
    _editor->drawObject(_temp, _editor->camview.matrix * _temp_movement.matrix);
  }
  _editor->endObjectDraw();

  _editor->beginEdgeDraw();
  for (auto&& it : _editor->objs) {
    _editor->drawEdge(it, _editor->camview.matrix * it->_offset.matrix);
  }
  _editor->endEdgeDraw();
  return 0;
}
int PluginCreate::resizeManager(int x, int y) {
  //return selectorPlugin->resizeManager(x, y);
  return 0;
}
int PluginCreate::mouseEntryManager(int state) {
  //return selectorPlugin->mouseEntryManager(state);
  return 0;
}
int PluginCreate::mouseMoveManager(int x, int y, int ox, int oy, std::set<NGin::Graphics::key_location>& down, bool in) {
  //return selectorPlugin->mouseMoveManager(x, y, ox, oy, down);
  return 0;
}
int PluginCreate::guiEventManager(NGin::Graphics::gui_event& evt, int mx, int my, std::set<NGin::Graphics::key_location>& down, bool in) {
  //return selectorPlugin->guiEventManager(evt, mx, my, down);
  return 0;
}

void PluginCreate::newObject(bool keep) {
  if (keep && _temp != NULL) {
    _temp->_offset = _temp_movement;
    _temp->bakeTransform();

    _editor->objs.push_back(_temp);
  }
  _temp = make_shared<Object>();
}

void PluginCreate::onAdded() {
  _config = NGin::Graphics::createGUI_T<NGin::Graphics::Panel>("objectPluginCreateConfigPanel", NGin::Graphics::fullContainer, 0x00000000, nullptr);
  NGin::Graphics::setElements(_config, "html/addToolbar.xml");

  _config->getElementById("objectPluginCreatePositionXInput")->data = this;
  _config->getElementById("objectPluginCreatePositionYInput")->data = this;
  _config->getElementById("objectPluginCreatePositionZInput")->data = this;

  _config->getElementById("objectPluginCreateSizeXInput")->data = this;
  _config->getElementById("objectPluginCreateSizeYInput")->data = this;
  _config->getElementById("objectPluginCreateSizeZInput")->data = this;


  static_pointer_cast<NGin::Graphics::Button, NGin::Graphics::GUIElement>(_config->getElementById("objectPluginCreateDoneNGin::Graphics::Button"))->data = this;

  _toolbarElement = NGin::Graphics::createGUI_T<NGin::Graphics::Button>("objectPluginCreateMainButton", NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30)), NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"), (void*)_editor, "+", -1, pluginCreateMainButton);
  _editor->registerSidebar(_toolbarElement);

  _ribbonElement = NGin::Graphics::createGUI_T<NGin::Graphics::Panel>("objectPluginObjectIcons", NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 110)), 0x00000000, nullptr);
  NGin::Graphics::addElement(static_pointer_cast<NGin::Graphics::Panel, NGin::Graphics::GUIElement>(_ribbonElement), NGin::Graphics::createGUI_T<NGin::Graphics::IconButton>("objectPluginCreateCubeIcon", NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30)), NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"), (void*)this, "X", -1, pluginCreateCubeIcon, "cube", "html/icons.ilf"));
  NGin::Graphics::addElement(static_pointer_cast<NGin::Graphics::Panel, NGin::Graphics::GUIElement>(_ribbonElement), NGin::Graphics::createGUI_T<NGin::Graphics::IconButton>("objectPluginCreateSphereIcon", NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 60)), NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"), (void*)this, "X", -1, pluginCreateSphereIcon, "sphere", "html/icons.ilf"));
  NGin::Graphics::addElement(static_pointer_cast<NGin::Graphics::Panel, NGin::Graphics::GUIElement>(_ribbonElement), NGin::Graphics::createGUI_T<NGin::Graphics::IconButton>("objectPluginCreateCylinderIcon", NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 60), NGin::Graphics::LinearScale(0, 90)), NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"), (void*)this, "X", -1, pluginCreateCylinderIcon, "cylinder", "html/icons.ilf"));
  /*NGin::Graphics::addElement((NGin::Graphics::PanelHwnd)_ribbonElement, NGin::Graphics::createGUI_T<IconButton>("objectPluginObjectCutIcon", LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 90), NGin::Graphics::LinearScale(0, 120)), NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"), "X", -1, pluginObjectCutIcon, (void*)this, "cut", "html/icons.ilf"));
  NGin::Graphics::addElement((NGin::Graphics::PanelHwnd)_ribbonElement, NGin::Graphics::createGUI_T<IconButton>("objectPluginObjectMoveIcon", LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 120), NGin::Graphics::LinearScale(0, 150)), NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"), "X", -1, pluginObjectMoveIcon, (void*)this, "move", "html/icons.ilf"));*/
}
void PluginCreate::onActivated() {
  _editor->registerRibbon(_ribbonElement);

  //selectorPlugin = (PluginSelect*)_editor->createDynamicPlugin("PluginSelect");
  //selectorPlugin->onActivated();
}
void PluginCreate::onDeactivated() {
  _editor->removeRibbon(_ribbonElement);

  //selectorPlugin->onDeactivated();
  //delete selectorPlugin;
}

void PluginCreate::onDone() {
  newObject(true);
  hideConfig();
}

void PluginCreate::createCube() {
  newObject(false);
  _temp->setCube({ 0.5,0.5,0.5 }, { 0,0,0.5 });
  _temp->upload();
  showConfig();
}
void PluginCreate::createSphere() {
  newObject(false);
  hideConfig();
}

void PluginCreate::createCylinder() {
  newObject(false);
  _temp->setCylinder({ 0.5,0.5,0.5 }, { 0,0,0.5 });
  _temp->upload();
  showConfig();
}

void PluginCreate::onMoveInput(float value, int axis) {
  //_temp->_mesh._transform(axis, 3) = value;
  //_temp->upload(); //The extra matrix should be sent to the videocard. Oh well
  _temp_movement.matrix.at(axis, 3) = value;
}
void PluginCreate::onResizeInput(float value, int axis) {
  //_temp->_mesh._transform(axis, 3) = value;
  //_temp->upload(); //The extra matrix should be sent to the videocard. Oh well
  _temp_movement.matrix.at(axis, axis) = value;
}

EditorPlugin * createPluginCreate(Editor * e, bool staticInit) {
  if (staticInit) {
    PluginCreate::staticInit();
    return NULL;
  } else {
    return new PluginCreate(e);
  }
}
