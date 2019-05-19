#include "PluginColor.h"

namespace pluginColorHSVCanvas {
  void renderManager(NGin::Graphics::CanvasHwnd me, int ax, int ay, int bx, int by, std::set<NGin::Graphics::key_location>& down) {
    ((PluginColor*)me->data)->HSV__renderManager(me, ax, ay, bx, by, down);
  }
  int resizeManager(NGin::Graphics::CanvasHwnd me, int x, int y) {
    return ((PluginColor*)me->data)->HSV__resizeManager(me, x, y);
  }
  int mouseEntryManager(NGin::Graphics::CanvasHwnd me, int state) {
    return ((PluginColor*)me->data)->HSV__mouseEntryManager(me, state);
  }
  int mouseMoveManager(NGin::Graphics::CanvasHwnd me, int x, int y, int ox, int oy, std::set<NGin::Graphics::key_location>& down) {
    return ((PluginColor*)me->data)->HSV__mouseMoveManager(me, x, y, ox, oy, down, me->isIn(x, y));
  }
  int guiEventManager(NGin::Graphics::CanvasHwnd me, NGin::Graphics::gui_event& evt, int mx, int my, std::set<NGin::Graphics::key_location>& down) {
    return ((PluginColor*)me->data)->HSV__guiEventManager(me, evt, mx, my, down, me->isIn(mx, my));
  }
}

void pluginColorPaintIcon(NGin::Graphics::ElemHwnd elem) {
  ((PluginColor*)elem->data)->paintColor();
}
void pluginColorFillIcon(NGin::Graphics::ElemHwnd elem) {
  ((PluginColor*)elem->data)->fillColor();
}
void pluginColorSampleIcon(NGin::Graphics::ElemHwnd elem) {
  ((PluginColor*)elem->data)->sampleColor();
}

void pluginColorMainButton(NGin::Graphics::ElemHwnd elem) {
  //Editor* e = (Editor*)editor;
  ((Editor*)elem->data)->activateStaticPlugin(((Editor*)elem->data)->findStaticPlugin("PluginColor"));
}

int PluginColor::renderManager(int ax, int ay, int bx, int by, std::set<NGin::Graphics::key_location>& down) {
  _editor->beginObjectDraw();
  for (auto&& it : _editor->objs) {
    if (it == highlightedObject) {
      _editor->drawObject(it, _editor->camview.matrix *it->_offset.matrix, 0x30ffffff, 1);
    } else {
      _editor->drawObject(it, _editor->camview.matrix *it->_offset.matrix);
    }
  }
  _editor->endObjectDraw();

  _editor->beginEdgeDraw();
  for (auto&& it : _editor->objs) {
    if (it == highlightedObject) {
      _editor->drawEdge(it, _editor->camview.matrix * it->_offset.matrix, 0xff303030);
    } else {
      _editor->drawEdge(it, _editor->camview.matrix * it->_offset.matrix, 0xff000000);
    }
  }
  _editor->endEdgeDraw();
  return 0;
}
int PluginColor::resizeManager(int x, int y) {
  return 0;
}
int PluginColor::mouseEntryManager(int state) {
  return 0;
}
int PluginColor::mouseMoveManager(int x, int y, int ox, int oy, std::set<NGin::Graphics::key_location>& down, bool in) {
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
    int nFace = -1;
    if (it->intersectRay(_editor->view.cameraEye, raydir, nDist, nFace)) {
      if (0 < nDist && nDist < dist) {
        dist = nDist;
        highlightedObject = it;
        selectedFace = nFace;
      }
    }
  }

  if (highlightedObject!=NULL && _mode == DrawMode_Paint && painting) {
    highlightedObject->_mesh._color[selectedFace] = _color;
    highlightedObject->upload();
    return 3;
  }

  if (oldhighlight != highlightedObject) {
    return 1;
  }


  return 0;
}
int PluginColor::guiEventManager(NGin::Graphics::gui_event& evt, int mx, int my, std::set<NGin::Graphics::key_location>& down, bool in) {
  if (in && evt._key._type == evt._key.type_mouse && evt._key._keycode == GLFW_MOUSE_BUTTON_LEFT) {
    if (!evt.captured && evt._type == evt.evt_down && highlightedObject != NULL) {
      evt.captured = true;
      if (_mode == DrawMode_Fill) {
        highlightedObject->_mesh.setColor(_color);
        highlightedObject->upload();
      }
      if (_mode == DrawMode_Pick) {
        _color = highlightedObject->_mesh._color[selectedFace];
        HSV__col = HSV__rgb2hsv(_color);
      }
      if (_mode == DrawMode_Paint) {
        painting = true;
        mouseMoveManager(mx, my, mx, my, down, in);
      }
    }
    if (evt._type == evt.evt_up) {
      painting = false;
    }
    return 1;
  }
  return 0;
}

void PluginColor::paintColor() {
  _mode = DrawMode_Paint;
  static_pointer_cast<NGin::Graphics::Button, NGin::Graphics::GUIElement>(_ribbonElement->getElementById("objectPluginCreatePaintIcon"))->stuck = true;
  static_pointer_cast<NGin::Graphics::Button, NGin::Graphics::GUIElement>(_ribbonElement->getElementById("objectPluginCreateFillIcon"))->stuck = false;
  static_pointer_cast<NGin::Graphics::Button, NGin::Graphics::GUIElement>(_ribbonElement->getElementById("objectPluginCreateSampleIcon"))->stuck = false;
}
void PluginColor::fillColor() {
  _mode = DrawMode_Fill;
  static_pointer_cast<NGin::Graphics::Button, NGin::Graphics::GUIElement>(_ribbonElement->getElementById("objectPluginCreatePaintIcon"))->stuck = false;
  static_pointer_cast<NGin::Graphics::Button, NGin::Graphics::GUIElement>(_ribbonElement->getElementById("objectPluginCreateFillIcon"))->stuck = true;
  static_pointer_cast<NGin::Graphics::Button, NGin::Graphics::GUIElement>(_ribbonElement->getElementById("objectPluginCreateSampleIcon"))->stuck = false;
}
void PluginColor::sampleColor() {
  _mode = DrawMode_Pick;
  static_pointer_cast<NGin::Graphics::Button, NGin::Graphics::GUIElement>(_ribbonElement->getElementById("objectPluginCreatePaintIcon"))->stuck = false;
  static_pointer_cast<NGin::Graphics::Button, NGin::Graphics::GUIElement>(_ribbonElement->getElementById("objectPluginCreateFillIcon"))->stuck = false;
  static_pointer_cast<NGin::Graphics::Button, NGin::Graphics::GUIElement>(_ribbonElement->getElementById("objectPluginCreateSampleIcon"))->stuck = true;
}

void PluginColor::onAdded() {
  _config = NGin::Graphics::createGUI_T<NGin::Graphics::Panel>("objectPluginColorConfigPanel", NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(1, 0), NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 200)), 0x00000000, nullptr);
  NGin::Graphics::setElements(_config, "html/colorToolbar.xml");
  NGin::Graphics::CanvasHwnd colorSelector = NGin::Graphics::createGUI_T<NGin::Graphics::Canvas>("objectEditorCanvas", NGin::Graphics::fullContainer, NGin::Graphics::IWindowManagers{
    pluginColorHSVCanvas::renderManager,
    pluginColorHSVCanvas::resizeManager,
    pluginColorHSVCanvas::guiEventManager,
    pluginColorHSVCanvas::mouseEntryManager,
    pluginColorHSVCanvas::mouseMoveManager,
  }, (void*)this);
  NGin::Graphics::addElement(static_pointer_cast<NGin::Graphics::Panel, NGin::Graphics::GUIElement>(_config->getElementById("objectPluginColorCanvasContainer")), colorSelector);


  _toolbarElement = NGin::Graphics::createGUI_T<NGin::Graphics::Button>("objectPluginColorMainButton", NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30)), NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"), (void*)_editor, "rgb", -1, pluginColorMainButton);
  _editor->registerSidebar(_toolbarElement);

  _ribbonElement = NGin::Graphics::createGUI_T<NGin::Graphics::Panel>("objectPluginColorIcons", NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 140)), 0x00000000, nullptr);
  NGin::Graphics::addElement(static_pointer_cast<NGin::Graphics::Panel, NGin::Graphics::GUIElement>(_ribbonElement), NGin::Graphics::createGUI_T<NGin::Graphics::IconButton>("objectPluginCreatePaintIcon",  NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30)), NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"), (void*)this, "X", -1, pluginColorPaintIcon, "paint", "html/icons.ilf"));
  NGin::Graphics::addElement(static_pointer_cast<NGin::Graphics::Panel, NGin::Graphics::GUIElement>(_ribbonElement), NGin::Graphics::createGUI_T<NGin::Graphics::IconButton>("objectPluginCreateFillIcon",   NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 60)), NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"), (void*)this, "X", -1, pluginColorFillIcon, "fill", "html/icons.ilf"));
  NGin::Graphics::addElement(static_pointer_cast<NGin::Graphics::Panel, NGin::Graphics::GUIElement>(_ribbonElement), NGin::Graphics::createGUI_T<NGin::Graphics::IconButton>("objectPluginCreateSampleIcon", NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 60), NGin::Graphics::LinearScale(0, 90)), NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"), (void*)this, "X", -1, pluginColorSampleIcon, "sample", "html/icons.ilf"));
}
void PluginColor::onActivated() {
  _editor->registerRibbon(_ribbonElement);

  showConfig();

}
void PluginColor::onDeactivated() {
  _editor->removeRibbon(_ribbonElement);

  hideConfig();

}

int PluginColor::HSV__renderManager(NGin::Graphics::CanvasHwnd me, int ax, int ay, int bx, int by, std::set<NGin::Graphics::key_location>& down) {
  //draw color wheel
  float wheelRadius = min(bx - ax, by - ay)*HSV__radiusMult;
  fVec2 center = fVec2((bx + ax) / 2.0, by - (bx - ax) / 2.0);

  float delta = 15;

  fVec2 color = center + fVec2(wheelRadius*sin(CONS_PI* HSV__col.x / 180.0), wheelRadius*cos(CONS_PI*HSV__col.x / 180.0))*HSV__triMult;
  fVec2 black = center + fVec2(wheelRadius*sin(CONS_PI* HSV__col.x / 180.0 + 2 * CONS_THIRD_PI), wheelRadius*cos(CONS_PI*HSV__col.x / 180.0 + 2 * CONS_THIRD_PI))*HSV__triMult;
  fVec2 white = center + fVec2(wheelRadius*sin(CONS_PI* HSV__col.x / 180.0 + 4 * CONS_THIRD_PI), wheelRadius*cos(CONS_PI*HSV__col.x / 180.0 + 4 * CONS_THIRD_PI))*HSV__triMult;

  NGin::Gll::gllBegin(NGin::Gll::GLL_QUADS);
  for (float h = 0; h < 360 - delta/2; h += delta) {
    NGin::Gll::gllColor(HSV__hsv2rgb(fVec3(h, 255, 255), 255));
    NGin::Gll::gllVertex(center + fVec2(wheelRadius*sin(CONS_PI*h / 180.0), wheelRadius*cos(CONS_PI*h / 180.0)));
    NGin::Gll::gllVertex(center + fVec2(wheelRadius*sin(CONS_PI*h / 180.0), wheelRadius*cos(CONS_PI*h / 180.0)) * HSV__innerMult);

    NGin::Gll::gllColor(HSV__hsv2rgb(fVec3(fmodf(h+ delta,360), 255, 255), 255));
    NGin::Gll::gllVertex(center + fVec2(wheelRadius*sin(CONS_PI*(h+ delta) / 180.0), wheelRadius*cos(CONS_PI*(h + delta) / 180.0)) * HSV__innerMult);
    NGin::Gll::gllVertex(center + fVec2(wheelRadius*sin(CONS_PI*(h + delta) / 180.0), wheelRadius*cos(CONS_PI*(h + delta) / 180.0)));
  }
  NGin::Gll::gllEnd();

  NGin::Gll::gllBegin(NGin::Gll::GLL_POLY);
  NGin::Gll::gllColor(HSV__hsv2rgb(fVec3(HSV__col.x, 255, 255), 255));
  NGin::Gll::gllVertex(color);
  NGin::Gll::gllColor(0xff000000);
  NGin::Gll::gllVertex(black);
  NGin::Gll::gllColor(0xffffffff);
  NGin::Gll::gllVertex(white);

  NGin::Gll::gllEnd();

  NGin::Gll::gllBegin(NGin::Gll::GLL_POLY);
  NGin::Gll::gllColor(0xff000000);
  NGin::Gll::gllVertex(center + fVec2(-0.9*wheelRadius, -1.1 * wheelRadius));
  NGin::Gll::gllVertex(center + fVec2(+0.9*wheelRadius, -1.1 * wheelRadius));
  NGin::Gll::gllColor(0xffffffff);
  NGin::Gll::gllVertex(center + fVec2(+0.9*wheelRadius, -1.3 * wheelRadius));
  NGin::Gll::gllVertex(center + fVec2(-0.9*wheelRadius, -1.3 * wheelRadius));
  NGin::Gll::gllEnd();

  NGin::Gll::gllBegin(NGin::Gll::GLL_POLY);
  NGin::Gll::gllColor(_color | 0xff000000);
  NGin::Gll::gllVertex(center + fVec2(-0.9*wheelRadius, -1.3 * wheelRadius));
  NGin::Gll::gllVertex(center + fVec2(-0.9*wheelRadius, -1.1 * wheelRadius));
  NGin::Gll::gllColor(_color & 0xffffff);
  NGin::Gll::gllVertex(center + fVec2(+0.9*wheelRadius, -1.1 * wheelRadius));
  NGin::Gll::gllVertex(center + fVec2(+0.9*wheelRadius, -1.3 * wheelRadius));
  NGin::Gll::gllEnd();

  fVec2 crossHairPos =
  black * (1 - HSV__col.z / 255) +
  color * (HSV__col.z / 255) * (HSV__col.y / 255) +
  white * (HSV__col.z / 255) * (1 - HSV__col.y / 255);

  colorargb antiColor = HSV__hsv2rgb(fVec3(fmodf(HSV__col.x + 180, 360), 255, 255), 255);

  NGin::Gll::gllColor(antiColor);
  NGin::Gll::gllBegin(NGin::Gll::GLL_QUADS);
  NGin::Gll::gllVertex(crossHairPos + fVec2(-1, -5));
  NGin::Gll::gllVertex(crossHairPos + fVec2(-1, +5));
  NGin::Gll::gllVertex(crossHairPos + fVec2(+1, +5));
  NGin::Gll::gllVertex(crossHairPos + fVec2(+1, -5));

  NGin::Gll::gllVertex(crossHairPos + fVec2(-5, -1));
  NGin::Gll::gllVertex(crossHairPos + fVec2(-5, +1));
  NGin::Gll::gllVertex(crossHairPos + fVec2(+5, +1));
  NGin::Gll::gllVertex(crossHairPos + fVec2(+5, -1));

  float alphaX = (wheelRadius * 1.8) * ((_color >> 24) - 127.5) / 255;

  NGin::Gll::gllVertex(center + fVec2(alphaX-1, -1.35 * wheelRadius));
  NGin::Gll::gllVertex(center + fVec2(alphaX-1, -1.05 * wheelRadius));
  NGin::Gll::gllVertex(center + fVec2(alphaX+1, -1.05 * wheelRadius));
  NGin::Gll::gllVertex(center + fVec2(alphaX+1, -1.35 * wheelRadius));
  NGin::Gll::gllEnd();

  return 0;

}
int PluginColor::HSV__resizeManager(NGin::Graphics::CanvasHwnd me, int x, int y) {
  return 0;

}
int PluginColor::HSV__mouseEntryManager(NGin::Graphics::CanvasHwnd me, int state) {
  return 0;

}
int PluginColor::HSV__mouseMoveManager(NGin::Graphics::CanvasHwnd me, int x, int y, int ox, int oy, std::set<NGin::Graphics::key_location>& down, bool in) {
  float wheelRadius = min(me->cbx - me->cax, me->cby - me->cay)*HSV__radiusMult;
  fVec2 center = fVec2((me->cbx + me->cax) / 2.0, me->cby - (me->cbx - me->cax) / 2.0);

  if (HSV__rotating) {
    HSV__col.x = 180.0 / CONS_PI * fmodf(atan2(x - center.x, y - center.y) + CONS_TWO_PI, CONS_TWO_PI);
    _color = HSV__hsv2rgb(HSV__col, (_color & 0xff000000) >> 24);
  }
  if (HSV__in_tri) {
    fVec2 color = center + fVec2(wheelRadius*sin(CONS_PI* HSV__col.x / 180.0), wheelRadius*cos(CONS_PI*HSV__col.x / 180.0))*HSV__triMult;
    fVec2 black = center + fVec2(wheelRadius*sin(CONS_PI* HSV__col.x / 180.0 + 2 * CONS_THIRD_PI), wheelRadius*cos(CONS_PI*HSV__col.x / 180.0 + 2 * CONS_THIRD_PI))*HSV__triMult;
    fVec2 white = center + fVec2(wheelRadius*sin(CONS_PI* HSV__col.x / 180.0 + 4 * CONS_THIRD_PI), wheelRadius*cos(CONS_PI*HSV__col.x / 180.0 + 4 * CONS_THIRD_PI))*HSV__triMult;

    float det = -black.y *color.x + black.x *color.y + black.y *white.x - color.y *white.x - black.x *white.y + color.x *white.y;

    fVec2 baryCoords = fVec2((-black.y+white.y)*(-black.x+x)+ (black.x - white.x) *(-black.y + y),
    (black.y - color.y)*(-black.x + x) + (-black.x + color.x)* (-black.y + y))/det;

    HSV__col.z = 255 * min(max(baryCoords.x+ baryCoords.y, 0.), 1.);
    HSV__col.y = 255 * min(max(baryCoords.x / (baryCoords.x + baryCoords.y), 0.), 1.);
    _color = HSV__hsv2rgb(HSV__col, (_color & 0xff000000) >> 24);
  }
  if (HSV__alpha) {
    int new_alpha = 255*min(max((x - center.x) / (1.8 * wheelRadius) + 0.5, 0.), 1.);
    _color = new_alpha << 24 | (_color & 0xffffff);
  }
  return 1;
}
int PluginColor::HSV__guiEventManager(NGin::Graphics::CanvasHwnd me, NGin::Graphics::gui_event& evt, int mx, int my, std::set<NGin::Graphics::key_location>& down, bool in) {
  if(evt._key._type == evt._key.type_mouse&& evt._key._keycode == GLFW_MOUSE_BUTTON_LEFT) {
    if(!evt.captured && in && evt._type == evt.evt_down) {
      float wheelRadius = min(me->cbx - me->cax, me->cby - me->cay)*HSV__radiusMult;
      fVec2 center = fVec2((me->cbx + me->cax) / 2.0, me->cby - (me->cbx - me->cax) / 2.0);
  
      float vecLength = (fVec2(mx, my) - center).length();

      if (wheelRadius * HSV__innerMult <= vecLength && vecLength <= wheelRadius) {
        HSV__rotating = true;
        evt.captured = true;
      }

      if (vecLength <= wheelRadius * HSV__triMult) {
        HSV__in_tri = true;
        evt.captured = true;
      }

      if (band(center + fVec2(-0.9*wheelRadius, -1.3 * wheelRadius) <= fVec2(mx, my) & fVec2(mx, my) <= center + fVec2(+0.9*wheelRadius, -1.1 * wheelRadius))) {
        HSV__alpha = true;
        evt.captured = true;
      }
      HSV__mouseMoveManager(me, mx, my, mx, my, down, in);
      return 1;
    }
    if (evt._type == evt.evt_up) {
      HSV__rotating = false;
      HSV__in_tri = false;
      HSV__alpha = false;
    }
    return 1;
  }
  return 0;
}

fVec3 PluginColor::HSV__rgb2hsv(colorargb val) {
  //255RGB -> 360H,255SV
  fVec3 rgb = fVec3((val & 0xff0000) >> 16, (val & 0xff00) >> 8, (val & 0xff) >> 0);
  //cout << rgb << endl;
  double      min, max, delta;
  fVec3 res;

  min = rgb.x < rgb.y ? rgb.x : rgb.y;
  min = min  < rgb.z ? min : rgb.z;

  max = rgb.x > rgb.y ? rgb.x : rgb.y;
  max = max  > rgb.z ? max : rgb.z;

  res.z = max;                                // v
  delta = max - min;
  if (delta < 0.001) {
    res.y = 0;
    res.x = 0; // undefined, maybe nan?
    return res;
  }
  if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
    res.y = 255*(delta / max);                  // s
  } else {
    res.y = 0.0;
    res.x = 0;                            // its now undefined
    return res;
  }
  if (rgb.x >= max)                           // > is bogus, just keeps compilor happy
    res.x = (rgb.y - rgb.z) / delta;        // between yellow & magenta
  else
    if (rgb.y >= max)
      res.x = 2.0 + (rgb.z - rgb.x) / delta;  // between cyan & yellow
    else
      res.x = 4.0 + (rgb.x - rgb.y) / delta;  // between magenta & cyan

  res.x *= 60.0;                              // degrees

  if (res.x < 0.0)
    res.x += 360.0;

  return res;
}
colorargb PluginColor::HSV__hsv2rgb(fVec3 val, uint8_t alpha) {
  //360H,255SV -> 255RGB
  double      hh, p, q, t, ff;
  long        i;

  fVec3 rgb;

  if (val.y <= 0.0) {       // < is bogus, just shuts up warnings
    rgb.x = val.z;
    rgb.y = val.z;
    rgb.z = val.z;
  } else {
    hh = val.x;
    if (hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = val.z / 255.0 * (255.0 - val.y);
    q = val.z / 255.0 * (255.0 - (val.y * ff));
    t = val.z / 255.0 * (255.0 - (val.y * (1.0 - ff)));

    switch (i) {
      case 0:
        rgb.x = val.z;
        rgb.y = t;
        rgb.z = p;
        break;
      case 1:
        rgb.x = q;
        rgb.y = val.z;
        rgb.z = p;
        break;
      case 2:
        rgb.x = p;
        rgb.y = val.z;
        rgb.z = t;
        break;

      case 3:
        rgb.x = p;
        rgb.y = q;
        rgb.z = val.z;
        break;
      case 4:
        rgb.x = t;
        rgb.y = p;
        rgb.z = val.z;
        break;
      case 5:
      default:
        rgb.x = val.z;
        rgb.y = p;
        rgb.z = q;
        break;
    }
  }
  //rgb *= 255;
  rgb = vecSwitch(rgb > 255, fVec3(255), rgb);
  rgb = vecSwitch(rgb < 0, fVec3(0), rgb);
  return (int(alpha)<<24) | (int(rgb.x) << 16) | (int(rgb.y) << 8) | (int(rgb.z) << 0);
}

EditorPlugin * createPluginColor(Editor * e, bool staticInit) {
  if (staticInit) {
    PluginColor::staticInit();
    return NULL;
  } else {
    return new PluginColor(e);
  }
}

const float PluginColor::HSV__radiusMult = 0.4;
const float PluginColor::HSV__innerMult = 0.8;
const float PluginColor::HSV__triMult = 0.7;