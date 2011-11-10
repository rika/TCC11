#include <iostream>
#include <cstdlib>

#include <cv.h>

#include "FilterData.hpp"
#include "Object.hpp"
#include "Tracker.hpp"

using namespace std;

void usage (char* argv[]) {
    cout << "USAGE: " << argv[0] << " [infile] [start_frame] [end_frame] [v_const] [d_threshold] [t_threshold] [l_threshold]" << endl;
}

int main(int argc, char* argv[]) {

    int start_frame, end_frame;
    float vk, dt;
    int tt, lt;
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

    FilterData data(string(argv[1]), start_frame, end_frame);

    stringstream outfile;
    outfile << argv[1] << ".out";

    int id = 0;
    Object obj;
    while (true) {
        obj = data.getStart();
        if (obj.subject == -1) break;
        cout << "Tracking: " << id << " at (" << obj.coord.x << "," << obj.coord.y << ")" << endl; 
        Tracker * tracker = new Tracker(obj, id++, vk, dt, tt, lt);
       
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
