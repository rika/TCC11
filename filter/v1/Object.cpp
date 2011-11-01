#include "Object.hpp"

Object::Object (int frame, int subject, int x, int y, float height) {
    this->frame = frame;
    this->subject = subject;
    this->coord.x = x;
    this->coord.y = y;
    this->height = height;
}

bool Object::operator== (Object obj) {
    return (this->frame == obj.frame) &&
        (this->subject == obj.subject) &&
        (this->coord == obj.coord);
        // height == 1.0
}

