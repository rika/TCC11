#ifndef _INC_EDGE_H
#define _INC_EDGE_H

#include "Point.hpp"
#include "Vector.hpp"

#include <cmath>
#include <cstdio>

using namespace std;

class Edge {
public:
  Edge(double x1, double y1, double x2, double y2);
  Edge(double x1, double y1, double x2, double y2, bool blend);
  Edge(Point a, Point b);
  Edge(Point a, Point b, bool blend);
  ~Edge();
  
  Point a, b;
  bool blend;
  
  Edge offset(double dist);
  Point intersection(Edge other);
};

#endif
