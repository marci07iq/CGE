#include "PluginSelect.h"

void pluginSelectAllIcon(NGin::Graphics::ElemHwnd elem) {
  ((PluginSelect*)elem->data)->selectAll();
}
void pluginSelectClearIcon(NGin::Graphics::ElemHwnd elem) {
  ((PluginSelect*)elem->data)->deselectAll();
}

int PluginSelect::renderManager(int ax, int ay, int bx, int by, set<key_location>& down) {
  renderSelection(ax, ay, bx, by, down);
  return 0;
}
int PluginSelect::renderSelection(int ax, int ay, int bx, int by, set<key_location>& down) {
  _editor->beginObjectDraw();
  for (auto&& it : _editor->objs) {
    if (selectedObjects.count(it) || it == highlightedObject) {
      _editor->drawObject(it, it->_offset.matrix, 0x30ffffff, 1);
    } else {
      _editor->drawObject(it, it->_offset.matrix, 0x00ffffff, 1);
    }
  }
  _editor->endObjectDraw();

  _editor->beginEdgeDraw();
  for (auto&& it : _editor->objs) {
    if (selectedObjects.count(it)) {
      if (it == highlightedObject) {
        _editor->drawEdge(it, it->_offset.matrix, 0xffff3030);
      } else {
        _editor->drawEdge(it, it->_offset.matrix, 0xffff0000);
      }
    } else {
      if (it == highlightedObject) {
        _editor->drawEdge(it, it->_offset.matrix, 0xff404040);
      } else {
        _editor->drawEdge(it, it->_offset.matrix, 0xff000000);
      }
    }
  }
  _editor->endEdgeDraw();
  return 0;
}
int PluginSelect::resizeManager(int x, int y) {
  return 0;
}
int PluginSelect::mouseEntryManager(int state) {
  return 0;
}
int PluginSelect::mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down, bool in) {
  GLdouble pos3D_ax = 0, pos3D_ay = 0, pos3D_az = 0;

  //cout << 2.0*(x -  _editor->view.viewport[0])/ _editor->view.viewport[2]-1 << " " << 2.0*(y - _editor->view.viewport[1]) / _editor->view.viewport[3] - 1 << endl;

  // get 3D coordinates based on window coordinates
  gluUnProject(x, y, 0,
    _editor->view.model_view, _editor->view.projection, _editor->view.viewport,
    &pos3D_ax, &pos3D_ay, &pos3D_az);

  vec3<double> raydir = vec3<double>{ pos3D_ax, pos3D_ay, pos3D_az } -_editor->view.cameraEye;

  float dist = INFINITY;
  shared_ptr<Object> oldhighlight = highlightedObject;
  highlightedObject = NULL;

  for (auto&& it : _editor->objs) {
    float nDist = INFINITY;
    int nFace;
    if (it->intersectRay(_editor->view.cameraEye, raydir, nDist, nFace)) {
      if (0 < nDist && nDist < dist) {
        dist = nDist;
        highlightedObject = it;
      }
    }
  }

  if (oldhighlight != highlightedObject) {
    return 1;
  }
  return 0;
}
int PluginSelect::guiEventManager(gui_event& evt, int mx, int my, set<key_location>& down, bool in) {

  if (evt._key._type == evt._key.type_mouse) {
    if (!evt.captured && evt._type == evt.evt_pressed && in) {
      if (evt._key._keycode == 0) {
        if (!down.count(key_location(key(GLFW_KEY_LEFT_SHIFT, evt._key.type_key)))) {
          evt.captured = true;
          selectedObjects.clear();
        }
        if (highlightedObject) {
          evt.captured = true;
          selectedObjects.insert(highlightedObject);
        }
        /*polyRays.push_front(rayori);*/
        return 1;
      }
    }
  }
  return 0;
}

void PluginSelect::onAdded() {
  _ribbonElement = NGin::Graphics::createGUI_T<Panel>("objectPluginSelectIcons", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(0, 80)), 0x00000000, nullptr);
  NGin::Graphics::addElement(static_pointer_cast<Panel, GUIElement>(_ribbonElement), NGin::Graphics::createGUI_T<IconButton>("objectPluginSelectAllIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(0, 30)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), (void*)this, "X", 8, pluginSelectAllIcon, "selectall", "html/icons.ilf"));
  NGin::Graphics::addElement(static_pointer_cast<Panel, GUIElement>(_ribbonElement), NGin::Graphics::createGUI_T<IconButton>("objectPluginSelectClearIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 30), LinearScale(0, 60)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), (void*)this, "X", 9, pluginSelectClearIcon, "deselect", "html/icons.ilf"));
}

void PluginSelect::onActivated() {
  _editor->registerRibbon(_ribbonElement);
}

void PluginSelect::onDeactivated() {
  _editor->removeRibbon(_ribbonElement);
}

EditorPlugin * createPluginSelect(Editor * e, bool staticInit) {
  if (staticInit) {
    PluginSelect::staticInit();
    return NULL;
  } else {
    return new PluginSelect(e);
  }
}
