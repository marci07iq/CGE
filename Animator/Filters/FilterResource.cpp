#include "FilterResource.h"

Raw_FrameBuffer::Raw_FrameBuffer(iVec2 size) {
  glGenFramebuffers(1, &_id);
  _size = size;

  _depth_attachment = 0;
}

void Raw_FrameBuffer::attachColor(int colorInd, GLenum format, GLenum type) {
  if (_col_attachments.count(colorInd)) {
    throw 1;
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
  if (_depth_attachment != 0) {
    throw 1;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, _id);

  GLuint depthrenderbuffer;
  glGenRenderbuffers(1, &depthrenderbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _size.x, _size.y);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

  _depth_attachment = depthrenderbuffer;
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

Filter_Resource_ColorBuffer::Filter_Resource_ColorBuffer(FrameBuffer source, int col_att_id) : Filter_Resource() {
  _source = source;
  _col_att_id = col_att_id;
}
Filter_Resource::Type Filter_Resource_ColorBuffer::staticType() {
  return Type_ColorBuffer;
}
Filter_Resource::Type Filter_Resource_ColorBuffer::type() {
  return staticType();
}

//Depth

Filter_Resource_DepthBuffer::Filter_Resource_DepthBuffer(FrameBuffer source) : Filter_Resource() {
  _source = source;
}
Filter_Resource::Type Filter_Resource_DepthBuffer::staticType() {
  return Type_DepthBuffer;
}
Filter_Resource::Type Filter_Resource_DepthBuffer::type() {
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

Filter_Resource_Input::Filter_Resource_Input(weak_ptr<Filter> filter, string internalName, string displayName, string description, Filter_Resource::Type type) {
  _filter = filter;
  _internalName = internalName;
  _displayName = displayName;
  _description = description;
  _type = type;
}
void Filter_Resource_Input::bindInput(weak_ptr<Filter_Resource_Output> binding) {
  auto binding_l = binding.lock();
  assert(_type == binding_l->type());
  _binding = binding;
}
Filter_Resource::Type Filter_Resource_Input::type() {
  return _type;
}
