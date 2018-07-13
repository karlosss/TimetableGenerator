//
// Created by karlos on 6/30/18.
//

#ifndef ROZVRH_TIMETABLE_H
#define ROZVRH_TIMETABLE_H

#include <iostream>
#include <string>
#include <fstream>
#include <dirent.h>
#include <vector>

#include "config.h"

using namespace std;


class Subject{
public:
    unordered_set<int> classes;
    string name;
    unordered_set<int> teachers;
    unordered_set<int> students;
    string print(const unordered_map<int, string> & reverse_teacher_mapping,
                 const unordered_map<int, string> & reverse_student_mapping);
};


class Timetable{
public:
    Timetable();
private:
    void _parse();
    void _parse_file(const string & name);
    unordered_map<string, int> _teacher_mapping;
    unordered_map<string, int> _student_mapping;
    unordered_map<int, string> _reverse_teacher_mapping;
    unordered_map<int, string> _reverse_student_mapping;
    vector<Subject> _subjects;
};

#endif //ROZVRH_TIMETABLE_H
