#include "Main.h"

void createSettings(NGin::Graphics::TableHwnd& table) {
  NGin::Graphics::deleteElements(table);
  for (auto&& it : keybinds) {
    NGin::Graphics::PanelHwnd row = NGin::Graphics::createGUI_T<Panel>("objectKeybindRow" + to_string(it.first), LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(1, 0)), getColor("tablerow", "bgcolor"), nullptr);
    NGin::Graphics::LabelHwnd    name = NGin::Graphics::createGUI_T<Label>("objectKeybindLabel" + to_string(it.first), LocationData(LinearScale(0, 5), LinearScale(0, 25), LinearScale(0, 25), LinearScale(0.7, -50)), getColor("label", "bgcolor"), getColor("label", "activecolor"), getColor("label", "textcolor"), nullptr, it.second.display, 0);
    NGin::Graphics::ControlHwnd  ctrl = NGin::Graphics::createGUI_T<ControlSetting>("objectKeybindInput" + to_string(it.first), LocationData(LinearScale(0, 5), LinearScale(0, 25), LinearScale(0.7, -45), LinearScale(1, -5)), getColor("control", "bgcolor"), getColor("control", "activecolor"), getColor("control", "textcolor"), nullptr, it.second, it.first, keybindReply);
    NGin::Graphics::addElement(row, name);
    NGin::Graphics::addElement(row, ctrl);
    NGin::Graphics::addElement(table, row);
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

void editorMenuNewButton(NGin::Graphics::ElemHwnd sender) {
  cout << "NEW" << endl;
}
void editorMenuOpenButton(NGin::Graphics::ElemHwnd sender) {
  cout << "OPEN" << endl;
  string file = openFileSelector("Select file", { { "CGE model file (*.cmf)","*.cmf" } });
  if (file.length()) {
    openFile(file);
  } else {
    cout << "Cancelled" << endl;
  }
}

void editorMenuSaveButton(NGin::Graphics::ElemHwnd sender) {
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
void editorMenuExportButton(NGin::Graphics::ElemHwnd sender) {
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
void editorMenuImportButton(NGin::Graphics::ElemHwnd sender) {
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
void settingsWindowSetup(NGin::Graphics::WinHwnd win) {

  glfwMakeContextCurrent(win->rawHwnd);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  objectSettingsWindowHwnd = win;
  NGin::Graphics::setElements(objectSettingsWindowHwnd->myPanel, "html/mainSettings.xml");
  NGin::Graphics::TableHwnd table = static_pointer_cast<Table, GUIElement>(NGin::Graphics::getElementById("objectSettingsMenuTable"));
  createSettings(table);
}
void editorMenuSettingsButton(NGin::Graphics::ElemHwnd sender) {
  NGin::Graphics::CreateMainWindow("Settings", NGin::Graphics::defaultWindowManagers, 0, 0, false, 0, 0, false, 0, NULL, settingsWindowSetup);
}
void editorMenuExitButton(NGin::Graphics::ElemHwnd sender) {
  NGin::Graphics::forceShutdown();
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
/*void editorToolAddButton(NGin::Graphics::ElemHwnd sender) {
  if(currentToolbar == Toolbars::AddToolbar) {
    currentToolbar = Toolbars::NoToolbar;
    NGin::Graphics::deleteElements((Panel*)NGin::Graphics::getElementById("objectToolContainer"));
    return;
  }
  currentToolbar = Toolbars::AddToolbar;
  NGin::Graphics::setElements((Panel*)NGin::Graphics::getElementById("objectToolContainer"),"html/addToolbar.xml");
}
void editorToolSelectButton(NGin::Graphics::ElemHwnd sender) {
  if (currentToolbar == Toolbars::SelectToolbar) {
    currentToolbar = Toolbars::NoToolbar;
    NGin::Graphics::deleteElements((Panel*)NGin::Graphics::getElementById("objectToolContainer"));
    return;
  }
  currentToolbar = Toolbars::SelectToolbar;
  NGin::Graphics::setElements((Panel*)NGin::Graphics::getElementById("objectToolContainer"), "html/objectToolbar.xml");
}
void editorToolBooleanButton(NGin::Graphics::ElemHwnd sender) {
  if (currentToolbar == Toolbars::BooleanToolbar) {
    currentToolbar = Toolbars::NoToolbar;
    NGin::Graphics::deleteElements((Panel*)NGin::Graphics::getElementById("objectToolContainer"));
    return;
  }
  currentToolbar = Toolbars::BooleanToolbar;
  string ele = "objectToolContainer";
  string fl = "html/booleanToolbar.xml";
  NGin::Graphics::setElements((Panel*)NGin::Graphics::getElementById(ele), fl);
}
*/

//Add toolbar
/*void addToolbarCubeButton(NGin::Graphics::ElemHwnd sender) {

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

void mainWindowSetup(NGin::Graphics::WinHwnd win) {
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

  glEnable(GL_BLEND);

  NGin::Graphics::setElements(objectMainWindowHwnd->myPanel, "html/mainScreen.xml");

  Gll::gllInit("NGin/GUI/GLL_Res/");

  objectMainCanvasHwnd = NGin::Graphics::createGUI_T<Canvas>("objectEditorCanvas", fullContainer, IWindowManagers{
    MainGameCanvas::renderManager,
    MainGameCanvas::resizeManager,
    MainGameCanvas::guiEventManager,
    MainGameCanvas::mouseEntryManager,
    MainGameCanvas::mouseMoveManager,
  }, nullptr);

  NGin::Graphics::LabelBindHwnd objectEditorScaleBinder = static_pointer_cast<LabelBind, GUIElement>(NGin::Graphics::getElementById("objectEditorScaleBinder"));
  objectEditorScaleBinder->text = new TextBind<
    TextBindFunc<float, Editor*>
  >("1 : %",
    TextBindFunc<float, Editor*>(&(Editor::getScale), &mainEditor)
    );

  NGin::Graphics::addElement(static_pointer_cast<Panel, GUIElement>(NGin::Graphics::getElementById("objectEditorCanvasContainer")), objectMainCanvasHwnd);

  win->autoRedraw = false;
}
void initGraphics() {
  NGin::Graphics::setName<ClickCallback>("editorMenuNewButton", editorMenuNewButton);
  NGin::Graphics::setName<ClickCallback>("editorMenuOpenButton", editorMenuOpenButton);
  NGin::Graphics::setName<ClickCallback>("editorMenuSaveButton", editorMenuSaveButton);
  NGin::Graphics::setName<ClickCallback>("editorMenuExportButton", editorMenuExportButton);
  NGin::Graphics::setName<ClickCallback>("editorMenuImportButton", editorMenuImportButton);
  NGin::Graphics::setName<ClickCallback>("editorMenuSettingsButton", editorMenuSettingsButton);
  NGin::Graphics::setName<ClickCallback>("editorMenuExitButton", editorMenuExitButton);

  NGin::Graphics::setName<TextValidatorFunc>("numericalValidator", numericalValidator);
  NGin::Graphics::setName<TextValidatorFunc>("textValidator", textValidator);
  NGin::Graphics::setName<TextValidatorFunc>("floatValidator", floatValidator);


  NGin::Graphics::initGraphics();
  glfwSetErrorCallback(glfwErrorCb);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 
  
  NGin::Graphics::CreateMainWindow("CGE", NGin::Graphics::defaultWindowManagers, 1080, 768, true, 0, 0, false, 0, NULL, mainWindowSetup);

  NGin::Graphics::cleanQueues();
}

int main(int argc, char *argv[]) {
  setlocale(LC_ALL, "en_GB.UTF-8");
  srand(time(NULL));
  ran1(time(NULL));

  loadKeybinds();
  loadColors();

  initGraphics();
 
  mainEditor.init(
    objectMainCanvasHwnd,
    static_pointer_cast<TableRow, GUIElement>(NGin::Graphics::getElementById("objectEditorToolRibbon")),
    static_pointer_cast<Table, GUIElement>(NGin::Graphics::getElementById("objectEditorToolbar")),
    static_pointer_cast<Panel, GUIElement>(NGin::Graphics::getElementById("objectToolContainer"))
  );

  mainEditor.registerPlugin("PluginSelect", createPluginSelect);

  mainEditor.registerPlugin("PluginCreate", createPluginCreate);
  mainEditor.registerPlugin("PluginObject", createPluginObject);
  mainEditor.registerPlugin("PluginBoolean", createPluginBoolean);
  mainEditor.registerPlugin("PluginColor", createPluginColor);
  mainEditor.registerPlugin("PluginBuilding", createPluginBuilding);

  mainEditor.findStaticPlugin("PluginObject");
  mainEditor.findStaticPlugin("PluginBoolean");
  mainEditor.findStaticPlugin("PluginCreate");
  mainEditor.findStaticPlugin("PluginColor");
  mainEditor.findStaticPlugin("PluginBuilding");

  mainEditor.activateStaticPlugin(mainEditor.findStaticPlugin("PluginObject"));

  if (argc > 1) {
    cout << "Opening file " << argv[1] << endl;
    openFile(string(argv[1]));
  }

  NGin::Graphics::requestRedraw();
  NGin::Graphics::mainLoop();

  saveKeybinds();

  return 0;
}