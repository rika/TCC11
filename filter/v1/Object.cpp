#include "Object.hpp"

#include <iostream>
using namespace std;

Object::Object (int frame, int subject, int x, int y, float height) {
    this->frame = frame;
    this->subject = subject;
    this->coord.x = x;
    this->coord.y = y;
    this->height = height;
    if (x < -1000 || x > 10000 || y < -1000 || y > 10000) {
        cout << "COORD INFI" << endl;
        exit(-1);
    }
}

bool Object::operator== (Object obj) {
    return (this->frame == obj.frame) &&
        (this->subject == obj.subject) &&
        (this->coord == obj.coord);
        // height == 1.0
}

