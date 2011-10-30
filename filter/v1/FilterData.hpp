#ifndef _FILTERDATA_HPP_
#define _FILTERDATA_HPP_

#include <list>
#include <string>
#include "Object.hpp" 

class FilterData {
private:
    int currentFrame;
    bool forward;
    list<Object>[] frameObjects;

public:
    FilterData(string filePath, int startFrame, int endFrame);
    ~FilterData();
    Object * getStart();
    Object * locate(cv::Point predict);
    bool isFinished();
    void remove(list<Object>);
};

#endif
