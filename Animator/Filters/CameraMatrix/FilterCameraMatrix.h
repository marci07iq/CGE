#pragma once

#include "../Filter.h"

class FilterCameraMatrix : public Filter {
  
  void staticInit() {
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
    _params.insert({ "resolution", make_shared<Filter_Resource_Input>(me, "resolution", "Resolution", "Resolution of output stream", Filter_Resource::Type_Object) });
    //Field x, y

    _params.insert({ "cam_eye", make_shared<Filter_Resource_Input>(me, "cam_eye", "Eye", "Eye position of the camera", Filter_Resource::Type_Object) });
    _params.insert({ "cam_dir", make_shared<Filter_Resource_Input>(me, "cam_dir", "Direction", "Look direction of camera", Filter_Resource::Type_Object) });
    _params.insert({ "cam_up", make_shared<Filter_Resource_Input>(me, "cam_up", "Up", "Up direction [UNUSED]", Filter_Resource::Type_Object) });
    //Fields x, y, z

    _params.insert({ "fov", make_shared<Filter_Resource_Input>(me, "fov", "FOV", "Field of view angle", Filter_Resource::Type_Float) });
    //In radians

    _outputs.insert({ "mat", make_shared<Filter_Resource_Output>(me, "mat", "Matrix", "The finished camera matrix", make_shared<Filter_Resource_Object>()) });
    //Fields 0, 1, 2, .. 15
  }

  void configure() {
    
  }

  void calculate(float t) {
    fVec2 resolution;

    fVec3 cam_eye;
    fVec3 cam_dir;
    fVec3 cam_up;

    float fov;
  }
};