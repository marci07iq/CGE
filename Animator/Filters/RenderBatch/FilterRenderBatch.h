#pragma once

#include "../Filter.h"

/*class FilterRenderBatch : public Filter {
public:
  
  static void staticInit() {
    static bool first = true;
    if (first) {
      first = false;
    }
  }

  FilterRenderBatch(weak_ptr<EditorContext> ctx) : Filter(ctx) {
    staticInit();

  }

  void init() {
    weak_ptr<Filter> me = weak_from_this();
    
    addInput("cam_mat", "Camera Matrix", "World to screen space matrix", Filter_Resource::Type_Object, false, NGin::Graphics::getIcon("matrix", ilfPath));
    addInput("obj_mat", "Object Matrix", "Object to world space matrix", Filter_Resource::Type_Object, false, NGin::Graphics::getIcon("matrix", ilfPath));

    addInput("shader", "Shader", "The shader to use", Filter_Resource::Type_Shader, false, NGin::Graphics::getIcon("shader", ilfPath));

    //addInput("texture", "Texture", "The texture to use [optional]", Filter_Resource::Type, NGin::Graphics::getIcon("texture", ilfPath));

    addInput("objects", "Objects", "VAO Objects to use", Filter_Resource::Type_VAO, true, NGin::Graphics::getIcon("vao", ilfPath));

    addOutput("out", "Out", "Output renderbatch", Filter_Resource_IO_Base::Restriction_Dynamic, make_shared<Filter_Resource_Object>(), NGin::Graphics::getIcon("out", ilfPath));

    updateSize();
  }

  void configure() {
    
  }

  void calculate(float t) {
    shared_ptr<Filter_Resource_Object> res = make_shared<Filter_Resource_Object>();

    //Matrix
    Matrix4f identity = Matrix4f();
    identity.setIdentity();
    
    Matrix4f cam = _inputs["cam_mat"]->get_Matrix4f(t, identity);
    Matrix4f obj = _inputs["obj_mat"]->get_Matrix4f(t, identity);
    Matrix4f fullmat = cam * obj;
    shared_ptr<Filter_Resource_Object> res_fullmat = make_shared<Filter_Resource_Object>();
    res_fullmat->set_Matrix4f(fullmat);
    res->_elems["mat"] = res_fullmat;

    //Shader
    Shader shader = _inputs["shader"]->get_Shader(t);
    
    shared_ptr<Filter_Resource_Shader> res_shader = make_shared<Filter_Resource_Shader>();
    res_shader->_shader = shader;
    res->_elems["shader"] = res_shader;

    //Objects
    shared_ptr<Filter_Resource_Object> res_object = make_shared<Filter_Resource_Object>();

    int n = 0;
    for (auto&& it : _inputs["objects"]->_bindings) {
      res_object->_elems[to_string(n)] = it.lock()->getAs<Filter_Resource_VAO>(t);
    }
    res->_elems["objects"] = res_object;
  }
};
*/