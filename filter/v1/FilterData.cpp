#include <iostream>
#include <fstream>
#include <cstdlib>

#include "FilterData.hpp"

#define TWIN_SIZE 10
#define WIN_DIST 50

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
    resultObjects = new list<Object>[endFrame - startFrame + 1];
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

bool FilterData::isStart(Object obj) {
    //cout << " isStart(" << obj.subject << ")" << endl;
    list<Object>::iterator it;
    //cout << "  frame: " << obj.frame << endl;
    int frame = obj.frame-start;
    for (it = frameObjects[frame].begin(); it != frameObjects[frame].end(); it++) {
        //cout << "  comparing with: " << (*it).subject << endl;
        if (obj.subject != (*it).subject && dist(obj, *it) < WIN_DIST)
            return false;
    }

    Object * p;
    Object * q;
    p = &obj;

    for (frame = obj.frame-start+1; frame < obj.frame-start+TWIN_SIZE; frame++) {
        //cout << "  frame: " << frame+start << endl;
        for (it = frameObjects[frame].begin(); it != frameObjects[frame].end(); it++) {
            //cout << "  comparing with: " << (*it).subject << endl;
            q = NULL;
            if (dist(*p, *it) < WIN_DIST) {
                if (q != NULL) return false;
                q = &(*it);
            }
        }
    }
    return true;
}

Object * FilterData::getStart() {
    for (int i = 0; i <= end-start-(TWIN_SIZE-1); i++) {
        //cout << "frame: " << i << endl;
        list<Object>::iterator it;
        for (it = frameObjects[i].begin(); it != frameObjects[i].end(); it++)
            if (isStart(*it)) return &(*it);
    }
    return NULL;
}

list<Object> FilterData::get(int frame) {
    if (frame < start || frame > end) {
        cout << "FilterData.get: Invalid frame " << frame << endl;
        exit(-1);
    }
    //cout << frame << " " << start << " " << end << endl;
    return frameObjects[frame - start];
}

void FilterData::update(list<Object> trackedSet, list<Object> predictSet) {
    list<Object>::iterator it;
    for (it = trackedSet.begin(); it != trackedSet.end(); it++) {
        cout << "Removing (" << (*it).coord.x << "," << (*it).coord.y << ") at frame " << (*it).frame << endl;
        frameObjects[(*it).frame-start].remove(*it);
    }
    for (it = predictSet.begin(); it != predictSet.end(); it++)
        resultObjects[(*it).frame-start].push_back(*it);
}

void FilterData::writeResult(string filePath) {
    ofstream outfile;

    outfile.open(filePath.c_str(), ios::out);
    if (!outfile.is_open()) {
        cout << "FilterData: Could not open: " << filePath << endl;
        exit(-1);
    }

    outfile << start << " " << end << endl;
    for (int frame = start; frame <= end; frame++) {
        list<Object> l = resultObjects[frame-start];
        list<Object>::iterator it;
        outfile << l.size() << endl;
        for (it = l.begin(); it != l.end(); it++)
            outfile << (*it).subject << " " << (*it).height << " " << (*it).coord.x << " " << (*it).coord.y << endl;
    }
    outfile.close();
}
