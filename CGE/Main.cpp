#include "Graphics/Render.h"

void createSettings(Graphics::TableHwnd& table) {
  Graphics::deleteElements(table);
  int i = 0;
  for (auto&& it : keybinds) {
    Graphics::TablerowHwnd row = Graphics::createTableRow("objectKeybindRow" + to_string(i), LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(1, 0)), getColor("tablerow", "bgcolor"));
    Graphics::LabelHwnd    name = Graphics::createLabel("objectKeybindLabel" + to_string(i), LocationData(LinearScale(0, 5), LinearScale(0, 25), LinearScale(0, 25), LinearScale(0.7, -50)), getColor("label", "bgcolor"), getColor("label", "activecolor"), getColor("label", "textcolor"), it.second.display, 0);
    Graphics::ControlHwnd  ctrl = Graphics::createControl("objectKeybindInput" + to_string(i), LocationData(LinearScale(0, 5), LinearScale(0, 25), LinearScale(0.7, -45), LinearScale(1, -5)), getColor("control", "bgcolor"), getColor("control", "activecolor"), getColor("control", "textcolor"), it.second, i, keybindReply);
    Graphics::addElement(row, name);
    Graphics::addElement(row, ctrl);
    Graphics::addElement(table, row);
    ++i;
  }
}

void editorMenuNewButton(Graphics::ElemHwnd sender) {
  cout << "NEW" << endl;
}
void editorMenuOpenButton(Graphics::ElemHwnd sender) {
  cout << "OPEN" << endl;
}
void editorMenuSaveButton(Graphics::ElemHwnd sender) {
  cout << "SAVE" << endl;
}
void settingsWindowSetup(Graphics::WinHwnd win) {
  objectSettingsWindowHwnd = win;
  Graphics::setElements(objectSettingsWindowHwnd->myPanel, "html/mainSettings.xml");
  Graphics::TableHwnd table = (Graphics::TableHwnd)Graphics::getElementById("objectSettingsMenuTable");
  createSettings(table);
}
void editorMenuSettingsButton(Graphics::ElemHwnd sender) {
  Graphics::CreateMainWindow("Settings", Graphics::defaultWindowManagers, 0, 0, false, 0, 0, false, 0, NULL, settingsWindowSetup);
}
void editorMenuExitButton(Graphics::ElemHwnd sender) {
  Graphics::forceShutdown();
}


enum Toolbars {
  NoToolbar,
  AddToolbar,
  SelectToolbar,
  BooleanToolbar,
};
Toolbars currentToolbar;

void editorToolAddButton(Graphics::ElemHwnd sender) {
  if(currentToolbar == Toolbars::AddToolbar) {
    currentToolbar = Toolbars::NoToolbar;
    Graphics::deleteElements((Panel*)Graphics::getElementById("objectToolContainer"));
    return;
  }
  currentToolbar = Toolbars::AddToolbar;
  Graphics::setElements((Panel*)Graphics::getElementById("objectToolContainer"),"html/addToolbar.xml");
}
void editorToolSelectButton(Graphics::ElemHwnd sender) {
  if (currentToolbar == Toolbars::SelectToolbar) {
    currentToolbar = Toolbars::NoToolbar;
    Graphics::deleteElements((Panel*)Graphics::getElementById("objectToolContainer"));
    return;
  }
  currentToolbar = Toolbars::SelectToolbar;
  Graphics::setElements((Panel*)Graphics::getElementById("objectToolContainer"), "html/objectToolbar.xml");
}
void editorToolBooleanButton(Graphics::ElemHwnd sender) {
  if (currentToolbar == Toolbars::BooleanToolbar) {
    currentToolbar = Toolbars::NoToolbar;
    Graphics::deleteElements((Panel*)Graphics::getElementById("objectToolContainer"));
    return;
  }
  currentToolbar = Toolbars::BooleanToolbar;
  string ele = "objectToolContainer";
  string fl = "html/booleanToolbar.xml";
  Graphics::setElements((Panel*)Graphics::getElementById(ele), fl);
}

//Add toolbar
void addToolbarCubeButton(Graphics::ElemHwnd sender) {

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
  cout << i << c << endl;
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
  //glDisable(GL_POINT_SMOOTH);
  //glDisable(GL_LINE_SMOOTH);
  //glDisable(GL_POLYGON_SMOOTH);
  //glHint(GL_POINT_SMOOTH, GL_DONT_CARE);
  //glHint(GL_LINE_SMOOTH, GL_DONT_CARE);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
//#define GL_MULTISAMPLE_ARB 0x809D
//  glDisable(GL_MULTISAMPLE_ARB);

  Graphics::setElements(objectMainWindowHwnd->myPanel, "html/mainScreen.xml");

  baseShader.create("Graphics/Core");
  edgeShader.create("Graphics/Edges", 7);
  Gll::gllInit("../NGin/GUI/GLL_Res/");

  objectMainCanvasHwnd = Graphics::createCanvas("objectEditorCanvas", fullContainer, IWindowManagers{
    MainGameCanvas::renderManager,
    MainGameCanvas::resizeManager,
    MainGameCanvas::guiEventManager,
    MainGameCanvas::mouseEntryManager,
    MainGameCanvas::mouseMoveManager,
  });
  Graphics::addElement((Graphics::PanelHwnd)Graphics::getElementById("objectEditorCanvasContainer"), objectMainCanvasHwnd);

}
void initGraphics() {
  Graphics::setName("editorMenuNewButton", editorMenuNewButton);
  Graphics::setName("editorMenuOpenButton", editorMenuOpenButton);
  Graphics::setName("editorMenuSaveButton", editorMenuSaveButton);
  Graphics::setName("editorMenuSettingsButton", editorMenuSettingsButton);
  Graphics::setName("editorMenuExitButton", editorMenuExitButton);

  Graphics::setName("editorToolAddButton", editorToolAddButton);
  Graphics::setName("editorToolSelectButton", editorToolSelectButton);
  Graphics::setName("editorToolBooleanButton", editorToolBooleanButton);

  Graphics::setName("addToolbarCubeButton", addToolbarCubeButton);

  //glutInitContextVersion(4, 2);
  //glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
  //glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

  //glutInit(&argc, argv);
  Graphics::initGraphics();
  glfwSetErrorCallback(glfwErrorCb);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); // We don't want the old OpenGL 
  
  Graphics::CreateMainWindow("CGE", Graphics::defaultWindowManagers, 1080, 768, true, 0, 0, false, 0, NULL, mainWindowSetup);
}

int main() {
  setlocale(LC_ALL, "");
  srand(time(NULL));
  ran1(time(NULL));

  loadKeybinds();
  loadColors();

  initGraphics();

  Graphics::cleanQueues();

  Object o1;
  o1.setCube();
  o1.setShared(reinterpret_cast<void*>(0xffff0000));

  o1.upload();
  objs.push_back(&o1);

  Object o2;
  o2.setCube({2,0.5,0.2},{0,0,0});
  o2.setShared(reinterpret_cast<void*>(0xff00ff00));

  o2.upload();
  objs.push_back(&o2);

  Object o3;
  o3.setCube({ 0.2,1,2 }, { 0,1,0 });
  o3.setShared(reinterpret_cast<void*>(0xff0000ff));

  o3.upload();
  objs.push_back(&o3);


  Graphics::requestRedraw();
  Graphics::mainLoop();

  saveKeybinds();

  return 0;
}