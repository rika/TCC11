#ifndef _INC_OBJECT_HPP
#define _INC_OBJECT_HPP

#include "Vector.hpp"

class Object {
public:
  Object(double h, int cx, int cy);
  ~Object();

  Vector position;
  double height;
};

#endif
