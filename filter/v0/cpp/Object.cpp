#include "Object.hpp"

using namespace FVV;

Object::Object() {
  this->subject = -1;
  this->position = Vector();
  this->visibleArea = 0.0;
  this->height = 0.0;
}

Object::Object(int subject, double px, double py, double visibleArea) {
  this->subject = subject;
  this->position = Vector(px, py, 0.0);
  this->visibleArea = visibleArea;
  this->height = 0.0;
}

Object::Object(int subject, double h, int cx, int cy) {
  this->subject = subject;
  this->height = h;
  this->position = Vector(cx, cy, 0.0);
  this->visibleArea = 0.0;
}

Object::~Object() {
  
}

