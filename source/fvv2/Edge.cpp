#include "Edge.hpp"

Edge::Edge(double x1, double y1, double x2, double y2) {
  this->a = Point(x1, y1);
  this->b = Point(x2, y2);
  this->blend = false;
}

Edge::Edge(double x1, double y1, double x2, double y2, bool blend) {
  this->a = Point(x1, y1);
  this->b = Point(x2, y2);
  this->blend = blend;
}

Edge::Edge(Point a, Point b) {
  this->a = a;
  this->b = b;
  this->blend = false;
}

Edge::Edge(Point a, Point b, bool blend) {
  this->a = a;
  this->b = b;
  this->blend = blend;
}

Edge::~Edge() {
  
}

Edge Edge::offset(double dist) {
  Vector lineDirection = Vector(b.x - a.x, b.y - a.y, 0.0);
  Vector lineNormal = Vector(-lineDirection.y, lineDirection.x, 0.0);
  
  lineNormal = lineNormal.normalize();
  
  Vector pa = Vector(a.x, a.y, 0.0);
  Vector pb = Vector(b.x, b.y, 0.0);
  
  pa = pa.translate(lineNormal.mult(dist));
  pb = pb.translate(lineNormal.mult(dist));
  
  Edge e = Edge(pa.x, pa.y, pb.x, pb.y);
  
  return e;
}

Point Edge::intersection(Edge other) {
  double a1, b1, c1;
  double a2, b2, c2;

  Vector l1 = Vector(this->a.y - this->b.y,
		    this->b.x - this->a.x,
		    0.0);
  l1.z = - l1.x * this->a.x - l1.y * this->a.y;

  Vector l2 = Vector(other.a.y - other.b.y,
		    other.b.x - other.a.x,
		    0.0);
  l2.z = - l2.x * other.a.x - l2.y * other.a.y;

  Vector l3 = l1.cross(l2);
  
  Point i = Point(l3.x / l3.z, l3.y / l3.z);
  
  return i;
}
