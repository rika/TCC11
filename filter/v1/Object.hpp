#ifndef _OBJECT_HPP_
#define _OBJECT_HPP_

class Object {
    public:
    int frame;
    int subject;
    int x, y;
    float height;

    Object (int frame, int subject, int x, int y, float height);
};

#endif

