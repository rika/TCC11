#ifndef _INC_BOUNDARY_H
#define _INC_BOUNDARY_H

#include "Edge.hpp"
#include "Point.hpp"

#include <GL/gl.h>
#include <GL/glut.h>

#include <vector>

using namespace std;

class Boundary {
public:
  Boundary();
  ~Boundary();

  void begin();
  void end();
  void insert(Point p);
  void calculate();
  void render(bool invert);

  vector<Edge> outerEdges;
  vector<Edge> innerEdges;

  bool hasLastPoint;
  Point lastPoint;
};

#endif
