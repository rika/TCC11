#ifndef _OBJECT_HPP_
#define _OBJECT_HPP_

#include <cv.h>

class Object {
    public:
    int frame;
    int subject;
    cv::Point coord;
    float height;

    Object ();
    Object (int frame, int subject, int x, int y, float height);
    bool operator== (Object obj);
    Object operator= (Object obj);
};

float dist (Object a, Object b);

#endif

