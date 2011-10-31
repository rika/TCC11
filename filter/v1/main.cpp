#include <iostream>
#include <cstdlib>

#include <cv.h>

#include "FilterData.hpp"
#include "Object.hpp"

#define FILENAME "gpoints.conf"
#define START_FRAME 1510
#define END_FRAME   2999

using namespace std;

int main() {

    FilterData data(FILENAME, START_FRAME, END_FRAME);
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

    while ((Object * obj_pt = data.getStart()) != NULL) {
        Tracker * tracker = new Tracker(obj_pt);
        tracker->trackForward();
        tracker->trackBackward();
        data.update(tracker->trackedSet, tracker->holes);
        delete tracker;
    }

    cout << "done??" << endl;

    return 0;
}
