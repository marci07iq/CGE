#include "Main.h"

Graphics::WinHwnd objectMainWindowHwnd;
PreviewCanvas* objectMainPreviewCanvasHwnd;

void frameLoop() {
  while (Graphics::windows.size()) {
    Graphics::requestRedraw();
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

void mainWindowSetup(Graphics::WinHwnd win) {
  objectMainWindowHwnd = win;
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

  Graphics::setElements(objectMainWindowHwnd->myPanel, "html/mainScreen.xml");

  Gll::gllInit("NGin/GUI/GLL_Res/");

  objectMainPreviewCanvasHwnd = new PreviewCanvas("objectEditorCanvas", fullContainer, IWindowManagers(), NULL);

  /*Graphics::LabelBindHwnd objectEditorScaleBinder = (Graphics::LabelBindHwnd)(Graphics::getElementById("objectEditorScaleBinder"));
  objectEditorScaleBinder->text = new TextBind<
    TextBindFunc<float, Editor*>
  >("1 : %",
    TextBindFunc<float, Editor*>(&(Editor::getScale), &mainEditor)
    );*/

  Graphics::addElement((Graphics::PanelHwnd)Graphics::getElementById("objectEditorCanvasContainer"), objectMainPreviewCanvasHwnd);

  win->autoRedraw = false;
}
void initGraphics() {
  //Graphics::setName<ClickCallback>("editorMenuNewButton", editorMenuNewButton);

  Graphics::initGraphics();
  glfwSetErrorCallback(glfwErrorCb);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

  Graphics::CreateMainWindow("CGE", Graphics::defaultWindowManagers, 1080, 768, true, 0, 0, false, 0, NULL, mainWindowSetup);

  Graphics::cleanQueues();
}

shared_ptr<EditorContext> ctx;

int main(int argc, char* argv[]) {
  setlocale(LC_ALL, "en_GB.UTF-8");
  srand(time(NULL));
  ran1(time(NULL));

  loadKeybinds();
  loadColors();

  initGraphics();

  ctx = make_shared<EditorContext>();
  shared_ptr<Filter> globalDummy = make_shared<Filter>(ctx);

  shared_ptr<Filter_Resource_Output> resolution = make_shared<Filter_Resource_Output>(globalDummy, "resolution", "Resolution", "Resolution of the output frame", make_shared<Filter_Resource_Object>());
  resolution->_res->castTo< Filter_Resource_Object>()->set_fVec2(fVec2(1920, 1080));


  shared_ptr<FilterColor> filt = make_shared<FilterColor>(ctx);

  filt->init();
  filt->_params["resolution"]->bindInput(resolution);
  filt->configure();
  ctx->_filters.push_back(filt);

  objectMainPreviewCanvasHwnd->_inputs["result"]->bindInput(filt->_outputs["out"]);

  Graphics::requestRedraw();

  thread frames(frameLoop);
  Graphics::mainLoop();
  frames.join();
  objectMainPreviewCanvasHwnd->finishRender();

  saveKeybinds();

  return 0;
}