#ifndef _INC_CAMERA_H
#define _INC_CAMERA_H

#include "Vector.hpp"
#include "Quaternion.hpp"

class Camera {
public:
  Camera();
  Camera(Vector eye, Vector center, Vector up, double fov);
  ~Camera();

  Vector eye;
  Vector center;
  Vector up;
  double fov;

  void rotate(Vector axis, double angle);
};

#endif
