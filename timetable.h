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
#include <random>

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


class SubjectContainer{
public:
    SubjectContainer(const unordered_set<int> & classes, const unordered_map<string, int> & subject_mapping,
                     const vector<unordered_set<int>> & not_together_sets);
    bool add(const Subject & s);
    int test(const Subject & s);
    void clear();
    int collision_cnt();
    unordered_map<int, vector<int>> collisions();
    string print(const unordered_map<int, string> & reverse_teacher_mapping,
                 const vector<Subject> & reverse_subject_mapping);
private:
    unordered_set<int> _classes;
    unordered_set<int> _subjects;
    unordered_set<int> _teachers;
    unordered_map<int, vector<int>> _students;
    unordered_map<string, int> _subject_mapping;
    vector<unordered_set<int>> _not_together_sets;
    int _collision_cnt;
};


class Timetable{
public:
    Timetable();
    bool ok();
    void generate();
    string print();
    void print_from_perm(const vector<int> & permutation);
private:
    unordered_set<int> _subjects_to_ignore;
    void _clear();
    vector<int> _breed(const vector<vector<int>> & v);
    int _total_col;
    int _grouping_col;
    unordered_map<int, vector<int>> _col_map;
    unordered_map<int, vector<int>> _grouping_col_map;
    vector<int> _random_permutation();
    bool _generate_with_permutation(const vector<int> & permutation);
    void _parse();
    void _parse_file(const string & name);
    void _parse_options(const string & name);
    unordered_map<string, int> _teacher_mapping;
    unordered_map<string, int> _student_mapping;
    unordered_map<string, int> _subject_mapping;
    unordered_map<int, string> _reverse_teacher_mapping;
    unordered_map<int, string> _reverse_student_mapping;
    vector<Subject> _subjects;
    vector<unordered_set<int>> _together_sets;
    vector<unordered_set<int>> _not_together_sets;
    bool _construct_ok;
    vector<SubjectContainer> _subject_containers;
};

int random_int(int min, int max);
bool compar(const pair<int, vector<int>> & a, const pair<int, vector<int>> & b);

#endif //ROZVRH_TIMETABLE_H
