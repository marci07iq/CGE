#pragma once

#include "../Main/Defs.h"

//typedef uint64_t FrameUUID;

class Raw_FrameBuffer {
public:
  GLuint _id = 0;
  iVec2 _size;

  map<int, GLuint> _col_attachments;
  GLuint _depth_attachment = 0;

  Raw_FrameBuffer(iVec2 size);

  void attachColor(int colorInd, GLenum format = GL_RGBA8, GLenum type = GL_UNSIGNED_BYTE);

  void attachDepth();
  bool hasDepth();

  bool valid();

  void bind();

  ~Raw_FrameBuffer();
};

typedef shared_ptr<Raw_FrameBuffer> FrameBuffer;

class Filter_Resource : public enable_shared_from_this<Filter_Resource> {
public:

  enum Type {
    Type_Undef,
    Type_RenderBuffer,
    Type_Float,
    Type_String,
    Type_Object,
    Type_VAO,
    Type_Shader,
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
class Filter_Resource_RenderBuffer : public Filter_Resource {
public:
  FrameBuffer _source;
  vector<int> _col_att_ids;

  Filter_Resource_RenderBuffer(FrameBuffer source, vector<int> col_att_ids = vector<int>());

  static Type staticType();

  Type type();
};

class Filter_Resource_Float : public Filter_Resource {
public:
  float _val;

  Filter_Resource_Float(float val = 0);

  static Type staticType();

  Type type();

  float get_float(float def) const {
    return _val;
  }
};

class Filter_Resource_String : public Filter_Resource {
public:
  string _val;

  Filter_Resource_String(string val = 0);

  static Type staticType();

  Type type();

  string get_string(string def) const {
    return _val;
  }
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

class Filter_Resource_VAO : public Filter_Resource {
public:
  Gll::PolyVao  _vao;

  Filter_Resource_VAO();

  static Type staticType();

  Type type();

  Gll::PolyVao get_VAO(Gll::PolyVao def) const {
    return _vao;
  }
};

class Filter_Resource_Shader : public Filter_Resource {
public:
  Shader _shader;

  Filter_Resource_Shader();

  static Type staticType();

  Type type();

  Shader get_Shader(Shader def) const {
    return _shader;
  }
};

class Filter;
//Outputs
class Filter_Resource_Output : public enable_shared_from_this<Filter_Resource_Output>
{
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

  //Eval and cast
  template <typename T>
  shared_ptr<T> getAs(float t) {
    _filter.lock()->calculate(t);
    return castTo<T>();
  }

  #define FILTER_RESOURCE_OUTPUT_GET(type, hostType, defal) \
  type get_##type(type def = defal) const { \
    if (_res->validateType<hostType>()) { \
      return _res->castTo<hostType>()->get_##type(def); \
    } \
    return def; \
  }

  using VAO = Gll::PolyVao;
  FILTER_RESOURCE_OUTPUT_GET(VAO, Filter_Resource_VAO, VAO());
  FILTER_RESOURCE_OUTPUT_GET(Shader, Filter_Resource_Shader, Shader());
  FILTER_RESOURCE_OUTPUT_GET(string, Filter_Resource_String, "");
  FILTER_RESOURCE_OUTPUT_GET(float, Filter_Resource_Float, 0);
  FILTER_RESOURCE_OUTPUT_GET(fVec2, Filter_Resource_Object, fVec2(0, 0));
  FILTER_RESOURCE_OUTPUT_GET(fVec3, Filter_Resource_Object, fVec3(0, 0, 0));
  FILTER_RESOURCE_OUTPUT_GET(Matrix4f, Filter_Resource_Object, Matrix4f());

#undef FILTER_RESOURCE_OUTPUT_GET

};

//Inputs
class Filter_Resource_Input : public enable_shared_from_this<Filter_Resource_Input> {
  public:
  //Modify with care
  vector<weak_ptr<Filter_Resource_Output>> _bindings;

  //Modify with care
  Filter_Resource::Type _type;

  bool _isArray;

  weak_ptr<Filter> _filter;

  string _internalName;
  string _displayName;
  string _description;

  Filter_Resource_Input(weak_ptr<Filter> filter, string internalName, string displayName, string description, Filter_Resource::Type type, bool isArray = false);

  //to insert on the end: -1, any
  //to insert to beginning: 0, true
  void bindInput(weak_ptr<Filter_Resource_Output> binding, int to = -1, bool before = false);

  int hasBinding() const;
  void cleanBindings();

  template <typename T>
  bool validateType(int id = 0) {
    return _bindings[id].lock()->validateType<T>();
  }

  template <typename T>
  shared_ptr<T> castTo(int id = 0) {
    return _bindings[id].lock()->castTo<T>();
  }

  //Eval and cast
  template <typename T>
  shared_ptr<T> getAs(float t, int id = 0) {
    return _bindings[id].lock()->getAs<T>(t);
  }

  Filter_Resource::Type type();

  #define FILTER_RESOURCE_INPUT_GET(type, hostType, defal) \
  type get_##type(type def = defal, int id = 0) const { \
    if (id >= hasBinding() || _bindings[id].expired()) return def; \
    return _bindings[id].lock()->get_##type(defal); \
  }

  using VAO = Gll::PolyVao;
  FILTER_RESOURCE_INPUT_GET(VAO, Filter_Resource_VAO, VAO());
  FILTER_RESOURCE_INPUT_GET(Shader, Filter_Resource_Shader, Shader());
  FILTER_RESOURCE_INPUT_GET(string, Filter_Resource_String, "");
  FILTER_RESOURCE_INPUT_GET(float, Filter_Resource_Float, 0);
  FILTER_RESOURCE_INPUT_GET(fVec2, Filter_Resource_Object, fVec2(0, 0));
  FILTER_RESOURCE_INPUT_GET(fVec3, Filter_Resource_Object, fVec3(0, 0, 0));
  FILTER_RESOURCE_INPUT_GET(Matrix4f, Filter_Resource_Object, Matrix4f());

#undef FILTER_RESOURCE_INPUT_GET
};
