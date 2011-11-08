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
/*
float ColorTablef[42][3]={
    {1.00, 1.00, 1.00},    {0.00, 1.00, 1.00},
    {0.85, 0.85, 0.85},    {0.00, 0.85, 0.85},
    {0.70, 0.70, 0.70},    {0.00, 0.70, 0.70},
    {0.55, 0.55, 0.55},    {0.00, 0.55, 0.55},
    {0.40, 0.40, 0.40},    {0.00, 0.40, 0.40},
    {0.25, 0.25, 0.25},    {0.00, 0.25, 0.25},

    {1.00, 1.00, 0.00},    {1.00, 0.00, 1.00},
    {0.85, 0.85, 0.00},    {0.85, 0.00, 0.85},
    {0.70, 0.70, 0.00},    {0.70, 0.00, 0.70},
    {0.55, 0.55, 0.00},    {0.55, 0.00, 0.55},
    {0.40, 0.40, 0.00},    {0.40, 0.00, 0.40},
    {0.25, 0.25, 0.00},    {0.25, 0.00, 0.25},

    {0.00, 0.00, 1.00},    {0.00, 1.00, 0.00},
    {0.00, 0.00, 0.85},    {0.00, 0.85, 0.00},
    {0.00, 0.00, 0.70},    {0.00, 0.70, 0.00},
    {0.00, 0.00, 0.55},    {0.00, 0.55, 0.00},
    {0.00, 0.00, 0.40},    {0.00, 0.40, 0.00},
    {0.00, 0.00, 0.25},    {0.00, 0.25, 0.00},

    {1.00, 0.00, 0.00},
    {0.85, 0.00, 0.00},
    {0.70, 0.00, 0.00},
    {0.55, 0.00, 0.00},
    {0.40, 0.00, 0.00},
    {0.25, 0.00, 0.00}
};*/


FilterData * data;
Tracker * tracker;
int id;
int start_frame, end_frame;
stringstream outfile;

void init(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "USAGE: " << argv[0] << " [infile] [start_frame] [end_frame]" << endl;
        exit(0);
    }
    stringstream s(argv[2]);
    stringstream e(argv[3]);

    if ( (s >> start_frame).fail() || (e >> end_frame).fail()) {
        cout << "USAGE: " << argv[0] << " [infile] [start_frame] [end_frame]" << endl;
        exit(0);
    }

    cout << "Infile: " << argv[1] << " [" << start_frame << ", " << end_frame << "]" << endl;


    data = new FilterData(string(argv[1]), start_frame, end_frame);
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
        tracker = new Tracker(obj, id++);
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
