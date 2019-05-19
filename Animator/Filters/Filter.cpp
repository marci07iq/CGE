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
  cax = 0;
  cay = 0;
#endif
}

void Filter::init() {

}
void Filter::configure() {

}

void Filter::calculate(float t) {

}

void Filter::addInput(string internalName, string displayName, string description, Filter_Resource::Type type, bool isArray
#ifdef M_CLIENT
  , NGin::Graphics::Icon* icon
#endif
) {
  _inputs[internalName] = make_shared<Filter_Resource_Input>(weak_from_this(), internalName, displayName, description, Filter_Resource_IO_Base::Restriction_Dynamic, type, isArray
#ifdef M_CLIENT
    , icon
#endif
    );
}
void Filter::addParam(string internalName, string displayName, string description, Filter_Resource::Type type, bool isArray
#ifdef M_CLIENT
  , NGin::Graphics::Icon* icon
#endif
) {
  _params[internalName] = make_shared<Filter_Resource_Input>(weak_from_this(), internalName, displayName, description, Filter_Resource_IO_Base::Restriction_Static, type, isArray
#ifdef M_CLIENT
    , icon
#endif
    );
}
void Filter::addOutput(string internalName, string displayName, string description, Filter_Resource_IO_Base::Restriction restriction, shared_ptr<Filter_Resource> res
#ifdef M_CLIENT
  , NGin::Graphics::Icon* icon
#endif
) {
  _outputs[internalName] = make_shared<Filter_Resource_Output>(weak_from_this(), internalName, displayName, description, restriction, res
#ifdef M_CLIENT
    , icon
#endif
    );
}

#ifdef M_CLIENT
const int FilterGUI::toolbar_width = 24;

FilterGUI::FilterGUI(string name, NGin::Graphics::LocationData llocation, colorargb lbgcolor, colorargb lactivecolor, colorargb ltextcolor, shared_ptr<EditorContext> editor)
  : NGin::Graphics::GUIElement(name, llocation, lbgcolor, lactivecolor, ltextcolor, editor.get()) {
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

int FilterGUI::mouseMoved(int mx, int my, int ox, int oy, std::set<NGin::Graphics::key_location>& down) {
  int res = 0;
  if (dragging) {
    dragOffset += iVec2(ox - mx, oy - my);
    res |= 1;
  }

  for (auto&& filt : _editor.lock()->_filters) {
    res |= filt->mouseMoved_base(dragOffset, mx, my, ox, oy, down);
    if (res & 2) { return res; }
  }
  if (!_draggedConnection.expired()) {
    res |= 1;
  }
  if (
    cax <= ox && ox <= cax + 2 * Filter_Resource_IO_Base::iconPad + Filter_Resource_IO_Base::iconWidth&&
    cay <= oy && oy <= cby
  ) {
    res |= 1;
  }
  return res;
}

int FilterGUI::guiEvent(NGin::Graphics::gui_event& evt, int mx, int my, std::set<NGin::Graphics::key_location>& down) {
  int res = 0;
  if (evt._key._type == NGin::Graphics::key::type_mouse && !evt.captured && evt._type == NGin::Graphics::gui_event::evt_down && isIn(mx, my)) {
    if (evt._key._keycode == GLFW_MOUSE_BUTTON_LEFT) {
      _draggedConnection.reset();
      res |= 1;
      if (
        cax <= mx && mx <= cax + 2 * Filter_Resource_IO_Base::iconPad + Filter_Resource_IO_Base::iconWidth &&
        cay <= my && my <= cby
      ) {
        iVec2 pos = iVec2(cax + Filter_Resource_IO_Base::iconPad, cby);
        pos -= iVec2(0, Filter_Resource_IO_Base::iconPad + Filter_Resource_IO_Base::iconWidth);

        for (auto&& it : EditorContext::_registeredFilters) {
          pos -= iVec2(0, Filter_Resource_IO_Base::iconPad + Filter_Resource_IO_Base::iconWidth);

          if (
            pos.x <= mx && mx <= pos.x + Filter_Resource_IO_Base::iconWidth &&
            pos.y <= my && my <= pos.y + Filter_Resource_IO_Base::iconWidth
            ) {

            shared_ptr<Filter> filt = it.second._constructor(_editor);
            filt->init();
            filt->cax = mx + dragOffset.x;
            filt->cay = my + dragOffset.y;
            filt->dragging = true;
            _editor.lock()->_filters.push_back(filt);
            evt.captured = true;

            break;
          }

          NGin::Gll::gllColor(textColor);
          NGin::Gll::gllIcon(it.second._icon, pos.x, pos.y, pos.x + Filter_Resource_IO_Base::iconWidth, pos.y + Filter_Resource_IO_Base::iconWidth);
        }
      }
    }
    if (evt._key._keycode == GLFW_MOUSE_BUTTON_RIGHT) {
      dragging = true;
      evt.captured = true;
    }
  }

  if (evt._key._type == NGin::Graphics::key::type_mouse && evt._type == NGin::Graphics::gui_event::evt_up && isIn(mx, my) && evt._key._keycode == GLFW_MOUSE_BUTTON_LEFT) {
    if (
      cax <= mx && mx <= cax + 2 * Filter_Resource_IO_Base::iconPad + Filter_Resource_IO_Base::iconWidth &&
      cay <= my && my <= cby
      ) {
      iVec2 pos = iVec2(cax + Filter_Resource_IO_Base::iconPad, cby);
      pos -= iVec2(0, Filter_Resource_IO_Base::iconPad + Filter_Resource_IO_Base::iconWidth);

      if (
        pos.x <= mx && mx <= pos.x + Filter_Resource_IO_Base::iconWidth &&
        pos.y <= my && my <= pos.y + Filter_Resource_IO_Base::iconWidth
      ) {
        _editor.lock()->_filters.remove_if([](shared_ptr<Filter>& f) { return f->dragging; });
      }
    }
  }

  for (auto&& filt : _editor.lock()->_filters) {
    res |= filt->guiEvent_base(dragOffset, evt, mx, my, down);
    if (res & 2) { return res; }
  }

  if (evt._key._type == NGin::Graphics::key::type_mouse && !evt.captured && evt._type == NGin::Graphics::gui_event::evt_up && isIn(mx, my)) {
    if (evt._key._keycode == GLFW_MOUSE_BUTTON_LEFT) {
      _draggedConnection.reset();
      res |= 1;
    }
    if (evt._key._keycode == GLFW_MOUSE_BUTTON_RIGHT) {
      dragging = false;
    }
  }
  return res;
}

void FilterGUI::render(set<NGin::Graphics::key_location>& down) {
  shared_ptr<EditorContext> editor = _editor.lock();

  for (auto&& filt : editor->_filters) {
    filt->render_base(dragOffset - iVec2(cax, cay), down);
  }


  auto inp = _draggedConnection.lock();
  iVec2 inp_pos;
  if (inp != nullptr && inp->_filter.lock()->findOutput(inp, inp_pos)) {
    inp_pos -= dragOffset - iVec2(cax, cay);
    NGin::Gll::gllBegin(NGin::Gll::GLL_QUADS);
    NGin::Gll::gllColor(activeColor);
    NGin::Gll::gllVertex(inp_pos.x, inp_pos.y - 5);
    NGin::Gll::gllVertex(inp_pos.x, inp_pos.y + 5);
    NGin::Gll::gllVertex(NGin::Graphics::current->oldMouseX, NGin::Graphics::current->oldMouseY + 5);
    NGin::Gll::gllVertex(NGin::Graphics::current->oldMouseX, NGin::Graphics::current->oldMouseY - 5);
    NGin::Gll::gllEnd();
  }

  NGin::Gll::gllBegin(NGin::Gll::GLL_QUADS);
  NGin::Gll::gllColor(bgColor);
  NGin::Gll::gllVertex(cax, cay);
  NGin::Gll::gllVertex(cax, cby);
  NGin::Gll::gllVertex(cax + 2* Filter_Resource_IO_Base::iconPad + Filter_Resource_IO_Base::iconWidth, cby);
  NGin::Gll::gllVertex(cax + 2* Filter_Resource_IO_Base::iconPad + Filter_Resource_IO_Base::iconWidth, cay);
  NGin::Gll::gllEnd();

  iVec2 pos = iVec2(cax + Filter_Resource_IO_Base::iconPad, cby);
  pos -= iVec2(0, Filter_Resource_IO_Base::iconPad + Filter_Resource_IO_Base::iconWidth);

  if (
    pos.x <= NGin::Graphics::current->oldMouseX && NGin::Graphics::current->oldMouseX <= pos.x + Filter_Resource_IO_Base::iconWidth &&
    pos.y <= NGin::Graphics::current->oldMouseY && NGin::Graphics::current->oldMouseY <= pos.y + Filter_Resource_IO_Base::iconWidth
    ) {
    NGin::Gll::gllBegin(NGin::Gll::GLL_QUADS);
    NGin::Gll::gllColor(activeColor);
    NGin::Gll::gllVertex(pos.x, pos.y);
    NGin::Gll::gllVertex(pos.x, pos.y + Filter_Resource_IO_Base::iconWidth);
    NGin::Gll::gllVertex(pos.x + Filter_Resource_IO_Base::iconWidth, pos.y + Filter_Resource_IO_Base::iconWidth);
    NGin::Gll::gllVertex(pos.x + Filter_Resource_IO_Base::iconWidth, pos.y);
    NGin::Gll::gllEnd();
  }

  NGin::Gll::gllColor(textColor);
  NGin::Gll::gllIcon(NGin::Graphics::getIcon("close", ilfPath), pos.x, pos.y, pos.x + Filter_Resource_IO_Base::iconWidth, pos.y + Filter_Resource_IO_Base::iconWidth);

  for (auto&& it : EditorContext::_registeredFilters) {
    pos -= iVec2(0, Filter_Resource_IO_Base::iconPad + Filter_Resource_IO_Base::iconWidth);

    if (
      pos.x <= NGin::Graphics::current->oldMouseX && NGin::Graphics::current->oldMouseX <= pos.x + Filter_Resource_IO_Base::iconWidth &&
      pos.y <= NGin::Graphics::current->oldMouseY && NGin::Graphics::current->oldMouseY <= pos.y + Filter_Resource_IO_Base::iconWidth
      ) {
      NGin::Gll::gllBegin(NGin::Gll::GLL_QUADS);
      NGin::Gll::gllColor(activeColor);
      NGin::Gll::gllVertex(pos.x, pos.y);
      NGin::Gll::gllVertex(pos.x, pos.y + Filter_Resource_IO_Base::iconWidth);
      NGin::Gll::gllVertex(pos.x + Filter_Resource_IO_Base::iconWidth, pos.y + Filter_Resource_IO_Base::iconWidth);
      NGin::Gll::gllVertex(pos.x + Filter_Resource_IO_Base::iconWidth, pos.y);
      NGin::Gll::gllEnd();
    }

    NGin::Gll::gllColor(textColor);
    NGin::Gll::gllIcon(it.second._icon, pos.x, pos.y, pos.x + Filter_Resource_IO_Base::iconWidth, pos.y + Filter_Resource_IO_Base::iconWidth);
  }
}

bool Filter::isIn(iVec2 offset, int mx, int my) {
  return
    cax <= offset.x + mx && offset.x + mx <= cax + cw &&
    cay <= offset.y + my && offset.y + my <= cay + ch;
}

int Filter::mouseMoved_base(iVec2 offset, int mx, int my, int ox, int oy, std::set<NGin::Graphics::key_location>& down) {
  int res = 0;

  if (dragging) {
    cax += mx - ox;
    cay += my - oy;
    res = 1;
  }

  iVec2 pos;
  pos = iVec2(cax, cay + ch - Filter_Resource_IO_Base::iconWidth - Filter_Resource_IO_Base::iconPad) - offset;
  for (auto&& it : _inputs) {
    res |= it.second->mouseMoved(iVec2(mx, my) - pos, iVec2(ox, oy) - pos, down);
    pos = it.second->next(pos);
    if (res & 2)  return res;
  }
  pos = iVec2(cax + cw - Filter_Resource_IO_Base::iconWidth, cay + ch - Filter_Resource_IO_Base::iconWidth - Filter_Resource_IO_Base::iconPad) - offset;
  for (auto&& it : _outputs) {

    res |= it.second->mouseMoved(iVec2(mx, my) - pos, iVec2(ox, oy) - pos, down);
    pos = it.second->next(pos);
    if (res & 2)  return res;
  }
  pos = iVec2(cax + Filter_Resource_IO_Base::iconWidth + Filter_Resource_IO_Base::iconPad, cay + ch) - offset;
  for (auto&& it : _params) {

    res |= it.second->mouseMoved(iVec2(mx, my) - pos, iVec2(ox, oy) - pos, down);
    pos = it.second->next(pos);

    if (res & 2)  return res;
  }

  if (res & 2) return res;
  return res | mouseMoved(offset, mx, my, ox, oy, down);
}
int Filter::guiEvent_base(iVec2 offset, NGin::Graphics::gui_event evt, int mx, int my, std::set<NGin::Graphics::key_location>& down) {
  int res = 0;

  if (evt._key._type == NGin::Graphics::key::type_mouse && evt._key._keycode == GLFW_MOUSE_BUTTON_LEFT) {
    if (cax + cw - 20 <= offset.x + mx && offset.x + mx <= cax + cw &&
      cay + ch - 20 <= offset.y + my && offset.y + my <= cay + ch)
      if (!evt.captured && evt._type == NGin::Graphics::gui_event::evt_down) {
        evt.captured = true;
        res |= 2;
        dragging = true;
      }
    if (evt._type == NGin::Graphics::gui_event::evt_up) {
      dragging = false;
      res |= 1;
    }

    iVec2 pos;
    pos = iVec2(cax, cay + ch - Filter_Resource_IO_Base::iconWidth - Filter_Resource_IO_Base::iconPad) - offset;
    for (auto&& it : _inputs) {
      res |= it.second->guiEvent(evt, iVec2(mx, my) - pos, down);
      pos = it.second->next(pos);
      if (res & 2)  return res;
    }
    pos = iVec2(cax + cw - Filter_Resource_IO_Base::iconWidth, cay + ch - Filter_Resource_IO_Base::iconWidth - Filter_Resource_IO_Base::iconPad) - offset;
    for (auto&& it : _outputs) {

      res |= it.second->guiEvent(evt, iVec2(mx, my) - pos, down);
      pos = it.second->next(pos);
      if (res & 2)  return res;
    }
    pos = iVec2(cax + Filter_Resource_IO_Base::iconWidth + Filter_Resource_IO_Base::iconPad, cay + ch) - offset;
    for (auto&& it : _params) {

      res |= it.second->guiEvent(evt, iVec2(mx, my) - pos, down);
      pos = it.second->next(pos);

      if (res & 2)  return res;
    }
  }

  if (res & 2) return res;
  return res | guiEvent(offset, evt, mx, my, down);
}
void Filter::render_base(iVec2 offset, std::set<NGin::Graphics::key_location>& down) {
  NGin::Gll::gllBegin(NGin::Gll::GLL_QUADS);
  NGin::Gll::gllColor(getGUI_ctx()->bgColor);
  NGin::Gll::gllVertex(cax - offset.x, cay - offset.y);
  NGin::Gll::gllVertex(cax - offset.x, cay - offset.y + ch);
  NGin::Gll::gllVertex(cax - offset.x + cw, cay - offset.y + ch);
  NGin::Gll::gllVertex(cax - offset.x + cw, cay - offset.y);
  NGin::Gll::gllEnd();

  NGin::Gll::gllBegin(NGin::Gll::GLL_QUADS);
  NGin::Gll::gllColor(getGUI_ctx()->activeColor);
  NGin::Gll::gllVertex(cax - offset.x + cw - 20, cay - offset.y + ch - 20);
  NGin::Gll::gllVertex(cax - offset.x + cw - 20, cay - offset.y + ch);
  NGin::Gll::gllVertex(cax - offset.x + cw, cay - offset.y + ch);
  NGin::Gll::gllVertex(cax - offset.x + cw, cay - offset.y + ch - 20);
  NGin::Gll::gllEnd();

  {
    iVec2 pos = iVec2(cax, cay + ch - Filter_Resource_IO_Base::iconWidth - Filter_Resource_IO_Base::iconPad) - offset;
    for (auto&& it : _inputs) {
      it.second->render(pos, down);
      pos = it.second->next(pos);
    }
  }
  {
    iVec2 pos = iVec2(cax + cw - Filter_Resource_IO_Base::iconWidth, cay + ch - Filter_Resource_IO_Base::iconWidth - Filter_Resource_IO_Base::iconPad) - offset;
    for (auto&& it : _outputs) {
      it.second->render(pos, down);
      pos = it.second->next(pos);
    }
  }
  {
    iVec2 pos = pos = iVec2(cax + Filter_Resource_IO_Base::iconWidth + Filter_Resource_IO_Base::iconPad, cay + ch) - offset;
    for (auto&& it : _params) {
      it.second->render(pos, down);
      pos = it.second->next(pos);
    }
  }
  NGin::Gll::gllEnd();

  render(offset, down);
}

bool Filter::findOutput(shared_ptr<Filter_Resource_Output> val, iVec2& pos) {
  pos = iVec2(cax + cw - Filter_Resource_IO_Base::iconWidth, cay + ch - Filter_Resource_IO_Base::iconWidth - Filter_Resource_IO_Base::iconPad);
  for (auto&& it : _outputs) {
    if (it.second == val) {
      pos += iVec2(Filter_Resource_IO_Base::iconWidth / 2, -Filter_Resource_IO_Base::iconWidth / 2);
      return true;
    }
    pos = it.second->next(pos);
  }
  return false;
}

int Filter::mouseEnter(iVec2 offset, int state) {
  return 0;
}

int Filter::mouseMoved(iVec2 offset, int mx, int my, int ox, int oy, std::set<NGin::Graphics::key_location>& down) {
  return 0;
}
int Filter::guiEvent(iVec2 offset, NGin::Graphics::gui_event evt, int mx, int my, std::set<NGin::Graphics::key_location>& down) {
  return 0;
}
void Filter::render(iVec2 offset, std::set<NGin::Graphics::key_location>& down) {

}

void Filter::updateSize() {
  iVec2 internalSize = getInternalSize();

  int in_size = 0;
  for (auto&& it : _inputs) {
    in_size += it.second->hasBinding() + (it.second->_isArray ? 1 : 0);
  }

  cw = 
    2*(Filter_Resource_IO_Base::iconWidth + Filter_Resource_IO_Base::iconPad) +
    max<int>(_params.size() * (Filter_Resource_IO_Base::iconWidth + Filter_Resource_IO_Base::iconPad) - Filter_Resource_IO_Base::iconPad, internalSize.x);
  ch = 2*(Filter_Resource_IO_Base::iconWidth + Filter_Resource_IO_Base::iconPad) +
    max<int>(max<int>(in_size, _outputs.size()) * (Filter_Resource_IO_Base::iconWidth + Filter_Resource_IO_Base::iconPad) - Filter_Resource_IO_Base::iconPad, internalSize.y);
}

iVec2 Filter::getInternalSize() {
  return iVec2(0, 0);
}

#endif

map<string, FilterDescription> EditorContext::_registeredFilters;
void EditorContext::registerFilter(string name, FilterDescription creator) {
  _registeredFilters[name] = creator;
}

void EditorContext::init() {
  _filters.clear();

  _globalDummy = make_shared<Filter>(weak_from_this());
  _exit = make_shared<FilterExitNode>(weak_from_this());
  _exit->init();
  _filters.push_back(_exit);
}

void EditorContext::load_caf(string filename) {

}

void EditorContext::save_caf(string filename) {

}
