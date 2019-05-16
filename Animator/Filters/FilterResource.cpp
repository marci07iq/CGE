#include "Filter.h"

Raw_FrameBuffer::Raw_FrameBuffer(iVec2 size) {
  glfwMakeContextCurrent(objectPreviewWindowHwnd->rawHwnd);

  glGenFramebuffers(1, &_id);
  _size = size;

  _depth_attachment = 0;

  glfwMakeContextCurrent(NGin::Graphics::current->rawHwnd);
}

void Raw_FrameBuffer::attachColor(int colorInd, GLenum format, GLenum type) {
  if (_col_attachments.count(colorInd)) {
    throw "Color buffer already attached";
  }

  glfwMakeContextCurrent(objectPreviewWindowHwnd->rawHwnd);


  glBindFramebuffer(GL_FRAMEBUFFER, _id);

  // The texture we're going to render to
  GLuint renderedTexture;
  glGenTextures(1, &renderedTexture);

  // "Bind" the newly created texture : all future texture functions will modify this texture
  glBindTexture(GL_TEXTURE_2D, renderedTexture);

  // Give an empty image to OpenGL ( the last "0" )
  glTexImage2D(GL_TEXTURE_2D, 0, format, _size.x, _size.y, 0, format, type, 0);

  // Poor filtering. Needed !
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorInd, renderedTexture, 0);

  _col_attachments[colorInd] = renderedTexture;

  glfwMakeContextCurrent(NGin::Graphics::current->rawHwnd);

}
void Raw_FrameBuffer::attachDepth() {
  // The depth buffer
  if (hasDepth()) {
    throw "Depth buffer already attached";
  }

  glfwMakeContextCurrent(objectPreviewWindowHwnd->rawHwnd);

  glBindFramebuffer(GL_FRAMEBUFFER, _id);

  GLuint depthrenderbuffer;
  glGenRenderbuffers(1, &depthrenderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _size.x, _size.y);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

  _depth_attachment = depthrenderbuffer;

  glfwMakeContextCurrent(NGin::Graphics::current->rawHwnd);

}
bool Raw_FrameBuffer::hasDepth() {
  return _depth_attachment != 0;
}
bool Raw_FrameBuffer::valid() {
  glfwMakeContextCurrent(objectPreviewWindowHwnd->rawHwnd);

  glBindFramebuffer(GL_FRAMEBUFFER, _id);

  // Always check that our framebuffer is ok
  bool res = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
  glfwMakeContextCurrent(NGin::Graphics::current->rawHwnd);

  return res;
}
void Raw_FrameBuffer::bind() {
  glBindFramebuffer(GL_FRAMEBUFFER, _id);
  glViewport(0, 0, _size.x, _size.y);

  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
}

Raw_FrameBuffer::~Raw_FrameBuffer() {

}

//Filter Resource

Filter_Resource::Filter_Resource() {}
Filter_Resource::Type Filter_Resource::type() {
  return Type_Undef;
}

void Filter_Resource::parseString(const string& s, int& i) {
}

void Filter_Resource::parseString_ignore(const string& s, int& i) {
  while (i < s.size() && s[i] == ' ') {
    ++i;
  }
}

//Color

Filter_Resource_RenderBuffer::Filter_Resource_RenderBuffer(FrameBuffer source, vector<int> col_att_ids) : Filter_Resource() {
  _source = source;
  _col_att_ids = col_att_ids;
}
Filter_Resource::Type Filter_Resource_RenderBuffer::staticType() {
  return Type_RenderBuffer;
}
Filter_Resource::Type Filter_Resource_RenderBuffer::type() {
  return staticType();
}

//Float

Filter_Resource_Float::Filter_Resource_Float(float val) : Filter_Resource() {
  _val = val;
}
Filter_Resource::Type Filter_Resource_Float::staticType() {
  return Type_Float;
}
Filter_Resource::Type Filter_Resource_Float::type() {
  return staticType();
}

int Filter_Resource_Float::parseString_int(const string& s, int& i, bool allowNeg) {
  bool negsign = false;
  if (s[i] == '-') {
    ++i;
    if (allowNeg) {
      negsign = true;
    } else {
      return 0;
    }
  }

  int res = 0;

  while (i < s.size() && '0' <= s[i] && s[i] <= '9') {
    res = 10 * res + s[i] - '0';
    ++i;
  }
  if (negsign) {
    res = -res;
  }
  return res;
}

float Filter_Resource_Float::parseString_float(const string& s, int& i) {
  float main = parseString_int(s, i, true);
  if (s[i] == '.') {
    int io = ++i;
    float frac = parseString_int(s, i, false);
    frac *= pow(frac, io - i);
    main += frac;
  }
  if (s[i] == 'e' || s[i] == 'E') {
    int exp = parseString_int(s, i, true);
    main *= pow(10, exp);
  }
  parseString_ignore(s, i);

  return main;
}

void Filter_Resource_Float::parseString(const string& s, int& i) {
  _val = parseString_float(s, i);
}

bool Filter_Resource_Float::peekType_float(const string& s, const int& i) {
  if ('0' <= s[i] && s[i] <= '9') return true;
  if (s[i] == '.' || s[i] == '-') return true;
  if (s[i] == 'e' || s[i] == 'E') return true;
  return false;
}

//String

Filter_Resource_String::Filter_Resource_String(string val) : Filter_Resource() {
  _val = val;
}
Filter_Resource::Type Filter_Resource_String::staticType() {
  return Type_String;
}
Filter_Resource::Type Filter_Resource_String::type() {
  return staticType();
}

string Filter_Resource_String::parseString_string(const string& s, int& i) {
  if (s[i] != '\"') return "";
  ++i;
  string res = "";
  while (i < s.size() && s[i] != '\"') {
    char c = s[i];
    if (c == '\\') {
      ++i;
      if (i < s.size()) {
        switch (s[i]) {
        case 'a':
          c = '\a';
          break;
        case 'b':
          c = '\b';
          break;
        case 'e':
          c = '\e';
          break;
        case 'f':
          c = '\f';
          break;
        case 'n':
          c = '\n';
          break;
        case 'r':
          c = '\r';
          break;
        case 't':
          c = '\t';
          break;
        case 'v':
          c = '\v';
          break;
        case '\\':
          c = '\\';
          break;
        case '\'':
          c = '\'';
          break;
        case '\"':
          c = '\"';
          break;
        case '\?':
          c = '\?';
          break;
        }
      } else {
        return res;
      }
    }
    ++i;
    res += c;
  }
  ++i;
  parseString_ignore(s, i);
  return res;

}

void Filter_Resource_String::parseString(const string& s, int& i) {
  _val = parseString_string(s, i);
}
bool Filter_Resource_String::peekType_string(const string& s, const int& i) {
  if (s[i] == '\"') return true;
  return false;
}

//Object

Filter_Resource_Object::Filter_Resource_Object() : Filter_Resource() {

}
Filter_Resource::Type Filter_Resource_Object::staticType() {
  return Type_Object;
}
Filter_Resource::Type Filter_Resource_Object::type() {
  return staticType();
}

map<string, shared_ptr<Filter_Resource>> Filter_Resource_Object::parseString_obj(const string& s, int& i) {
  map<string, shared_ptr<Filter_Resource>> res;

  if (s[i] != '{') return res;
  ++i;
  parseString_ignore(s, i);

  while (i < s.size() && s[i] != '}') {
    string name = Filter_Resource_String::parseString_string(s, i);
    parseString_ignore(s, i);
    if (i >= s.size()) return res;
    if (s[i] == ':') {
      ++i;
    }
    parseString_ignore(s, i);

    res[name] = parseString_arb(s, i);

    parseString_ignore(s, i);
    if (s[i] == ',') {
      ++i;
    }
    parseString_ignore(s, i);
  }
  ++i;

  return res;
}

shared_ptr<Filter_Resource> Filter_Resource_Object::parseString_arb(const string& s, int& i) {
  parseString_ignore(s, i);

  if (Filter_Resource_Object::peekType_obj(s, i)) {
    shared_ptr<Filter_Resource_Object> obj = make_shared<Filter_Resource_Object>();
    obj->parseString(s, i);
    return obj;
  }
  if (Filter_Resource_Float::peekType_float(s, i)) {
    shared_ptr<Filter_Resource_Float> flo = make_shared<Filter_Resource_Float>(0);
    flo->parseString(s, i);
    return flo;
  }
  if (Filter_Resource_String::peekType_string(s, i)) {
    shared_ptr<Filter_Resource_String> str = make_shared<Filter_Resource_String>("");
    str->parseString(s, i);
    return str;
  }

  return nullptr;
}

void Filter_Resource_Object::parseString(const string& s, int& i) {
  _elems = parseString_obj(s, i);
}
bool Filter_Resource_Object::peekType_obj(const string& s, const int& i) {
  if (s[i] == '{') return true;
  return false;
}

//VAO

Filter_Resource_VAO::Filter_Resource_VAO() : Filter_Resource() {
  
}
Filter_Resource::Type Filter_Resource_VAO::staticType() {
  return Type_VAO;
}
Filter_Resource::Type Filter_Resource_VAO::type() {
  return staticType();
}

//Shader

Filter_Resource_Shader::Filter_Resource_Shader() : Filter_Resource() {

}
Filter_Resource::Type Filter_Resource_Shader::staticType() {
  return Type_Shader;
}
Filter_Resource::Type Filter_Resource_Shader::type() {
  return staticType();
}

//Output

Filter_Resource_Output::Filter_Resource_Output(weak_ptr<Filter> filter, string internalName, string displayName, string description, Restriction restriction, shared_ptr<Filter_Resource> res
#ifdef M_CLIENT
  , Icon* icon
#endif
) : Filter_Resource_IO_Base(filter, internalName, displayName, description, restriction
#ifdef M_CLIENT
  , icon
#endif
){
  _res = res;
}
Filter_Resource::Type Filter_Resource_Output::type() const {
  return _res->type();
}

//Input

Filter_Resource_Input::Filter_Resource_Input(weak_ptr<Filter> filter, string internalName, string displayName, string description, Restriction restriction, Filter_Resource::Type type, bool isArray
#ifdef M_CLIENT
  , Icon* icon
#endif
) : Filter_Resource_IO_Base(filter, internalName, displayName, description, restriction
#ifdef M_CLIENT
  , icon
#endif
) {
  _type = type;
  _isArray = isArray;
  cleanBindings();
}
bool Filter_Resource_Input::checkCompatibility(shared_ptr<Filter_Resource_Output> binding) {
  return (_type == binding->type() && compatibleRestriction(binding->_restriction, _restriction));
}
bool Filter_Resource_Input::tryBindInput(weak_ptr<Filter_Resource_Output> binding, int to, bool before) {
  auto binding_l = binding.lock();
  if (!checkCompatibility(binding_l)) return false;

  if (_isArray) {
    if (to == -1) {
      _bindings.push_back(binding);
    }
    else if (before) {
      auto it = _bindings.begin() + to;
      _bindings.insert(it, binding);
    } else {
      _bindings[to] = binding;
    }
  } else {
    assert(to == -1 || to == 0);
    _bindings[0] = binding;
  }

  cleanBindings();

  if (_restriction == Restriction_Static) {
    _filter.lock()->configure();
  }

  return true;
}
int Filter_Resource_Input::hasBinding() const {
  if (_isArray) {
    return _bindings.size() - 1;
  }
  return 1;
}
void Filter_Resource_Input::cleanBindings() {
  if (_isArray) {
    for (int i = 0; i < _bindings.size(); i++) {
      if (_bindings[i].expired()) {
        _bindings.erase(_bindings.begin() + i);
        --i;
      }
    }
    _bindings.push_back(weak_ptr<Filter_Resource_Output>());
  } else {
    _bindings.resize(1);
  }
}
Filter_Resource::Type Filter_Resource_Input::type() {
  return _type;
}

bool Filter_Resource_IO_Base::compatibleRestriction(Restriction out, Restriction in) {
  return (in == Restriction_Dynamic || out == Restriction_Static);
}

bool Filter_Resource_IO_Base::isIn(iVec2 localPos) {
  return band(vecSwitch(iVec2(0) < iconSize, iVec2(0), iconSize) <= localPos) & band(localPos <= vecSwitch(iVec2(0) > iconSize, iVec2(0), iconSize));
}

int Filter_Resource_IO_Base::mouseEnter(int state) {
  bool oactive = active;
  active &= state; //if mouse leaves, deactivate.
  return oactive xor active; //rerender if changed
}

int Filter_Resource_IO_Base::mouseMoved(iVec2 mouse_now, iVec2 mouse_old, set<key_location>& down) {
  bool oactive = active;
  active = isIn(mouse_now);
  return oactive xor active; //if state changed
}

int Filter_Resource_IO_Base::guiEvent(gui_event& evt, iVec2 mouse_now, set<key_location>& down) {
  return 0;
}

void Filter_Resource_IO_Base::render(iVec2 screenPos, set<key_location>& down) {
  shared_ptr<FilterGUI> gui = _filter.lock()->getGUI_ctx();
  Gll::gllBegin(Gll::GLL_QUADS);
  Gll::gllColor((active || _icon == nullptr) ? gui->activeColor : gui->bgColor);
  Gll::gllVertex(screenPos + iVec2(0, -iconWidth));
  Gll::gllVertex(screenPos + iVec2(0, 0));
  Gll::gllVertex(screenPos + iVec2(iconWidth, 0));
  Gll::gllVertex(screenPos + iVec2(iconWidth, -iconWidth));
  Gll::gllEnd();
  if (_icon != nullptr) {
    Gll::gllColor(gui->textColor);
    Gll::gllIcon(_icon, screenPos.x, screenPos.y - iconWidth, screenPos.x + iconWidth, screenPos.y);
  }
}

iVec2 Filter_Resource_IO_Base::next(iVec2 pos) {
  return pos;
}

int Filter_Resource_Output::guiEvent(gui_event& evt, iVec2 mouse_now, set<key_location>& down) {
  shared_ptr<FilterGUI> gui = _filter.lock()->getGUI_ctx();
  int res = 0;

  if (evt._key._type == key::type_mouse && evt._key._keycode == GLFW_MOUSE_BUTTON_LEFT) {
    if (!evt.captured && evt._type == gui_event::evt_down && isIn(mouse_now)) {
        evt.captured = true;
        gui->_draggedConnection = static_pointer_cast<Filter_Resource_Output, Filter_Resource_IO_Base>(shared_from_this());
        res |= 2;
    }
  }
  return res;
}
void Filter_Resource_Output::render(iVec2 screenPos, set<key_location>& down) {
  Filter_Resource_IO_Base::render(screenPos, down);
}

iVec2 Filter_Resource_Output::next(iVec2 pos) {
  return pos + nextV;
}


int Filter_Resource_Input::guiEvent(gui_event& evt, iVec2 mouse_now, set<key_location>& down) {
  shared_ptr<FilterGUI> gui = _filter.lock()->getGUI_ctx();
  int res = 0;

  if (evt._key._type == key::type_mouse && evt._key._keycode == GLFW_MOUSE_BUTTON_LEFT) {
    if (evt._type == gui_event::evt_up && isIn(mouse_now)) {
      shared_ptr<Filter_Resource_Output> dragged = gui->_draggedConnection.lock();
      if (!gui->_draggedConnection.expired()) {
        if(tryBindInput(dragged)) {
          res |= 1; 
        }
      }
    }
  }
  return res;
}
void Filter_Resource_Input::render(iVec2 screenPos, set<key_location>& down) {

  shared_ptr<FilterGUI> gui = _filter.lock()->getGUI_ctx();

  auto inp = _bindings.front().lock();
  iVec2 inp_pos;
  if (inp != nullptr && inp->_filter.lock()->findOutput(inp, inp_pos)) {
    inp_pos -= gui->dragOffset;

    Gll::gllBegin(Gll::GLL_QUADS);
    Gll::gllColor(gui->activeColor);
    Gll::gllVertex(inp_pos.x, inp_pos.y - 5);
    Gll::gllVertex(inp_pos.x, inp_pos.y + 5);
    Gll::gllVertex(screenPos.x + iconWidth / 2, screenPos.y - iconWidth / 2 + 5);
    Gll::gllVertex(screenPos.x + iconWidth / 2, screenPos.y - iconWidth / 2 - 5);
    Gll::gllEnd();
  }

  Filter_Resource_IO_Base::render(screenPos, down);
}

iVec2 Filter_Resource_Input::next(iVec2 pos) {
  return pos + nextH * _bindings.size();
}


const iVec2 Filter_Resource_IO_Base::iconSize = iVec2(iconWidth, -iconWidth); //from top left to bottom right corner
const iVec2 Filter_Resource_IO_Base::nextV = iVec2(0, -iconWidth - iconPad); //next vertically down
const iVec2 Filter_Resource_IO_Base::nextH = iVec2(iconWidth + iconPad, 0); //next horizontally right