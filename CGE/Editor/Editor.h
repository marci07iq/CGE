#pragma once

#include "../CSG/Object.h"

class EditorPlugin;
class Editor;

typedef EditorPlugin*(*PluginCreator)(Editor*, bool);

class Editor {
public:
  static NGin::Graphics::Texture _editorIcons;

  static NGin::Graphics::Shader _baseShader;
  static GLuint _baseShader_transform;
  static GLuint _baseShader_mix_color;
  static GLuint _baseShader_res_alpha;

  static NGin::Graphics::Shader _edgeShader;
  static GLuint _edgeShader_transform;
  //static GLuint _edgeShader_transform2;
  static GLuint _edgeShader_cam_eye;
  static GLuint _edgeShader_color;

  static NGin::Graphics::Shader _checkShader;
  static GLuint _checkShader_transform;

  static NGin::Graphics::Shader _lineShader;
  static GLuint _lineShader_modview;
  static GLuint _lineShader_camview;
  static GLuint _lineShader_cam_eye;

  static GLuint _coordinate_vao;
  static GLuint _coordinate_vbo_pos;
  static GLuint _coordinate_vbo_col;
  static GLuint _coordinate_vbo_rad;

  //Plugin types
  static map<string, PluginCreator> _pluginTypes;
  static void registerPlugin(string name, PluginCreator creator);


  map<string, EditorPlugin*> _staticPlugins;

  //list<EditorPlugin*> _pluginStack;
  EditorPlugin* _currentPlugin;

  NGin::Graphics::CanvasHwnd _main;

  NGin::Graphics::TablerowHwnd _toolribbon; //Ribbon bar up top; for small NGin::Graphics::Button
  NGin::Graphics::TableHwnd _toolbar; //Sidebar for access to plugins
  NGin::Graphics::PanelHwnd _config; //Large pane next to sidebar

  list<shared_ptr<Object>> objs;

  NGin::Graphics::OpenGLData view;

  Transform modview;
  //float worldM[16];

  Transform camview;
  //float cameraM[16];

  bool drawDown;
  bool turnDown;

  //list<fVec3> polyRays;
  polar_vec3 viewOffset = { 5,1,1 };
  fVec3 viewCenter;

  Editor();

  static void staticInit();;

  void init(NGin::Graphics::CanvasHwnd main, NGin::Graphics::TablerowHwnd toolribbon, NGin::Graphics::TableHwnd sidebar, NGin::Graphics::PanelHwnd config);

  EditorPlugin* findStaticPlugin(string name);
  void activateStaticPlugin(EditorPlugin* plugin);
  EditorPlugin* createDynamicPlugin(string name);

  void registerRibbon(NGin::Graphics::ElemHwnd elem);
  void removeRibbon(NGin::Graphics::ElemHwnd elem);

  void registerSidebar(NGin::Graphics::ElemHwnd elem);
  void removeSidebar(NGin::Graphics::ElemHwnd elem);

  int renderManager(int ax, int ay, int bx, int by, std::set<NGin::Graphics::key_location>& down);
  void drawCoordinateSystem(int ax, int ay, int bx, int by);
  void drawXYZ(Transform& modview, Transform& camview, fVec3& eye);

  void beginObjectDraw();
  void drawObject(shared_ptr<Object> what, Matrix4f objectTransform, colorargb mix = 0x00000000, float resAlpha = 1);
  void endObjectDraw();
  
  void beginEdgeDraw();
  void drawEdge(shared_ptr<Object> what, Matrix4f objectTransform, colorargb edge = 0xff000000);
  void endEdgeDraw();

  void removeObject(shared_ptr<Object> obj) {
    objs.remove(obj);
    obj->_toDelete = true;
  }

  float getScale();

  int resizeManager(int x, int y);
  int mouseEntryManager(int state);
  int mouseMoveManager(int x, int y, int ox, int oy, std::set<NGin::Graphics::key_location>& down, bool in);
  int guiEventManager(NGin::Graphics::gui_event& evt, int mx, int my, std::set<NGin::Graphics::key_location>& down, bool in);
};

enum Icons {
  //Global
  Icon_New = 0,
  Icon_Open = 1,
  Icon_Save = 2,
  Icon_Close = 3,
  Icon_Resize = 4,

};