#pragma once

#include "Framework.h"

class PluginBuilding;

namespace Building {
  class AnchorPoint;
  class Wallsegment;
  class VirtualCorner;

  class Wallsegment;
  class BuildingObject;
  class BuildingFloor;
  class Building;

  class AnchorPoint : public fVec2, public std::enable_shared_from_this<AnchorPoint> {
  public:
    bool _toDelete = false;

    BuildingFloor* _floor;

    enum Type {
      Type_AnchorPoint,
      Type_WallCorner,
      Type_VirtualCorner
    };

    virtual Type type() {
      return Type::Type_AnchorPoint;
    }

    AnchorPoint(double x, double y, BuildingFloor* floor) : fVec2(x, y), _floor(floor) {

    }
  };

  class WallCorner : public AnchorPoint, public std::enable_shared_from_this<WallCorner> {
  public:
    list<shared_ptr<Wallsegment>> _walls;
    //List right (cw) virtual corner for each wall. If ony one wall-> list both corners
    list<shared_ptr<VirtualCorner>> _virtualCorners;

    void addWall(shared_ptr<Wallsegment> wall);
    void modifyWall(shared_ptr<Wallsegment> wall);
    void deleteWall(shared_ptr<Wallsegment> wall);

    virtual Type type() {
      return Type::Type_WallCorner;
    }

    WallCorner(double x, double y, BuildingFloor* _floor);
  };

  class VirtualCorner : public AnchorPoint, public std::enable_shared_from_this<VirtualCorner> {
  public:
    shared_ptr<WallCorner> _corner;

    //Left and right given by angular sorting (from +y, towards +x) (cw in OGL screen coords)
    shared_ptr<Wallsegment> _left;
    shared_ptr<Wallsegment> _right;

    virtual Type type() {
      return Type::Type_VirtualCorner;
    }

    VirtualCorner(double x, double y, BuildingFloor* _floor);
  };

  class Wallsegment: public std::enable_shared_from_this<Wallsegment> {
  public:
    bool _toDelete = false;

    shared_ptr<WallCorner> _pos[2];

    //Virutual corners (for connected rendering)
    shared_ptr<VirtualCorner> _pos_virt[2][2]; //Id, Left/Right

    BuildingFloor* _floor;

    float _width;
    float _startHeight;
    float _endHeight;

    Wallsegment(shared_ptr<WallCorner> a, shared_ptr<WallCorner> b, float width, BuildingFloor* floor);

    int cornerSide(WallCorner* what);

    void compile();

    ~Wallsegment();
  };

  class BuildingObject : public AnchorPoint, public std::enable_shared_from_this<BuildingObject> {
    public:

  };

  class BuildingFloor : public std::enable_shared_from_this<BuildingFloor> {
  public:
    bool _toDelete = false;

    list<shared_ptr<Wallsegment>> _walls;
    list<weak_ptr<WallCorner>> _corners;

    list<BuildingObject> _objects;

    Gll::PolyVao _vao;

    float _startHeight;
    float _height;

    string _name;

    shared_ptr<Building> _building;

    fVec2 Render2D_offset;
    
    //unit/px
    float Render2D_scale = 1;

    fVec2 Render2D_mousePos;

    shared_ptr<WallCorner> dragStart;
    
    //Event
    int renderManager(int ax, int ay, int bx, int by, set<key_location>& down);
    int resizeManager(int x, int y);
    int mouseEntryManager(int state);
    int mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down, bool in);
    int guiEventManager(gui_event& evt, int mx, int my, set<key_location>& down, bool in);

    //Activation
    void onAdded();
    void onActivated();
    void onDeactivated();

    //Unique functions
    shared_ptr<WallCorner> createWallCorner(fVec2 at);
    shared_ptr<VirtualCorner> createVirtualCorner(fVec2 at);

    shared_ptr<Wallsegment> addWall(shared_ptr<WallCorner> a, shared_ptr<WallCorner> b, float width);
    void deleteWall(shared_ptr<Wallsegment> wall);

    void upload();
    void render();
    
    float Render2D_gridSize();
    fVec2 Render2D_toScreenCoordinates(fVec2 real);
    fVec2 Render2D_toRealCoordinates(fVec2 screen);

    shared_ptr<WallCorner> Render2D_snapToScreenCoords(fVec2 screen, float& dist, fVec2& closestPoint, bool addIfNew = true, bool noSnap = false);
  };

  class Building : public std::enable_shared_from_this<Building> {
  public:
    map<string, BuildingFloor> _floors;
    PluginBuilding* _plugin;

    shared_ptr<BuildingFloor> createFloor(string name, float low, float height);
  };
}

class PluginBuilding : public EditorPlugin {
public:
  static Shader _checkShader_2D;
  static GLuint _checkShader_2D_scale;
  static GLuint _checkShader_2D_offset;

  shared_ptr<Building::Building> _building;
  shared_ptr<Building::BuildingFloor> _selectedFloor;

  enum WallMode {
    WallModeUnd,
    WallModeAdd,
    WallModeMod,
    WallModeDel,
  };
  WallMode _mode;

  PluginBuilding(Editor* e) : EditorPlugin(e) {
  }

  static void staticInit() {
    static bool first = true;
    if (first) {
      first = false;
      _checkShader_2D = make_shared<Shader_Raw>("Editor/Checkboard_2D");
      _checkShader_2D_scale = glGetUniformLocation(_checkShader_2D->_pID, "scale");
      _checkShader_2D_offset = glGetUniformLocation(_checkShader_2D->_pID, "offset");
    }
  }

  int renderManager(int ax, int ay, int bx, int by, set<key_location>& down);
  int resizeManager(int x, int y);
  int mouseEntryManager(int state);
  int mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down, bool in);
  int guiEventManager(gui_event& evt, int mx, int my, set<key_location>& down, bool in);

  string getClassName() {
    return "PluginBuilding";
  }

  void onAdded();
  void onActivated();
  void onDeactivated();

  void setMode(WallMode mode);
};

EditorPlugin* createPluginBuilding(Editor* e, bool staticInit);