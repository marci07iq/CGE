#include "FilterLiteral.h"

void FilterLiteral::valInput_cb(string& s) {
  int i = 0;
  shared_ptr<Filter_Resource> res = Filter_Resource_Object::parseString_arb(s, i);

  if (res == nullptr) return;

  addOutput("data", "Data", "The output data", Filter_Resource_IO_Base::Restriction_Static,res, getIcon("out", ilfPath));
}

void FilterLiteral::valInput_cb_stat(NGin::Graphics::ElemHwnd elem, string& s) {
  if (!elem->active) {
    ((FilterLiteral*)(elem->data))->valInput_cb(s);
  }
}

int FilterLiteral::mouseEnter(iVec2 offset, int state) {
  return _valInput->mouseEnter(state);
}

int FilterLiteral::mouseMoved(iVec2 offset, int mx, int my, int ox, int oy, set<key_location>& down) {
  return _valInput->mouseMoved(mx + offset.x - cax, my + offset.x - cay, ox + offset.x - cax, oy + offset.x - cay, down);
}

int FilterLiteral::guiEvent(iVec2 offset, gui_event evt, int mx, int my, set<key_location>& down) {
  return _valInput->guiEvent(evt, mx + offset.x - cax, my + offset.x - cay, down);
}

void FilterLiteral::render(iVec2 offset, set<key_location>& down) {
  Gll::pushOffset(-offset + iVec2(cax, cay));
  _valInput->render(down);
  Gll::popOffset();
}
