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


class EditorContext {
public:
  StreamData _stream_desc;
  list<shared_ptr<Filter>> _filters;
};

class IOVideoData : public StreamData {
  string _filename;
  string _encoder;
};

typedef map<string, shared_ptr<Filter_Resource_Input>> Filter_Static_Params;

class Filter : public enable_shared_from_this<Filter> {
public:
  string _id;

  map<string, shared_ptr<Filter_Resource_Output>> _outputs;
  map<string, shared_ptr<Filter_Resource_Input>> _inputs;

  weak_ptr<EditorContext> _ctx;

  Filter_Static_Params _params;

  Filter(weak_ptr<EditorContext> ctx);

  //Constructor doesn't have shared_from_this
  virtual void init();

  virtual void configure();

  virtual void calculate(float t);
};