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
    _params.insert({ "resolution", make_shared<Filter_Resource_Input>(me, "resolution", "Resolution", "Resolution of output stream", Filter_Resource::Type_Object) });

    _outputs.insert({ "out", make_shared<Filter_Resource_Output>(me, "out", "Out", "Output image", make_shared<Filter_Resource_RenderBuffer>(nullptr)) });
    _inputs.insert({ "in", make_shared<Filter_Resource_Input>(me, "in", "In", "Image to draw on", Filter_Resource::Type_RenderBuffer) });
    _inputs.insert({ "cam_mat", make_shared<Filter_Resource_Input>(me, "cam_mat", "Cam Matrix", "Camera matrix", Filter_Resource::Type_RenderBuffer) });

    updateSize();
  }

  void configure() {
    /*
    _img = make_shared<Raw_FrameBuffer>(_params["resolution"]->get_fVec2());
    _img->attachColor(0, GL_RGBA, GL_UNSIGNED_BYTE);
    assert(_img->valid());

    _outputs["out"]->castTo<Filter_Resource_ColorBuffer>()->_source = _img;
    _outputs["out"]->castTo<Filter_Resource_ColorBuffer>()->_col_att_id = 0;*/
  }

  void calculate(float t) {
    
  }
};