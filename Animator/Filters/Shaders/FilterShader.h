#pragma once

#include "../Filter.h"

class FilterShader : public Filter {
  Shader _shader;
public:

  static void staticInit() {
    static bool first = true;
    if (first) {
      first = false;
    }
  }

  FilterShader(weak_ptr<EditorContext> ctx) : Filter(ctx) {
    staticInit();
  }

  void init() {
    weak_ptr<Filter> me = weak_from_this();
    _params.insert({ "path", make_shared<Filter_Resource_Input>(me, "path", "Path", "Path to shader", Filter_Resource::Type_String) });

    _outputs.insert({ "out", make_shared<Filter_Resource_Output>(me, "out", "Out", "Shader object", make_shared<Filter_Resource_Shader>()) });

    updateSize();
  }

  void configure() {
    string path = _params["path"]->get_string();

    _shader = make_shared<Shader_Raw>(path);

    _outputs["out"]->castTo<Filter_Resource_Shader>()->_shader = _shader;
  }

  void calculate(float t) {
    
  }
};