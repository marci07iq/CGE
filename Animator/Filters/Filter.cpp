#include "Filter.h"

int StreamData::frameId(float time) {
  return round((time - _startTime) * _frameRate);
}

//Filter

Filter::Filter(weak_ptr<EditorContext> ctx) {
  _ctx = ctx;
}

void Filter::init() {

}
void Filter::configure() {

}

void Filter::calculate(float t) {

}
