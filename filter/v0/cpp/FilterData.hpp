#ifndef _FILTERDATA_HPP_
#define _FILTERDATA_HPP_

#include <cv.h>
#include <vector>
#include <string>

class FilterData {
private:
    int currentFrame;
    bool forward;
    vector<cv::Point>[] frameObjects;

public:
    FilterData(string filePath, int startFrame, int endFrame);
    ~FilterData();
    cv::Point getStart();
    cv::Point locate(cv::Point predict);
    bool isFinished();
    void remove(Vector<cv::Point>);
};

#endif
