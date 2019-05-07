#pragma once

#include "../Filters/AllFilters.h"
#include "../Encode/Encode.h"

class OutputNode : public Filter {

};

class PreviewCanvas : public Canvas {
  shared_ptr<VideoEncoder> _enc;
  bool _playing;
  NGin::Graphics::ElemHwnd _parent;

  float _time;
public:

  map<string, shared_ptr<Filter_Resource_Input>> _inputs;

  void staticInit() {
    static bool first = true;
    if (first) {
      first = false;
      FilterTexture::staticInit();
    }
  }

  PreviewCanvas(string lname, LocationData llocation, IWindowManagers lmanagers, void* ldata, NGin::Graphics::ElemHwnd parent) : Canvas(lname, llocation, lmanagers, ldata) {
    staticInit();
    _inputs.insert({ "result", make_shared<Filter_Resource_Input>(weak_ptr<Filter>(), "result", "Result", "The final image", Filter_Resource::Type_RenderBuffer) });

    _parent = parent;
    _parent->getElementById("objectEditorTimelineSlider")->data = this;
    _parent->getElementById("objectEditorTimelinePauseButton")->data = this;

    //_data = make_shared<GlobalVideoData>();
    //_data->_filename = "test.mp4";
    //_data->_frameRate = 30;
    //_data->_resolution = iVec2(640, 480);
    //_data->_encoder = /*"h264_qsv";/*/"mpeg1video";

    //_frameData = make_shared<FrameData>(_data);
    //_frameData->_time = 0;
    //_frameData->_frameCount = 0;

    setPlay(false);
    scrollTimeline(0, true);
  }

  void configure() {
    EncodeData enc;
    enc.bitrate = 8000000;
    enc.encoder = "h264_nvenc";
    enc.filename = "test.mp4";


    //_enc = make_shared<VideoEncoder>(_inputs["result"]->_binding.lock(), enc);
  }

  //aspect: w/h
  void setViewport(float aspect);

  void scrollTimeline(float t, bool refreshSlider = false) {
    _time = t;
    if (refreshSlider) {
      (static_pointer_cast<Slider, GUIElement>(_parent->getElementById("objectEditorTimelineSlider")))->val = _time;
    }
  }

  bool isPlaying() {
    return _playing;
  }
  void setPlay(bool playing) {
    _playing = playing;
    (static_pointer_cast<Button, GUIElement>(_parent->getElementById("objectEditorTimelinePauseButton")))->text = playing ? "||" : ">";
    cout << playing << endl;
  }

  void render(set<key_location>& down);
  int mouseEnter(int state);
  int mouseMoved(int x, int y, int ox, int oy, set<key_location>& down);
  int guiEvent(gui_event& evt, int mx, int my, set<key_location>& down);

  void finishRender();
};
