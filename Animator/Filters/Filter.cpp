#include "Filter.h"

int StreamData::frameId(float time) {
  return round((time - _startTime) * _frameRate);
}

bool isBetween(iVec2 lower, iVec2 what, iVec2 upper) {
  return lower.x <= what.x && what.x <= upper.x &&
    lower.y <= what.y && what.y <= upper.y;
}

//Filter

Filter::Filter(weak_ptr<EditorContext> ctx) {
  _ctx = ctx;
  #ifdef M_CLIENT
  updateSize();
#endif
}

void Filter::init() {

}
void Filter::configure() {

}

void Filter::calculate(float t) {

}

#ifdef M_CLIENT
FilterGUI::FilterGUI(string name, LocationData llocation, colorargb lbgcolor, colorargb lactivecolor, colorargb ltextcolor, shared_ptr<EditorContext> editor)
  : GUIElement(name, llocation, lbgcolor, lactivecolor, ltextcolor, editor.get()) {
  _editor = editor;
}

void FilterGUI::getRect() {

}
void FilterGUI::getRect(int winWidth, int winHeight, int offsetX, int offsetY) {
  cax = offsetX + location.getLeft(winWidth);
  cay = offsetY + location.getBot(winHeight);
  cbx = offsetX + location.getRight(winWidth);
  cby = offsetY + location.getTop(winHeight);

  getRect();
}

int FilterGUI::mouseEnter(int state) {
  int res = 0;
  for (auto&& filt : _editor.lock()->_filters) {
    res |= filt->mouseEnter(dragOffset, state);
    if (res & 2) { return res; }
  }
  return res;
}

int FilterGUI::mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down) {
  int res = 0;
  for (auto&& filt : _editor.lock()->_filters) {
    res |= filt->mouseMoved_base(dragOffset, mx, my, ox, oy, down);
    if (res & 2) { return res; }
  }
  return res;
}

int FilterGUI::guiEvent(gui_event& evt, int mx, int my, set<key_location>& down) {
  int res = 0;
  for (auto&& filt : _editor.lock()->_filters) {
    res |= filt->guiEvent_base(dragOffset, evt, mx, my, down);
    if (res & 2) { return res; }
  }
  return res;
}

void FilterGUI::render(set<key_location>& down) {
  for (auto&& filt : _editor.lock()->_filters) {
    filt->render_base(dragOffset, down);
  }
}

bool Filter::isIn(iVec2 offset, int mx, int my) {
  return
    cax <= offset.x + mx && offset.x + mx <= cax + cw &&
    cay <= offset.y + my && offset.y + my <= cay + ch;
}

int Filter::mouseMoved_base(iVec2 offset, int mx, int my, int ox, int oy, set<key_location>& down) {
  int res = 0;

  if (dragging) {
    cax += mx - ox;
    cay += my - oy;
    res = 1;
  }

  if (res & 2) return res;
  return res | mouseMoved(offset, mx, my, ox, oy, down);
}
int Filter::guiEvent_base(iVec2 offset, gui_event evt, int mx, int my, set<key_location>& down) {
  int res = 0;

  if (evt._key._type == key::type_mouse && evt._key._keycode == GLFW_MOUSE_BUTTON_LEFT) {
    if (cax + cw - 20 <= offset.x + mx && offset.x + mx <= cax + cw &&
      cay + ch - 20 <= offset.y + my && offset.y + my <= cay + ch)
      if (!evt.captured && evt._type == gui_event::evt_down) {
        evt.captured = true;
        res |= 2;
        dragging = true;
      }
    if (evt._type == gui_event::evt_up) {
      dragging = false;
      res |= 1;
    }

    if (!evt.captured && evt._type == gui_event::evt_down) {
      iVec2 pos = iVec2(cax + cw - 20, cay + ch - 25) - offset;
      for (auto&& it : _outputs) {
        if (isBetween(pos + iVec2(0, -20), iVec2(mx, my), pos + iVec2(20, 0))) {
          evt.captured = true;
          getGUI_ctx()->_draggedConnection = it.second;
          res |= 2;
        }
        pos += iVec2(0, -25);
      }
    }


    if (evt._type == gui_event::evt_up) {
      shared_ptr<Filter_Resource_Output> dragged = getGUI_ctx()->_draggedConnection.lock();
      if (!getGUI_ctx()->_draggedConnection.expired()) {
        iVec2 pos = iVec2(cax, cay + ch - 25) - offset;
        for (auto&& it : _inputs) {
          if (isBetween(pos + iVec2(0, -20), iVec2(mx, my), pos + iVec2(20, 0))) {
            if (it.second->_type == dragged->type()) {
              it.second->bindInput(dragged);
            }
            res |= 1;
          }
          pos += iVec2(0, -25);
        }
        pos = iVec2(cax + 25, cay + ch) - offset;
        for (auto&& it : _params) {
          if (isBetween(pos + iVec2(0, -20), iVec2(mx, my), pos + iVec2(20, 0))) {
            if (it.second->_type == dragged->type()) {
              it.second->bindInput(dragged);
            }
            res |= 1;
          }
          pos += iVec2(25, 0);
        }
      }
    }
  }

  if (res & 2) return res;
  return res | guiEvent(offset, evt, mx, my, down);
}
void Filter::render_base(iVec2 offset, set<key_location>& down) {
  Gll::gllBegin(Gll::GLL_QUADS);
  Gll::gllColor(getGUI_ctx()->bgColor);
  Gll::gllVertex(cax - offset.x, cay - offset.y);
  Gll::gllVertex(cax - offset.x, cay - offset.y + ch);
  Gll::gllVertex(cax - offset.x + cw, cay - offset.y + ch);
  Gll::gllVertex(cax - offset.x + cw, cay - offset.y);
  Gll::gllEnd();

  Gll::gllBegin(Gll::GLL_QUADS);
  Gll::gllColor(getGUI_ctx()->activeColor);
  Gll::gllVertex(cax - offset.x + cw - 20, cay - offset.y + ch - 20);
  Gll::gllVertex(cax - offset.x + cw - 20, cay - offset.y + ch);
  Gll::gllVertex(cax - offset.x + cw, cay - offset.y + ch);
  Gll::gllVertex(cax - offset.x + cw, cay - offset.y + ch - 20);
  Gll::gllEnd();

  Gll::gllBegin(Gll::GLL_QUADS);
  Gll::gllColor(getGUI_ctx()->activeColor);
  {
    iVec2 pos = iVec2(cax, cay + ch - 25) - offset;
    for (auto&& it : _inputs) {
      Gll::gllVertex(pos.x, pos.y - 20);
      Gll::gllVertex(pos.x, pos.y);
      Gll::gllVertex(pos.x + 20, pos.y);
      Gll::gllVertex(pos.x + 20, pos.y - 20);
      
      auto inp = it.second->_bindings.front().lock();
      iVec2 inp_pos;
      if (inp != nullptr && inp->_filter.lock()->findOutput(inp, inp_pos)) {
        inp_pos -= offset;
        Gll::gllVertex(inp_pos.x, inp_pos.y - 5);
        Gll::gllVertex(inp_pos.x, inp_pos.y + 5);
        Gll::gllVertex(pos.x + 10, pos.y - 5);
        Gll::gllVertex(pos.x + 10, pos.y - 15);
      }

      pos += iVec2(0, -25);
    }
  }
  {
    iVec2 pos = iVec2(cax + cw - 20, cay + ch - 25) - offset;
    for (auto&& it : _outputs) {
      Gll::gllVertex(pos.x, pos.y - 20);
      Gll::gllVertex(pos.x, pos.y);
      Gll::gllVertex(pos.x + 20, pos.y);
      Gll::gllVertex(pos.x + 20, pos.y - 20);
      pos += iVec2(0, -25);
    }
  }
  {
    iVec2 pos = iVec2(cax + 25, cay + ch) - offset;
    for (auto&& it : _params) {
      Gll::gllVertex(pos.x, pos.y - 20);
      Gll::gllVertex(pos.x, pos.y);
      Gll::gllVertex(pos.x + 20, pos.y);
      Gll::gllVertex(pos.x + 20, pos.y - 20);

      auto inp = it.second->_bindings.front().lock();
      iVec2 inp_pos;
      if (inp != nullptr && inp->_filter.lock()->findOutput(inp, inp_pos)) {
        inp_pos -= offset;
        Gll::gllVertex(inp_pos.x, inp_pos.y - 5);
        Gll::gllVertex(inp_pos.x, inp_pos.y + 5);
        Gll::gllVertex(pos.x + 10, pos.y - 5);
        Gll::gllVertex(pos.x + 10, pos.y - 15);
      }

      pos += iVec2(25, 0);
    }
  }
  Gll::gllEnd();

  render(offset, down);
}

bool Filter::findOutput(shared_ptr<Filter_Resource_Output> val, iVec2& pos) {
  pos = iVec2(cax + cw - 20, cay + ch - 25);
  for (auto&& it : _outputs) {
    if (it.second == val) {
      pos += iVec2(10, -10);
      return true;
    }
    pos += iVec2(0, -25);
  }
  return false;
}

int Filter::mouseEnter(iVec2 offset, int state) {
  return 0;
}

int Filter::mouseMoved(iVec2 offset, int mx, int my, int ox, int oy, set<key_location>& down) {
  return 0;
}
int Filter::guiEvent(iVec2 offset, gui_event evt, int mx, int my, set<key_location>& down) {
  return 0;
}
void Filter::render(iVec2 offset, set<key_location>& down) {

}

void Filter::updateSize() {
  iVec2 internalSize = getInternalSize();
  cw = 25 + 25 + max<int>(_params.size() * 25 - 5, internalSize.x);
  ch = 25 + 25 + max<int>(max<int>(_inputs.size(), _outputs.size()) * 25 - 5, internalSize.y);
}

iVec2 Filter::getInternalSize() {
  return iVec2(0, 0);
}

#endif

void EditorContext::init() {
  _globalDummy = make_shared<Filter>(weak_from_this());
  _exit = make_shared<FilterExitNode>(weak_from_this());
  _exit->init();
  _filters.push_back(_exit);
}
