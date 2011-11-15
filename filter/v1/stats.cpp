#include <iostream>
#include <cstdlib>

#include "FilterData.hpp"
#include "Object.hpp"

using namespace std;

int main(int argc, char *argv[]) {
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

    FilterData data(string(argv[1]), start_frame, end_frame, 0, 0);
    cout << "file: " << argv[1] << " [" << start_frame << ", " << end_frame << "]" << endl;

    int sub[1000];
    for (int i = 0; i < 1000; i++) sub[i] = 0;
    int nsub = 0;
    pair<double, int> d_max = make_pair(0, -1);
    pair<double, int> d_min = make_pair(9999, -1);
    pair<double, int> n_max = make_pair(0, -1);
    pair<double, int> n_min = make_pair(999, -1);
    pair<double, int> n_mean = make_pair(0, 0);
    pair<double, int> v_max = make_pair(0, -1);
    pair<double, int> v_min = make_pair(999, -1);
    pair<double, int> v_mean = make_pair(0, 0);
    pair<double, int> x_max = make_pair(0, -1);
    pair<double, int> x_min = make_pair(9999, -1);
    pair<double, int> y_max = make_pair(0, -1);
    pair<double, int> y_min = make_pair(9999, -1);

    map<int, Object> lastFrame;
    for (int i = start_frame; i <= end_frame; i++) {
        list<Object> l = data.get(i);
        list<Object>::iterator it;
        list<Object>::iterator jt;

        n_mean.first += l.size();
        n_mean.second ++;

        if (n_max.first < l.size()) {
            n_max.first = l.size();
            n_max.second = i;
        }

        if (n_min.first > l.size()) {
            n_min.first = l.size();
            n_min.second = i;
        }

//        cout << "FRAME: " << i << endl;

        for (it = l.begin(); it != l.end(); it++) {

            if (sub[(*it).subject] == 0) {
                sub[(*it).subject] = 1;
                nsub++;
            }

            if (x_max.first < (*it).coord.x) {
                x_max.first = (*it).coord.x;
                x_max.second = i;
            }
            if (x_min.first > (*it).coord.x) {
                x_min.first = (*it).coord.x;
                x_min.second = i;
            }
            if (y_max.first < (*it).coord.y) {
                y_max.first = (*it).coord.y;
                y_max.second =i;
            }
            if (y_min.first > (*it).coord.y) {
                y_min.first = (*it).coord.y;
                y_min.second = i;
            }


            for (jt = l.begin(); jt != l.end(); jt++) {
                if ((*it).subject != (*jt).subject) {
                    double d = dist(*it, *jt);
                    if (d_max.first < d) {
                        d_max.first = d;
                        d_max.second = i;
                    }
                    if (d_min.first > d) {
                        d_min.first = d;
                        d_min.second = i;
                    }
                }
            }
            map<int, Object>::const_iterator kt = lastFrame.find((*it).subject);
            if (kt != lastFrame.end()) {
                double v = dist(*it, (*kt).second);
                v_mean.first += v;
                v_mean.second ++;
//               cout << &(*it) << " ";
//               cout << (*it).subject << "(" << (*it).coord.x << "," <<(*it).coord.y << ") at " << (*it).frame << " " ;
//               cout << &((*kt).second) << " "; 
//               cout << (*kt).second.subject << "(" << (*kt).second.coord.x << "," <<(*kt).second.coord.y << ") at " << (*kt).second.frame << " ";
//               cout << v << endl;
                if (v_max.first < v) {
                    v_max.first = v;
                    v_max.second = i;
                }
                if (v_min.first > v) {
                    v_min.first = v;
                    v_min.second = i;
                }
            }
        }
        lastFrame.clear();
        for (it = l.begin(); it != l.end(); it++) {
            //cout << "ADD " << (*it).subject << " " << (*it) << endl;
            lastFrame[(*it).subject] = (*it);
        }
    }
    n_mean.first /= n_mean.second;
    v_mean.first /= v_mean.second;

    cout << endl;
    cout << "Dist max = " << d_max.first << " [" << d_max.second << "]" << endl; 
    cout << "Dist min = " << d_min.first << " [" << d_min.second << "]" << endl; 
    cout << "X max = " << x_max.first << " [" << x_max.second << "]" << endl; 
    cout << "X min = " << x_min.first << " [" << x_min.second << "]" << endl; 
    cout << "Y max = " << y_max.first << " [" << y_max.second << "]" << endl; 
    cout << "Y min = " << y_min.first << " [" << y_min.second << "]" << endl; 
    cout << "N max = " << n_max.first << " [" << n_max.second << "]" << endl; 
    cout << "N min = " << n_min.first << " [" << n_min.second << "]" << endl; 
    cout << "N mean = " << n_mean.first << endl;
    cout << "# frames = " << n_mean.second << endl; 
    cout << "V max = " << v_max.first << " [" << v_max.second << "]" << endl; 
    cout << "V min = " << v_min.first << " [" << v_min.second << "]" << endl; 
    cout << "V mean = " << v_mean.first << endl;
    cout << "# v = " << v_mean.second << endl; 
    cout << "# subjects = " << nsub << endl;

    return 0;
}
