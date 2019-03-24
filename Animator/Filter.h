#pragma once

#include "Defs.h"

class Raw_FrameBuffer {
  GLuint _id = 0;
  iVec2 _size;

  map<int, GLuint> _col_attachments;
  GLuint _depth_attachment;

public:
  Raw_FrameBuffer(iVec2 size) {
    glGenFramebuffers(1, &_id);
    _size = size;

    _depth_attachment = 0;
  }

  void attachColor(int colorInd, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE) {
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

  void attachDepth() {
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

  bool valid() {
    glBindFramebuffer(GL_FRAMEBUFFER, _id);
    
    // Always check that our framebuffer is ok
    return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
  }

  void bind() {
    glBindBuffer(GL_FRAMEBUFFER, _id);
  }

  ~Raw_FrameBuffer() {

  }
};

struct GlobalFrameData {
  float _time;
  size_t _frameCount;
  float _frameRate;
  iVec2 _resolution;
};

typedef shared_ptr<Raw_FrameBuffer> FrameBuffer;

class Filter_Resource {
public:
  weak_ptr<Filter> _filter;

  string _internalName;
  string _displayName;
  string _description;

  enum Type {
    Type_Undef,
    Type_ColorBuffer,
    Type_DepthBuffer,
    Type_Data
  };

  virtual Type type() {
    return Type_Undef;
  }
};

//Outputs
class Filter_Resource_Output : public Filter_Resource {
};

//Inputs
class Filter_Resource_Input : public Filter_Resource {
  Type _type;
  weak_ptr<Filter_Resource_Output> _binding;

  Type type() {
    return _type;
  }
};

//Outputs
class Filter_Resource_Color : public Filter_Resource_Output {
public:
  FrameBuffer _source;
  int _col_att_id;

  Type type() {
    return Type_ColorBuffer;
  }
};

class Filter_Resource_Depth : public Filter_Resource_Output {
  FrameBuffer _source;

  Type type() {
    return Type_DepthBuffer;
  }
};

class Filter {
  string _id;

  map<string, shared_ptr<Filter_Resource_Output>> _outputs;
  map<string, shared_ptr<Filter_Resource_Input>> _inputs;

  virtual void calculate(const GlobalFrameData& data) {
    
  }
};