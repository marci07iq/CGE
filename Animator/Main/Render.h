#pragma once

#include "../Filters/AllFilters.h"
#include "../Encode/Encode.h"

class OutputNode : public Filter {

};

class PreviewCanvas : public Canvas {
public:
  static Shader _texshader;
  static GLint _texshader_uniform_texture;

  //shared_ptr<VideoEncoder> _enc;

  map<string, shared_ptr<Filter_Resource_Input>> _inputs;

  void staticInit() {
    static bool first = true;
    if (first) {
      first = false;

      _texshader.create("Filters/Shaders/FilterTexture");
      _texshader_uniform_texture = glGetUniformLocation(_texshader._pID, "uniform_texture");
    }
  }

  PreviewCanvas(string lname, LocationData llocation, IWindowManagers lmanagers, void* ldata) : Canvas(lname, llocation, lmanagers, ldata) {
    staticInit();
    _inputs.insert({ "result", make_shared<Filter_Resource_Input>(weak_ptr<Filter>(), "result", "Result", "The final image", Filter_Resource::Type_ColorBuffer) });

    //_data = make_shared<GlobalVideoData>();
    //_data->_filename = "test.mp4";
    //_data->_frameRate = 30;
    //_data->_resolution = iVec2(640, 480);
    //_data->_encoder = /*"h264_qsv";/*/"mpeg1video";
    
    //_frameData = make_shared<FrameData>(_data);
    //_frameData->_time = 0;
    //_frameData->_frameCount = 0;

    //_enc = make_shared<VideoEncoder>(_data);
  }

  //aspect: w/h
  void setViewport(float aspect);

  void render(set<key_location>& down);
  int mouseEnter(int state);
  int mouseMoved(int x, int y, int ox, int oy, set<key_location>& down);
  int guiEvent(gui_event& evt, int mx, int my, set<key_location>& down);

  void finishRender();
};