#pragma once

#include "../Main/Defs.h"

//typedef uint64_t FrameUUID;

class Raw_FrameBuffer {
public:
  GLuint _id = 0;
  iVec2 _size;

  map<int, GLuint> _col_attachments;
  GLuint _depth_attachment;

  Raw_FrameBuffer(iVec2 size);

  void attachColor(int colorInd, GLenum format = GL_RGBA8, GLenum type = GL_UNSIGNED_BYTE);

  void attachDepth();

  bool valid();

  void bind();

  ~Raw_FrameBuffer();
};

typedef shared_ptr<Raw_FrameBuffer> FrameBuffer;

class Filter_Resource : public enable_shared_from_this<Filter_Resource> {
public:

  enum Type {
    Type_Undef,
    Type_ColorBuffer,
    Type_DepthBuffer,
    Type_Float,
    Type_String,
    Type_Object,
  };

  Filter_Resource();

  virtual Type type();

  template <typename T>
  bool validateType() {
    return (T::staticType() == type());

  }

  template <typename T>
  shared_ptr<T> castTo() {
    assert(validateType<T>());
    return static_pointer_cast<T, Filter_Resource>(shared_from_this());
  }
};

//Types
class Filter_Resource_ColorBuffer : public Filter_Resource {
public:
  FrameBuffer _source;
  int _col_att_id;

  Filter_Resource_ColorBuffer(FrameBuffer source, int col_att_id);

  static Type staticType();

  Type type();
};

class Filter_Resource_DepthBuffer : public Filter_Resource {
public:
  FrameBuffer _source;

  Filter_Resource_DepthBuffer(FrameBuffer source);

  static Type staticType();

  Type type();
};

class Filter_Resource_Float : public Filter_Resource {
public:
  float _val;

  Filter_Resource_Float(float val = 0);

  static Type staticType();

  Type type();
};

class Filter_Resource_String : public Filter_Resource {
public:
  string _val;

  Filter_Resource_String(string val = 0);

  static Type staticType();

  Type type();
};

class Filter_Resource_Object : public Filter_Resource {
public:
  map<string, shared_ptr<Filter_Resource>> _elems;

  Filter_Resource_Object();

  static Type staticType();

  Type type();

  float getFloat(const string& id, const float def = 0) const {
    auto it = _elems.find(id);
    if (it == _elems.end()) {
      return def;
    }
    if (!it->second->validateType<Filter_Resource_Float>()) {
      return def;
    }
    return it->second->castTo<Filter_Resource_Float>()->_val;
  }

  void setFloat(const string& id, const float& val) {
    _elems[id] = make_shared<Filter_Resource_Float>(val);
  }


  fVec2 get_fVec2(fVec2 def = fVec2(0, 0)) const {
    return fVec2(
      getFloat("x", def.x),
      getFloat("y", def.y)
    );
  }

  fVec3 get_fVec3(fVec3 def = fVec3(0, 0, 0)) const {
    return fVec3(
      getFloat("x", def.x),
      getFloat("y", def.y),
      getFloat("z", def.z)
    );
  }

  Matrix4f get_Matrix4f(Matrix4f def = Matrix4f()) const {
    for (int i = 0; i < 16; i++) {
      def._vals[i] = getFloat(to_string(i), def._vals[i]);
    }
  }

  void set_fVec2(fVec2 val) {
    setFloat("x", val.x);
    setFloat("y", val.y);
  }

  void set_fVec3(fVec3 val) {
    setFloat("x", val.x);
    setFloat("y", val.y);
    setFloat("z", val.z);
  }

  void set_Matrix4f(Matrix4f val) {
    for (int i = 0; i < 16; i++) {
      setFloat(to_string(i), val._vals[i]);
    }
  }
};

class Filter;
//Outputs
class Filter_Resource_Output : public enable_shared_from_this<Filter_Resource_Output> {
public:
  shared_ptr<Filter_Resource> _res;

  weak_ptr<Filter> _filter;

  string _internalName;
  string _displayName;
  string _description;

  Filter_Resource_Output(weak_ptr<Filter> filter, string internalName, string displayName, string description, shared_ptr<Filter_Resource> res);

  Filter_Resource::Type type() const;

  template <typename T>
  bool validateType() {
    return (T::staticType() == type());
  }

  template <typename T>
  shared_ptr<T> castTo() {
    assert(validateType<T>());
    return static_pointer_cast<T, Filter_Resource>(_res);
  }
};

//Inputs
class Filter_Resource_Input : public enable_shared_from_this<Filter_Resource_Input> {
public:
  Filter_Resource::Type _type;
  weak_ptr<Filter_Resource_Output> _binding;

  weak_ptr<Filter> _filter;

  string _internalName;
  string _displayName;
  string _description;

  Filter_Resource_Input(weak_ptr<Filter> filter, string internalName, string displayName, string description, Filter_Resource::Type type);

  void bindInput(weak_ptr<Filter_Resource_Output> binding);


  template <typename T>
  bool validateType() {
    shared_ptr<Filter_Resource_Output> val = _binding.lock();
    return (T::staticType() == val->_res->type());
  }


  template <typename T>
  shared_ptr<T> castTo() {
    shared_ptr<Filter_Resource_Output> val = _binding.lock();
    assert(validateType<T>());
    return static_pointer_cast<T, Filter_Resource>(val->_res);
  }

  Filter_Resource::Type type();
};
