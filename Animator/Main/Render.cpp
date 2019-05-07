#include "Render.h"

void editorTimelineSlider(NGin::Graphics::ElemHwnd elem, float& t) {
  ((PreviewCanvas*)elem->data)->scrollTimeline(t);
}

void editorTimelinePauseButton(NGin::Graphics::ElemHwnd elem) {
  ((PreviewCanvas*)elem->data)->setPlay(!((PreviewCanvas*)elem->data)->isPlaying());
}