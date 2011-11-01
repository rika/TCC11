
#include "Tracker.hpp"

#define DP_K 4
#define MP_K 2
#define SN_K 128
#define V_K 16

#define FORWARD 1
#define BACKWARD -1

#define THRESHOLD 10

float dist(float ax, float ay, float bx, float by) {
    float dx = ax - bx;
    float dy = ay - by;
    return sqrt(dx*dx+dy*dy);
}

Tracker::Tracker (Object * obj_pt, int id) {
    initObj = obj_pt;
    this->id = id;
    conDenF = initConDensation (obj_pt);
    conDenB = initConDensation (obj_pt);
    trackedSet.push_back(*obj_pt);
    predictSet.push_back(Object(obj_pt->frame, id, (int) obj_pt->coord.x, (int) obj_pt->coord.y, obj_pt->height));
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

    cvmSet(lower, 0, 0, 0);
    cvmSet(upper, 0, 0, 0);
    cvmSet(lower, 1, 0, 0);
    cvmSet(upper, 1, 0, 0);

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
        float d = dist (obj_pt->coord.x, obj_pt->coord.y,
                con->flSamples[i][0], con->flSamples[i][1]);
        if (d < 1) d = 1;
        con->flConfidence[i] = 1/d;
    }
}

void Tracker::track (FilterData * data) {
    trackAux(conDenF, data, FORWARD);
    trackAux(conDenB, data, BACKWARD);
    if (predictSet.size() < 20) predictSet.clear();
}

Object * nearest (int x, int y, list<Object> l, int d_max) {
    list<Object>::iterator it;
    Object * obj = NULL;
    float D = d_max;
    for (it = l.begin(); it != l.end(); it++) {
        float d = dist(x, y, (*it).coord.x, (*it).coord.y);
        if (d < D) {
            D = d;
            obj = &(*it);
        }
    }
    return obj;
}

void Tracker::trackAux (CvConDensation * con, FilterData * data, int step) {
    list<Object> fails;
    list<Object>::iterator it;

    int nf = 0;
    int lastx = (int) con->State[0];
    int lasty = (int) con->State[1];
    for (int frame = initObj->frame+step; frame < data->end && frame > data->start; frame += step) {
        cout << " [" << frame << "]" << endl;
        // predict
        cvConDensUpdateByTime(con);
        int px = (int) con->State[0];
        int py = (int) con->State[1];
        cout << " PREDICT at (" << px << ","<< py << ")" << endl;

        // locate
        Object * obj = nearest (px, py, data->get(frame), V_K);
        if (obj == NULL)
            obj = nearest (lastx, lasty, data->get(frame), V_K+5);

        lastx = px;
        lasty = py;

        if (obj != NULL) {
            cout << " FOUND obj at (" << obj->coord.x << ","<< obj->coord.y << ")" << endl;
            trackedSet.push_back(*obj);
            predictSet.push_back(Object(frame, id, px, py, initObj->height));
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
            fails.push_back(Object(frame, id, px, py, initObj->height));
        }
    }
}
