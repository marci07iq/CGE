#include "FilterLiteral.h"

void FilterLiteral::valInput_cb(string& s) {
  int i = 0;
  shared_ptr<Filter_Resource> res = Filter_Resource_Object::parseString_arb(s, i);

  if (res == nullptr) return;

  if (_outputs["data"] != nullptr && res->type() == _outputs["data"]->type()) {
    _outputs["data"]->_res = res;
  } else {
    addOutput("data", "Data", "The output data", Filter_Resource_IO_Base::Restriction_Static, res, NGin::Graphics::getIcon("out", ilfPath));
  }
}

void FilterLiteral::valInput_cb_stat(NGin::Graphics::ElemHwnd elem, string& s) {
  if (!elem->active) {
    ((FilterLiteral*)(elem->data))->valInput_cb(s);
  }
}

int FilterLiteral::mouseEnter(iVec2 offset, int state) {
  return _valInput->mouseEnter(state);
}

int FilterLiteral::mouseMoved(iVec2 offset, int mx, int my, int ox, int oy, std::set<NGin::Graphics::key_location>& down) {
  return _valInput->mouseMoved(mx + offset.x - cax, my + offset.y - cay, ox + offset.x - cax, oy + offset.y - cay, down);
}

int FilterLiteral::guiEvent(iVec2 offset, NGin::Graphics::gui_event evt, int mx, int my, std::set<NGin::Graphics::key_location>& down) {
  return _valInput->guiEvent(evt, mx + offset.x - cax, my + offset.y - cay, down);
}

void FilterLiteral::render(iVec2 offset, std::set<NGin::Graphics::key_location>& down) {
  NGin::Gll::pushOffset(-offset + iVec2(cax, cay));
  _valInput->render(down);
  NGin::Gll::popOffset();
}
