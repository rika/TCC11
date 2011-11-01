#ifndef _TRACKER_HPP_
#define _TRACKER_HPP_

#include "Object.hpp"
#include "FilterData.hpp"

#include <cv.h>
#include <cvaux.h>
#include <highgui.h>

#include <list>

using namespace std;

class Tracker {
    private:
    int id;
    int startFrame;
    CvConDensation * conDenF;
    CvConDensation * conDenB;
    void trackAux(CvConDensation * con, FilterData * data, int step);
    CvConDensation * initConDensation(Object * obj_pt);
    void updateProbDens (CvConDensation * con, Object * obj_pt);
    
    public:
    list<Object> trackedSet;
    list<Object> predictSet;

    Tracker (Object * obj_pt, int id);
    ~Tracker();
    void track (FilterData * data);

};

#endif
