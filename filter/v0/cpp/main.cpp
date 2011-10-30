


#define FILENAME "gpoints.conf"
#define START_FRAME 1510
#define END_FRAME   2999

using namespace cv;

int main(int argc, char *argv[])
{
    FilterData data(FILENAME, START_FRAME, END_FRAME);

    while((Point start = data->getStart()) != NULL) {
        Tracker tracker(start);

        list<Point> holes;
        list<Point> fails;

        while (!done) {
            Point p = tracker->predict();
            Point v = data->locate(p);
            if (v != NULL) {
                holes U= fails;
                clear(fails);
                tracker->correct(v);
            }

            else {
                fails U= p;
            }
            frame ++/--; // done?
        }

        data->remove(found);
        subject[id++] = found U holes;
        delete(holes);
    }

    // write subj;

    return 0;
}

