#include <iostream>
#include <fstream>
#include <cstdlib>

#include "FilterData.hpp"

using namespace std;

FilterData::FilterData(string filePath, int startFrame, int endFrame) {

    ifstream infile;
    int n, subject, x, y;
    float height;

    // open file    
    infile.open(filePath.c_str());
    if (!infile.is_open()) {
        cout << "FilterData: Could not open: " << filePath << endl;
        exit(-1);
    }

    // check frames range
    infile >> start >> end;
    if (startFrame < start or endFrame > end) {
        cout << "FilterData: Frames out of range of the tracking data" << endl;
        infile.close();
        exit(-1);
    }

    // read and store data
    frameObjects = new list<Object>[endFrame - startFrame + 1];
    for (int frame = start; frame <= end && frame <= endFrame; frame++) {
        infile >> n;
        for (int j = 0; j < n; j++) {
            infile >> subject >> height >> x >> y;
            if (frame >= startFrame) {
                Object obj(frame, subject, x, y, height);
                frameObjects[frame-startFrame].push_back(obj);
            }
        }
    }
    this->start = startFrame;
    this->end = endFrame;
    infile.close();
}

FilterData::~FilterData() {
    if (frameObjects) delete[] frameObjects;
}

Object * FilterData::getStart() {

}

void FilterData::remove(list<Object>) {
}

list<Object> FilterData::get(int frame) {
    if (frame < start || frame > end) {
        cout << "FilterData.get: Invalid frame " << frame << endl;
        exit(-1);
    }
    //cout << frame << " " << start << " " << end << endl;
    return frameObjects[frame - start];
}
