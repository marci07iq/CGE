#pragma once

#include "../Filter.h"

class FilterCameraMatrix : public Filter {
  
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
    _inputs.insert({ "resolution", make_shared<Filter_Resource_Input>(me, "resolution", "Resolution", "Resolution of output stream", Filter_Resource::Type_Object) });
    //Field x, y

    _inputs.insert({ "cam_eye", make_shared<Filter_Resource_Input>(me, "cam_eye", "Eye", "Eye position of the camera", Filter_Resource::Type_Object) });
    _inputs.insert({ "cam_dir", make_shared<Filter_Resource_Input>(me, "cam_dir", "Direction", "Look direction of camera", Filter_Resource::Type_Object) });
    _inputs.insert({ "cam_up", make_shared<Filter_Resource_Input>(me, "cam_up", "Up", "Up direction [UNUSED]", Filter_Resource::Type_Object) });
    //Fields x, y, z

    _inputs.insert({ "fov", make_shared<Filter_Resource_Input>(me, "fov", "FOV", "Field of view angle", Filter_Resource::Type_Float) });
    //In radians

    _outputs.insert({ "mat", make_shared<Filter_Resource_Output>(me, "mat", "Matrix", "The finished camera matrix", make_shared<Filter_Resource_Object>()) });
    //Fields 0, 1, 2, .. 15

    updateSize();
  }

  void configure() {
    
  }

  void calculate(float t) {
    fVec2 resolution = _inputs["resolution"]->get_fVec2(fVec2(1, 1));

    fVec3 cam_eye = _inputs["cam_eye"]->get_fVec3(fVec3(0, 0, 0));
    fVec3 cam_dir = _inputs["cam_dir"]->get_fVec3(fVec3(0, 0, 1));
    fVec3 cam_up = _inputs["cam_up"]->get_fVec3(fVec3(0, 1, 0));

    float fov = _inputs["fov"]->get_float(1);

    Transform res;
    res.setIdentity();
    res.createLook(cam_eye, cam_dir, cam_up);
    res.project(fov, resolution.x / resolution.y, 1000, 0.1);

    _outputs["mat"]->castTo<Filter_Resource_Object>()->set_Matrix4f(res.matrix);
  }
};