#pragma once

#include "FilterResource.h"

//typedef uint64_t FrameUUID;


class StreamData {
public:
  iVec2 _resolution;

  int _frameRate;

  float _startTime;
  int _frameCount;


  int frameId(float time);
};

class Filter;
class FilterExitNode;
#ifdef M_CLIENT
class FilterGUI;
#endif

class EditorContext;

struct FilterDescription {
  string _display_name;
  shared_ptr<Filter>(*_constructor)(weak_ptr<EditorContext>);
  Icon* _icon;
};

class EditorContext : public enable_shared_from_this<EditorContext> {
public:
  StreamData _stream_desc;
  list<shared_ptr<Filter>> _filters;
  shared_ptr<FilterExitNode> _exit;
  shared_ptr<Filter> _globalDummy;

  static map<string, FilterDescription> _registeredFilters;

  static void registerFilter(string name, FilterDescription description);

  void init();
#ifdef M_CLIENT
  shared_ptr<FilterGUI> gui;
#endif
};

class IOVideoData : public StreamData {
  string _filename;
  string _encoder;
};

#ifdef M_CLIENT
class FilterGUI : public GUIElement {
public:
  //Global coord to low left
  iVec2 dragOffset = iVec2(-100, -100);
  
  static const int toolbar_width;

  bool dragging = false;

  weak_ptr<EditorContext> _editor;
  weak_ptr<Filter_Resource_Output> _draggedConnection;

  FilterGUI(string name, LocationData llocation, colorargb lbgcolor, colorargb lactivecolor, colorargb ltextcolor, shared_ptr<EditorContext> editor);

  void getRect();
  void getRect(int winWidth, int winHeight, int offsetX, int offsetY);

  int mouseEnter(int state);
  int mouseMoved(int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent(gui_event& evt, int mx, int my, set<key_location>& down);
  void render(set<key_location>& down);
};

#endif

class Filter : public enable_shared_from_this<Filter> {

public:
  string _id;
  weak_ptr<EditorContext> _ctx;

  map<string, shared_ptr<Filter_Resource_Output>> _outputs;
  map<string, shared_ptr<Filter_Resource_Input>> _inputs;
  map<string, shared_ptr<Filter_Resource_Input>> _params;

  int cw, ch; //Width and height
  //Global coord
  int cax, cay; //Left, bottom

  bool dragging = false;

  Filter(weak_ptr<EditorContext> ctx);

  //Constructor doesn't have shared_from_this
  virtual void init();

  virtual void configure();

  virtual void calculate(float t);

  void addInput(string internalName, string displayName, string description, Filter_Resource::Type type, bool isArray = false
#ifdef M_CLIENT
    , Icon* icon = nullptr
#endif
  );

  void addParam(string internalName, string displayName, string description, Filter_Resource::Type type, bool isArray = false
#ifdef M_CLIENT
    , Icon* icon = nullptr
#endif
  );
  void addOutput(string internalName, string displayName, string description, Filter_Resource_IO_Base::Restriction restriction, shared_ptr<Filter_Resource> res
#ifdef M_CLIENT
    , Icon* icon = nullptr
#endif
  );

public:
  
#ifdef M_CLIENT
  shared_ptr<FilterGUI> getGUI_ctx() {
    return _ctx.lock()->gui;
  }

  bool isIn(iVec2 offset, int mx, int my);

  virtual int mouseEnter(iVec2 offset, int state);
private:
  //Do not call from outside
  virtual int mouseMoved(iVec2 offset, int mx, int my, int ox, int oy, set<key_location>& down);
  virtual int guiEvent(iVec2 offset, gui_event evt, int mx, int my, set<key_location>& down);
  virtual void render(iVec2 offset, set<key_location>& down);
public:

  int mouseMoved_base(iVec2 offset, int mx, int my, int ox, int oy, set<key_location>& down);
  int guiEvent_base(iVec2 offset, gui_event evt, int mx, int my, set<key_location>& down);
  void render_base(iVec2 offset, set<key_location>& down);

  bool findOutput(shared_ptr<Filter_Resource_Output> val, iVec2& pos);

  void updateSize();
  virtual iVec2 getInternalSize();
#endif
};

class FilterExitNode : public Filter {
public:
  static void staticInit() {
    static bool first = true;
    if (first) {
      first = false;

    }
  }

  FilterExitNode(weak_ptr<EditorContext> ctx) : Filter(ctx) {
    staticInit();
  }

  void init() {
    addInput("result", "Result", "Final image", Filter_Resource::Type_RenderBuffer, false, getIcon("finish", ilfPath));

    updateSize();

    cax = 100;
    cay = 100;
  }

  void configure() {
  }

  void calculate(float t) {
    //_inputs["in"]->getAs<Filter_Resource_RenderBuffer>(t);
  }

  shared_ptr<Filter_Resource_RenderBuffer> getFrame(float t) {
    return _inputs["result"]->getAs<Filter_Resource_RenderBuffer>(t);
  }
};

template<typename T>
shared_ptr<Filter> createFilter_T(weak_ptr<EditorContext> ctx) {
  shared_ptr<T> res = make_shared<T>(ctx);
  res->init();
  return static_pointer_cast<Filter, T>(res);
}