#ifndef _INC_QUATERNION_H
#define _INC_QUATERNION_H

#include <cmath>

class Quaternion {
public:
  Quaternion();
  Quaternion(double x, double y, double z, double w);
  ~Quaternion();

  double x;
  double y;
  double z;
  double w;

  double length();
  Quaternion normalize();
  Quaternion conjugate();
  Quaternion mult(Quaternion other);
};

#endif
