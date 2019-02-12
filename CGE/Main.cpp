#include "Main.h"

void createSettings(Graphics::TableHwnd& table) {
  Graphics::deleteElements(table);
  for (auto&& it : keybinds) {
    Graphics::PanelHwnd row = Graphics::createPanel("objectKeybindRow" + to_string(it.first), LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(1, 0)), getColor("tablerow", "bgcolor"), NULL);
    Graphics::LabelHwnd    name = Graphics::createLabel("objectKeybindLabel" + to_string(it.first), LocationData(LinearScale(0, 5), LinearScale(0, 25), LinearScale(0, 25), LinearScale(0.7, -50)), getColor("label", "bgcolor"), getColor("label", "activecolor"), getColor("label", "textcolor"), NULL, it.second.display, 0);
    Graphics::ControlHwnd  ctrl = Graphics::createControl("objectKeybindInput" + to_string(it.first), LocationData(LinearScale(0, 5), LinearScale(0, 25), LinearScale(0.7, -45), LinearScale(1, -5)), getColor("control", "bgcolor"), getColor("control", "activecolor"), getColor("control", "textcolor"), NULL, it.second, it.first, keybindReply);
    Graphics::addElement(row, name);
    Graphics::addElement(row, ctrl);
    Graphics::addElement(table, row);
  }
}

void openFile(string file) {
  DataElement* data = new DataElement();
  loadFromFile(data, file);
  int version = -1;
  vSFunc<int>(version, data);
  cout << "File version: " << version << endl;
  for (auto&& it : data->_children) {
    shared_ptr<Object> newObj = make_shared<Object>();
    newObj->set(it);
    newObj->upload();
    mainEditor.objs.push_back(newObj);
  }
  delete data;
}

void editorMenuNewButton(Graphics::ElemHwnd sender, void* data) {
  cout << "NEW" << endl;
}
void editorMenuOpenButton(Graphics::ElemHwnd sender, void* data) {
  cout << "OPEN" << endl;
  string file = openFileSelector("Select file", { { "CGE model file (*.cmf)","*.cmf" } });
  if (file.length()) {
    openFile(file);
  } else {
    cout << "Cancelled" << endl;
  }
}

void editorMenuSaveButton(Graphics::ElemHwnd sender, void* data) {
  cout << "SAVE" << endl;
  string file = saveFileSelector("Select file", { { "CGE model file (*.cmf)","*.cmf" } }, "cmf");
  if (file.length()) {
    DataElement* fileData = new DataElement();
    vGFunc(CGE_FILE_CMF_VERSION, fileData);
    for (auto&& it : mainEditor.objs) {
      DataElement* filePart = new DataElement();
      it->get(filePart);
      fileData->addChild(filePart);
    }
    saveToFile(fileData, file);
    
  } else {
    cout << "Cancelled" << endl;
  }
}
void editorMenuExportButton(Graphics::ElemHwnd sender, void* data) {
  cout << "EXPORT" << endl;
  string file = saveFileSelector("Select file", { { "Polygon File (*.ply)","*.ply" } });
  if (file.length()) {
    Mesh compact;
    list<Mesh*> meshPtrs;
    for (auto&& it : mainEditor.objs) {
      meshPtrs.push_back(&(it->_mesh));
    }
    compactOperation(meshPtrs, compact);
    ofstream outFile(file, ios::binary);
    compact.writePly(outFile, false, true);
  } else {
    cout << "Cancelled" << endl;
  }
}
void editorMenuImportButton(Graphics::ElemHwnd sender, void* data) {
  cout << "IMPORT" << endl;
  string file = openFileSelector("Select file", { { "Polygon File (*.ply)","*.ply" },{ "Any file (*.*)","*.*" } });
  if (file.length()) {
    shared_ptr<Object> newObj = make_shared<Object>();
    ifstream inFile(file, ios::binary);
    newObj->_mesh.readPly(inFile);
    newObj->upload();
    mainEditor.objs.push_back(newObj);
  } else {
    cout << "Cancelled" << endl;
  }
}
void settingsWindowSetup(Graphics::WinHwnd win) {
  objectSettingsWindowHwnd = win;
  Graphics::setElements(objectSettingsWindowHwnd->myPanel, "html/mainSettings.xml");
  Graphics::TableHwnd table = (Graphics::TableHwnd)Graphics::getElementById("objectSettingsMenuTable");
  createSettings(table);
}
void editorMenuSettingsButton(Graphics::ElemHwnd sender, void* data) {
  Graphics::CreateMainWindow("Settings", Graphics::defaultWindowManagers, 0, 0, false, 0, 0, false, 0, NULL, settingsWindowSetup);
}
void editorMenuExitButton(Graphics::ElemHwnd sender, void* data) {
  Graphics::forceShutdown();
}

/*
enum Toolbars {
  NoToolbar,
  AddToolbar,
  SelectToolbar,
  BooleanToolbar,
};
Toolbars currentToolbar;
*/
/*void editorToolAddButton(Graphics::ElemHwnd sender) {
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
*/

//Add toolbar
/*void addToolbarCubeButton(Graphics::ElemHwnd sender) {

}*/


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
  glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);

  Graphics::setElements(objectMainWindowHwnd->myPanel, "html/mainScreen.xml");

  Gll::gllInit("../NGin/GUI/GLL_Res/");

  objectMainCanvasHwnd = Graphics::createCanvas("objectEditorCanvas", fullContainer, IWindowManagers{
    MainGameCanvas::renderManager,
    MainGameCanvas::resizeManager,
    MainGameCanvas::guiEventManager,
    MainGameCanvas::mouseEntryManager,
    MainGameCanvas::mouseMoveManager,
  }, NULL);
  Graphics::addElement((Graphics::PanelHwnd)Graphics::getElementById("objectEditorCanvasContainer"), objectMainCanvasHwnd);
}
void initGraphics() {
  Graphics::setName<ClickCallback>("editorMenuNewButton", editorMenuNewButton);
  Graphics::setName<ClickCallback>("editorMenuOpenButton", editorMenuOpenButton);
  Graphics::setName<ClickCallback>("editorMenuSaveButton", editorMenuSaveButton);
  Graphics::setName<ClickCallback>("editorMenuExportButton", editorMenuExportButton);
  Graphics::setName<ClickCallback>("editorMenuImportButton", editorMenuImportButton);
  Graphics::setName<ClickCallback>("editorMenuSettingsButton", editorMenuSettingsButton);
  Graphics::setName<ClickCallback>("editorMenuExitButton", editorMenuExitButton);

  Graphics::setName<TextValidatorFunc>("numericalValidator", numericalValidator);
  Graphics::setName<TextValidatorFunc>("textValidator", textValidator);
  Graphics::setName<TextValidatorFunc>("floatValidator", floatValidator);


  Graphics::initGraphics();
  glfwSetErrorCallback(glfwErrorCb);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
  
  Graphics::CreateMainWindow("CGE", Graphics::defaultWindowManagers, 1080, 768, true, 0, 0, false, 0, NULL, mainWindowSetup);

  Graphics::cleanQueues();
}

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, "en_GB.UTF-8");
  srand(time(NULL));
  ran1(time(NULL));

  loadKeybinds();
  loadColors();

  initGraphics();
 
  mainEditor.init(objectMainCanvasHwnd, (Graphics::TablerowHwnd)Graphics::getElementById("objectEditorToolRibbon"), (Graphics::TableHwnd)Graphics::getElementById("objectEditorToolbar"), (Graphics::PanelHwnd)Graphics::getElementById("objectToolContainer"));

  mainEditor.registerPlugin("PluginSelect", createPluginSelect);

  mainEditor.registerPlugin("PluginCreate", createPluginCreate);
  mainEditor.registerPlugin("PluginObject", createPluginObject);
  mainEditor.registerPlugin("PluginBoolean", createPluginBoolean);
  mainEditor.registerPlugin("PluginColor", createPluginColor);

  mainEditor.findStaticPlugin("PluginObject");
  mainEditor.findStaticPlugin("PluginBoolean");
  mainEditor.findStaticPlugin("PluginCreate");
  mainEditor.findStaticPlugin("PluginColor");

  mainEditor.activateStaticPlugin(mainEditor.findStaticPlugin("PluginObject"));

  if (argc > 1) {
    cout << "Opening file " << argv[1] << endl;
    openFile(string(argv[1]));
  }

  Graphics::requestRedraw();
  Graphics::mainLoop();

  saveKeybinds();

  return 0;
}