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

#define TWIN_SIZE 5
#define WIN_DIST 100

float dist (Object a, Object b) {
    int dx = a.coord.x - b.coord.x;
    int dy = a.coord.y - b.coord.y;
    return sqrt(dx*dx + dy*dy);
}

bool FilterData::isStart(Object obj) {
    cout << " isStart(" << obj.subject << ")" << endl;
    list<Object>::iterator it;
    cout << "  frame: " << obj.frame << endl;
    int frame = obj.frame-start;
    for (it = frameObjects[frame].begin(); it != frameObjects[frame].end(); it++) {
        cout << "  comparing with: " << (*it).subject << endl;
        if (obj.subject != (*it).subject && dist(obj, *it) < WIN_DIST)
            return false;
    }

    Object * p;
    Object * q;
    p = &obj;

    for (frame = obj.frame-start+1; frame < obj.frame-start+TWIN_SIZE; frame++) {
        cout << "  frame: " << frame+start << endl;
        for (it = frameObjects[frame].begin(); it != frameObjects[frame].end(); it++) {
            cout << "  comparing with: " << (*it).subject << endl;
            q = NULL;
            if (dist(*p, *it) < WIN_DIST) {
                if (q != NULL) return false;
                q = &(*it);
            }
        }
    }
    return true;
}

Object FilterData::getStart() {
    for (int i = 0; i <= end-start-(TWIN_SIZE-1); i++) {
        cout << "frame: " << i << endl;
        list<Object>::iterator it;
        for (it = frameObjects[i].begin(); it != frameObjects[i].end(); it++)
            if (isStart(*it)) return *it;
    }
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
