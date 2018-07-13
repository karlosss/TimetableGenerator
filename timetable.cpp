//
// Created by karlos on 6/30/18.
//

#include "timetable.h"

Timetable::Timetable() {
    this->_parse();
}

void Timetable::_parse() {
    DIR* dir;
    dirent* ent;
    if ((dir = opendir(SOURCES_ROOT.c_str())) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            string name(ent->d_name);
            if(name == "." || name == "..") continue;
            this->_parse_file(name);
        }
        closedir (dir);
        for(auto & s: this->_subjects){
            cout << s.print(this->_reverse_teacher_mapping, this->_reverse_student_mapping) << endl;
        }
    }
    else {
        cout << "Cannot open directory: " << SOURCES_ROOT << endl;
    }
}

void Timetable::_parse_file(const string &name) {
    string line;
    ifstream f(SOURCES_ROOT+"/"+name);

    string subj_name;

    vector<string> teachers;

    if(f.is_open()){
        if(!getline(f, line)) return;
        unsigned long pos = line.rfind('(');
        if(pos == string::npos) return;
        subj_name = line.substr(0, pos-1);
        line = line.substr(pos+1);
        line = line.substr(0, line.length()-1);
        pos = line.find(" a ");
        teachers.push_back(line.substr(0, pos));
        if(pos != string::npos){
            teachers.push_back(line.substr(pos+3));
        }
        if(!getline(f, line)) return;
        int student_cnt = stoi(line);
        if(!student_cnt) return; // subject has no students

        Subject s;
        s.name = subj_name;

        for(auto & t : teachers){
            if(this->_teacher_mapping.find(t) == this->_teacher_mapping.end()){
                this->_teacher_mapping.insert(make_pair(t, this->_teacher_mapping.size()));
                this->_reverse_teacher_mapping.insert(make_pair(this->_teacher_mapping.size()-1, t));
            }
            s.teachers.insert(this->_teacher_mapping[t]);
        }

        string student;
        string cls;
        for(int i = 0; i < student_cnt; ++i){
            if(!getline(f, line)) return;
            student = line;
            pos = line.rfind(';');
            cls = line.substr(pos+2);
            s.classes.insert(CLASSES_MAPPING.at(cls));
            if(this->_student_mapping.find(student) == this->_student_mapping.end()){
                this->_student_mapping.insert(make_pair(student, this->_student_mapping.size()));
                this->_reverse_student_mapping.insert(make_pair(this->_student_mapping.size()-1, student));
            }
            s.students.insert(this->_student_mapping[student]);
        }

        this->_subjects.push_back(s);
    }
}

string Subject::print(const unordered_map<int, string> &reverse_teacher_mapping,
                      const unordered_map<int, string> &reverse_student_mapping) {
    string out = "SUBJECT: ";
    out += this->name;
    out += "\n";
    out += "TEACHERS: ";
    for(auto & t: this->teachers){
        out += reverse_teacher_mapping.at(t);
        out += ", ";
    }
    out += "\n";
    out += "CLASSES: ";
    for(auto & c: this->classes){
        out += to_string(c);
        out += ", ";
    }
    out += "\n";
    out += "STUDENTS:\n";
    for(auto & s: this->students){
        out += reverse_student_mapping.at(s);
        out += "\n";
    }
    out += "-------------------------------------------";
    return out;
}
