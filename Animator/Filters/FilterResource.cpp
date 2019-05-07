#include "FilterResource.h"

Raw_FrameBuffer::Raw_FrameBuffer(iVec2 size) {
  glGenFramebuffers(1, &_id);
  _size = size;

  _depth_attachment = 0;
}

void Raw_FrameBuffer::attachColor(int colorInd, GLenum format, GLenum type) {
  if (_col_attachments.count(colorInd)) {
    throw "Color buffer already attached";
  }

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
}
void Raw_FrameBuffer::attachDepth() {
  // The depth buffer
  if (hasDepth()) {
    throw "Depth buffer already attached";
  }

  glBindFramebuffer(GL_FRAMEBUFFER, _id);

  GLuint depthrenderbuffer;
  glGenRenderbuffers(1, &depthrenderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _size.x, _size.y);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

  _depth_attachment = depthrenderbuffer;
}
bool Raw_FrameBuffer::hasDepth() {
  return _depth_attachment != 0;
}
bool Raw_FrameBuffer::valid() {
  glBindFramebuffer(GL_FRAMEBUFFER, _id);

  // Always check that our framebuffer is ok
  return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
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

//Object

Filter_Resource_Object::Filter_Resource_Object() : Filter_Resource() {

}
Filter_Resource::Type Filter_Resource_Object::staticType() {
  return Type_Object;
}
Filter_Resource::Type Filter_Resource_Object::type() {
  return staticType();
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

Filter_Resource_Output::Filter_Resource_Output(weak_ptr<Filter> filter, string internalName, string displayName, string description, shared_ptr<Filter_Resource> res) {
  _filter = filter;
  _internalName = internalName;
  _displayName = displayName;
  _description = description;
  _res = res;
}
Filter_Resource::Type Filter_Resource_Output::type() const {
  return _res->type();
}

//Input

Filter_Resource_Input::Filter_Resource_Input(weak_ptr<Filter> filter, string internalName, string displayName, string description, Filter_Resource::Type type, bool isArray) {
  _filter = filter;
  _internalName = internalName;
  _displayName = displayName;
  _description = description;
  _type = type;
  _isArray = isArray;
  cleanBindings();
}
void Filter_Resource_Input::bindInput(weak_ptr<Filter_Resource_Output> binding, int to, bool before) {
  auto binding_l = binding.lock();
  assert(_type == binding_l->type());

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
