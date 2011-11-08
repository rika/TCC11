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
    int state;
    bool init;
    list<Object> fails;
    int nf, lastx, lasty, lasts, lastf, tframe;
    int px, py;
    bool stepAux(CvConDensation * con, FilterData * data, int step);
    Object obj_v;
    void displayCon(CvConDensation * con);

    int id;
    Object initObj;
    CvConDensation * conDenF;
    CvConDensation * conDenB;
    void trackAux(CvConDensation * con, FilterData * data, int step);
    CvConDensation * initConDensation(Object obj);
    void updateProbDens (CvConDensation * con, Object obj);
    
    public:
    list<Object> trackedSet;
    list<Object> predictSet;

    Tracker (Object obj, int id);
    ~Tracker();
    void track (FilterData * data);

    void display (FilterData * data);
    bool step (FilterData * data);

};

#endif
