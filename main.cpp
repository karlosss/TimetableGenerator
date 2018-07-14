#include <iostream>

#include "timetable.h"

int main(int argc, char** argv) {
    Timetable t;
    if(!t.ok()) return 0;
    t.generate();
}
