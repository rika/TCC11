#include <iostream>
#include <fstream>
#include <cstdlib>

#include "ObjectData.hpp"

using namespace std;

ObjectData::ObjectData(string objectDataPath, int startFrame, int endFrame) {
    bool push;
    int start, end, frame, n, subject, cx, cy;
    double height;
    ifstream infile;

    this->startFrame = startFrame;
    this->endFrame = endFrame;

    infile.open(objectDataPath.c_str());
    if (!infile.is_open()) {
        cout << "Could not open: " << objectDataPath.c_str() << endl;
        exit(-1);
    }

    infile >> start >> end;
    if (startFrame < start or endFrame > end) {
        cout << "Frames out of range of the tracking data" << endl;
        infile.close();
        exit(-1);
    }

    frameObjects = new vector<FVV::Object>[endFrame - startFrame + 1];

    for (int frame = start; frame <= end && frame <= endFrame; frame++) {
        infile >>  n;
        if (frame >= startFrame)
            push = true;
        else
            push = false;
        for (int j = 0; j < n; j++) {
            infile >> subject >> height >> cx >> cy;
            if (push) {
                frameObjects[frame-startFrame].push_back(FVV::Object(subject, height, cx, cy));
            }
        }
    }
    infile.close();
}

ObjectData::~ObjectData() {
    if (frameObjects) delete frameObjects;
}

vector<FVV::Object> ObjectData::getObjects(int frame) {
    int index = frame - startFrame;
    if (index < 0 or index >= endFrame - startFrame + 1) {
        cout << "Invalid frame: "<< frame << endl;
        exit(-1);
    }
    return frameObjects[index];
}
