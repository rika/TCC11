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
    float v_const;
    float d_threshold;
    int t_threshold;
    int l_threshold;
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
    CvConDensation * initConDensation(Object obj);
    void updateProbDens (CvConDensation * con, Object obj);
    void updateProbDens2 (CvConDensation * con, Object obj, list<Object> l);
    
    public:
    list<Object> trackedSet;
    list<Object> predictSet;

    Tracker (Object obj, int id, float vk, float dt, int tt, int lt);
    ~Tracker();

    void display (FilterData * data);
    bool step (FilterData * data);

};

#endif
