//
// Created by karlos on 6/30/18.
//

#include <algorithm>
#include "timetable.h"

Timetable::Timetable() {
    this->_grouping_col = 0;
    this->_parse();
}

void Timetable::_parse() {
    this->_construct_ok = false;
    DIR* dir;
    dirent* ent;
    if ((dir = opendir(SOURCES_ROOT.c_str())) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            string name(ent->d_name);
            if(name == "." || name == "..") continue;
            this->_parse_file(name);
        }
        closedir (dir);
        this->_parse_options(CONFIG_NAME);
        for(auto & ts: this->_together_sets){
            Subject s;
            unordered_map<int, vector<int>> student_subject;
            for(auto & it: ts){
                this->_subjects_to_ignore.insert(it);
                s.name += this->_subjects[it].name;
                s.name += " + ";
                for(auto & t: this->_subjects[it].teachers){
                    if(s.teachers.find(t) != s.teachers.end()){
                        cout << "Cannot group subject with same teachers!" << endl;
                        throw;
                    }
                    s.teachers.insert(t);
                }
                for(auto & c: this->_subjects[it].classes){
                    s.classes.insert(c);
                }
                for(auto & stud: this->_subjects[it].students){
                    if(s.students.find(stud) != s.students.end()){
                        ++this->_grouping_col;
                    }
                    if(student_subject.find(stud) != student_subject.end()){
                        student_subject[stud].push_back(it);
                    }
                    else{
                        vector<int> tmp;
                        tmp.push_back(it);
                        student_subject.insert(make_pair(stud, tmp));
                    }
                    s.students.insert(stud);
                }
            }
            for(auto & ss: student_subject){
                if(ss.second.size() >= 2){
                    this->_grouping_col_map.insert(ss);
                }
            }
            this->_subjects.push_back(s);
            this->_subject_mapping.insert(make_pair(s.name, this->_subjects.size()-1));
        }
        for(auto & s: this->_subjects){
            cout << s.print(this->_reverse_teacher_mapping, this->_reverse_student_mapping) << endl;
        }
        for(int i = 0; i < this->_together_sets.size(); ++i){
            cout << "TOGETHER SET: ";
            for(auto & it: this->_together_sets[i]){
                cout << this->_subjects[it].name << ", ";
            }
            cout << endl;
        }
        for(int i = 0; i < this->_not_together_sets.size(); ++i){
            cout << "NOT TOGETHER SET: ";
            for(auto & it: this->_not_together_sets[i]){
                cout << this->_subjects[it].name << ", ";
            }
            cout << endl;
        }
        cout << "GROUPING COLLISIONS COUNT: " << this->_grouping_col << endl;
        cout << "GROUPING COLLISIONS: " << endl;
        for(auto & c: this->_grouping_col_map){
            cout << this->_reverse_student_mapping[c.first] << ": ";
            for(int i = 0; i < c.second.size(); ++i){
                cout << this->_subjects[c.second[i]].name << ", ";
            }
            cout << endl;
        }
        this->_construct_ok = true;
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
        while(true){
            if(!getline(f, line)) break;
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
        this->_subject_mapping.insert(make_pair(s.name, this->_subjects.size()-1));
    }
}

void Timetable::_parse_options(const string &name) {
    string line;
    ifstream f(SOURCES_ROOT+"/"+name);
    bool positive;

    if(f.is_open()){
        if(!getline(f, line)) return;
        if(line == "+"){
            positive = true;
        }
        else if(line == "-"){
            positive = false;
        }
        else{
            return;
        }

        unordered_set<int> s;

        while(true){
            if(!getline(f, line)){
                if(!s.empty()){
                    if(positive){
                        this->_together_sets.push_back(s);
                    }
                    else{
                        this->_not_together_sets.push_back(s);
                    }
                }
                return;
            };
            if(line == "+"){
                if(positive){
                    this->_together_sets.push_back(s);
                }
                else{
                    this->_not_together_sets.push_back(s);
                }
                s.clear();
                positive = true;
            }
            else if(line == "-"){
                if(positive){
                    this->_together_sets.push_back(s);
                }
                else{
                    this->_not_together_sets.push_back(s);
                }
                s.clear();
                positive = false;
            }
            else{
                s.insert(this->_subject_mapping[line]);
            }
        }
    }
}

bool Timetable::ok() {
    return this->_construct_ok;
}

bool compar(const pair<int, vector<int>> & a, const pair<int, vector<int>> & b){
    return a.first < b.first;
}

void Timetable::generate() {
    for(int i = 0; i < SUBJECT_CONTAINERS.size(); ++i){
        SubjectContainer c(SUBJECT_CONTAINERS[i], this->_subject_mapping, this->_not_together_sets);
        this->_subject_containers.push_back(c);
    }

    vector<pair<int, vector<int>>> results;
    vector<vector<int>> nextgen;

    for(int i = 0; i < BREED_SIZE; ++i){
        vector<int> perm = this->_random_permutation();
        if(this->_generate_with_permutation(perm)){
            results.push_back(make_pair(this->_total_col+this->_grouping_col, perm));
        }
        else{
            cout << "NO SOLUTION EXISTS!" << endl;
            continue;
        }
        this->_clear();
    }

    cout << "Initial population generated." << endl;

    sort(results.begin(), results.end(), compar);

    for(int iter = 0; iter < 1000; ++iter) {
        for (int i = 0; i < BREED_SIZE*0.2; ++i) {
            nextgen.push_back(results[i].second);
        }
        cout << "Best 20 % preserved." << endl;
        unordered_set<int> idx;
        for (int i = 0; i < BREED_SIZE*0.3; ++i) {
            int x;
            while(true){
                x = random_int(BREED_SIZE / 5, BREED_SIZE-1);
                if(idx.find(x) == idx.end()){
                    idx.insert(x);
                    break;
                }
            }
            nextgen.push_back(results[x].second);
        }
        cout << "Random 30 % preserved." << endl;
        idx.clear();
        while (nextgen.size() < BREED_SIZE) {
            nextgen.push_back(this->_breed(nextgen));
        }
        cout << "Breeded." << endl;
        results.clear();

        for (int i = 0; i < BREED_SIZE; ++i) {
            if (this->_generate_with_permutation(nextgen[i])) {
                results.push_back(make_pair(this->_total_col + this->_grouping_col, nextgen[i]));
            } else {
                results.push_back(make_pair(INT32_MAX, nextgen[i]));
            }
            this->_clear();
        }

        sort(results.begin(), results.end(), compar);

        for(int i = 0; i < 10; ++i){
            cout << "COL: " << results[i].first << ", PERM: ";
            for(int j = 0; j < results[i].second.size(); ++j){
                cout << results[i].second[j] << " ";
            }
            cout << endl;
        }
        cout << "--------------------------" << endl;
        nextgen.clear();
    }
}

bool Timetable::_generate_with_permutation(const vector<int> & permutation) {
    vector<Subject> subjects_copy = this->_subjects;
    vector<Subject> subjects_permuted;
    for(int i = 0; i < permutation.size(); ++i){
        subjects_permuted.push_back(subjects_copy[permutation[i]]);
        subjects_copy.erase(subjects_copy.begin()+permutation[i]);
    }

    for(int i = 0; i < subjects_permuted.size(); ++i){
        if(this->_subjects_to_ignore.find(this->_subject_mapping[subjects_permuted[i].name]) != this->_subjects_to_ignore.end()) continue;
//        cout << "PLACING SUBJECT: " << subjects_permuted[i].name << endl;
        int min = INT32_MAX;
        int min_cont = -1;
        for(int j = 0; j < this->_subject_containers.size(); ++j){
            int tmp = this->_subject_containers[j].test(subjects_permuted[i]);
//            cout << "Container collision count " << j << ": " << tmp << endl;
            if(tmp == -1) continue;
            if(tmp < min){
                min = tmp;
                min_cont = j;
            }
        }
        if(min_cont == -1){
            return false;
        }
        else{
//            cout << "Placing the subject in container " << min_cont << endl;
            this->_subject_containers[min_cont].add(subjects_permuted[i]);
        }
    }

    this->_total_col = 0;
    for(int i = 0; i < this->_subject_containers.size(); ++i){
        this->_total_col += this->_subject_containers[i].collision_cnt();
        for(auto & c: this->_subject_containers[i].collisions()){
            this->_col_map.insert(c);
        }
    }

    return true;
}

vector<int> Timetable::_random_permutation() {
    vector<int> out;
    for(int i = this->_subjects.size()-1; i >= 0; --i){
        out.push_back(random_int(0, i));
    }
    return out;
}

string Timetable::print() {
    string out;
    for(int i = 0; i < this->_subject_containers.size(); ++i){
        out += this->_subject_containers[i].print(this->_reverse_teacher_mapping, this->_subjects);
        out += "\n";
    }

    out += "COLLISION COUNT: ";
    out += to_string(this->_total_col);
    out += "\n";
    out += "COLLISIONS:\n";
    for(auto & c: this->_col_map){
        out += this->_reverse_student_mapping[c.first];
        out += ": ";
        for(int i = 0; i < c.second.size(); ++i){
            out += this->_subjects[c.second[i]].name;
            out += ", ";
        }
        out += "\n";
    }
    return out;
}

void Timetable::_clear() {
    this->_col_map.clear();
    this->_total_col = 0;
    for(int i = 0; i < this->_subject_containers.size(); ++i){
        this->_subject_containers[i].clear();
    }
}

vector<int> Timetable::_breed(const vector<vector<int>> & v) {
    vector<int> out;

    int x1, x2, x3, x4;

    x1 = random_int(0, (BREED_SIZE * 0.5) - 1);
    while (true) {
        x2 = random_int(0, (BREED_SIZE * 0.5) - 1);
        if (x1 != x2) break;
    }
    while (true) {
        x3 = random_int(0, (BREED_SIZE * 0.5) - 1);
        if (x1 != x3 && x2 != x3) break;
    }
    while (true) {
        x4 = random_int(0, (BREED_SIZE * 0.5) - 1);
        if (x1 != x4 && x2 != x4 && x3 != x4) break;
    }

    for(int i = 0; i < 3; ++i){
        out.push_back(v[x1][i]);
    }
    for(int i = 3; i < 7; ++i){
        out.push_back(v[x2][i]);
    }
    for(int i = 7; i < v[0].size()/2; ++i){
        out.push_back(v[x3][i]);
    }
    for(int i = v[0].size()/2; i < v[0].size(); ++i){
        out.push_back(v[x4][i]);
    }
    return out;
}

void Timetable::print_from_perm(const vector<int> &permutation) {
    this->_generate_with_permutation(permutation);
    cout << this->print() << endl;
    this->_clear();
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

SubjectContainer::SubjectContainer(const unordered_set<int> & classes,
                                   const unordered_map<string, int> & subject_mapping,
                                   const vector<unordered_set<int>> & not_together_sets) {
    this->_classes = classes;
    this->_subject_mapping = subject_mapping;
    this->_not_together_sets = not_together_sets;
    this->_collision_cnt = 0;
}

int SubjectContainer::collision_cnt() {
    return this->_collision_cnt;
}

int SubjectContainer::test(const Subject &s) {
    for(auto & t: s.teachers){
        if(this->_teachers.find(t) != this->_teachers.end()){
//            cout << "TEACHER ALREADY PRESENT IN THE CONTAINER!" << endl;
            return -1;
        };
    }

    for(auto & y: s.classes){
        if(this->_classes.find(y) == this->_classes.end()){
//            cout << "SUBJECT CANNOT BE IN THE CONTAINER DUE TO YEARS!" << endl;
            return -1;
        }
    }

    for(int i = 0; i < this->_not_together_sets.size(); ++i){
        if(this->_not_together_sets[i].find(this->_subject_mapping[s.name]) != this->_not_together_sets[i].end()){
            for(auto & subj: this->_subjects){
                if(this->_not_together_sets[i].find(subj) != this->_not_together_sets[i].end()){
//                    cout << "SUBJECT CANNOT BE IN THE CONTAINER DUE TO ADDITIONAL RESTRICTIONS!" << endl;
                    return -1;
                };
            }
        }
    }

    int col = 0;

    for(auto & stud: s.students){
        if(this->_students.find(stud) != this->_students.end()) ++col;
    }

    return col;
}

bool SubjectContainer::add(const Subject &s) {
    int col = this->test(s);
    if(col == -1) return false;

    for(auto & t: s.teachers){
        this->_teachers.insert(t);
    }
    this->_subjects.insert(this->_subject_mapping[s.name]);
    for(auto & stud: s.students){
        if(this->_students.find(stud) == this->_students.end()){
            vector<int> tmp;
            tmp.push_back(this->_subject_mapping[s.name]);
            this->_students.insert(make_pair(stud, tmp));
        }
        else{
            this->_students[stud].push_back(this->_subject_mapping[s.name]);
            ++this->_collision_cnt;
        }
    }

    return true;
}

unordered_map<int, vector<int>> SubjectContainer::collisions() {
    unordered_map<int, vector<int>> ret;
    for(auto & c: this->_students){
        if(c.second.size() > 1){
            ret.insert(c);
        }
    }
    return ret;
}

void SubjectContainer::clear() {
    this->_collision_cnt = 0;
    this->_subjects.clear();
    this->_teachers.clear();
    this->_students.clear();
}

string SubjectContainer::print(const unordered_map<int, string> &reverse_teacher_mapping,
                               const vector<Subject> &reverse_subject_mapping) {
    string out;
    out = "CONTAINER\n";
    out += "YEAR GROUPS: ";
    for(auto & y: this->_classes){
        out += to_string(y);
        out += ", ";
    }
    out += "\n";
    out += "COLLISION COUNT: ";
    out += to_string(this->collision_cnt());
    out += "\n";
    out += "SUBJECTS:\n";
    for(auto & s: this->_subjects){
        out += "  - ";
        out += reverse_subject_mapping[s].name;
        out += " (";
        for(auto & t: reverse_subject_mapping[s].teachers){
            out += reverse_teacher_mapping.at(t);
            out += ", ";
        }
        out += "), ";
        for(auto &y: reverse_subject_mapping[s].classes){
            out += to_string(y);
            out += "r ";
        }
        out += "\n";
    }
    out += "-----------------------------------";
    return out;
}

int random_int(int min, int max){
    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(min,max); // guaranteed unbiased

    return uni(rng);
}