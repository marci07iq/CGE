#include "ToolObject.h"

void pluginObjectMainButton(Graphics::ElemHwnd elem, void* editor) {
  //Editor* e = (Editor*)editor;
  ((Editor*)editor)->activateStaticPlugin(((Editor*)editor)->findStaticPlugin("PluginObject"));
}

void pluginObjectDeleteIcon(Graphics::ElemHwnd elem, void* plugin) {
  //mainEditor.activateStaticPlugin(mainEditor.findStaticPlugin("PluginObject"));
}
void pluginObjectCopyIcon(Graphics::ElemHwnd elem, void* plugin) {
  //mainEditor.activateStaticPlugin(mainEditor.findStaticPlugin("PluginObject"));
}
void pluginObjectPasteIcon(Graphics::ElemHwnd elem, void* plugin) {
  //mainEditor.activateStaticPlugin(mainEditor.findStaticPlugin("PluginObject"));
}
void pluginObjectCutIcon(Graphics::ElemHwnd elem, void* plugin) {
  //mainEditor.activateStaticPlugin(mainEditor.findStaticPlugin("PluginObject"));
}
void pluginObjectMoveIcon(Graphics::ElemHwnd elem, void* plugin) {
  //mainEditor.activateStaticPlugin(mainEditor.findStaticPlugin("PluginObject"));
}

int PluginObject::renderManager(int ax, int ay, int bx, int by, set<key_location>& down) {
  return selectorPlugin->renderManager(ax, ay, bx, by, down);
}
int PluginObject::resizeManager(int x, int y) {
  return selectorPlugin->resizeManager(x, y);
}
int PluginObject::mouseEntryManager(int state) {
  return selectorPlugin->mouseEntryManager(state);
}
int PluginObject::mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down) {
  return selectorPlugin->mouseMoveManager(x, y, ox, oy, down);
}
int PluginObject::guiEventManager(gui_event evt, int mx, int my, set<key_location>& down) {
  return selectorPlugin->guiEventManager(evt, mx, my, down);
}

void PluginObject::onAdded() {
  _toolbarElement = Graphics::createButton("objectPluginObjectMainButton", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(0, 30)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), "O", -1, pluginObjectMainButton, (void*)_editor);
  _editor->registerSidebar(_toolbarElement);

  _ribbonElement = Graphics::createPanel("objectPluginObjectIcons", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(0, 170)), 0x00000000);
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginObjectDeleteIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(0, 30)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), "X", -1, pluginObjectDeleteIcon, (void*)this, "delete", "html/icons.ilf"));
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginObjectCopyIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 30), LinearScale(0, 60)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), "X", -1, pluginObjectCopyIcon, (void*)this, "copy", "html/icons.ilf"));
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginObjectPaseIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 60), LinearScale(0, 90)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), "X", -1, pluginObjectPasteIcon, (void*)this, "paste", "html/icons.ilf"));
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginObjectCutIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 90), LinearScale(0, 120)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), "X", -1, pluginObjectCutIcon, (void*)this, "cut", "html/icons.ilf"));
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginObjectMoveIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 120), LinearScale(0, 150)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), "X", -1, pluginObjectMoveIcon, (void*)this, "move", "html/icons.ilf"));
}

void PluginObject::onActivated() {
  _editor->registerRibbon(_ribbonElement);

  selectorPlugin = (PluginSelect*)_editor->createDynamicPlugin("PluginSelect");
  selectorPlugin->onActivated();
}

void PluginObject::onDeactivated() {
  _editor->removeRibbon(_ribbonElement);

  selectorPlugin->onDeactivated();
  delete selectorPlugin;
}

/*void doCarve() {
  if (polyRays.size() >= 3) {
    Object obj;
    obj.setCone(polyRays, view.cameraEye, -viewOffset.toCartesian(), 0.1, 100);
    obj.setShared(reinterpret_cast<void*>(0xff0000));
    auto it = objs.begin();
    while (it != objs.end()) {
      Object* res = new Object(**it - obj); //cut
      (*it)->clean();
      Object* toWipe = *it;
      *it = res;
      delete toWipe;
      res->upload();
      ++it;
    }
  }
  //Object* res = new Object(obj);
  //res->upload();
  //objs.push_back(res);
}*/

EditorPlugin * createPluginObject(Editor * e) {
  return new PluginObject(e);
}
