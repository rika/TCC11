#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "Tracker.hpp"

#define DP_K 4
#define MP_K 2
#define SN_K 128
#define V_K 16

#define FORWARD 1
#define BACKWARD -1

#define THRESHOLD 20
#define D_THRESHOLD 30

float dist(float ax, float ay, float bx, float by) {
    float dx = ax - bx;
    float dy = ay - by;
    return sqrt(dx*dx+dy*dy);
}

Tracker::Tracker (Object obj, int id) {
    state = FORWARD;
    init = true;

    initObj = obj;
    this->id = id;
    conDenF = initConDensation (obj);
    conDenB = initConDensation (obj);
    trackedSet.push_back(initObj);
    predictSet.push_back(initObj);
}

Tracker::~Tracker () {
    trackedSet.clear();
    predictSet.clear();
    cvReleaseConDensation(&conDenF);
    cvReleaseConDensation(&conDenB);
}

CvConDensation * Tracker::initConDensation (Object obj) { 
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

    con->State[0] = obj.coord.x;
    con->State[1] = obj.coord.y;
    con->State[2] = 0;
    con->State[3] = 0;

    for (int i = 0; i < con->SamplesNum; i++) {
        con->flSamples[i][0] = obj.coord.x;
        con->flSamples[i][1] = obj.coord.y;
        con->flSamples[i][2] = 0;
        con->flSamples[i][3] = 0;
        con->flConfidence[i] = 1;
    }

    return con;
}


void Tracker::updateProbDens (CvConDensation * con, Object obj) {
    for (int i = 0; i < con->SamplesNum; i++) {
        float d = dist (obj.coord.x, obj.coord.y,
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

Object nearest (int subject, int x, int y, list<Object> l, int d_max) {
    list<Object>::iterator it;
    Object obj;
    obj.subject = -1;
    float D = d_max;
    for (it = l.begin(); it != l.end(); it++) {
        if ((*it).subject == subject) return (*it);
        float d = dist(x, y, (*it).coord.x, (*it).coord.y);
        if (d < D) {
            D = d;
            obj = (*it);
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
    int lasts = initObj.subject;
    for (int frame = initObj.frame+step; frame < data->end && frame > data->start; frame += step) {
        cout << " [" << frame << "]" << endl;
        // predict
        cvConDensUpdateByTime(con);
        int px = (int) con->State[0];
        int py = (int) con->State[1];
        cout << " PREDICT at (" << px << ","<< py << ")" << endl;

        // locate
        Object obj;
        obj = nearest (lasts, px, py, data->get(frame), V_K);
        if (obj.subject == -1)
            obj = nearest (-1, lastx, lasty, data->get(frame), D_THRESHOLD);

        if (obj.subject != -1) {
            lastx = obj.coord.x;
            lasty = obj.coord.y;
            lasts = obj.subject;
            cout << " FOUND obj at (" << obj.coord.x << ","<< obj.coord.y << ")" << endl;
            trackedSet.push_back(obj);
            predictSet.push_back(Object(frame, id, px, py, initObj.height));
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
            fails.push_back(Object(frame, id, px, py, initObj.height));
        }
    }
}

bool Tracker::step (FilterData * data) {
    bool done;
    if (state == FORWARD) {
        done = stepAux(conDenF, data, FORWARD);
        if (done) state = BACKWARD;
        return false;
    }
    // state == BACKWARD
    done = stepAux(conDenB, data, BACKWARD);
    if (done) {
        if (predictSet.size() < 20) predictSet.clear();
        return true;
    }
    return false;
}

bool Tracker::stepAux (CvConDensation * con, FilterData * data, int step) {
    list<Object>::iterator it;

    if (init) {
        nf = 0;
        lastx = (int) con->State[0];
        lasty = (int) con->State[1];
        //lasts = initObj.subject;
        tframe = initObj.frame+step;
        init = false;
    }
    if (tframe < data->end && tframe > data->start) {


        cout << " [" << tframe << "]" << endl;
        // predict
        cvConDensUpdateByTime(con);
        px = (int) con->State[0];
        py = (int) con->State[1];
        cout << " PREDICT at (" << px << ","<< py << ")" << endl;

        // locate
        Object obj;
        obj = nearest (-1, px, py, data->get(tframe), V_K);
        if (obj.subject == -1)
            obj = nearest (-1, lastx, lasty, data->get(tframe), D_THRESHOLD);

        obj_v.subject = -1;
        if (obj.subject != -1) {
            obj_v = obj;
            lastx = obj.coord.x;
            lasty = obj.coord.y;
            lasts = obj.subject;

            cout << " FOUND obj at (" << obj.coord.x << ","<< obj.coord.y << ")" << endl;
            trackedSet.push_back(obj);
            predictSet.push_back(Object(tframe, id, px, py, initObj.height));
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
                init = true;
                return true;
            }
            fails.push_back(Object(tframe, id, px, py, initObj.height));
        }
        lastf = tframe;
        tframe += step;
        return false;
    }
    else {
        init = true;
        return true;
    }
}

#define WHITE 0
#define GREY 1

#define H 400
#define W 1000

float ColorTablef[5][3] = {
    {0.0, 1.0, 0.0},
    {1.0, 0.0, 0.0},
    {0.0, 0.0, 0.0},
    {0.9, 0.9, 0.9},
    {0.8, 0.8, 0.8}
};

void sq_display(float x, float y, float side, int color) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glColor3fv(ColorTablef[color]);
    glRectf(-side/2, -side/2, side/2, side/2);
    glPopMatrix();
}


void Tracker::displayCon (CvConDensation * con) {
    // amostras
    for (int i = 0; i < con->SamplesNum; i++)
        sq_display (con->flSamples[i][0], con->flSamples[i][1], 1.0, 2);
    // predicao
    sq_display(px, py, 10.0, 1); 
    // medida
    if(obj_v.subject != -1) sq_display(obj_v.coord.x, obj_v.coord.y, 10.0, 0);
}


void display_str(float x, float y, string s) {
    void * font = GLUT_BITMAP_9_BY_15;
    glPushMatrix();
    glLoadIdentity();
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(x, y);
    for (string::iterator i = s.begin(); i != s.end(); i++) {
        char c = *i;
        glutBitmapCharacter(font, c);
    }
    glPopMatrix();
}

void Tracker::display(FilterData * data) {
    list<Object> l = data->get(lastf);
    list<Object>::iterator it;

    stringstream ss;
    ss << "Frame: " << lastf << " ID: " << id;
    display_str(24, 24, ss.str());

    sq_display (lastx, lasty, 2*D_THRESHOLD, 3);
    sq_display (px, py, 2*V_K, 4);

    for (it = l.begin(); it != l.end(); it++) {
        (*it).display();
        //sq_display((*it).coord.x, (*it).coord.y, 12.0 , 1);
    }

    if (state == FORWARD) displayCon(conDenF);
    else displayCon(conDenB);
}
