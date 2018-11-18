#include "Graphics/Render.h"

void editorMenuNewButton(string sender) {

}
void editorMenuOpenButton(string sender) {

}
void editorMenuSaveButton(string sender) {

}
void editorMenuExitButton(string sender) {
  glutExit();
}

enum Toolbars {
  NoToolbar,
  AddToolbar,
  SelectToolbar,
  BooleanToolbar,
};

Toolbars currentToolbar;

void editorToolAddButton(string sender) {
  if(currentToolbar == Toolbars::AddToolbar) {
    currentToolbar = Toolbars::NoToolbar;
    Graphics::deleteElements((Panel*)Graphics::getElementById("objectToolContainer"));
    return;
  }
  currentToolbar = Toolbars::AddToolbar;
  Graphics::setElements((Panel*)Graphics::getElementById("objectToolContainer"),"html/addToolbar.xml");
}
void editorToolSelectButton(string sender) {
  if (currentToolbar == Toolbars::SelectToolbar) {
    currentToolbar = Toolbars::NoToolbar;
    Graphics::deleteElements((Panel*)Graphics::getElementById("objectToolContainer"));
    return;
  }
  currentToolbar = Toolbars::SelectToolbar;
  Graphics::setElements((Panel*)Graphics::getElementById("objectToolContainer"), "html/objectToolbar.xml");
}
void editorToolBooleanButton(string sender) {
  if (currentToolbar == Toolbars::BooleanToolbar) {
    currentToolbar = Toolbars::NoToolbar;
    Graphics::deleteElements((Panel*)Graphics::getElementById("objectToolContainer"));
    return;
  }
  currentToolbar = Toolbars::BooleanToolbar;
  Graphics::setElements((Panel*)Graphics::getElementById("objectToolContainer"), "html/booleanToolbar.xml");
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

void initGraphics() {
  Graphics::setName("editorMenuNewButton", editorMenuNewButton);
  Graphics::setName("editorMenuOpenButton", editorMenuOpenButton);
  Graphics::setName("editorMenuSaveButton", editorMenuSaveButton);
  Graphics::setName("editorMenuExitButton", editorMenuExitButton);

  Graphics::setName("editorToolAddButton", editorToolAddButton);
  Graphics::setName("editorToolSelectButton", editorToolSelectButton);
  Graphics::setName("editorToolBooleanButton", editorToolBooleanButton);

  glutInitContextVersion(4, 2);
  //glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
  glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

  int argc = 0;
  char **argv = new char*[0];
  glutInit(&argc, argv);

  objectMainWindowHwnd = Graphics::CreateMainWindow("CGE", Graphics::defaultWindowManagers, 1080, 768, true, 0, 0, false);

  glewExperimental = GL_TRUE;
  GLenum err = glewInit();

  if (err != GLEW_OK) {
    cerr << "GLEW init error: " << glewGetErrorString(err) << endl;
  }
  cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;

  if (GLEW_VERSION_4_2) {
    cout << "OpenGL 4.2 supported." << endl;
  }

  // During init, enable debug output
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  glDisable(GL_DITHER);
  glDisable(GL_POINT_SMOOTH);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_POLYGON_SMOOTH);
  //glHint(GL_POINT_SMOOTH, GL_DONT_CARE);
  //glHint(GL_LINE_SMOOTH, GL_DONT_CARE);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
#define GL_MULTISAMPLE_ARB 0x809D
  glDisable(GL_MULTISAMPLE_ARB);

  Graphics::setElements(objectMainWindowHwnd->myPanel, "html/mainScreen.xml");

  objectMainCanvasHwnd = Graphics::createCanvas("objectEditorCanvas", fullContainer, IWindowManagers{
    MainGameCanvas::renderManager,
    MainGameCanvas::resizeManager,
    MainGameCanvas::guiEventManager,
    MainGameCanvas::mouseEntryManager,
    MainGameCanvas::mouseMoveManager,
  });
  Graphics::addElement((Graphics::PanelHwnd)Graphics::getElementById("objectEditorCanvasContainer"), objectMainCanvasHwnd);

  baseShader.create("Graphics/Core");
  edgeShader.create("Graphics/Edges", 7);

}

int main() {
  setlocale(LC_ALL, "");
  srand(time(NULL));
  ran1(time(NULL));

  loadKeybinds();
  loadColors();

  initGraphics();

  Object o1;
  o1.setCube();
  o1.setShared(reinterpret_cast<void*>(0xffff00));

  Object o2;
  o2.setCube({2,0.5,0.2},{0,0,0});
  o2.setShared(reinterpret_cast<void*>(0x0000ff));

  Object o3;
  o3.setCube({ 0.2,1,2 }, { 0,1,0 });
  o3.setShared(reinterpret_cast<void*>(0x00ff00));

  Object* res = new Object(o1 - o2 + o3);

  res->upload();
  objs.push_back(res);


  glutPostRedisplay();
  glutMainLoop();

  return 0;
}