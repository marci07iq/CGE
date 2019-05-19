#pragma once

#include "../Filters/AllFilters.h"
#include "../Encode/Encode.h"

class PreviewCanvas : public NGin::Graphics::Canvas {
  shared_ptr<VideoEncoder> _enc;
  bool _playing;
  NGin::Graphics::ElemHwnd _parent;
  weak_ptr<EditorContext> _ctx;

  float _time;
public:

  void staticInit() {
    static bool first = true;
    if (first) {
      first = false;
      FilterTexture::staticInit();
    }
  }

  PreviewCanvas(string lname, NGin::Graphics::LocationData llocation, NGin::Graphics::IWindowManagers lmanagers, void* ldata, NGin::Graphics::ElemHwnd parent, shared_ptr<EditorContext> ctx) : NGin::Graphics::Canvas(lname, llocation, lmanagers, ldata) {
    staticInit();

    _ctx = ctx;

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
      (static_pointer_cast<NGin::Graphics::Slider, NGin::Graphics::GUIElement>(_parent->getElementById("objectEditorTimelineSlider")))->val = _time;
    }
  }

  bool isPlaying() {
    return _playing;
  }
  void setPlay(bool playing) {
    _playing = playing;
    (static_pointer_cast<NGin::Graphics::Button, NGin::Graphics::GUIElement>(_parent->getElementById("objectEditorTimelinePauseButton")))->text = playing ? "||" : ">";
  }

  void render(set<NGin::Graphics::key_location>& down);
  int mouseEnter(int state);
  int mouseMoved(int x, int y, int ox, int oy, std::set<NGin::Graphics::key_location>& down);
  int guiEvent(NGin::Graphics::gui_event& evt, int mx, int my, std::set<NGin::Graphics::key_location>& down);

  void finishRender();
};
