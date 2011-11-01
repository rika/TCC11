
#include "Tracker.hpp"

#define DP_K 4
#define MP_K 2
#define SN_K 100
#define V_K 16

#define FORWARD 1
#define BACKWARD -1

#define THRESHOLD 4

Tracker::Tracker (Object * obj_pt, int id) {
    this->id = id;
    startFrame = obj_pt->frame;
    conDenF = initConDensation (obj_pt);
    conDenB = initConDensation (obj_pt);
    trackedSet.push_back(*obj_pt);
    predictSet.push_back(*obj_pt);
}

Tracker::~Tracker () {
    cvReleaseConDensation(&conDenF);
    cvReleaseConDensation(&conDenB);
}

CvConDensation * Tracker::initConDensation (Object * obj_pt) { 
    CvConDensation * con;
    con = cvCreateConDensation (DP_K, MP_K, SN_K);
    CvMat * lower = cvCreateMat (DP_K, 1, CV_32F);
    CvMat * upper = cvCreateMat (DP_K, 1, CV_32F);

    cvmSet(lower, 0, 0, -V_K);
    cvmSet(upper, 0, 0,  V_K);
    cvmSet(lower, 1, 0, -V_K);
    cvmSet(upper, 1, 0,  V_K);

    cvmSet(lower, 2, 0, -V_K);
    cvmSet(upper, 2, 0,  V_K);
    cvmSet(lower, 3, 0, -V_K);
    cvmSet(upper, 3, 0,  V_K);

    for (int i = 0; i < 16; i++)
        con->DynamMatr[i] = 0;
    con->DynamMatr[0]  = 1;
    con->DynamMatr[2]  = 1;
    con->DynamMatr[5]  = 1;
    con->DynamMatr[7]  = 1;
    con->DynamMatr[10] = 1;
    con->DynamMatr[15] = 1;

    cvConDensInitSampleSet (con, lower, upper);

    cvReleaseMat(&lower);
    cvReleaseMat(&upper);

    con->State[0] = obj_pt->coord.x;
    con->State[1] = obj_pt->coord.y;
    con->State[2] = 0;
    con->State[3] = 0;

    for (int i = 0; i < con->SamplesNum; i++) {
        con->flSamples[i][0] = obj_pt->coord.x;
        con->flSamples[i][1] = obj_pt->coord.y;
        con->flSamples[i][2] = 0;
        con->flSamples[i][3] = 0;
        con->flConfidence[i] = 1;
    }

    return con;
}


void Tracker::updateProbDens (CvConDensation * con, Object * obj_pt) {
    for (int i = 0; i < con->SamplesNum; i++) {
        float dx = (obj_pt->coord.x - con->flSamples[i][0]);
        float dy = (obj_pt->coord.y - con->flSamples[i][1]);
        con->flConfidence[i] = 1/ sqrt(dx*dx + dy*dy);
    }
}

void Tracker::track (FilterData * data) {
    trackAux(conDenF, data, FORWARD);
    trackAux(conDenB, data, BACKWARD);
}

void Tracker::trackAux (CvConDensation * con, FilterData * data, int step) {
    list<Object> fails;
    list<Object>::iterator it;

    int nf = 0;
    for (int frame = startFrame+step; frame < data->end && frame > data->start; frame += step) {
        cout << " [" << frame << "]" << endl;
        // predict
        cvConDensUpdateByTime(con);
        int px = (int) con->State[0];
        int py = (int) con->State[1];

        // locate
        list<Object> l = data->get(frame);
        Object * obj = NULL;
        float dist = V_K;
        for (it = l.begin(); it != l.end(); it++) {
            float dx = px - (*it).coord.x;
            float dy = py - (*it).coord.y;
            float d = sqrt(dx*dx + dy*dy);
            if (d < dist) {
                dist = d;
                obj = &(*it);
            }
        }
        cout << " PREDICT at (" << px << ","<< py << ")" << endl;

        if (obj != NULL) {
            cout << " FOUND obj at (" << obj->coord.x << ","<< obj->coord.y << ")" << endl;
            trackedSet.push_back(*obj);
            predictSet.push_back(Object(frame, id, px, py, 1.0));
            for (it = fails.begin(); it != fails.end(); it++) {
                cout << "  HOLE at (" << (*it).coord.x << ","<< (*it).coord.y << ")" << endl;
                predictSet.push_back(*it);
            }
            fails.clear();
            nf = 0;
            updateProbDens (con, obj);
        }
        else {
            nf++;
            if (nf == THRESHOLD) {
                cout << "  BREAK!" << endl;
                break;
            }
            fails.push_back(Object(frame, id, px, py, 1.0));
        }
    }
}
