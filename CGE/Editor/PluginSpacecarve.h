#pragma once

#include "Framework.h"

class PluginSpacecarve : public EditorPlugin {
public:
    

  PluginSpacecarve(Editor* e) : EditorPlugin(e) {
    //_ribbonElement = Graphics::createPanel("elementToolRibbonTemplate", LocationData(LinearScale(0,0), LinearScale(1, 0), LinearScale(0, 0), LinearScale(0, 10)), 0xffff0000);
  }

  static void staticInit() {

  }

  int renderManager(int ax, int ay, int bx, int by, set<key_location>& down);
  int renderSelection(int ax, int ay, int bx, int by, set<key_location>& down);
  int resizeManager(int x, int y);
  int mouseEntryManager(int state);
  int mouseMoveManager(int x, int y, int ox, int oy, set<key_location>& down, bool in);
  int guiEventManager(gui_event& evt, int mx, int my, set<key_location>& down, bool in);   

  string getClassName() {
    return "PluginSpacecarve";
  }

  void onAdded();
  void onActivated();
  void onDeactivated();

  void reset() {
      
  }
};

EditorPlugin* createPluginSelect(Editor* e, bool staticInit);

namespace OctTree {
  namespace Memory {
    typedef iVec2 address;
    //Blocks are 16 x 1

    bool isValid(const address& a) {
      return a.y > 0;
    }

    address colorAddress(colorargb col) {
      return { ((col >> 24) & 0xc0) | ((col >> 18) & 0x30) | ((col >> 12) & 0x0c) | ((col >> 6) & 0x03), 0 };
    }

    class Memory;

    class Node {
      address _addr[8];
      friend class Memory;
    public:
      address _self;
      Memory* _mem;

      Node() {
        _self = NULL;
      }
      Node(address self) {
        _self = self;
      }

      address get(int i) const {
        return _addr[i];
      }
      void set(int i, const address& val) {
        _addr[i] = val;
        _mem->updateBlock(_self);
      }

      void set(const address val[8]) {
        for (int i = 0; i < 8; i++) {
          _addr[i] = val[i];
        }
        _mem->updateBlock(_self);
      }
    };

    class Memory {
      friend class Node;
      void updateBlock(address _add) {
        
        unsigned char data[16];
        for (int i = 0; i < 8; i++) {
          data[2 * i + 0] = _nodes[_add.x / 16][_add.y].get(i).x;
          data[2 * i + 1] = _nodes[_add.x / 16][_add.y].get(i).y;
        }

        glBindTexture(GL_TEXTURE_2D, _data._raw->id);

        glTexSubImage2D(GL_TEXTURE_2D, 0, _add.x, _add.y, 16, 1, GL_RED_INTEGER, GL_UNSIGNED_BYTE, data);
      }
      Texture _data;
    public:
      Node _nodes[16][256];

      Memory() {
        _data._raw = make_shared<RawTexture>();

        glGenTextures(1, &(_data._raw->id));

        glBindTexture(GL_TEXTURE_2D, _data._raw->id);

        unsigned char image[256 * 256];
        for (int i = 0; i < 256; i++) {
          image[i] = 0;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, 256, 256, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, image);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      }

      void reset() {
        address root_col = colorAddress(0xffffffff);
        address root_data[8] = { root_col,root_col,root_col,root_col,root_col,root_col,root_col,root_col };
        _nodes[0][1].set(root_data);
      }
    };
  }
  class Tree {
    Memory::Memory _memory;
    static const int levels = 10;
    void set(iVec3 loc, int level) {
      Memory::Node* current = &(_memory._nodes[0][1]);
      while (level > 0) {


        --level;
      }
    }
  };

};