//
// Created by karlos on 7/13/18.
//

#ifndef ROZVRH_CONFIG_H
#define ROZVRH_CONFIG_H

#include <iostream>
#include <unordered_map>
#include <vector>
#include <unordered_set>

using namespace std;

const string CONFIG_NAME = "cfg";
const string SOURCES_ROOT = "/home/karlos/timetable_sources";
const vector<unordered_set<int>> SUBJECT_CONTAINERS = {
        {2},
        {4},
        {4},
        {3, 4},
        {3, 4},
        {3, 4},
        {3, 4},
        {3, 4},
        {3, 4}
};

const unordered_map<string, int> CLASSES_MAPPING = {
        {"1.A", 2},
        {"1.B", 2},
        {"1.C", 2},
        {"R5.A", 2},
        {"2.A", 3},
        {"2.B", 3},
        {"2.C", 3},
        {"R6.A", 3},
        {"3.A", 4},
        {"3.B", 4},
        {"3.C", 4},
        {"R7.A", 4},
};


#endif //ROZVRH_CONFIG_H
