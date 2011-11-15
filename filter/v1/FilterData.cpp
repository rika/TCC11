#include <iostream>
#include <fstream>
#include <cstdlib>

#include "FilterData.hpp"

using namespace std;

FilterData::FilterData(string filePath, int startFrame, int endFrame, float dt, int lt) {

    d_threshold = dt;
    l_threshold = lt;
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
    list<Object>::iterator it;
    int frame = obj.frame-start;
    for (it = frameObjects[frame].begin(); it != frameObjects[frame].end(); it++) {
        if (obj.subject != (*it).subject && dist(obj, *it) < d_threshold)
            return false;
    }

    Object p;
    Object q;
    p = obj;

    for (frame = obj.frame-start+1; frame < obj.frame-start+l_threshold; frame++) {
        if (frame > end) return false;
        q = Object(-1, -1, -1, -1, -1);
        for (it = frameObjects[frame].begin(); it != frameObjects[frame].end(); it++) {
            if (dist(p, *it) < d_threshold) {
                if (q.subject != -1) return false;
                q = (*it);
            }
        }
        if (q.subject == -1) return false;
        p = q;
    }
    return true;
}

Object FilterData::getStart() {
    for (int i = 0; i <= end-start-(l_threshold-1); i++) {
        list<Object>::iterator it;
        for (it = frameObjects[i].begin(); it != frameObjects[i].end(); it++)
            if (isStart(*it)) return *it;
    }
    return Object(-1, -1, -1, -1, -1);
}

list<Object> FilterData::get(int frame) {
    if (frame < start || frame > end) {
        cout << "FilterData.get: Invalid frame " << frame << endl;
        exit(-1);
    }
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

#define MAX_OBJS 1000

void FilterData::smooth() {
    vector<Object> * objects = new vector<Object>[MAX_OBJS];

    for (int frame = start; frame <= end; frame++) {
        list<Object> l = resultObjects[frame-start];
        list<Object>::iterator it;
        for (it = l.begin(); it != l.end(); it++) {
            int sub = (*it).subject;
            objects[sub].push_back((*it));
        }
        resultObjects[frame-start].clear();
    }

    for (int i = 0; i < MAX_OBJS; i++) {
        if (objects[i].size() >= 5) {
            for (unsigned j = 2; j < objects[i].size()-2; j++) {
                Object p1 = objects[i].at(j-2);
                Object p2 = objects[i].at(j-1);
                Object o3 = objects[i].at(j);
                Object a2 = objects[i].at(j+1);
                Object a1 = objects[i].at(j+2);
                float x = (p1.coord.x + 2*p2.coord.x + 3*o3.coord.x +
                    2*a2.coord.x + a1.coord.x) / 9.0;
                float y = (p1.coord.y + 2*p2.coord.y + 3*o3.coord.y +
                    2*a2.coord.y + a1.coord.y) / 9.0;

                o3.coord.x = (int)x;
                o3.coord.y = (int)y;
                resultObjects[o3.frame-start].push_back(o3);
            }
        }
    
    }

}
