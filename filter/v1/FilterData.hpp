#ifndef _FILTERDATA_HPP_
#define _FILTERDATA_HPP_

#include <list>
#include <string>
#include "Object.hpp" 

using namespace std;

class FilterData {
    private:
    int currentFrame;
    bool forward;
    list<Object> * frameObjects;
    list<Object> * resultObjects;
    bool isStart(Object obj);

    public:
    int start, end;
    FilterData(string filePath, int startFrame, int endFrame);
    ~FilterData();
    Object * getStart();
    list<Object> get(int frame);
    void update(list<Object> trackedSet, list<Object> predictSet);
    void writeResult(string filePath);
};

#endif
