#include "PluginColor.h"

namespace pluginColorHSVCanvas {
  void renderManager(Canvas* me, int ax, int ay, int bx, int by, set<key_location>& down) {
    ((PluginColor*)me->data)->HSV__renderManager(me, ax, ay, bx, by, down);
  }
  int resizeManager(Canvas* me, int x, int y) {
    return ((PluginColor*)me->data)->HSV__resizeManager(me, x, y);
  }
  int mouseEntryManager(Canvas* me, int state) {
    return ((PluginColor*)me->data)->HSV__mouseEntryManager(me, state);
  }
  int mouseMoveManager(Canvas* me, int x, int y, int ox, int oy, set<key_location>& down) {
    return ((PluginColor*)me->data)->HSV__mouseMoveManager(me, x, y, ox, oy, down, me->isIn(x, y));
  }
  int guiEventManager(Canvas* me, gui_event& evt, int mx, int my, set<key_location>& down) {
    return ((PluginColor*)me->data)->HSV__guiEventManager(me, evt, mx, my, down, me->isIn(mx, my));
  }
}

void pluginColorPaintIcon(Graphics::ElemHwnd elem, void* plugin) {
  ((PluginColor*)plugin)->paintColor();
}
void pluginColorFillIcon(Graphics::ElemHwnd elem, void* plugin) {
  ((PluginColor*)plugin)->fillColor();
}
void pluginColorSampleIcon(Graphics::ElemHwnd elem, void* plugin) {
  ((PluginColor*)plugin)->sampleColor();
}

void pluginColorMainButton(Graphics::ElemHwnd elem, void* editor) {
  //Editor* e = (Editor*)editor;
  ((Editor*)editor)->activateStaticPlugin(((Editor*)editor)->findStaticPlugin("PluginColor"));
}

int PluginColor::renderManager(int ax, int ay, int bx, int by, set<key_location>& down) {
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
int PluginColor::mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down, bool in) {
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
int PluginColor::guiEventManager(gui_event& evt, int mx, int my, set<key_location>& down, bool in) {
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
  ((Graphics::ButtonHwnd)(_ribbonElement->getElementById("objectPluginCreatePaintIcon")))->stuck = true;
  ((Graphics::ButtonHwnd)(_ribbonElement->getElementById("objectPluginCreateFillIcon")))->stuck = false;
  ((Graphics::ButtonHwnd)(_ribbonElement->getElementById("objectPluginCreateSampleIcon")))->stuck = false;
}
void PluginColor::fillColor() {
  _mode = DrawMode_Fill;
  ((Graphics::ButtonHwnd)(_ribbonElement->getElementById("objectPluginCreatePaintIcon")))->stuck = false;
  ((Graphics::ButtonHwnd)(_ribbonElement->getElementById("objectPluginCreateFillIcon")))->stuck = true;
  ((Graphics::ButtonHwnd)(_ribbonElement->getElementById("objectPluginCreateSampleIcon")))->stuck = false;
}
void PluginColor::sampleColor() {
  _mode = DrawMode_Pick;
  ((Graphics::ButtonHwnd)(_ribbonElement->getElementById("objectPluginCreatePaintIcon")))->stuck = false;
  ((Graphics::ButtonHwnd)(_ribbonElement->getElementById("objectPluginCreateFillIcon")))->stuck = false;
  ((Graphics::ButtonHwnd)(_ribbonElement->getElementById("objectPluginCreateSampleIcon")))->stuck = true;
}

void PluginColor::onAdded() {
  _config = Graphics::createPanel("objectPluginColorConfigPanel", LocationData(LinearScale(0, 0), LinearScale(1, 0), LinearScale(0, 0), LinearScale(0, 200)), 0x00000000, NULL);
  Graphics::setElements(_config, "html/colorToolbar.xml");
  Graphics::CanvasHwnd colorSelector = Graphics::createCanvas("objectEditorCanvas", fullContainer, IWindowManagers{
    pluginColorHSVCanvas::renderManager,
    pluginColorHSVCanvas::resizeManager,
    pluginColorHSVCanvas::guiEventManager,
    pluginColorHSVCanvas::mouseEntryManager,
    pluginColorHSVCanvas::mouseMoveManager,
  }, (void*)this);
  Graphics::addElement((Graphics::PanelHwnd)(_config->getElementById("objectPluginColorCanvasContainer")), colorSelector);


  _toolbarElement = Graphics::createButton("objectPluginColorMainButton", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(0, 30)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), (void*)_editor, "rgb", -1, pluginColorMainButton);
  _editor->registerSidebar(_toolbarElement);

  _ribbonElement = Graphics::createPanel("objectPluginColorIcons", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(0, 140)), 0x00000000, NULL);
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginCreatePaintIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(0, 30)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), (void*)this, "X", -1, pluginColorPaintIcon, "paint", "html/icons.ilf"));
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginCreateFillIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 30), LinearScale(0, 60)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), (void*)this, "X", -1, pluginColorFillIcon, "fill", "html/icons.ilf"));
  //Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginCreateColorsIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 60), LinearScale(0, 90)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), "X", -1, pluginColorColorsIcon, (void*)this, "colors", "html/icons.ilf"));
  Graphics::addElement((Graphics::PanelHwnd)_ribbonElement, Graphics::createIconButton("objectPluginCreateSampleIcon", LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 60), LinearScale(0, 90)), getColor("button", "bgcolor"), getColor("button", "activecolor"), getColor("button", "textcolor"), (void*)this, "X", -1, pluginColorSampleIcon, "sample", "html/icons.ilf"));
}
void PluginColor::onActivated() {
  _editor->registerRibbon(_ribbonElement);

  showConfig();

}
void PluginColor::onDeactivated() {
  _editor->removeRibbon(_ribbonElement);

  hideConfig();

}

int PluginColor::HSV__renderManager(Canvas* me, int ax, int ay, int bx, int by, set<key_location>& down) {
  //draw color wheel
  float wheelRadius = min(bx - ax, by - ay)*HSV__radiusMult;
  fVec2 center = fVec2((bx + ax) / 2.0, by - (bx - ax) / 2.0);

  float delta = 15;

  fVec2 color = center + fVec2(wheelRadius*sin(CONS_PI* HSV__col.x / 180.0), wheelRadius*cos(CONS_PI*HSV__col.x / 180.0))*HSV__triMult;
  fVec2 black = center + fVec2(wheelRadius*sin(CONS_PI* HSV__col.x / 180.0 + 2 * CONS_THIRD_PI), wheelRadius*cos(CONS_PI*HSV__col.x / 180.0 + 2 * CONS_THIRD_PI))*HSV__triMult;
  fVec2 white = center + fVec2(wheelRadius*sin(CONS_PI* HSV__col.x / 180.0 + 4 * CONS_THIRD_PI), wheelRadius*cos(CONS_PI*HSV__col.x / 180.0 + 4 * CONS_THIRD_PI))*HSV__triMult;

  Gll::gllBegin(Gll::GLL_QUADS);
  for (float h = 0; h < 360 - delta/2; h += delta) {
    Gll::gllColor(HSV__hsv2rgb(fVec3(h, 255, 255), 255));
    Gll::gllVertex(center + fVec2(wheelRadius*sin(CONS_PI*h / 180.0), wheelRadius*cos(CONS_PI*h / 180.0)));
    Gll::gllVertex(center + fVec2(wheelRadius*sin(CONS_PI*h / 180.0), wheelRadius*cos(CONS_PI*h / 180.0)) * HSV__innerMult);

    Gll::gllColor(HSV__hsv2rgb(fVec3(fmodf(h+ delta,360), 255, 255), 255));
    Gll::gllVertex(center + fVec2(wheelRadius*sin(CONS_PI*(h+ delta) / 180.0), wheelRadius*cos(CONS_PI*(h + delta) / 180.0)) * HSV__innerMult);
    Gll::gllVertex(center + fVec2(wheelRadius*sin(CONS_PI*(h + delta) / 180.0), wheelRadius*cos(CONS_PI*(h + delta) / 180.0)));
  }
  Gll::gllEnd();

  Gll::gllBegin(Gll::GLL_POLY);
  Gll::gllColor(HSV__hsv2rgb(fVec3(HSV__col.x, 255, 255), 255));
  Gll::gllVertex(color);
  Gll::gllColor(0xff000000);
  Gll::gllVertex(black);
  Gll::gllColor(0xffffffff);
  Gll::gllVertex(white);

  Gll::gllEnd();

  Gll::gllBegin(Gll::GLL_POLY);
  Gll::gllColor(0xff000000);
  Gll::gllVertex(center + fVec2(-0.9*wheelRadius, -1.1 * wheelRadius));
  Gll::gllVertex(center + fVec2(+0.9*wheelRadius, -1.1 * wheelRadius));
  Gll::gllColor(0xffffffff);
  Gll::gllVertex(center + fVec2(+0.9*wheelRadius, -1.3 * wheelRadius));
  Gll::gllVertex(center + fVec2(-0.9*wheelRadius, -1.3 * wheelRadius));
  Gll::gllEnd();

  Gll::gllBegin(Gll::GLL_POLY);
  Gll::gllColor(_color | 0xff000000);
  Gll::gllVertex(center + fVec2(-0.9*wheelRadius, -1.3 * wheelRadius));
  Gll::gllVertex(center + fVec2(-0.9*wheelRadius, -1.1 * wheelRadius));
  Gll::gllColor(_color & 0xffffff);
  Gll::gllVertex(center + fVec2(+0.9*wheelRadius, -1.1 * wheelRadius));
  Gll::gllVertex(center + fVec2(+0.9*wheelRadius, -1.3 * wheelRadius));
  Gll::gllEnd();

  fVec2 crossHairPos =
  black * (1 - HSV__col.z / 255) +
  color * (HSV__col.z / 255) * (HSV__col.y / 255) +
  white * (HSV__col.z / 255) * (1 - HSV__col.y / 255);

  colorargb antiColor = HSV__hsv2rgb(fVec3(fmodf(HSV__col.x + 180, 360), 255, 255), 255);

  Gll::gllColor(antiColor);
  Gll::gllBegin(Gll::GLL_QUADS);
  Gll::gllVertex(crossHairPos + fVec2(-1, -5));
  Gll::gllVertex(crossHairPos + fVec2(-1, +5));
  Gll::gllVertex(crossHairPos + fVec2(+1, +5));
  Gll::gllVertex(crossHairPos + fVec2(+1, -5));

  Gll::gllVertex(crossHairPos + fVec2(-5, -1));
  Gll::gllVertex(crossHairPos + fVec2(-5, +1));
  Gll::gllVertex(crossHairPos + fVec2(+5, +1));
  Gll::gllVertex(crossHairPos + fVec2(+5, -1));

  float alphaX = (wheelRadius * 1.8) * ((_color >> 24) - 127.5) / 255;

  Gll::gllVertex(center + fVec2(alphaX-1, -1.35 * wheelRadius));
  Gll::gllVertex(center + fVec2(alphaX-1, -1.05 * wheelRadius));
  Gll::gllVertex(center + fVec2(alphaX+1, -1.05 * wheelRadius));
  Gll::gllVertex(center + fVec2(alphaX+1, -1.35 * wheelRadius));
  Gll::gllEnd();

  return 0;

}
int PluginColor::HSV__resizeManager(Canvas* me, int x, int y) {
  return 0;

}
int PluginColor::HSV__mouseEntryManager(Canvas* me, int state) {
  return 0;

}
int PluginColor::HSV__mouseMoveManager(Canvas* me, int x, int y, int ox, int oy, set<key_location>& down, bool in) {
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
int PluginColor::HSV__guiEventManager(Canvas* me, gui_event& evt, int mx, int my, set<key_location>& down, bool in) {
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