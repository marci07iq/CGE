#include "PluginCreate.h"

void pluginCreatePositionXInput(Graphics::ElemHwnd sender, PluginCreate * plugin) {
  plugin->onMoveInput(strTo<float>(((Graphics::TextInputHwnd)sender)->text), 0);
}
void pluginCreatePositionYInput(Graphics::ElemHwnd sender, PluginCreate * plugin) {
  plugin->onMoveInput(strTo<float>(((Graphics::TextInputHwnd)sender)->text), 1);
}
void pluginCreatePositionZInput(Graphics::ElemHwnd sender, PluginCreate * plugin) {
  plugin->onMoveInput(strTo<float>(((Graphics::TextInputHwnd)sender)->text), 2);
}

void pluginCreateDoneButton(Graphics::ElemHwnd sender, PluginCreate* plugin) {
  plugin->onDone();
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


int PluginCreate::renderManager(int ax, int ay, int bx, int by, set<key_location>& down) {
  _editor->beginObjectDraw();
  for (auto&& it : _editor->objs) {
    _editor->drawObject(it, 0x7fffffff);
  }
  if(_temp != NULL) {
    _editor->beginObjectDraw(_temp_movement);
    _editor->drawObject(_temp);
  }
  _editor->endObjectDraw();

  _editor->beginEdgeDraw();
  for (auto&& it : _editor->objs) {
    _editor->drawEdge(it);
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
int PluginCreate::guiEventManager(gui_event evt, int mx, int my, set<key_location>& down, bool in) {
  //return selectorPlugin->guiEventManager(evt, mx, my, down);
  return 0;
}

void PluginCreate::newObject(bool keep) {
  if (keep && _temp != NULL) {

    double transMatrix[16];
    //_temp_movement.transpose();
    _temp_movement.read(transMatrix);
    _temp_movement.setIdentity();

    Eigen::Matrix4d mat = Eigen::Map<Eigen::Matrix4d>(transMatrix);
    _temp->applyTransform(mat);

    _editor->objs.push_back(_temp);
  }
  _temp = make_shared<Object_Raw>();
}

void PluginCreate::onAdded() {
  _config = Graphics::createPanel("objectPluginCreateConfigPanel", fullContainer, 0x00000000);
  Graphics::setElements(_config,"html/addToolbar.xml");

  ((Graphics::TextInputHwnd)_config->getElementById("objectPluginCreatePositionXInput"))->data = this;
  ((Graphics::TextInputHwnd)_config->getElementById("objectPluginCreatePositionYInput"))->data = this;
  ((Graphics::TextInputHwnd)_config->getElementById("objectPluginCreatePositionZInput"))->data = this;

  ((Graphics::ButtonHwnd)_config->getElementById("objectPluginCreateDoneButton"))->data = this;

  _toolbarElement = Graphics::createButton("objectPluginCreateMainButton", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(0, 30)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), "+", -1, pluginCreateMainButton, (void*)_editor);
  _editor->registerSidebar(_toolbarElement);

  _ribbonElement = Graphics::createPanel("objectPluginObjectIcons", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(0, 110)), 0x00000000);
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginCreateCubeIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(0, 30)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), "X", 16, pluginCreateCubeIcon, (void*)this, "cube", "html/icons.ilf"));
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginCreateSphereIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 30), LinearScale(0, 60)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), "X", -1, pluginCreateSphereIcon, (void*)this, "sphere", "html/icons.ilf"));
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginCreateCylinderIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 60), LinearScale(0, 90)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), "X", -1, pluginCreateCylinderIcon, (void*)this, "cylinder", "html/icons.ilf"));
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
  _temp->setCube({ 1,1,1 }, { 0,0,0 });
  _temp->upload();
  showConfig();
}
void PluginCreate::createSphere() {
  newObject(false);
  hideConfig();
}

EditorPlugin * createPluginCreate(Editor * e, bool staticInit) {
  if (staticInit) {
    PluginCreate::staticInit();
    return NULL;
  } else {
    return new PluginCreate(e);
  }
}
