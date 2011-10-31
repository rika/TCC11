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

    FilterData * data;
    data = new FilterData(FILENAME, START_FRAME, END_FRAME);

    for (int i = START_FRAME; i <= END_FRAME; i++) {
        cout << "frame " << i << ": " << endl;
        list<Object> l = data->get(i);
        list<Object>::iterator j;
        for (j = l.begin(); j != l.end(); j++) {
            cout << " " << (*j).subject;
            //cout << " (" << (*j).x << "," << (*j).y << ")";
        }
        cout << endl;
    }

    cout << "done??" << endl;

    delete data;

    return 0;
}
