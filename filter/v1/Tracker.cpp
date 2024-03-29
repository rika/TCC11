#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "Tracker.hpp"

#define DP_K 4
#define MP_K 2
#define SN_K 100

#define FORWARD 1
#define BACKWARD -1

float dist(float ax, float ay, float bx, float by) {
    float dx = ax - bx;
    float dy = ay - by;
    return sqrt(dx*dx+dy*dy);
}

Tracker::Tracker (Object obj, int id, float vk, float dt, int tt, int lt) {
    v_const = vk;
    d_threshold = dt;
    t_threshold = tt;
    l_threshold = lt;
    state = FORWARD;
    init = true;
    lastf = obj.frame;

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

    cvmSet(lower, 0, 0, -v_const);
    cvmSet(upper, 0, 0,  v_const);
    cvmSet(lower, 1, 0, -v_const);
    cvmSet(upper, 1, 0,  v_const);

    cvmSet(lower, 2, 0, -1.5*v_const);
    cvmSet(upper, 2, 0,  1.5*v_const);
    cvmSet(lower, 3, 0, -1.5*v_const);
    cvmSet(upper, 3, 0,  1.5*v_const);

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
    /*
    float sumX = 0;
    float sumY = 0;
    for (int i = 0; i < con->SamplesNum; i++) {
        sumX += con->flSamples[i][0];
        sumY += con->flSamples[i][1];
    }
    float meanX = sumX /(float)con->SamplesNum;
    float meanY = sumY /(float)con->SamplesNum;

    float sqmX = 0;
    float sqmY = 0;
    for (int i = 0; i < con->SamplesNum; i++) {
        sqmX += (con->flSamples[i][0]-meanX)*(con->flSamples[i][0]-meanX);
        sqmY += (con->flSamples[i][1]-meanY)*(con->flSamples[i][1]-meanY);
    }
    float varianceX = sqmX /(float)con->SamplesNum-1;
    float varianceY = sqmY /(float)con->SamplesNum-1;
    if (varianceX == 0) varianceX = 1;
    if (varianceY == 0) varianceY = 1;

	for(int i = 0; i < con->SamplesNum; i++){
		float ProbX = 1;
		float ProbY = 1;
		
		float DifX = obj.coord.x - con->flSamples[i][0];
		float DifY = obj.coord.y - con->flSamples[i][1];
		
		ProbX *= (float) exp( -1 * (DifX) * (DifX) / ( 2 * varianceX ) );
		ProbY *= (float) exp( -1  * (DifY) * (DifY) / ( 2 * varianceY ) );

		con->flConfidence[i] = ProbX * ProbY ;

		if (abs(DifX) < 5 || abs(DifY) < 5)
			con->flConfidence[i] = 1;
    }
    */

    for (int i = 0; i < con->SamplesNum; i++) {
        float d = dist (obj.coord.x, obj.coord.y,
                con->flSamples[i][0], con->flSamples[i][1]);
        //if (d < 1) d = 1;
        con->flConfidence[i] = 1/d;
    }

    //normalize
    float sum = 0;
    for (int i = 0; i < con->SamplesNum; i++)
        sum += con->flConfidence[i];
    for (int i = 0; i < con->SamplesNum; i++)
        con->flConfidence[i] /= sum;

}

void getVars(list<Object> l, float* varX, float* varY) {
    list<Object>::iterator it;
    float meanX = 0;
    float meanY = 0;
    for (it = l.begin(); it != l.end(); it++) {
        meanX += (*it).coord.x;
        meanY += (*it).coord.x;
    }
    meanX /= l.size();
    meanY /= l.size();

    *varX = 0;
    *varY = 0;
    for (it = l.begin(); it != l.end(); it++) {
        *varX += ((*it).coord.x - meanX)*((*it).coord.x - meanX);
        *varY += ((*it).coord.y - meanY)*((*it).coord.y - meanY);
    }
    *varX /= l.size()-1;
    *varY /= l.size()-1;
}

void Tracker::updateProbDens2 (CvConDensation * con, Object obj, list<Object> l) {
    list<Object>::iterator it;
    float varX, varY;
    getVars(l, &varX, &varY);

    float sum = 0;
    for (int i = 0; i < con->SamplesNum; i++) {
        float probX = 1;
        float probY = 1;
        for (it = l.begin(); it != l.end(); it++) {
            float dx = (*it).coord.x - con->flSamples[i][0];
            float dy = (*it).coord.y - con->flSamples[i][1];

            probX *= (float) exp(-1*dx*dx / (2*varX));
            probY *= (float) exp(-1*dy*dy / (2*varY));
        }
        float d = dist (obj.coord.x, obj.coord.y,
                con->flSamples[i][0], con->flSamples[i][1]);
        if (d < 1) d = 1;
        sum += con->flConfidence[i] *= probX*probY * (1/d);
    }
    for (int i = 0; i < con->SamplesNum; i++)
        con->flConfidence[i] /= sum;
}

Object nearest (int subject, int x, int y, list<Object> l, int d_max) {
    list<Object>::iterator it;
    Object obj;
    obj.subject = -1;
    float D = 1.1*d_max;
    for (it = l.begin(); it != l.end(); it++) {
        if ((*it).subject == subject) return (*it);
        float d = dist(x, y, (*it).coord.x, (*it).coord.y);
        if (d <= D) {
            D = d;
            obj = (*it);
        }
    }
    return obj;
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
        if ((int)predictSet.size() < l_threshold) predictSet.clear();
        return true;
    }
    return false;
}

bool Tracker::stepAux (CvConDensation * con, FilterData * data, int step) {
    list<Object>::iterator it;

    if (init) {
        nf = 0;
        found = true;
        fails.clear();
        lastx = (int) con->State[0];
        lasty = (int) con->State[1];
        //lasts = initObj.subject;
        tframe = initObj.frame+step;
        init = false;
    }
    cout << "tframe: " << tframe << endl;
    if (tframe < data->end && tframe > data->start) {

        cout << " [" << tframe << "]" << endl;
        // predict
        if (found) {
            cvConDensUpdateByTime(con);
            px = (int) con->State[0];
            py = (int) con->State[1];
            cout << " PREDICT at (" << px << ","<< py << ")" << endl;
        }

        // locate
        Object obj;
        obj = nearest (-1, px, py, data->get(tframe), v_const);
        if (obj.subject == -1) {
            obj = nearest (-1, lastx, lasty, data->get(tframe), d_threshold);
        }

        obj_v.subject = -1;
        if (obj.subject != -1) {
            found = true;
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
            //updateProbDens2 (con, obj, data->get(tframe));
        }
        else {
            found = false;
            nf++;
            if (nf == t_threshold) {
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
        sq_display (con->flSamples[i][0], con->flSamples[i][1], 2.0, 2);
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

    
//    sq_display (lastx, lasty, 2*d_threshold, 3);
//    sq_display (px, py, 2*v_const, 4);

    for (it = l.begin(); it != l.end(); it++) {
        (*it).display();
        //sq_display((*it).coord.x, (*it).coord.y, 12.0 , 1);
    }

    if (state == FORWARD) displayCon(conDenF);
    else displayCon(conDenB);
}
