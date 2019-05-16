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
    addParam("resolution", "Resolution", "Resolution of output stream", Filter_Resource::Type_Object, false, getIcon("resolution", ilfPath));

    addInput("in", "In", "Image to draw on", Filter_Resource::Type_RenderBuffer, getIcon("in", ilfPath));
    addInput("cam_mat", "Cam Matrix", "Camera matrix", Filter_Resource::Type_RenderBuffer, getIcon("matrix", ilfPath));

    addOutput("out", "Out", "Output image", Filter_Resource_IO_Base::Restriction_Dynamic, make_shared<Filter_Resource_RenderBuffer>(nullptr), getIcon("out", ilfPath));

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
