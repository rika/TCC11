#include <iostream>
#include <fstream>
#include <cstdlib>

#include "FilterData.hpp"

using namespace std;

FilterData::FilterData(string filePath int startFrame, int endFrame) {

    ifstream infile;
    int start, end;
    int n, subject, x, y;
    float height;

    // open file    
    infile.open(filePath.c_str());
    if (!infile.is_open()) {
        cout << "Could not open: " << filePath << endl;
        exit(-1);
    }

    // check frames range
    infile >> start >> end;
    if (startFrame < start or endFrame > end) {
        cout << "Frames out of range of the tracking data" << endl;
        infile.close();
        exit(-1);
    }

    // read and store data
    frameObjects = new vector<cv::Point>[endFrame - startFrame + 1];
    for (int frame = start; frame <= end && frame <= endFrame; frame++) {
        infile >> n;
        for (int j = 0; j < n; j++) {
            infile >> subject >> height >> x >> y;
            if (frame >= startFrame)
                frameObjects[frame-startFrame].push_back(cv::Point p(x, y));
        }
    }
    infile.close();
}

FilterData::~FilterData() {
    if (frameObjects) delete frameObjects;
}

cv::Point FilterData::getStart() {

}

cv::Point locate(cv::Point predict) {

}

void FilterData::remove(Vector<cv::Point>) {
}
