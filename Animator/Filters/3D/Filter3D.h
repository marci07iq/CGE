#pragma once

#include "../Filter.h"

class Filter3D : public Filter {
public:
  
  static void staticInit() {
    static bool first = true;
    if (first) {
      first = false;
    }
  }

  Filter3D(weak_ptr<EditorContext> ctx) : Filter(ctx) {
    staticInit();

  }

  void init() {
    weak_ptr<Filter> me = weak_from_this();
    addInput("in", "In", "Image to draw on", Filter_Resource::Type_RenderBuffer, false, NGin::Graphics::getIcon("texture", ilfPath));

    addInput("cam_mat", "Camera Matrix", "World to screen space matrix", Filter_Resource::Type_Object, false, NGin::Graphics::getIcon("matrix", ilfPath));
    addInput("obj_mat", "Object Matrix", "Object to world space matrix", Filter_Resource::Type_Object, false, NGin::Graphics::getIcon("matrix", ilfPath));

    addInput("shader", "Shader", "The shader to use", Filter_Resource::Type_Shader, false, NGin::Graphics::getIcon("shader", ilfPath));

    //addInput("texture", "Texture", "The texture to use [optional]", Filter_Resource::Type, NGin::Graphics::getIcon("texture", ilfPath));

    addInput("objects", "Objects", "VAO Objects to use", Filter_Resource::Type_VAO, true, NGin::Graphics::getIcon("vao", ilfPath));

    addOutput("out", "Out", "Output image", Filter_Resource_IO_Base::Restriction_Dynamic, make_shared<Filter_Resource_RenderBuffer>(nullptr), NGin::Graphics::getIcon("texture", ilfPath));

    updateSize();
  }

  void configure() {

  }

  void calculate(float t) {
    shared_ptr<Filter_Resource_RenderBuffer> in = _inputs["in"]->getAs<Filter_Resource_RenderBuffer>(t);

    //Matrix
    Matrix4f identity = Matrix4f();
    identity.setIdentity();

    Matrix4f cam = _inputs["cam_mat"]->get_Matrix4f(t, identity);
    Matrix4f obj = _inputs["obj_mat"]->get_Matrix4f(t, identity);
    Matrix4f fullmat = cam * obj;

    //Shader
    NGin::Graphics::Shader shader = _inputs["shader"]->get_Shader(t);
    shader->bind();

    //Bind FBO
    if (in == nullptr) return;
    in->_source->bind();

    GLint loc = glGetUniformLocation(shader->_pID, "u_camera");
    if (loc != -1) {
      glUniformMatrix4fv(loc, 1, true, fullmat._vals);
    }
    //Objects
    glEnable(GL_DEPTH_TEST);
    for (auto&& it : _inputs["objects"]->_bindings) {
      if (!it.expired()) {
        shared_ptr<Filter_Resource_VAO> vao(it.lock()->getAs<Filter_Resource_VAO>(t));

        glBindVertexArray(vao->_vao->vao);
        glDrawArrays(GL_TRIANGLES, 0, vao->_vao->size);
      }
    }

    shader->unbind();
    //glBindFramebuffer(0, 0);

    _outputs["out"]->castTo<Filter_Resource_RenderBuffer>()->_source = in->_source;
    _outputs["out"]->castTo<Filter_Resource_RenderBuffer>()->_col_att_ids = in->_col_att_ids;

  }
};
