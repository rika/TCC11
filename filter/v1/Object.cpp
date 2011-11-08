#include "Object.hpp"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <iostream>
using namespace std;

Object::Object () {
    this->frame = -1;
    this->subject = -1;
    this->coord.x = 0;
    this->coord.y = 0;
    this->height = 0;
}

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

Object& Object::operator= (const Object& obj) {
    frame = obj.frame;
    subject = obj.subject;
    coord = obj.coord;
    height = obj.height;
    return *this;
}

bool Object::operator== (Object obj) {
    return (this->frame == obj.frame) &&
        (this->subject == obj.subject) &&
        (this->coord == obj.coord);
        // height == 1.0
}

void Object::display () {
    float side = 10;
    glPushMatrix();
    glTranslatef(coord.x, coord.y, 0);
    glColor3f(0.6, 0.6, 0.6);
    glRectf(-side/2, -side/2, side/2, side/2);
    glPopMatrix();

}

float dist (Object a, Object b) {
    int dx = a.coord.x - b.coord.x;
    int dy = a.coord.y - b.coord.y;
    return sqrt(dx*dx + dy*dy);
}
