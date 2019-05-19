#pragma once

#include "../Filter.h"

class FilterCameraMatrix : public Filter {
public:
  static void staticInit() {
    static bool first = true;
    if (first) {
      first = false;

      
    }
  }

  FilterCameraMatrix(weak_ptr<EditorContext> ctx) : Filter(ctx) {
    staticInit();
  }

  void init() {
    weak_ptr<Filter> me = weak_from_this();
    addInput("resolution", "Resolution", "Resolution of output stream", Filter_Resource::Type_Object, false, NGin::Graphics::getIcon("resolution", ilfPath));
    //Field x, y

    addInput("cam_eye", "Eye", "Eye position of the camera", Filter_Resource::Type_Object, false, NGin::Graphics::getIcon("eye", ilfPath));
    addInput("cam_dir", "Direction", "Look direction of camera", Filter_Resource::Type_Object, false, NGin::Graphics::getIcon("vector", ilfPath));
    addInput("cam_up", "Up", "Up direction [UNUSED]", Filter_Resource::Type_Object, false, NGin::Graphics::getIcon("up", ilfPath));
    //Fields x, y, z

    addInput("fov", "FOV", "Field of view angle", Filter_Resource::Type_Float, false, NGin::Graphics::getIcon("fov", ilfPath));
    //In radians

    addOutput("mat", "Matrix", "The finished camera matrix", Filter_Resource_IO_Base::Restriction_Dynamic, make_shared<Filter_Resource_Object>(), NGin::Graphics::getIcon("matrix", ilfPath));
    //Fields 0, 1, 2, .. 15

    updateSize();
  }

  void configure() {
    
  }

  void calculate(float t) {
    fVec2 resolution = _inputs["resolution"]->get_fVec2(t, fVec2(1, 1));

    fVec3 cam_eye = _inputs["cam_eye"]->get_fVec3(t, fVec3(0, 0, 0));
    fVec3 cam_dir = _inputs["cam_dir"]->get_fVec3(t, fVec3(0, 0, 1));
    fVec3 cam_up = _inputs["cam_up"]->get_fVec3(t, fVec3(0, 1, 0));

    float fov = _inputs["fov"]->get_float(t, 2*CONS_PI/3);

    Transform res;
    res.setIdentity();
    res.createLook(cam_eye, cam_dir, cam_up);
    res.project(fov, resolution.x / resolution.y, 10, 0.1);

    _outputs["mat"]->castTo<Filter_Resource_Object>()->set_Matrix4f(res.matrix);
  }
};
