#include "PluginObject.h"

void pluginObjectMovePositionXInput(Graphics::ElemHwnd sender, void * plugin, string& val) {
  ((PluginObject*)plugin)->onMoveInput(strTo<float>(val), 0);
}
void pluginObjectMovePositionYInput(Graphics::ElemHwnd sender, void * plugin, string& val) {
  ((PluginObject*)plugin)->onMoveInput(strTo<float>(val), 1);
}
void pluginObjectMovePositionZInput(Graphics::ElemHwnd sender, void * plugin, string& val) {
  ((PluginObject*)plugin)->onMoveInput(strTo<float>(val), 2);
}

void pluginObjectMoveSizeXInput(Graphics::ElemHwnd sender, void * plugin, string& val) {
  ((PluginObject*)plugin)->onResizeInput(strTo<float>(val), 0);
}
void pluginObjectMoveSizeYInput(Graphics::ElemHwnd sender, void * plugin, string& val) {
  ((PluginObject*)plugin)->onResizeInput(strTo<float>(val), 1);
}
void pluginObjectMoveSizeZInput(Graphics::ElemHwnd sender, void * plugin, string& val) {
  ((PluginObject*)plugin)->onResizeInput(strTo<float>(val), 2);
}

void pluginObjectRotationTiltInput(Graphics::ElemHwnd sender, void * plugin, string& val) {
  ((PluginObject*)plugin)->onRotateInput(strTo<float>(val), 0);
}
void pluginObjectRotationDirInput(Graphics::ElemHwnd sender, void * plugin, string& val) {
  ((PluginObject*)plugin)->onRotateInput(strTo<float>(val), 1);
}
void pluginObjectRotationTurnInput(Graphics::ElemHwnd sender, void * plugin, string& val) {
  ((PluginObject*)plugin)->onRotateInput(strTo<float>(val), 2);
}

void pluginObjectMoveDoneButton(Graphics::ElemHwnd sender, void* plugin) {
  ((PluginObject*)plugin)->onDone();
}

void pluginObjectDeleteIcon(Graphics::ElemHwnd elem, void* plugin) {
  ((PluginObject*)plugin)->onDeleteIcon();
}
void pluginObjectCopyIcon(Graphics::ElemHwnd elem, void* plugin) {
  ((PluginObject*)plugin)->onCopyIcon();
}
void pluginObjectPasteIcon(Graphics::ElemHwnd elem, void* plugin) {
  ((PluginObject*)plugin)->onPasteIcon();
}
void pluginObjectCutIcon(Graphics::ElemHwnd elem, void* plugin) {
  ((PluginObject*)plugin)->onCutIcon();
}
void pluginObjectMoveIcon(Graphics::ElemHwnd elem, void* plugin) {
  ((PluginObject*)plugin)->onMoveIcon();
}

void pluginObjectMainButton(Graphics::ElemHwnd elem, void* editor) {
  //Editor* e = (Editor*)editor;
  ((Editor*)editor)->activateStaticPlugin(((Editor*)editor)->findStaticPlugin("PluginObject"));
}

void PluginObject::staticInit() {
  Graphics::setName<TextInputFunc>("pluginObjectMovePositionXInput", pluginObjectMovePositionXInput);
  Graphics::setName<TextInputFunc>("pluginObjectMovePositionYInput", pluginObjectMovePositionYInput);
  Graphics::setName<TextInputFunc>("pluginObjectMovePositionZInput", pluginObjectMovePositionZInput);

  Graphics::setName<TextInputFunc>("pluginObjectMoveSizeXInput", pluginObjectMoveSizeXInput);
  Graphics::setName<TextInputFunc>("pluginObjectMoveSizeYInput", pluginObjectMoveSizeYInput);
  Graphics::setName<TextInputFunc>("pluginObjectMoveSizeZInput", pluginObjectMoveSizeZInput);

  Graphics::setName<TextInputFunc>("pluginObjectRotationTiltInput", pluginObjectRotationTiltInput);
  Graphics::setName<TextInputFunc>("pluginObjectRotationDirInput", pluginObjectRotationDirInput);
  Graphics::setName<TextInputFunc>("pluginObjectRotationTurnInput", pluginObjectRotationTurnInput);

  Graphics::setName<ClickCallback>("pluginObjectMoveDoneButton", pluginObjectMoveDoneButton);
}

int PluginObject::renderManager(int ax, int ay, int bx, int by, set<key_location>& down) {
  _editor->beginObjectDraw();
  for (auto&& it : _editor->objs) {
    if (selectorPlugin->selectedObjects.count(it)) {
      _editor->drawObject(it, it->_offset.matrix, 0x80ff0000, 1);
    } else {
      if (selectorPlugin->highlightedObject == it) {
        _editor->drawObject(it, it->_offset.matrix, 0x30ffffff, 1);
      } else {
        _editor->drawObject(it, it->_offset.matrix, 0x00ff0000, 1);
      }
    }
  }
  //_editor->setObjectTransform(_temp_movement);
  for (auto&& it : _editor->objs) {
    if (selectorPlugin->selectedObjects.count(it)) {
      Matrix4f vals = it->_offset.matrix * _temp_movement.matrix;
      _editor->drawObject(it, vals, 0x8000ff00, 1);
    }
  }
  _editor->endObjectDraw();
  return 0;
}
int PluginObject::resizeManager(int x, int y) {
  return selectorPlugin->resizeManager(x, y);
}
int PluginObject::mouseEntryManager(int state) {
  return selectorPlugin->mouseEntryManager(state);
}
int PluginObject::mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down, bool in) {
  return selectorPlugin->mouseMoveManager(x, y, ox, oy, down, in);
}
int PluginObject::guiEventManager(gui_event& evt, int mx, int my, set<key_location>& down, bool in) {
  return selectorPlugin->guiEventManager(evt, mx, my, down, in);
}

void PluginObject::onAdded() {
  _config = Graphics::createPanel("objectPluginObjectMoveConfigPanel", fullContainer, 0x00000000, NULL);
  Graphics::setElements(_config, "html/moveToolbar.xml");

  ((Graphics::TextInputHwnd)_config->getElementById("objectPluginObjectMovePositionXInput"))->data = this;
  ((Graphics::TextInputHwnd)_config->getElementById("objectPluginObjectMovePositionYInput"))->data = this;
  ((Graphics::TextInputHwnd)_config->getElementById("objectPluginObjectMovePositionZInput"))->data = this;

  ((Graphics::TextInputHwnd)_config->getElementById("objectPluginObjectMoveSizeXInput"))->data = this;
  ((Graphics::TextInputHwnd)_config->getElementById("objectPluginObjectMoveSizeYInput"))->data = this;
  ((Graphics::TextInputHwnd)_config->getElementById("objectPluginObjectMoveSizeZInput"))->data = this;

  ((Graphics::TextInputHwnd)_config->getElementById("objectPluginObjectRotationTiltInput"))->data = this;
  ((Graphics::TextInputHwnd)_config->getElementById("objectPluginObjectRotationDirInput"))->data = this;
  ((Graphics::TextInputHwnd)_config->getElementById("objectPluginObjectRotationTurnInput"))->data = this;
  
  ((Graphics::ButtonHwnd)_config->getElementById("objectPluginObjectMoveDoneButton"))->data = this;

  _toolbarElement = Graphics::createButton("objectPluginObjectMainButton", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(0, 30)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), (void*)_editor, "O", -1, pluginObjectMainButton);
  _editor->registerSidebar(_toolbarElement);

  _ribbonElement = Graphics::createPanel("objectPluginObjectIcons", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(0, 170)), 0x00000000, NULL);
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginObjectDeleteIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(0, 30)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), (void*)this, "X", 16, pluginObjectDeleteIcon, "delete", "html/icons.ilf"));
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginObjectCopyIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 30), LinearScale(0, 60)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), (void*)this, "X", 17, pluginObjectCopyIcon, "copy", "html/icons.ilf"));
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginObjectPaseIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 60), LinearScale(0, 90)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), (void*)this, "X", 18, pluginObjectPasteIcon, "paste", "html/icons.ilf"));
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginObjectCutIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 90), LinearScale(0, 120)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), (void*)this, "X", 19, pluginObjectCutIcon, "cut", "html/icons.ilf"));
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginObjectMoveIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 120), LinearScale(0, 150)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), (void*)this, "X", -1, pluginObjectMoveIcon, "move", "html/icons.ilf"));
}
void PluginObject::onActivated() {
  _editor->registerRibbon(_ribbonElement);

  selectorPlugin = (PluginSelect*)_editor->createDynamicPlugin("PluginSelect");
  selectorPlugin->onActivated();

  showConfig();
}
void PluginObject::onDeactivated() {
  _editor->removeRibbon(_ribbonElement);

  selectorPlugin->onDeactivated();
  delete selectorPlugin;

  hideConfig();
}

void PluginObject::onDeleteIcon() {
  for (auto&& it : selectorPlugin->selectedObjects) {
    _editor->removeObject(it);
  }
}
void PluginObject::onCopyIcon() {
  _clipboard.clear();
  for (auto&& it : selectorPlugin->selectedObjects) {
    _clipboard.push_back(make_shared<Object>(it->_mesh));
  }
}
void PluginObject::onPasteIcon() {
  for (auto&& it : _clipboard) {
    shared_ptr<Object> clone = make_shared<Object>(it->_mesh);
    clone->upload();
    _editor->objs.push_back(clone);
    
  }
}
void PluginObject::onCutIcon() {
  onCopyIcon();
  onDeleteIcon();
}
void PluginObject::onMoveIcon() {
  
}

void PluginObject::onDone() {
  for (auto&& it : selectorPlugin->selectedObjects) {
    it->_offset.applyTransform(_temp_movement);
    it->upload();
  }

  selectorPlugin->selectedObjects.clear();
}

void PluginObject::onMoveInput(float value, int axis) {
  //_temp->_mesh._transform(axis, 3) = value;
  //_temp->upload(); //The extra matrix should be sent to the videocard. Oh well
  _temp_movement.matrix.at(axis, 3) = value;
}
void PluginObject::onResizeInput(float value, int axis) {
  //_temp->_mesh._transform(axis, 3) = value;
  //_temp->upload(); //The extra matrix should be sent to the videocard. Oh well
  _scale[axis] = value;
  recalcUpperBlock();
}
void PluginObject::onRotateInput(float value, int axis) {
  _rot[axis] = value;
  recalcUpperBlock();
}

void PluginObject::recalcUpperBlock() {
  Transform t;
  t.scale(_scale);

  fVec3 axis = {
  cos(_rot[1] / 180 * CONS_PI)*sin(_rot[0] / 180 * CONS_PI),
  sin(_rot[1] / 180 * CONS_PI)*sin(_rot[0] / 180 * CONS_PI),
  cos(_rot[0] / 180 * CONS_PI) };
  
  float cosTurn = cos(_rot[2] / 180 * CONS_PI);
  float sinTurn = sin(_rot[2] / 180 * CONS_PI);

  Transform rots;
  float withf[16] = {
    axis.x * axis.x * (1 - cosTurn) + cosTurn,
    axis.x * axis.y * (1 - cosTurn) - axis.z * sinTurn,
    axis.x * axis.z * (1 - cosTurn) + axis.y * sinTurn,
    0,

    axis.y * axis.x * (1 - cosTurn) + axis.z * sinTurn,
    axis.y * axis.y * (1 - cosTurn) + cosTurn,
    axis.y * axis.z * (1 - cosTurn) - axis.x * sinTurn,
    0,

    axis.z * axis.x * (1 - cosTurn) - axis.y * sinTurn,
    axis.z * axis.y * (1 - cosTurn) + axis.x * sinTurn,
    axis.z * axis.z * (1 - cosTurn) + cosTurn,
    0,
    0,0,0,1};
  rots.matrix.set(withf);
  t.matrix = rots.matrix * t.matrix;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      _temp_movement.matrix.at(i, j) = t.matrix.at(i, j);
    }
  }
}

EditorPlugin * createPluginObject(Editor * e, bool staticInit) {
  if (staticInit) {
    PluginObject::staticInit();
    return NULL;
  } else {
    return new PluginObject(e);
  }
}
