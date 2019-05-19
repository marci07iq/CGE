#include "PluginBuilding.h"

NGin::Graphics::Shader PluginBuilding::_checkShader_2D;
GLuint PluginBuilding::_checkShader_2D_scale;
GLuint PluginBuilding::_checkShader_2D_offset;

void pluginBuildingWallAddIcon(NGin::Graphics::ElemHwnd elem) {
  ((PluginBuilding*)elem->data)->setMode(PluginBuilding::WallModeAdd);
}
void pluginBuildingWallModIcon(NGin::Graphics::ElemHwnd elem) {
  ((PluginBuilding*)elem->data)->setMode(PluginBuilding::WallModeMod);
}
void pluginBuildingWallDelIcon(NGin::Graphics::ElemHwnd elem) {
  ((PluginBuilding*)elem->data)->setMode(PluginBuilding::WallModeDel);
}

void pluginBuildingMainButton(NGin::Graphics::ElemHwnd elem) {
  //Editor* e = (Editor*)editor;
  ((Editor*)elem->data)->activateStaticPlugin(((Editor*)elem->data)->findStaticPlugin("PluginBuilding"));
}


int PluginBuilding::renderManager(int ax, int ay, int bx, int by, std::set<NGin::Graphics::key_location>& down) {
  if (_selectedFloor) {
    if (_selectedFloor->_toDelete) {
      _selectedFloor = NULL;
      return 2;
    }
    return _selectedFloor->renderManager(ax, ay, bx, by, down);
  }
  return 2;
}
int PluginBuilding::resizeManager(int x, int y) {
  if (_selectedFloor) {
    if (_selectedFloor->_toDelete) {
      _selectedFloor = NULL;
      return 0;
    }
    return _selectedFloor->resizeManager(x, y);
  }
  return 0;
}
int PluginBuilding::mouseEntryManager(int state) {
  if (_selectedFloor) {
    if (_selectedFloor->_toDelete) {
      _selectedFloor = NULL;
      return 0;
    }
    return _selectedFloor->mouseEntryManager(state);
  }
  return 0;
}
int PluginBuilding::mouseMoveManager(int x, int y, int ox, int oy, std::set<NGin::Graphics::key_location>& down, bool in) {
  if (_selectedFloor) {
    if (_selectedFloor->_toDelete) {
      _selectedFloor = NULL;
      return 0;
    }
    return _selectedFloor->mouseMoveManager(x, y, ox, oy, down, in);
  }
  return 0;
}
int PluginBuilding::guiEventManager(NGin::Graphics::gui_event& evt, int mx, int my, std::set<NGin::Graphics::key_location>& down, bool in) {
  if (_selectedFloor) {
    if (_selectedFloor->_toDelete) {
      _selectedFloor = NULL;
      return 0;
    }
    return _selectedFloor->guiEventManager(evt, mx, my, down, in);
  }
  return 0;
}

void PluginBuilding::onAdded() {
  //_config = NGin::Graphics::createGUI_T<Panel>("objectPluginColorConfigPanel", LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(1, 0), NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 200)), 0x00000000, NULL);
  
  _toolbarElement = NGin::Graphics::createGUI_T<NGin::Graphics::IconButton>("objectPluginBuildingMainButton", NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30)), NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"), (void*)_editor, "building", -1, pluginBuildingMainButton, "building", "html/icons.ilf");
  _editor->registerSidebar(_toolbarElement);

  _building = make_shared<Building::Building>();
  _building->_plugin = this;
  _selectedFloor = _building->createFloor("0", 0, 3);

  _ribbonElement = NGin::Graphics::createGUI_T<NGin::Graphics::Panel>("objectPluginColorIcons", NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 140)), 0x00000000, nullptr);
  NGin::Graphics::addElement(static_pointer_cast<NGin::Graphics::Panel, NGin::Graphics::GUIElement>(_ribbonElement), NGin::Graphics::createGUI_T<NGin::Graphics::IconButton>("objectPluginBuildingWallAddIcon", NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30)), NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"), (void*)this, "X", -1, pluginBuildingWallAddIcon, "wall_add", "html/icons.ilf"));
  NGin::Graphics::addElement(static_pointer_cast<NGin::Graphics::Panel, NGin::Graphics::GUIElement>(_ribbonElement), NGin::Graphics::createGUI_T<NGin::Graphics::IconButton>("objectPluginBuildingWallModIcon", NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 60)), NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"), (void*)this, "X", -1, pluginBuildingWallModIcon, "wall_mod", "html/icons.ilf"));
  NGin::Graphics::addElement(static_pointer_cast<NGin::Graphics::Panel, NGin::Graphics::GUIElement>(_ribbonElement), NGin::Graphics::createGUI_T<NGin::Graphics::IconButton>("objectPluginBuildingWallDelIcon", NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 60), NGin::Graphics::LinearScale(0, 90)), NGin::Graphics::getColor("NGin::Graphics::Button", "bgcolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "activecolor"), NGin::Graphics::getColor("NGin::Graphics::Button", "textcolor"), (void*)this, "X", -1, pluginBuildingWallDelIcon, "wall_del", "html/icons.ilf"));

  setMode(WallModeUnd);
}
void PluginBuilding::onActivated() {
  _editor->registerRibbon(_ribbonElement);

  showConfig();

}
void PluginBuilding::onDeactivated() {
  _editor->removeRibbon(_ribbonElement);

  hideConfig();
}

void PluginBuilding::setMode(WallMode mode) {
  _mode = mode;

  static_pointer_cast<NGin::Graphics::Button, NGin::Graphics::GUIElement>((_ribbonElement->getElementById("objectPluginBuildingWallAddIcon")))->stuck = _mode == WallModeAdd;
  static_pointer_cast<NGin::Graphics::Button, NGin::Graphics::GUIElement>((_ribbonElement->getElementById("objectPluginBuildingWallModIcon")))->stuck = _mode == WallModeMod;
  static_pointer_cast<NGin::Graphics::Button, NGin::Graphics::GUIElement>((_ribbonElement->getElementById("objectPluginBuildingWallDelIcon")))->stuck = _mode == WallModeDel;
}


EditorPlugin * createPluginBuilding(Editor * e, bool staticInit) {
  if (staticInit) {
    PluginBuilding::staticInit();
    return NULL;
  } else {
    return new PluginBuilding(e);
  }
}

namespace Building {
  fVec2 getPointBetween(fVec2 left, fVec2 right, float width_l, float width_r) {
    fVec2 w = fVec2(width_l, -width_r) / 2;
    left.norm();
    right.norm();
    fVec2 x = fVec2(left.x, right.x);
    fVec2 y = fVec2(left.y, right.y);
    return fVec2(crs<double>(w, x), crs<double>(w, y)) / crs<double>(left, right);
  }

  //WallCorner
  void WallCorner::addWall(shared_ptr<Wallsegment> wall) {

    int meSide = wall->cornerSide(this);
    fVec2 dir = ((*(wall->_pos[1 - meSide])) - (*(wall->_pos[meSide]))).norm();

    if (_walls.size() == 0) {
      assert(_virtualCorners.size() == 0);
      _walls.push_back(wall);
      shared_ptr<VirtualCorner> newVirt;
      
      //First one is always the right
      //Right
      newVirt = _floor->createVirtualCorner(*(wall->_pos[meSide]) + fVec2(-dir.y, dir.x) * wall->_width / 2);
      newVirt->_left = wall;
      _virtualCorners.push_back(newVirt);
      wall->_pos_virt[meSide][1] = newVirt;

      //Only in this case we need the left one as well
      //Left
      newVirt = _floor->createVirtualCorner(*(wall->_pos[meSide]) + fVec2(dir.y, -dir.x) * wall->_width / 2);
      newVirt->_right = wall;
      _virtualCorners.push_back(newVirt);
      wall->_pos_virt[meSide][0] = newVirt;
      return;
    }
    if (_walls.size() == 1) {
      assert(_virtualCorners.size() == 2);
      int otherMe = _walls.front()->cornerSide(this);
      fVec2 otherdir = ((*(_walls.front()->_pos[1 - otherMe])) - (*(_walls.front()->_pos[otherMe]))).norm();
      _walls.push_back(wall);

      fVec2 virt_Pos;

      //Left
      virt_Pos = getPointBetween(otherdir, dir, _walls.front()->_width, wall->_width) + *(wall->_pos[meSide]);
      _virtualCorners.front()->x = virt_Pos.x;
      _virtualCorners.front()->y = virt_Pos.y;
      _virtualCorners.front()->_right = wall;
      wall->_pos_virt[meSide][0] = _virtualCorners.front();

      //Right
      virt_Pos = getPointBetween(dir, otherdir, wall->_width, _walls.front()->_width) + * (wall->_pos[meSide]);
      _virtualCorners.back()->x = virt_Pos.x;
      _virtualCorners.back()->y = virt_Pos.y;
      _virtualCorners.front()->_left = wall;
      wall->_pos_virt[meSide][1] = _virtualCorners.back();

      return;
    }
    
    //it1: the one to the left of the new wall
    auto it1 = _walls.begin();

    auto vit1 = _virtualCorners.begin();

    while (it1 != _walls.end()) {
      //it3: the one to the right
      auto it3 = it1;
      ++it3;
      if (it3 == _walls.end()) it3 = _walls.begin();

      int it1_side = (*it1)->cornerSide(this);
      fVec2 it1_dir = ((*((*it1)->_pos[1 - it1_side])) - (*((*it1)->_pos[it1_side]))).norm();
      
      int it3_side = (*it3)->cornerSide(this);
      fVec2 it3_dir = ((*((*it3)->_pos[1 - it3_side])) - (*((*it3)->_pos[it3_side]))).norm();
      
      if (crs(it3_dir - it1_dir, dir - it1_dir) <= 0) { //it1, wall, it3 in clockwise order
        //it2: before where we will insert
        auto it2 = it1;
        it2++;

        //vit2: before where we will insert
        auto vit2 = vit1;
        vit2++;

        fVec2  leftPoint = getPointBetween(it1_dir, dir, (*it1)->_width, wall->_width) + *(wall->_pos[meSide]);
        fVec2 rightPoint = getPointBetween(dir, it3_dir, wall->_width, (*it3)->_width) + *(wall->_pos[meSide]);

        //Set the left corner
        (*vit1)->x = leftPoint.x;
        (*vit1)->y = leftPoint.y;
        wall->_pos_virt[meSide][0] = *vit1;
        (*vit1)->_right = wall;
      
        //New virtual corner to the right
        vit2 = _virtualCorners.insert(vit2, _floor->createVirtualCorner(rightPoint));
        wall->_pos_virt[meSide][1] = *vit2;
        (*it3)->_pos_virt[it3_side][0] = *vit2;
        (*vit2)->_left = wall;
        (*vit2)->_right = (*it3);

        _walls.insert(it2, wall);
        return;
      }

      ++vit1;
      ++it1;
    }
  }
  void WallCorner::modifyWall(shared_ptr<Wallsegment> wall) {
    throw 1;
    int meSide = wall->cornerSide(this);
    fVec2 dir = ((*(wall->_pos[1 - meSide])) - (*(wall->_pos[meSide]))).norm();
    
    if (_walls.size() == 0) {
      throw 1;
      return;
    }
    if (_walls.size() == 1) {
      assert(_virtualCorners.size() == 2);
      assert(_walls.front() == wall);

      fVec2 otherdir = ((*(_walls.front()->_pos[1 - meSide])) - (*(_walls.front()->_pos[meSide]))).norm();

      fVec2 virt_Pos;

      virt_Pos = getPointBetween(otherdir, dir, _walls.front()->_width, wall->_width);
      _virtualCorners.front()->x = virt_Pos.x;
      _virtualCorners.front()->y = virt_Pos.y;
      wall->_pos_virt[meSide][0] = _virtualCorners.front();

      virt_Pos = getPointBetween(dir, otherdir, wall->_width, _walls.front()->_width);
      _virtualCorners.back()->x = virt_Pos.x;
      _virtualCorners.back()->y = virt_Pos.y;
      wall->_pos_virt[meSide][1] = _virtualCorners.back();

      return;
    }

    assert(_walls.size() == _virtualCorners.size());

    auto it2 = _walls.begin();
    auto vit2 = _virtualCorners.begin();

    while (it2 != _walls.end()) {
      if ((*it2) == wall) {
        break;
      }
      ++it2;
      ++vit2;
    }
    if (it2 == _walls.end()) {
      //Wall does not exist.
      throw 1;
      return;
    }

    auto it1 = it2++;
    if (it2 == _walls.end()) {
      it2 = _walls.begin();
    }

    auto vit1 = vit2++;
    if (vit1 == _virtualCorners.end()) {
      vit1 = _virtualCorners.begin();
    }

    int   it1_side = (*it1)->cornerSide(this);
    fVec2 it1_dir = ((*((*it1)->_pos[1 - it1_side])) - (*((*it1)->_pos[it1_side]))).norm();
    int   it2_side = (*it2)->cornerSide(this);
    fVec2 it2_dir = ((*((*it2)->_pos[1 - it2_side])) - (*((*it2)->_pos[it2_side]))).norm();

    fVec2 leftPoint  = getPointBetween(it1_dir, dir, (*it1)->_width, wall->_width);
    fVec2 rightPoint = getPointBetween(dir, it2_dir, wall->_width, (*it2)->_width);
    (*vit1)->x = leftPoint.x;
    (*vit1)->y = leftPoint.y;
    
    auto vit3 = vit1;
    ++vit3;
    _virtualCorners.insert(vit3, _floor->createVirtualCorner(rightPoint));
    
    auto it3 = it1;
    ++it3;
    _walls.insert(it3, wall);
  }
  void WallCorner::deleteWall(shared_ptr<Wallsegment> wall) {
    throw 1;
  }

  WallCorner::WallCorner(double x, double y, BuildingFloor* floor) : AnchorPoint(x, y, floor) {
    
  }

  //VirtualCorner
  VirtualCorner::VirtualCorner(double x, double y, BuildingFloor* floor) : AnchorPoint(x, y, floor) {

  }

  Wallsegment::Wallsegment(shared_ptr<WallCorner> a, shared_ptr<WallCorner> b, float width, BuildingFloor* floor) {
    _pos[0] = a;
    _pos[1] = b;
    _width = width;
    _floor = floor;
  }

  int Wallsegment::cornerSide(WallCorner * what) {
    int meSide = -1;
    if (_pos[0].get() == what) {
      meSide = 0;
    }
    if (_pos[1].get() == what) {
      meSide = 1;
    }
    if (meSide == -1) {
      throw 1;
    }

    return meSide;
  }

  void Wallsegment::compile() {
    NGin::Gll::gllColor(0xff808080);

    NGin::Gll::gllVertex(*(_pos[0]));
    NGin::Gll::gllVertex(*(_pos_virt[0][0]));
    NGin::Gll::gllVertex(*(_pos_virt[1][1]));
    NGin::Gll::gllVertex(*(_pos[1]));

    NGin::Gll::gllVertex(*(_pos[1]));
    NGin::Gll::gllVertex(*(_pos_virt[1][0]));
    NGin::Gll::gllVertex(*(_pos_virt[0][1]));
    NGin::Gll::gllVertex(*(_pos[0]));
  }

  //WallSegment
  Wallsegment::~Wallsegment() {
    _pos[0]->deleteWall(shared_from_this());
    _pos[1]->deleteWall(shared_from_this());
  }

  //BuildingFloor
  shared_ptr<WallCorner> BuildingFloor::createWallCorner(fVec2 at) {
    shared_ptr<WallCorner> newPtr = make_shared<WallCorner>(at.x, at.y, this);
    _corners.push_back(newPtr);
    return newPtr;
  }
  shared_ptr<VirtualCorner> BuildingFloor::createVirtualCorner(fVec2 at) {
    shared_ptr<VirtualCorner> newPtr = make_shared<VirtualCorner>(at.x, at.y, this);

    return newPtr;
  }

  shared_ptr<Wallsegment> BuildingFloor::addWall(shared_ptr<WallCorner> a, shared_ptr<WallCorner> b, float width) {
    shared_ptr<Wallsegment> wall = make_shared<Wallsegment>(a, b, width, this);
    a->addWall(wall->shared_from_this());
    b->addWall(wall->shared_from_this());
    _walls.push_back(wall);
    upload();
    return wall;
  }

  void BuildingFloor::deleteWall(shared_ptr<Wallsegment> wall) {
    wall->_pos[0]->deleteWall(wall);
    wall->_pos[1]->deleteWall(wall);
    _walls.remove(wall);
    upload();
  }

  void BuildingFloor::upload() {
    NGin::Gll::gllBegin(NGin::Gll::GLL_QUADS);

    for (auto&& it : _walls) {
      it->compile();
    }

    _vao = NGin::Gll::gllBuild();
  }

  void BuildingFloor::render() {


    NGin::Gll::gllRender(_vao, Render2D_offset, fVec2(Render2D_scale));
  }

  float BuildingFloor::Render2D_gridSize() {
    float grid_10 = pow(10, floor(log10(Render2D_scale * 101) * 1) / 1);
    float max_gird = 101 * Render2D_scale;
    
    float grid_2 = grid_10 * 2;
    grid_2 = grid_2 < max_gird ? grid_2: 0;
    float grid_5 = grid_10 * 5;
    grid_5 = grid_5 < max_gird ? grid_5 : 0;

    return max(grid_2, max(grid_5, grid_10));

    //return pow(10, floor(log10(Render2D_scale * 201) * 1) / 1);
  }
  fVec2 BuildingFloor::Render2D_toScreenCoordinates(fVec2 real) {
    return (real - Render2D_offset) / Render2D_scale;
  }
  fVec2 BuildingFloor::Render2D_toRealCoordinates(fVec2 screen) {
    return screen * Render2D_scale + Render2D_offset;
  }

  shared_ptr<WallCorner> BuildingFloor::Render2D_snapToScreenCoords(fVec2 screen, float & dist, fVec2& closestPoint, bool addIfNew, bool noSnap) {
    dist = INFINITY;
    shared_ptr<WallCorner> closest;

    clean_weak_ptr_list(_corners);
    
    //Already existing
    for (auto&& it : _corners) {
      if (!(it.expired())) {
        shared_ptr<WallCorner> sit = it.lock();
        float newDist = (screen - Render2D_toScreenCoordinates(*sit)).length();
        if (newDist < dist) {
          dist = newDist;
          closest = sit;
          closestPoint = *closest;
        }
      }
    }

    //Grid snap
    fVec2 realPos = Render2D_toRealCoordinates(screen);
    fVec2 closestGridPoint = realPos - (realPos + fVec2(Render2D_gridSize()/ 2)) % fVec2(Render2D_gridSize()) + fVec2(Render2D_gridSize() / 2);
    float screenDist = (screen - Render2D_toScreenCoordinates(closestGridPoint)).length();

    if (screenDist < dist * 0.9) {
      dist = screenDist;
      closestPoint = closestGridPoint;
      closest = NULL;
    }

    //Dont snap anywhere
    screenDist = (screen - Render2D_toScreenCoordinates(closestPoint)).length();
    if (screenDist > 20 || noSnap) {
      dist = 0;
      closestPoint = realPos;
      closest = NULL;
    }

    //Create shared_ptr
    if (addIfNew && closest == NULL) {
      closest = createWallCorner(closestPoint);
    }

    return closest;
  }

  shared_ptr<BuildingFloor> Building::createFloor(string name, float low, float height) {
    shared_ptr<BuildingFloor> res = make_shared<BuildingFloor>();
    res->_building = shared_from_this();
    res->_height = height;
    res->_startHeight = low;
    res->_name = name;

    res->onAdded();

    return res;
  }

  int BuildingFloor::renderManager(int ax, int ay, int bx, int by, std::set<NGin::Graphics::key_location>& down) {
    NGin::Graphics::resetViewport();
    
    float scale = Render2D_gridSize();

    //Draw squares
    GLuint quadVbo_pos;
    GLuint quadVbo_uv;
    GLuint quadVao;

    fVec2 scrMin = Render2D_toRealCoordinates(fVec2(ax, ay));
    fVec2 scrMax = Render2D_toRealCoordinates(fVec2(bx, by));

    float base[12] = {
      scrMin.x, scrMin.y,
      scrMin.x, scrMax.y,
      scrMax.x, scrMax.y,
      scrMax.x, scrMax.y,
      scrMax.x, scrMin.y,
      scrMin.x, scrMin.y
    };

    fVec2 uvMin = Render2D_toRealCoordinates(fVec2(ax, ay)) / scale;
    fVec2 uvMax = Render2D_toRealCoordinates(fVec2(bx, by)) / scale;

    float uv[12] = {
      uvMin.x, uvMin.y,
      uvMin.x, uvMax.y,
      uvMax.x, uvMax.y,
      uvMax.x, uvMax.y,
      uvMax.x, uvMin.y,
      uvMin.x, uvMin.y
    };

    glGenBuffers(1, &quadVbo_pos);
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo_pos);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), base, GL_STATIC_DRAW);

    glGenBuffers(1, &quadVbo_uv);
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo_uv);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), uv, GL_STATIC_DRAW);

    glGenVertexArrays(1, &quadVao);
    glBindVertexArray(quadVao);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo_pos);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, quadVbo_uv);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);


    PluginBuilding::_checkShader_2D->bind();

    glUniform2f(PluginBuilding::_checkShader_2D_scale,
      NGin::Graphics::current->width / 2.0 * Render2D_scale,
      NGin::Graphics::current->height / 2.0 * Render2D_scale);

    glUniform2f(PluginBuilding::_checkShader_2D_offset,
      Render2D_offset.x,
      Render2D_offset.y);
    
    glBindVertexArray(quadVao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    PluginBuilding::_checkShader_2D->unbind();

    glDeleteBuffers(1, &quadVbo_pos);
    glDeleteBuffers(1, &quadVbo_uv);
    glDeleteVertexArrays(1, &quadVao);

    render();
    
    fVec2 snapPoint;
    float dist;

    Render2D_snapToScreenCoords(Render2D_mousePos, dist, snapPoint, false, down.count(NGin::Graphics::key(GLFW_KEY_LEFT_SHIFT, NGin::Graphics::key::type_key)));
    snapPoint = Render2D_toScreenCoordinates(snapPoint);
    //cout << snapPoint.x << " " << snapPoint.y << endl;
    
    NGin::Gll::gllBegin(NGin::Gll::GLL_QUADS);
    NGin::Gll::gllColor(0xffff0000);
    NGin::Gll::gllVertex(snapPoint + fVec2(-5, -5));
    NGin::Gll::gllVertex(snapPoint + fVec2(5, -5));
    NGin::Gll::gllVertex(snapPoint + fVec2(5, 5));
    NGin::Gll::gllVertex(snapPoint + fVec2(-5, 5));
    NGin::Gll::gllEnd();



    return 2;
  }
  int BuildingFloor::resizeManager(int x, int y) {
    return 0;
  }
  int BuildingFloor::mouseEntryManager(int state) {
    return 0;
  }
  int BuildingFloor::mouseMoveManager(int x, int y, int ox, int oy, std::set<NGin::Graphics::key_location>& down, bool in) {
    Render2D_mousePos = fVec2(x, y);
    
    if (down.count(NGin::Graphics::key(GLFW_MOUSE_BUTTON_RIGHT, NGin::Graphics::key::type_mouse))) {
      Render2D_offset -= fVec2(x - ox, y - oy) * Render2D_scale;
    }
    //cout << x << " " << y << endl;
    //cout << Render2D_mousePos << endl;
    return 3;
  }
  int BuildingFloor::guiEventManager(NGin::Graphics::gui_event& evt, int mx, int my, std::set<NGin::Graphics::key_location>& down, bool in) {
    if (in && evt._key._type == NGin::Graphics::key::type_wheel) {
      float newScale = pow(10, round((log10(Render2D_scale) - 0.1 * evt._key._keycode) * 10) / 10);
      Render2D_offset += fVec2(mx, my) * (Render2D_scale - newScale);
      Render2D_scale = newScale;
    }

    if (!evt.captured && evt._key._type == NGin::Graphics::key::type_mouse && evt._key._keycode == GLFW_MOUSE_BUTTON_LEFT) { //mouse, down, left
      fVec2 snapPoint;
      float dist;
      if (evt._type == NGin::Graphics::gui_event::evt_down && in) {
        dragStart = Render2D_snapToScreenCoords(Render2D_mousePos, dist, snapPoint, true, down.count(NGin::Graphics::key(GLFW_KEY_LEFT_SHIFT, NGin::Graphics::key::type_key)));
        evt.captured = true;
      }
      if (evt._type == NGin::Graphics::gui_event::evt_up) {
        if (in) {
          shared_ptr<WallCorner> dragEnd = Render2D_snapToScreenCoords(Render2D_mousePos, dist, snapPoint, true, down.count(NGin::Graphics::key(GLFW_KEY_LEFT_SHIFT, NGin::Graphics::key::type_key)));
          if (dragStart != NULL && dragEnd != NULL && dragStart != dragEnd) {
            addWall(dragStart, dragEnd, Render2D_scale * 5);
            dragStart = NULL;
            dragEnd = NULL;
          }
        } else {
          dragStart = NULL;
        }
      }
    }
    return 1;
  }

  void BuildingFloor::onAdded() {
    upload();
  }

  void BuildingFloor::onActivated() {
  }

  void BuildingFloor::onDeactivated() {
  }

}