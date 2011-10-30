#ifndef _INC_OBJECT_DATA_HPP
#define _INC_OBJECT_DATA_HPP

#include <vector>
#include <string>

#include "Object.hpp"

using namespace std;

class ObjectData {
private:
    int startFrame;
    int endFrame;
    vector<FVV::Object> *frameObjects;

public:
    ObjectData(string objectDataPath, int startFrame, int endFrame);
    ~ObjectData();
    vector<FVV::Object> getObjects(int frame);
};

#endif
