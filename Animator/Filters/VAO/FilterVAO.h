#pragma once

#include "../Filter.h"

class FilterVAO : public Filter {
  NGin::Gll::PolyVao _vao;

public:
  static void staticInit() {
    static bool first = true;
    if (first) {
      first = false;
    }
  }

  FilterVAO(weak_ptr<EditorContext> ctx) : Filter(ctx) {
    staticInit();
  }

  void init() {
    weak_ptr<Filter> me = weak_from_this();
    addParam("path", "Path", "Path to shader", Filter_Resource::Type_String, false, NGin::Graphics::getIcon("path", ilfPath));

    addOutput("out", "Out", "VAO object", Filter_Resource_IO_Base::Restriction_Dynamic, make_shared<Filter_Resource_VAO>(), NGin::Graphics::getIcon("vao", ilfPath));

    updateSize();
  }

  void configure();

  void calculate(float t) {
    
  }
};