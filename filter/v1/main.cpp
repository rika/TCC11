#include <iostream>
#include <cstdlib>

#include <cv.h>

#include "FilterData.hpp"
#include "Object.hpp"
#include "Tracker.hpp"

#define INFILE  "gpoints.conf"
#define OUTFILE "out.conf"
#define START_FRAME 1510
#define END_FRAME   2999

using namespace std;

int main() {

    FilterData data(INFILE, START_FRAME, END_FRAME);
/*
    // LISTAGEM DOS ELEMENTOS DE CADA FRAME
    for (int i = START_FRAME; i <= END_FRAME; i++) {
        cout << "frame " << i << ": " << endl;
        list<Object> l = data.get(i);
        list<Object>::iterator it;
        for (it = l.begin(); it != l.end(); it++) {
            cout << " " << (*it).subject;
        }
        cout << endl;
    }
*/
/*
    // 1 START E LISTAGEM DOS OBJETOS NA JANELA TEMPORAL
    Object obj = data.getStart();
    cout << "start " << obj.subject << " at: (" << obj.coord.x << "," << obj.coord.y << ")" << endl;
    for (int i = obj.frame; i < obj.frame+5; i++) {
        cout << " frame: " << i << endl;
        list<Object> l = data.get(i);
        list<Object>::iterator it;
        for (it = l.begin(); it != l.end(); it++)
            cout << "  (" << (*it).coord.x << "," << (*it).coord.y << ")" << endl;
    }
*/

    int id = 0;
    Object * obj_pt;
    while ((obj_pt = data.getStart()) != NULL) {
        cout << "Tracking: " << id << " at (" << obj_pt->coord.x << "," << obj_pt->coord.y << ")"<< endl; 
        Tracker * tracker = new Tracker(obj_pt, id++);
        tracker->track(&data);
        data.update(tracker->trackedSet, tracker->predictSet);
        delete tracker;
    }

    cout << endl << "DONE ?? REMAINING OBJECTS: " << endl;

    for (int i = START_FRAME; i <= END_FRAME; i++) {
        cout << "frame " << i << ": ";
        list<Object> l = data.get(i);
        list<Object>::iterator it;
        for (it = l.begin(); it != l.end(); it++) {
            cout << " " << (*it).subject;
        }
        cout << endl;
    }

    data.writeResult(OUTFILE);

    return 0;
}
