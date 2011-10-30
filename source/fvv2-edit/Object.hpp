#ifndef _INC_OBJECT_HPP
#define _INC_OBJECT_HPP

#include "Vector.hpp"

namespace FVV {
  class Object {
  public:
    Object();
    Object(int subject, double px, double py, double visibleArea);
    Object(int subject, double h, int cx, int cy);
    ~Object();
    
    int subject;
    Vector position;
    double height;
    double visibleArea;
  };
}

#endif
