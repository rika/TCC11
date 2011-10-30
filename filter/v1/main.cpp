#include <iostream>

#include "DataFilter.hpp"
#include "Object.hpp"

#define FILENAME "gpoints.conf"
#define START_FRAME 1510
#define END_FRAME   2999

int main() {

    DataFilter data(FILENAME, START_FRAME, END_FRAME);

    for (int i = START_FRAME; i <= END_FRAME; i++) {
    
    
    }


    return 0;
}
