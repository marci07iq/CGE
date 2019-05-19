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
  
  //glfwMakeContextCurrent(win->rawHwnd);

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  glDisable(GL_DITHER);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  NGin::Graphics::setElements(objectPreviewWindowHwnd->myPanel, "html/previewScreen.xml");
  objectMainPreviewCanvasHwnd = make_shared<PreviewCanvas>("objectEditorCanvas", NGin::Graphics::fullContainer, NGin::Graphics::IWindowManagers(), nullptr, objectPreviewWindowHwnd->myPanel, ctx);
  NGin::Graphics::addElement(static_pointer_cast<NGin::Graphics::Panel, NGin::Graphics::GUIElement>(objectPreviewWindowHwnd->myPanel->getElementById("objectEditorCanvasContainer")), objectMainPreviewCanvasHwnd->shared_from_this());
  

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

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  NGin::Gll::gllInit("NGin/GUI/GLL_Res/");
  //Dont delete, this is added to the loaded ILF list
  new NGin::Graphics::IconLocationFile(ilfPath);

  NGin::Graphics::setElements(objectEditorWindowHwnd->myPanel, "html/mainScreen.xml");
  objectMainFilterGuiHwnd = make_shared<FilterGUI>("objectEditorCanvas", NGin::Graphics::fullContainer, NGin::Graphics::getColor("filter", "bgcolor"), NGin::Graphics::getColor("filter", "activecolor"), NGin::Graphics::getColor("filter", "textcolor"), ctx);
  ctx->gui = objectMainFilterGuiHwnd;
  NGin::Graphics::addElement(static_pointer_cast<NGin::Graphics::Panel, NGin::Graphics::GUIElement>(objectEditorWindowHwnd->myPanel->getElementById("objectEditorFilterGUIContainer")), objectMainFilterGuiHwnd->shared_from_this());

  win->autoRedraw = false;
}
void createEditorWindow() {
  NGin::Graphics::CreateMainWindow("Animator", NGin::Graphics::defaultWindowManagers, 1080, 768, true, 0, 0, false, 0, NULL, createEditorWindow_onSetup);

  NGin::Graphics::cleanQueues();
}

void initGraphics() {
  //NGin::Graphics::setName<ClickCallback>("editorMenuNewNGin::Graphics::Button", editorMenuNewNGin::Graphics::Button);
  NGin::Graphics::setName<NGin::Graphics::SliderInputFunc>("editorTimelineSlider", editorTimelineSlider);
  NGin::Graphics::setName<NGin::Graphics::ClickCallback>("editorTimelinePauseNGin::Graphics::Button", editorTimelinePauseButton);

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

  NGin::Graphics::loadKeybinds();
  NGin::Graphics::loadColors();
  ctx = make_shared<EditorContext>();
  ctx->_stream_desc._resolution = fVec2(1920, 1080);
  ctx->_stream_desc._frameRate = 30;

  initGraphics();

  EditorContext::registerFilter("Filter3D", FilterDescription{ "3D", &(createFilter_T<Filter3D>), NGin::Graphics::getIcon("3d", ilfPath) });
  //EditorContext::registerFilter("FilterRenderBatch", FilterDescription{ "RenderBatch", &(createFilter_T<FilterRenderBatch>), NGin::Graphics::getIcon("vao", ilfPath) });
  EditorContext::registerFilter("FilterCameraMatrix", FilterDescription{ "Camera matrix", &(createFilter_T<FilterCameraMatrix>), NGin::Graphics::getIcon("camera", ilfPath) });
  EditorContext::registerFilter("FilterColor", FilterDescription{ "Color", &(createFilter_T<FilterColor>), NGin::Graphics::getIcon("color", ilfPath) });
  EditorContext::registerFilter("FilterShader", FilterDescription{ "Shader", &(createFilter_T<FilterShader>), NGin::Graphics::getIcon("shader", ilfPath) });
  EditorContext::registerFilter("FilterVAO", FilterDescription{ "VAO", &(createFilter_T<FilterVAO>), NGin::Graphics::getIcon("vao", ilfPath) });

  EditorContext::registerFilter("FilterTexture", FilterDescription{ "Texture", &(createFilter_T<FilterTexture>), NGin::Graphics::getIcon("texture", ilfPath) });
  EditorContext::registerFilter("FilterLiteral", FilterDescription{ "Literal", &(createFilter_T<FilterLiteral>), NGin::Graphics::getIcon("out", ilfPath) });


  ctx->init();
  
  //shared_ptr<Filter_Resource_Output> resolution;
  

  /*{
    resolution = make_shared<Filter_Resource_Output>(ctx->_globalDummy, "resolution", "Resolution", "Resolution of the output frame", Filter_Resource_IO_Base::Restriction_Static, make_shared<Filter_Resource_Object>());
    resolution->_res->castTo< Filter_Resource_Object>()->set_fVec2(fVec2(192, 108));

    filt = make_shared<FilterColor>(ctx);

    filt->init();
    filt->_params["resolution"]->tryBindInput(resolution);
    ctx->_filters.push_back(filt);
  }*/

  shared_ptr<FilterLiteral> filt_dir = make_shared<FilterLiteral>(ctx);
  filt_dir->init();
  filt_dir->cax = 0;
  filt_dir->cay = 500;
  string eye_dir = "{\"x\":0, \"y\":0, \"z\":-1}";
  filt_dir->_valInput->text = eye_dir;
  filt_dir->valInput_cb(eye_dir);
  ctx->_filters.push_back(filt_dir);

  shared_ptr<FilterLiteral> filt_eye = make_shared<FilterLiteral>(ctx);
  filt_eye->init();
  filt_eye->cax = 0;
  filt_eye->cay = 400;
  string eye_pos = "{\"x\":1, \"y\":1, \"z\":2}";
  filt_eye->_valInput->text = eye_pos;
  filt_eye->valInput_cb(eye_pos);
  ctx->_filters.push_back(filt_eye);

  shared_ptr<FilterLiteral> filt_res = make_shared<FilterLiteral>(ctx);
  filt_res->init();
  filt_res->cax = 0;
  filt_res->cay = 300;
  string resolution = "{\"x\":1920, \"y\":1920}";
  filt_res->_valInput->text = resolution;
  filt_res->valInput_cb(resolution);
  ctx->_filters.push_back(filt_res);

  shared_ptr<FilterCameraMatrix> filt_cam = make_shared<FilterCameraMatrix>(ctx);
  filt_cam->init();
  filt_cam->cax = 300;
  filt_cam->cay = 400;
  ctx->_filters.push_back(filt_cam);
  filt_cam->_inputs["resolution"]->tryBindInput(filt_res->_outputs["data"]);
  filt_cam->_inputs["cam_dir"]->tryBindInput(filt_dir->_outputs["data"]);
  filt_cam->_inputs["cam_eye"]->tryBindInput(filt_eye->_outputs["data"]);

  shared_ptr<FilterColor> filt_col = make_shared<FilterColor>(ctx);
  filt_col->init();
  filt_col->cax = 300;
  filt_col->cay = 250;
  ctx->_filters.push_back(filt_col);
  filt_col->_params["resolution"]->tryBindInput(filt_res->_outputs["data"]);

  shared_ptr<FilterLiteral> filt_sha_path = make_shared<FilterLiteral>(ctx);
  filt_sha_path->init();
  filt_sha_path->cax = 0;
  filt_sha_path->cay = 100;
  string sha_path = "\"Filters/Shaders/Simple3D\"";
  filt_sha_path->_valInput->text = sha_path;
  filt_sha_path->valInput_cb(sha_path);
  ctx->_filters.push_back(filt_sha_path);
  
  shared_ptr<FilterShader> filt_sha = make_shared<FilterShader>(ctx);
  filt_sha->init();
  filt_sha->cax = 300;
  filt_sha->cay = 50;
  ctx->_filters.push_back(filt_sha);
  filt_sha->_params["path"]->tryBindInput(filt_sha_path->_outputs["data"]);

  shared_ptr<FilterLiteral> filt_ply_path = make_shared<FilterLiteral>(ctx);
  filt_ply_path->init();
  filt_ply_path->cax = 0;
  filt_ply_path->cay = 200;
  string ply_path = "\"test.ply\"";
  filt_ply_path->_valInput->text = sha_path;
  filt_ply_path->valInput_cb(ply_path);
  ctx->_filters.push_back(filt_ply_path);

  shared_ptr<FilterVAO> filt_ply = make_shared<FilterVAO>(ctx);
  filt_ply->init();
  filt_ply->cax = 300;
  filt_ply->cay = 150;
  ctx->_filters.push_back(filt_ply);
  filt_ply->_params["path"]->tryBindInput(filt_ply_path->_outputs["data"]);

  shared_ptr<Filter3D> filt_3d = make_shared<Filter3D>(ctx);
  filt_3d->init();
  filt_3d->cax = 500;
  filt_3d->cay = 100;
  ctx->_filters.push_back(filt_3d);
  filt_3d->_inputs["cam_mat"]->tryBindInput(filt_cam->_outputs["mat"]);
  filt_3d->_inputs["in"]->tryBindInput(filt_col->_outputs["out"]);
  filt_3d->_inputs["shader"]->tryBindInput(filt_sha->_outputs["out"]);
  filt_3d->_inputs["objects"]->tryBindInput(filt_ply->_outputs["out"]);

  ctx->_exit->cax = 700;
  ctx->_exit->cay = 200;
  ctx->_exit->_inputs["result"]->tryBindInput(filt_3d->_outputs["out"]);
  /*{
    //resolution = make_shared<Filter_Resource_Output>(ctx->_globalDummy, "resolution", "Resolution", "Resolution of the output frame", Filter_Resource_IO_Base::Restriction_Static, make_shared<Filter_Resource_Object>());
    //resolution->_res->castTo< Filter_Resource_Object>()->set_fVec2(fVec2(192, 108));

    shared_ptr<FilterColor> filt = make_shared<FilterColor>(ctx);

    filt->init();
    //filt->_params["resolution"]->tryBindInput(resolution);
    ctx->_filters.push_back(filt);

    //ctx->_exit->_inputs["result"]->tryBindInput(filt->_outputs["out"]);
  }*/

  objectMainPreviewCanvasHwnd->configure();

  NGin::Graphics::requestRedraw();

  //thread frames(frameLoop);
  NGin::Graphics::mainLoop();
  objectMainPreviewCanvasHwnd->finishRender();
  //frames.join();

  NGin::Graphics::saveKeybinds();

  return 0;
}