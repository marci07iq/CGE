#pragma once

#include "../Filter.h"

class FilterLiteral : public Filter {
public:
  
  NGin::Graphics::TextInputHwnd _valInput;

  static void staticInit() {
    static bool first = true;
    if (first) {
      first = false;

      
    }
  }

  FilterLiteral(weak_ptr<EditorContext> ctx) : Filter(ctx) {
    staticInit();
  }

  void init() {
    _valInput = NGin::Graphics::createGUI_T<TextInput>(
      "data",
      LocationData(LinearScale(0, 0), LinearScale(1, 0), LinearScale(0, 0), LinearScale(1, 0)),
      getColor("input", "bgcolor"),
      getColor("input", "activecolor"),
      getColor("input", "textcolor"),
      this,
      "",
      valInput_cb_stat);
    _valInput->getRect(200,20,Filter_Resource_IO_Base::iconWidth + Filter_Resource_IO_Base::iconPad, Filter_Resource_IO_Base::iconWidth + Filter_Resource_IO_Base::iconPad);
    updateSize();
  }



  void configure() {
    
  }

  void calculate(float t) {
    
  }

  void valInput_cb(string& s);
  static void valInput_cb_stat(NGin::Graphics::ElemHwnd elem, string& s);

#ifdef M_CLIENT
  int mouseEnter(iVec2 offset, int state);
private:
  //Do not call from outside
  int mouseMoved(iVec2 offset, int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(iVec2 offset, gui_event evt, int mx, int my, set<key_location>& down);
  void render(iVec2 offset, set<key_location>& down);


  iVec2 getInternalSize() {
    return iVec2(200, 20);
  }
#endif
};
