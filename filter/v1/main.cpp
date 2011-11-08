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

    int id = 0;
    Object obj;
    while (true) {
        obj = data.getStart();
        if (obj.subject == -1) break;
        cout << "Tracking: " << id << " at (" << obj.coord.x << "," << obj.coord.y << ")" << endl; 
        Tracker * tracker = new Tracker(obj, id++);
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
