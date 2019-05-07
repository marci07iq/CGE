#include "Main.h"

NGin::Graphics::WinHwnd objectEditorWindowHwnd;
NGin::Graphics::WinHwnd objectPreviewWindowHwnd;

shared_ptr<PreviewCanvas> objectMainPreviewCanvasHwnd;
shared_ptr<FilterGUI> objectMainFilterGuiHwnd;

shared_ptr<EditorContext> ctx;

void frameLoop() {
  while (NGin::Graphics::windows.size()) {
    NGin::Graphics::requestRedraw();
    this_thread::sleep_for(chrono::milliseconds(33));
  }
}

void GLAPIENTRY MessageCallback(GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userParam) {
  if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
      "** GL ERROR **",
      type, severity, message);
  }
}

void glfwErrorCb(int i, const char* c) {
  cout << i << " " << c << endl;
}

void createPreviewWindow_onSetup(NGin::Graphics::WinHwnd win) {
  objectPreviewWindowHwnd = win;
  
  glfwMakeContextCurrent(win->rawHwnd);

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  glDisable(GL_DITHER);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);

  NGin::Graphics::setElements(objectPreviewWindowHwnd->myPanel, "html/previewScreen.xml");
  objectMainPreviewCanvasHwnd = make_shared<PreviewCanvas>("objectEditorCanvas", fullContainer, IWindowManagers(), nullptr, objectPreviewWindowHwnd->myPanel);
  NGin::Graphics::addElement(static_pointer_cast<Panel, GUIElement>(objectPreviewWindowHwnd->myPanel->getElementById("objectEditorCanvasContainer")), objectMainPreviewCanvasHwnd->shared_from_this());
  

  win->autoRedraw = false;
}
void createPreviewWindow() {
  NGin::Graphics::CreateMainWindow("Preview", NGin::Graphics::defaultWindowManagers, 1080, 768, true, 0, 0, false, 0, nullptr, createPreviewWindow_onSetup);

  NGin::Graphics::cleanQueues();
}

void createEditorWindow_onSetup(NGin::Graphics::WinHwnd win) {
  objectEditorWindowHwnd = win;
  glewExperimental = GL_TRUE;

  glewInit();

  // get version info
  const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
  const GLubyte* version = glGetString(GL_VERSION); // version as a string
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  // During init, enable debug output
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  glDisable(GL_DITHER);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);


  Gll::gllInit("NGin/GUI/GLL_Res/");

  NGin::Graphics::setElements(objectEditorWindowHwnd->myPanel, "html/mainScreen.xml");
  objectMainFilterGuiHwnd = make_shared<FilterGUI>("objectEditorCanvas", fullContainer, getColor("filter", "bgcolor"), getColor("filter", "activecolor"), getColor("filter", "textcolor"), ctx);
  ctx->gui = objectMainFilterGuiHwnd;
  NGin::Graphics::addElement(static_pointer_cast<Panel, GUIElement>(objectEditorWindowHwnd->myPanel->getElementById("objectEditorFilterGUIContainer")), objectMainFilterGuiHwnd->shared_from_this());

  win->autoRedraw = false;
}
void createEditorWindow() {
  NGin::Graphics::CreateMainWindow("Animator", NGin::Graphics::defaultWindowManagers, 1080, 768, true, 0, 0, false, 0, NULL, createEditorWindow_onSetup);

  NGin::Graphics::cleanQueues();
}

void initGraphics() {
  //NGin::Graphics::setName<ClickCallback>("editorMenuNewButton", editorMenuNewButton);
  NGin::Graphics::setName<SliderInputFunc>("editorTimelineSlider", editorTimelineSlider);
  NGin::Graphics::setName<ClickCallback>("editorTimelinePauseButton", editorTimelinePauseButton);

  NGin::Graphics::initGraphics();
  glfwSetErrorCallback(glfwErrorCb);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

  createEditorWindow();
  createPreviewWindow();
}

int main(int argc, char* argv[]) {
  setlocale(LC_ALL, "en_GB.UTF-8");
  srand(time(NULL));
  ran1(time(NULL));

  loadKeybinds();
  loadColors();

  ctx = make_shared<EditorContext>();
  ctx->_stream_desc._resolution = fVec2(1920, 1080);
  ctx->_stream_desc._frameRate = 30;

  initGraphics();

  shared_ptr<Filter> globalDummy = make_shared<Filter>(ctx);

  shared_ptr<Filter_Resource_Output> resolution = make_shared<Filter_Resource_Output>(globalDummy, "resolution", "Resolution", "Resolution of the output frame", make_shared<Filter_Resource_Object>());
  resolution->_res->castTo< Filter_Resource_Object>()->set_fVec2(fVec2(1920, 1080));


  shared_ptr<FilterColor> filt = make_shared<FilterColor>(ctx);

  filt->init();
  filt->_params["resolution"]->bindInput(resolution);
  filt->configure();
  ctx->_filters.push_back(filt);

  objectMainPreviewCanvasHwnd->_inputs["result"]->bindInput(filt->_outputs["out"]);
  objectMainPreviewCanvasHwnd->configure();

  NGin::Graphics::requestRedraw();

  //thread frames(frameLoop);
  NGin::Graphics::mainLoop();
  objectMainPreviewCanvasHwnd->finishRender();
  //frames.join();

  saveKeybinds();

  return 0;
}