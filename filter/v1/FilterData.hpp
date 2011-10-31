#ifndef _FILTERDATA_HPP_
#define _FILTERDATA_HPP_

#include <list>
#include <string>
#include "Object.hpp" 

using namespace std;

class FilterData {
private:
    int currentFrame, start, end;
    bool forward;
    list<Object> * frameObjects;
    bool isStart(Object obj);

public:
    FilterData(string filePath, int startFrame, int endFrame);
    ~FilterData();
    Object * getStart();
    void remove(list<Object>);
    list<Object> get(int frame);
};

#endif
