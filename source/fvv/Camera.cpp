#include "Camera.hpp"

Camera::Camera() {

}

Camera::Camera(Vector eye, Vector center, Vector up, double fov) {
  this->eye = eye;
  this->center = center;
  this->up = up;
  this->fov = fov;
}

Camera::~Camera() {

}

void Camera::rotate(Vector axis, double angle) {
  Quaternion R;

  R.x = axis.x * sin(angle / 2.0);
  R.y = axis.y * sin(angle / 2.0);
  R.z = axis.z * sin(angle / 2.0);
  R.w = cos(angle / 2.0);

  Quaternion V = Quaternion(center.x, center.y, center.z, 0);
  Quaternion W;
  W = (R.mult(V)).mult(R.conjugate());
  this->center = Vector(W.x, W.y, W.z);

  V = Quaternion(up.x, up.y, up.z, 0);
  W = (R.mult(V)).mult(R.conjugate());
  this->up = Vector(W.x, W.y, W.z);
}

