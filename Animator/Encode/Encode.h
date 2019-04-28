#pragma once

#include "../Filters/Filter.h"

class Raw_Encoder_Data;

struct EncodeData {
  string filename;
  string encoder;
  int bitrate;
};

class VideoEncoder {
  shared_ptr<Raw_Encoder_Data> _encoder;

public:
  VideoEncoder(shared_ptr<Filter_Resource_Output> data, EncodeData encoding);

  void addFrame();

  void finish();
};