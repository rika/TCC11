#include <iostream>
#include <cstdlib>

#include <cv.h>

#include "FilterData.hpp"
#include "Object.hpp"
#include "Tracker.hpp"

using namespace std;

int main(int argc, char* argv[]) {

    int start_frame, end_frame;
    if (argc != 4) {
        cout << "USAGE: " << argv[0] << " [infile] [start_frame] [end_frame]" << endl;
        exit(0);
    }

    stringstream s(argv[2]);
    stringstream e(argv[3]);

    if ( (s >> start_frame).fail() || (e >> end_frame).fail()) {
        cout << "USAGE: " << argv[0] << " [infile] [start_frame] [end_frame]" << endl;
        exit(0);
    }

    FilterData data(string(argv[1]), start_frame, end_frame);

    stringstream outfile(argv[1]);
    outfile << ".out";

    int id = 0;
    Object obj;
    while (true) {
        obj = data.getStart();
        if (obj.subject == -1) break;
        cout << "Tracking: " << id << " at (" << obj.coord.x << "," << obj.coord.y << ")" << endl; 
        Tracker * tracker = new Tracker(obj, id++);
       
        bool done = false;
        while (!done) done = tracker->step(&data);

        data.update(tracker->trackedSet, tracker->predictSet);
        delete tracker;
    }

    cout << endl << "DONE ?? REMAINING OBJECTS: " << endl;

    for (int i = start_frame; i <= end_frame; i++) {
        cout << "frame " << i << ": ";
        list<Object> l = data.get(i);
        list<Object>::iterator it;
        for (it = l.begin(); it != l.end(); it++) {
            cout << " " << (*it).subject;
        }
        cout << endl;
    }

    data.writeResult(outfile.str());

    return 0;
}
