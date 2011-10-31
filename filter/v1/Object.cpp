#include "Object.hpp"

Object::Object (int frame, int subject, int x, int y, float height) {
    this->frame = frame;
    this->subject = subject;
    this->coord.x = x;
    this->coord.y = y;
    this->height = height;
}

