#ifndef _OBJECT_HPP_
#define _OBJECT_HPP_

#include <cv.h>

class Object {
    public:
    int frame;
    int subject;
    cv::Point coord;
    float height;

    Object (int frame, int subject, int x, int y, float height);
};

#endif

