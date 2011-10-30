#include "Quaternion.hpp"

Quaternion::Quaternion() {
  x = y = z = 0.0;
}

Quaternion::Quaternion(double x, double y, double z, double w) {
  this->x = x;
  this->y = y;
  this->z = z;
  this->w = w;
}

Quaternion::~Quaternion() {

}

double Quaternion::length() {
  return sqrt(x * x + y * y + z * z + w * w);
}

Quaternion Quaternion::normalize() {
  double l = length();

  return Quaternion(x / l, y / l, z / l, w / l);
}

Quaternion Quaternion::conjugate() {
  return Quaternion(-x, -y, -z, w);
}

Quaternion Quaternion::mult(Quaternion other) {
  Quaternion quaternion;

  quaternion.x = this->w * other.x + this->x * other.w + 
    this->y * other.z - this->z * other.y;
  quaternion.y = this->w * other.y - this->x * other.z +
    this->y * other.w + this->z * other.x;
  quaternion.z = this->w * other.z + this->x * other.y -
    this->y * other.x + this->z * other.w;
  quaternion.w = this->w * other.w - this->x * other.x -
    this->y * other.y - this->z * other.z;

  return quaternion;
}

