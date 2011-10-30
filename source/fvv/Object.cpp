#include "Object.hpp"

Object::Object(double h, int cx, int cy) {
  this->height = h;
  this->position = Vector(cx, cy, 0.0);
}

Object::~Object() {

}

