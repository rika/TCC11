#ifndef _INC_VECTOR_H
#define _INC_VECTOR_H

#include <cmath>

class Vector {
public:
  Vector();
  Vector(double x, double y, double z);
  ~Vector();

  double x;
  double y;
  double z;

  double length();
  Vector normalize();
  Vector translate(Vector t);
  double dot(Vector other);
  Vector cross(Vector other);
  Vector mult(double s);
};

#endif
