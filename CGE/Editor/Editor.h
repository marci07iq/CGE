#pragma once

#include "../Renderer/Transpose.h"

class EditorPlugin;
class Editor;

typedef EditorPlugin*(*PluginCreator)(Editor*, bool);

class Editor {
public:
  static Texture _editorIcons;

  static Shader _baseShader;
  static GLuint _baseShader_transform;
  static GLuint _baseShader_mix_color;
  static GLuint _baseShader_res_alpha;

  static Shader _edgeShader;
  static GLuint _edgeShader_transform;
  //static GLuint _edgeShader_transform2;
  static GLuint _edgeShader_cam_eye;
  static GLuint _edgeShader_color;

  //Plugin types
  static map<string, PluginCreator> _pluginTypes;
  static void registerPlugin(string name, PluginCreator creator);


  map<string, EditorPlugin*> _staticPlugins;

  //list<EditorPlugin*> _pluginStack;
  EditorPlugin* _currentPlugin;

  Graphics::CanvasHwnd _main;

  Graphics::TablerowHwnd _toolribbon; //Ribbon bar up top; for small button
  Graphics::TableHwnd _toolbar; //Sidebar for access to plugins
  Graphics::PanelHwnd _config; //Large pane next to sidebar

  list<shared_ptr<Object>> objs;

  OpenGLData view;

  Transpose modview;
  float worldM[16];

  Transpose camview;
  float cameraM[16];

  bool drawDown;
  bool turnDown;

  //list<fVec3> polyRays;
  polar_vec3 viewOffset = { 5,1,1 };
  fVec3 viewCenter;

  Editor();

  static void staticInit();;

  void init(Graphics::CanvasHwnd main, Graphics::TablerowHwnd toolribbon, Graphics::TableHwnd sidebar, Graphics::PanelHwnd config);

  EditorPlugin* findStaticPlugin(string name);
  void activateStaticPlugin(EditorPlugin* plugin);
  EditorPlugin* createDynamicPlugin(string name);

  void registerRibbon(Graphics::ElemHwnd elem);
  void removeRibbon(Graphics::ElemHwnd elem);

  void registerSidebar(Graphics::ElemHwnd elem);
  void removeSidebar(Graphics::ElemHwnd elem);


  int renderManager(int ax, int ay, int bx, int by, set<key_location>& down);

  void beginObjectDraw();
  void drawObject(shared_ptr<Object> what, colorargb mix = 0x00000000, float resAlpha = 1);
  void endObjectDraw();
  void beginEdgeDraw();
  void drawEdge(shared_ptr<Object> what, colorargb edge = 0xff000000);
  void endEdgeDraw();

  void removeObject(shared_ptr<Object> obj) {
    objs.remove(obj);
    obj->_toDelete = true;
  }

  int resizeManager(int x, int y);
  int mouseEntryManager(int state);
  int mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down);
  int guiEventManager(gui_event evt, int mx, int my, set<key_location>& down);
};

enum Icons {
  //Global
  Icon_New = 0,
  Icon_Open = 1,
  Icon_Save = 2,
  Icon_Close = 3,
  Icon_Resize = 4,

};