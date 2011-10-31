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
    Object obj = data.getStart();
    cout << "start " << obj.subject << " at: (" << obj.coord.x << "," << obj.coord.y << ")" << endl;
    for (int i = obj.frame; i < obj.frame+5; i++) {
        cout << " frame: " << i << endl;
        list<Object> l = data.get(i);
        list<Object>::iterator it;
        for (it = l.begin(); it != l.end(); it++)
            cout << "  (" << (*it).coord.x << "," << (*it).coord.y << ")" << endl;
    }

    cout << "done??" << endl;

    return 0;
}
