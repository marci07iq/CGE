#pragma once

#include "../Defs.h"

/*typedef vec3<Fraction> CSGVector;
typedef Fraction CSGScalar;

struct CSGVertex;
struct CSGPlane;
struct CSGPoly;
struct CSGNode;

struct CSGVertex {
  CSGVector _pos;
  CSGVector _norm;

  CSGVertex();
  CSGVertex(CSGVector pos, CSGVector norm);

  void flip();
  CSGVertex interpolate(const CSGVertex& other, CSGScalar t);
};

struct CSGPlane {
  CSGVector _norm;
  CSGScalar _w;
  CSGPlane(CSGVector norm, CSGScalar w);
  static CSGPlane fromPoints(CSGVector a, CSGVector b, CSGVector c);

  void flip();

  void splitPolygon(CSGPoly &polygon, list<CSGPoly> &coplanarFront, list<CSGPoly> &coplanarBack, list<CSGPoly> &front, list<CSGPoly> &back);
};

struct CSGPoly {
  vector<CSGVertex> _vertices;
  CSGPlane _plane;
  void* _shared;
  CSGPoly();
  CSGPoly(vector<CSGVertex> vertices, void* shared = NULL);
  CSGPoly(list<CSGVertex> vertices, void* shared = NULL);

  void flip();

  //bool intersectRay(fVec3 from, fVec3 dir, float& at);
};

struct CSGNode {
  shared_ptr<CSGPlane> _plane;
  shared_ptr<CSGNode> _front;
  shared_ptr<CSGNode> _back;
  list<shared_ptr<CSGPoly>> _polygons;

  void build(list<shared_ptr<CSGPoly>> polygons);

  CSGNode();
  CSGNode(list<shared_ptr<CSGPoly>> polygons);

  void invert();
  list<shared_ptr<CSGPoly>> clipPolygons(list<shared_ptr<CSGPoly>>& polygons);
  void clipTo(shared_ptr<CSGNode> bsp);
  list<shared_ptr<CSGPoly>> allPolygons();
};*/

/*namespace CSG {
  typedef float Scalar;
  typedef fVec3 Vertex;
  struct Edge;
  struct Tri;
  struct Poly;
  struct Object;

  //Scalar EPSILON = 1e-3;

  /*class MergeVertex : public Vertex {
    enum Type {
      None,
      Edge,
      In
    };
    Type _in[2];
  };*

  struct Edge {
    //Endpoints
    Vertex* _vertexs[2];

    //Two side triangles
    Tri* _tris[2];

    //Vertex id in side triangles on the far side
    int _vertex_ids[2];

    int vertexEnd(Vertex* v) {
      if (v == _vertexs[0]) return 0;
      if (v == _vertexs[1]) return 1;
      return -1;
    }

    int direction(Vertex* v1, Vertex* v2) {
      if (v1 == _vertexs[0] || v2 == _vertexs[1]) return 0;
      if (v1 == _vertexs[1] || v2 == _vertexs[0]) return 1;
      return -1;
    }

    int triSide(Tri* t) {
      if (t == _tris[0]) return 0;
      if (t == _tris[1]) return 1;
      return -1;
    }

    void linkTri(Tri* t, int from) {
      if (_tris[0] == NULL) {
        _tris[0] = t;
        _vertex_ids[0] = from;
        return;
      }
      if (_tris[1] == NULL) {
        _tris[1] = t;
        _vertex_ids[1] = from;
        return;
      }
      throw 1;
    }

    void unlinkTri(Tri* t) {
      int triside = triSide(t);
      t->_edges[_vertex_ids[triside]] = NULL;
      _vertex_ids[triside] = -1;
      _tris[triside] = NULL;
    }
  };
  struct Tri {
    Edge* _edges[3]; //v2-v3-v1
    Vertex* _vertexs[3]; //v1-v2-v3
    colorargb _color;

    bool isIn(fVec3 loc) {
      fVec3 n = crs((*_vertexs[1]) - (*_vertexs[0]), (*_vertexs[2]) - (*_vertexs[0]));
      return (
        dot(crs((*_vertexs[1]) - (*_vertexs[0]), loc - (*_vertexs[0])), n) > 0 &&
        dot(crs((*_vertexs[2]) - (*_vertexs[1]), loc - (*_vertexs[1])), n) > 0 &&
        dot(crs((*_vertexs[0]) - (*_vertexs[2]), loc - (*_vertexs[2])), n) > 0
        );
    }
    double intersect(fVec3 rayOri, fVec3 rayDir) {
      fVec3 n = crs((*_vertexs[1]) - (*_vertexs[0]), (*_vertexs[2]) - (*_vertexs[0]));
      return dot((*_vertexs[0]) - rayOri, n) / dot(rayDir, n);
    }

    int edgeBetween(Vertex* a, Vertex* b) {
      if (_edges[0]->direction(a, b) != -1) return 0;
      if (_edges[1]->direction(a, b) != -1) return 1;
      if (_edges[2]->direction(a, b) != -1) return 2;
      return -1;
    }

    int edgeOpposite(Vertex* a) {
      if (_vertexs[0] == a) return 0;
      if (_vertexs[1] == a) return 1;
      if (_vertexs[2] == a) return 2;
      return -1;
    }

    bool intersectRay(fVec3 from, fVec3 dir, float & at) {
      bool isIn = true;
      float loc = intersect(from, dir);
      fVec3 pt = from + dir * loc;
      for (int i = 0; i < 3 && isIn; i++) {
        if (dot(
            crs(
              *_vertexs[(i + 1) % 3] - *_vertexs[i],
              pt - *_vertexs[i]
            ),
          crs(*_vertexs[0] - *_vertexs[1], *_vertexs[0] - *_vertexs[2])
          ) < 0) {
          isIn = false;
        }
      }
      if (isIn) {
        at = min(at, loc);
      }
      return isIn;
    }

    void unlinkEdge(Edge* e) {
      int triside = e->triSide(this);
      _edges[e->_vertex_ids[triside]] = NULL;
      e->_vertex_ids[triside] = -1;
      e->_tris[triside] = NULL;
    }
  };

  class Object {
  public:
    list<Vertex*> _vertices;
    list<Edge*> _edges;
    list<Tri*> _tris;
    //Add/Find elements
    /*Edge* addEdge(Vertex* a, Vertex* b) {
      Edge* what = NULL;
      if (a == b) {
        throw 1;
      }
      for (auto&& it : _edges) {
        if (
          (it->_vertexa == a && it->_vertexb == b) ||
          (it->_vertexa == b && it->_vertexb == a)) {
          what = it;
          break;
        }
      }
      if (!what) {
        what = new Edge();
        what->_vertexa = a;
        what->_vertexb = b;
        _edges.push_back(what);
      }
      return what;
    }

    Tri* addTri(Vertex* pt_a, Vertex* pt_b, Vertex* pt_c, void * data) {
      if (pt_a == pt_b || pt_b == pt_c || pt_c == pt_a) {
        return new Tri();
      }
      Tri* res = new Tri();
      res->_vertexs[0] = pt_a;
      res->_vertexs[1] = pt_b;
      res->_vertexs[2] = pt_c;

      res->_edges[2] = addEdge(pt_a, pt_b);
      addToEdge(res->_edges[2], 2, res);
      res->_edges[0] = addEdge(pt_b, pt_c);
      addToEdge(res->_edges[0], 0, res);
      res->_edges[1] = addEdge(pt_c, pt_a);
      addToEdge(res->_edges[1], 1, res);

      res->_color = reinterpret_cast<colorargb>(data);

      _tris.push_back(res);

      return res;
    }*/

    /*Edge* createEdge(Vertex* a, Vertex* b) {
      Edge* e = new Edge();
      e->_vertexs[0] = a;
      e->_vertexs[1] = b;
      _edges.push_back(e);
      return e;
    }

    Tri* createTri(Vertex* a, Vertex* b, Vertex* c, Edge* bc, Edge* ca, Edge* ab) {
      Tri* t = new Tri();
      t->_vertexs[0] = a;
      t->_vertexs[1] = b;
      t->_vertexs[2] = c;

      t->_edges[0] = bc;
      t->_edges[1] = ca;
      t->_edges[2] = ab;

      bc->linkTri(t, 0);
      ca->linkTri(t, 1);
      ab->linkTri(t, 2);

      _tris.push_back(t);
      return t;
    }
    
    void splitEdge(Vertex* v, Edge* e) {
      Edge* va = createEdge(v, e->_vertexs[0]);
      Edge* vb = createEdge(v, e->_vertexs[1]);
      
      if (e->_tris[0] != NULL) {
        Tri* tri = e->_tris[0];
        Vertex* c = tri->_vertexs[e->_vertex_ids[0]];

        Edge* vc = createEdge(v, c);
        Edge* ac = tri->_edges[tri->edgeOpposite(e->_vertexs[1])];
        Edge* bc = tri->_edges[tri->edgeOpposite(e->_vertexs[0])];

        //Unlink existing edges
        ac->_tris[ac->triSide(tri)] = NULL;
        bc->_tris[ac->triSide(tri)] = NULL;

        //Link to new triangles
        createTri(e->_vertexs[0], c, v, vc, va, ac);
        createTri(e->_vertexs[1], c, v, vc, vb, bc);

        _tris.remove(tri);
        delete tri;
      }

      if (e->_tris[1] != NULL) {
        Tri* tri = e->_tris[1];
        Vertex* c = tri->_vertexs[e->_vertex_ids[1]];

        Edge* vc = createEdge(v, c);
        Edge* ac = tri->_edges[tri->edgeOpposite(e->_vertexs[1])];
        Edge* bc = tri->_edges[tri->edgeOpposite(e->_vertexs[0])];

        //Unlink existing edges
        ac->_tris[ac->triSide(tri)] = NULL;
        bc->_tris[ac->triSide(tri)] = NULL;

        createTri(e->_vertexs[0], c, v, vc, va, ac);
        createTri(e->_vertexs[1], c, v, vc, vb, bc);

        _tris.remove(tri);
        delete tri;
      }

      _edges.remove(e);
      delete e;
    }

    void removeVertex(Vertex* v) {
      //Find one edge
      Edge* e = NULL;
      for (auto&& it : _edges) {
        if (it->vertexEnd(v) != -1) {
          e = it;
          break;
        }
      }

      if (e == NULL) {
        throw 1; //Not found
      }

      /*struct TriLink {
        Edge* e;
        int tri_id; //Side of e with triangle
        int next_id; //Side of triangle with next edge
        int next_me; //Side of next edge facing this
        int out_id; //Side of triangle with third edge

        TriLink jumpNext(Vertex* around) {
          Tri* t = (tri_id == 0) ? e->_tric : e->_trid;
          
          int i;
          for(i = 0; i < 3; i++) {
            if (t->_edges[i] != e) { //If it is not the edge in, but has the vertex
              if(
                t->_edges[i]->_vertexa == around ||
                t->_edges[i]->_vertexb == around
              ) {
                next_id = i;
              } else {
                out_id = i;
              }
            }
          }

          out_id = 3- next_id - out_id;

          next_me = (t->_edges[next_id]->_tric == t) ? 0 : 1;

          return TriLink{ t->_edges[next_id] , 1- next_me, -1, -1, -1};
        }
      };*//*

      /*list<TriLink> loop;
      TriLink next = TriLink{e, 0};
      
      do { //Insert this and check next till you loop around.
        loop.push_back(next);
        next = next.jumpNext(v);
      } while(next.e != e);*//*

      list<Edge*> eloop; //edge
      list<Vertex*> vloop;
      list<Tri*> tloop;

      list<Edge*> internals;

      Vertex* start = e->_vertexs[e->vertexEnd(v)];
      Vertex* next = start;
      Tri* tri = e->_tris[0];

      Edge* nextIntEdge = e;
      do {
        Edge* outerEdge = tri->_edges[tri->edgeOpposite(v)];
        eloop.push_back(outerEdge);
        vloop.push_back(next);
        tloop.push_back(tri);
        internals.push_back(nextIntEdge);

        //Move nextIntEdge
        nextIntEdge = tri->_edges[tri->edgeOpposite(next)];

        //Move next
        next = outerEdge->_vertexs[1-outerEdge->vertexEnd(next)];

        //Assert that the new next and nextInternalEdge are linked
        assert(nextIntEdge->direction(v, next) != -1);

        //Unlink outerEdge from tri
        int outerEdgeInSide = outerEdge->triSide(tri);
        outerEdge->_tris[outerEdgeInSide] = NULL;
        outerEdge->_vertex_ids[outerEdgeInSide] = -1;

        //Move tri
        tri = nextIntEdge->_tris[1-nextIntEdge->triSide(tri)];

      } while (next != start);

      assert(vloop.size() > 2); //If it was less than that, what was this vertex connected to...

      //Outer edges are all that stay in the system, and have been unlinked. Wipe intenals
      while (internals.size()) {
        _edges.remove(internals.back());
        delete internals.back();
        internals.pop_back();
      }
      while (tloop.size()) {
        _tris.remove(tloop.back());
        delete tloop.back();
        tloop.pop_back();
      }
      _vertices.remove(v);
      delete v;

      list<Edge*> fanEdges;
      fanEdges.push_back(eloop.front());
      eloop.pop_front();

      //Headvertex: the base for intenral fans.
      Vertex* headVertex = vloop.front();
      //Remove head
      vloop.pop_front();
      
      auto vit = vloop.begin();
      ++vit; //#1 is directly connected
      auto vit_end = vit;
      ++vit_end; //Prescan for end, dont include that.
      while (vit_end != vloop.end()) {
        fanEdges.push_back(createEdge(v, *vit));
        ++vit;
        ++vit_end;
      }

      fanEdges.push_back(eloop.back());
      eloop.pop_back();

      //fanEdges are now the edges connected to head
      //eloop is all the rest. Both in order.

      Edge* last = fanEdges.front();
      fanEdges.pop_front();

      assert(eloop.size() == fanEdges.size());

      auto loopit = eloop.begin();
      auto fanit = fanEdges.begin();
      auto vertit2 = vloop.begin();
      auto vertit1 = vertit2++;

      while (loopit != eloop.end()) {
        Tri* newTri = createTri(headVertex, *vertit1, *vertit2, *loopit, *fanit, last);

        last = *fanit;

        ++loopit;
        ++fanit;
        ++vertit1;
        ++vertit2;
      }
    }
    
    void validate() {
        map<Edge*, int> e_cnts;
        map<Vertex*, int> v_cnts;
        for (auto&& it : _tris) {
          e_cnts[it->_edges[0]]++;
          e_cnts[it->_edges[1]]++;
          e_cnts[it->_edges[2]]++;

          v_cnts[it->_vertexs[0]]++;
          v_cnts[it->_vertexs[1]]++;
          v_cnts[it->_vertexs[2]]++;
        }

        for (auto&& it : _edges) {
          if (!e_cnts[it]) {
            cout << "Rogue edge..." << endl;
          }
        }

        for (auto&& it : _vertices) {
          if (!v_cnts[it]) {
            cout << "Rogue vertex..." << endl;
          }
        }
      }//*

    bool intersectRay(fVec3 from, fVec3 dir, float & at) {
      bool intersectAny = false;
      for (auto&& it : _tris) {
        intersectAny |= it->intersectRay(from, dir, at);
      }
      return intersectAny;
    }
  };

  class MergeObject {
    list<Vertex*> _vertices;
    list<Edge*> _edges;
    list<Tri*> _tris;

    void addObject(Object* o, int type) {
      
    }
  };
}

namespace CSG_2 {
  typedef iVec3 Vertex;

  class Edge;

  class Tri {
    Vertex* _vertexes[3];
    Edge* _edges[3];
    Tri* _tris[3];
  };

  class Edge {
    Vertex* _vertexes[2];
  };
}

class OctTree {
  enum Fill {
    Empty,
    Mixed,
    Solid
  };
  Fill fill;
  OctTree* _parts[2][2][2];

  OctTree(Fill f) {
    fill = f;
  }

  inline bool check(int id, int min, int max, int half) {
    return (id==0) ? min < half : half <= max;
  }

  inline int clipMin(int id, int min, int half) {
    return (id==0) ? min : std::max(half, min);
  }
  inline int clipMax(int id, int max, int half) {
    return (id == 0) ? std::min(max, half-1) : max;
  }

  bool changeRegion(iVec3 min, iVec3 max, int depth, Fill to) {
    int mySize = 1 << depth;

    if (
    bor(min == iVec3{ 0,0,0 }) &&
    bor(max == iVec3{ mySize - 1, mySize - 1,mySize - 1 })) { //Full
      //Delete subdivisions
      for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
          for (int k = 0; k < 2; k++) {
            if (_parts[i][j][k]) delete _parts[i][j][k];
          }
        }
      }

      //Set me
      fill = to;
    } else {
      int half = mySize / 2;

      for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
          for (int k = 0; k < 2; k++) {
            if (!_parts[i][j][k]) _parts[i][j][k] = new OctTree(fill);
            if (
              check(i, min.x, max.x, half) &&
              check(j, min.y, max.y, half) &&
              check(k, min.z, max.z, half)) {
              changeRegion({
                clipMin(i, min.x, half),
                clipMin(j, min.y, half),
                clipMin(k, min.z, half)}, {
                clipMax(i, max.x, half),
                clipMax(j, max.y, half),
                clipMax(k, max.z, half) },
                depth - 1,
                to);
            }
          }
        }
      }
      fill = Mixed;

    }
  }

  bool getValue(iVec3 at, int depth) {
    if (fill != Mixed) {
      return (fill == Solid) ? true : false;
    } else {
      return getValue(at % (1 << (depth - 1)), depth - 1);
    }
  }
};*/

struct Mesh {
  Eigen::MatrixXd _V;
  Eigen::MatrixXi _F;

  int faces() {
    return _F.rows();
  }

  fVec3 vertex(int face, int side) {
    int vertId = _F(face, side);
    return {_V(vertId, 0), _V(vertId, 1), _V(vertId, 2)};
  }

  bool intersectRay(fVec3 from, fVec3 dir, float & at) {
    bool intersectAny = false;
    for (int i = 0; i < faces(); i++) {
      fVec3 va = vertex(i, 0);
      fVec3 vb = vertex(i, 1);
      fVec3 vc = vertex(i, 2);

      fVec3 n = crs(vb - va, vc - va);
      float inter = dot(va - from, n) / dot(dir, n);

      fVec3 pos = from + dir * inter;

      if (
        dot(crs(vb - va, pos - va), n) > 0 &&
        dot(crs(vc - vb, pos - vb), n) > 0 &&
        dot(crs(va - vc, pos - vc), n) > 0) {
        intersectAny = true;
        at = min(at, inter);
      }
         
    }
    return intersectAny;
  }
};

extern Graphics::WinHwnd objectMainWindowHwnd;
extern Graphics::CanvasHwnd objectMainCanvasHwnd;

extern Graphics::WinHwnd objectSettingsWindowHwnd;
