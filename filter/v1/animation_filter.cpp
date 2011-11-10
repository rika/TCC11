#include <iostream>
#include <cstdlib>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "FilterData.hpp"
#include "Object.hpp"
#include "Tracker.hpp"

#define IDLE_INIT_TIME 20  // Tempo de sleep entre frames em milisegundos

using namespace std;

int width  = 1152;
int height = 400;

float ftL = -50;
float ftB = 450;
float ftR= 1150;
float ftT= -50;

float steptw, stepth;

bool paused = true;

FilterData * data;
Tracker * tracker;
int id;

void usage (char* argv[]) {
    cout << "USAGE: " << argv[0] << " [infile] [start_frame] [end_frame] [v_const] [d_threshold] [t_threshold] [l_threshold]" << endl;
}

int start_frame, end_frame;
float vk, dt;
int tt, lt;
stringstream outfile;

void init(int argc, char* argv[]) {

    if (argc != 8) {
        usage(argv);
        exit(0);
    }

    stringstream ss(argv[2]), se(argv[3]), sv(argv[4]), sd(argv[5]), st(argv[6]), sl(argv[7]);
    bool fail = false;
    fail = fail || (ss >> start_frame).fail();
    fail = fail || (se >> end_frame).fail();
    fail = fail || (sv >> vk).fail();
    fail = fail || (sd >> dt).fail();
    fail = fail || (st >> tt).fail();
    fail = fail || (sl >> lt).fail();

    if (fail) {
        usage(argv);
        exit(0);
    }

    data = new FilterData(string(argv[1]), start_frame, end_frame);

    cout << "Infile: " << argv[1] << " [" << start_frame << ", " << end_frame << "]" << endl;

    outfile << argv[1] << ".out";
    cout << "Outfile: " << outfile.str() << endl;

    id = 0;
    steptw = (ftR-ftL)/width;
    stepth = (ftT-ftB)/height;
}

void display() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(ftL, ftR, ftB, ftT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (tracker != NULL)
        tracker->display(data);

    glutSwapBuffers();
}

void reshape(int w, int h) {
    width = w;
    height = h;
    stepth = (ftT-ftB)/h;
    steptw = (ftR-ftL)/w;
    glViewport(0,0,width,height);
}

void finalize() {
    cout << endl << "DONE ?? REMAINING OBJECTS: " << endl;

    for (int i = start_frame; i <= end_frame; i++) {
        cout << "frame " << i << ": ";
        list<Object> l = data->get(i);
        list<Object>::iterator it;
        for (it = l.begin(); it != l.end(); it++) {
            cout << " " << (*it).subject;
        }
        cout << endl;
    }

    data->writeResult(outfile.str());
    exit(0);
}

void step(int t) {

    if (tracker != NULL) {
        bool done = tracker->step(data);

        if (done) {
            data->update(tracker->trackedSet, tracker->predictSet);
            delete tracker;
            tracker = NULL;
        }
        glutPostRedisplay();
    }
    else {
        Object obj = data->getStart();
        if (obj.subject == -1) finalize();
        cout << "Tracking: " << id << " at (" << obj.coord.x << "," << obj.coord.y << ")" << endl; 
        tracker = new Tracker(obj, id++, vk, dt, tt, lt);
    }
    if (!paused)
        glutTimerFunc(t, step, t);
}

void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'q':
            exit(0);
            break;
        case 'Q':
            exit(0);
            break;
        case 'p':
            if(paused) {
                paused = false;
                glutTimerFunc(0, step, IDLE_INIT_TIME);
            }
            else
                paused = true;
            break;

        case 'P':
            if(paused) {
                paused = false;
                glutTimerFunc(0, step, IDLE_INIT_TIME);
            }
            else
                paused = true;
            break;
        case ' ':
            if(paused)
                glutTimerFunc(0, step, 0);
            break;
        default:
            break;
    }
}

int main(int argc, char* argv[]) {
    
    init(argc, argv);
    argc = 1;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow(argv[0]);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(IDLE_INIT_TIME, step, IDLE_INIT_TIME);
    glutKeyboardFunc(keyboard);

    glutMainLoop();

    return 0;
}
