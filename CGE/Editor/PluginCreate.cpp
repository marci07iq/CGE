#include "PluginCreate.h"

void pluginCreatePositionXInput(Graphics::ElemHwnd sender, void * plugin, string& val) {
  ((PluginCreate*)plugin)->onMoveInput(strTo<float>(val), 0);
}
void pluginCreatePositionYInput(Graphics::ElemHwnd sender, void * plugin, string& val) {
  ((PluginCreate*)plugin)->onMoveInput(strTo<float>(val), 1);
}
void pluginCreatePositionZInput(Graphics::ElemHwnd sender, void * plugin, string& val) {
  ((PluginCreate*)plugin)->onMoveInput(strTo<float>(val), 2);
}

void pluginCreateSizeXInput(Graphics::ElemHwnd sender, void * plugin, string& val) {
  ((PluginCreate*)plugin)->onResizeInput(strTo<float>(val), 0);
}
void pluginCreateSizeYInput(Graphics::ElemHwnd sender, void * plugin, string& val) {
  ((PluginCreate*)plugin)->onResizeInput(strTo<float>(val), 1);
}
void pluginCreateSizeZInput(Graphics::ElemHwnd sender, void * plugin, string& val) {
  ((PluginCreate*)plugin)->onResizeInput(strTo<float>(val), 2);
}

void pluginCreateDoneButton(Graphics::ElemHwnd sender, void* plugin) {
  ((PluginCreate*)plugin)->onDone();
}

void pluginCreateMainButton(Graphics::ElemHwnd elem, void* editor) {
  //Editor* e = (Editor*)editor;
  ((Editor*)editor)->activateStaticPlugin(((Editor*)editor)->findStaticPlugin("PluginCreate"));
}

void pluginCreateCubeIcon(Graphics::ElemHwnd elem, void* plugin) {
  ((PluginCreate*)plugin)->createCube();
}
void pluginCreateSphereIcon(Graphics::ElemHwnd elem, void* plugin) {
  ((PluginCreate*)plugin)->createSphere();
}
void pluginCreateCylinderIcon(Graphics::ElemHwnd elem, void* plugin) {
  ((PluginCreate*)plugin)->createCylinder();
}


void PluginCreate::staticInit() {
  Graphics::setName<TextInputFunc>("pluginCreatePositionXInput", pluginCreatePositionXInput);
  Graphics::setName<TextInputFunc>("pluginCreatePositionYInput", pluginCreatePositionYInput);
  Graphics::setName<TextInputFunc>("pluginCreatePositionZInput", pluginCreatePositionZInput);

  Graphics::setName<TextInputFunc>("pluginCreateSizeXInput", pluginCreateSizeXInput);
  Graphics::setName<TextInputFunc>("pluginCreateSizeYInput", pluginCreateSizeYInput);
  Graphics::setName<TextInputFunc>("pluginCreateSizeZInput", pluginCreateSizeZInput);

  Graphics::setName<ClickCallback>("pluginCreateDoneButton", pluginCreateDoneButton);
}

int PluginCreate::renderManager(int ax, int ay, int bx, int by, set<key_location>& down) {
  _editor->beginObjectDraw();
  for (auto&& it : _editor->objs) {
    _editor->drawObject(it, it->_offset.matrix, 0x7fffffff);
  }
  if(_temp != NULL) {
    //_editor->setObjectTransform(_temp_movement);
    _editor->drawObject(_temp, _temp_movement.matrix);
  }
  _editor->endObjectDraw();

  _editor->beginEdgeDraw();
  for (auto&& it : _editor->objs) {
    _editor->drawEdge(it, it->_offset.matrix);
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
int PluginCreate::mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down, bool in) {
  //return selectorPlugin->mouseMoveManager(x, y, ox, oy, down);
  return 0;
}
int PluginCreate::guiEventManager(gui_event& evt, int mx, int my, set<key_location>& down, bool in) {
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
  _config = Graphics::createPanel("objectPluginCreateConfigPanel", fullContainer, 0x00000000, NULL);
  Graphics::setElements(_config,"html/addToolbar.xml");

  ((Graphics::TextInputHwnd)_config->getElementById("objectPluginCreatePositionXInput"))->data = this;
  ((Graphics::TextInputHwnd)_config->getElementById("objectPluginCreatePositionYInput"))->data = this;
  ((Graphics::TextInputHwnd)_config->getElementById("objectPluginCreatePositionZInput"))->data = this;
  
  ((Graphics::TextInputHwnd)_config->getElementById("objectPluginCreateSizeXInput"))->data = this;
  ((Graphics::TextInputHwnd)_config->getElementById("objectPluginCreateSizeYInput"))->data = this;
  ((Graphics::TextInputHwnd)_config->getElementById("objectPluginCreateSizeZInput"))->data = this;

  
  ((Graphics::ButtonHwnd)_config->getElementById("objectPluginCreateDoneButton"))->data = this;

  _toolbarElement = Graphics::createButton("objectPluginCreateMainButton", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(0, 30)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), (void*)_editor, "+", -1, pluginCreateMainButton);
  _editor->registerSidebar(_toolbarElement);

  _ribbonElement = Graphics::createPanel("objectPluginObjectIcons", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(0, 110)), 0x00000000, NULL);
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginCreateCubeIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(0, 30)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), (void*)this, "X", -1, pluginCreateCubeIcon, "cube", "html/icons.ilf"));
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginCreateSphereIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 30), LinearScale(0, 60)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), (void*)this, "X", -1, pluginCreateSphereIcon, "sphere", "html/icons.ilf"));
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginCreateCylinderIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 60), LinearScale(0, 90)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), (void*)this, "X", -1, pluginCreateCylinderIcon, "cylinder", "html/icons.ilf"));
  /*Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginObjectCutIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 90), LinearScale(0, 120)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), "X", -1, pluginObjectCutIcon, (void*)this, "cut", "html/icons.ilf"));
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginObjectMoveIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 120), LinearScale(0, 150)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), "X", -1, pluginObjectMoveIcon, (void*)this, "move", "html/icons.ilf"));*/
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
