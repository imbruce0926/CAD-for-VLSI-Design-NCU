// PA3_Floorplanning
// Input file: latency constrain, info of node connection
// Output file: number of ADD & MUL used, executing vertex in each timing
// This program use List Scheduling algorithm under MR-LCS restrition
// 2023/3/31 Version 4
// Author: Pin Hsien Lee

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <stack>
#include <queue>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <random>

#define seed         5
#define iter_temp    50
#define freezing     0.1
// #define pts          300
#define init_P       0.99
#define t0           -1
#define iter_T       5
#define cost_f       100


using namespace std;

    class Size {
    public:
        double width, height;
    };
    
    class Module {
    public:
        double area;
        Size aspect;  
    };
    
    class Block {
    public:
        int Lindex, Rindex;
        Size aspect;
    };

    class Node {
    public:
        class Node *left, *right, *parent;
        string data;
        int choosen;
        bool update;
        vector<Block> blks;
    };

    class Floorplanning { 
    public:
        Floorplanning();
        void read_file(fstream &, string, clock_t);

    private:
        void write_file(fstream &);
        void choose_possible_size(int, Node *);
        Node * SPE_to_tree(vector<string>);
        Node * creat_node(string, bool);
        void bottom_up(Node *);
        double max(double, double);
        bool is_equal(double, double);
        int choose_min_area(Node *);
        void top_down(Node *);
        void M1(bool, Node **, Node **, int *, int *);
        void M2(bool, Node **, int *, int *);
        Node * M3(int *, int *);
        bool legal_check(int);
        int find_int(int);
        int find_VH(int);
        void find_int_VH(int, int *, int *);
        void print(Node *ptr);
        void find_node_ij(Node **, Node **, int, int);
        void find_node_i(Node **, int, int);
        vector<string> SA(vector<string> &);
        void update_cooling_factor(double *, double);
        int select_move();
        void postorder(Node *);
        bool is_movable(double, double);
        void delete_tree(Node *);
        void swap(int, int);
        double init_temp();
        double calculate_cost(Node *);
        vector<string> init_SPE(vector<string> &);

        int moduleCount;
        double total_area;
        Node *root, *avail;
        vector<Module> module;
        vector<string> SPE;
        vector<Size> valid_size;
        string outputname;
        fstream myFile;
        clock_t start;
        double global_cost;
        int pts;
    };

    bool mycompare(Block, Block);

int main(int argc, char *argv[]) {

    fstream myFile;
    Floorplanning F;
    string intputFile = argv[1]; 
    string outputFile = argv[2];

    clock_t start;
    start = clock();
    
    myFile.open(intputFile, ios::in);
    F.read_file(myFile, outputFile, start);
    myFile.close();

    intputFile = "t20.txt";
    outputFile = "t20_out.txt";
    start = clock();
    cout << "time = " << double(clock()-start)/CLOCKS_PER_SEC << " s"<<endl;

    myFile.open(intputFile, ios::in);
    F.read_file(myFile, outputFile, start);
    myFile.close();
    cout << "time = " << double(clock()-start)/CLOCKS_PER_SEC << " s"<<endl;

    intputFile = "t50.txt";
    outputFile = "t50_out.txt";
    start = clock();

    myFile.open(intputFile, ios::in);
    F.read_file(myFile, outputFile, start);
    myFile.close();
    cout << "time = " << double(clock()-start)/CLOCKS_PER_SEC << " s"<<endl;

    return 0;
}

Floorplanning::Floorplanning() {
    avail = NULL;
    root = NULL;
    srand(seed);
    total_area = 0;
    global_cost = 1.79769e+308;
}

void Floorplanning::read_file(fstream& myFile, string outputfile, clock_t start_time) {

    double temp;
    Module temp_; 
    Node *tmp = NULL;

    outputname = outputfile;
    if(outputfile == "t500_out.txt")
        pts = 10;
    else
        pts = 300;

    start = start_time;

    myFile >> moduleCount;

    while(myFile >> temp) {
        myFile >> temp;
        module.push_back(temp_);
        module.back().area = temp;
        total_area += temp;
    }
    cout << "total area is " << fixed << setprecision(2)<< total_area << endl;

    for(int i = 0; i < 4 * moduleCount-2; i++) {
        tmp = creat_node("", false);
        tmp->right = avail;
        avail = tmp;
    }

    vector<string> spe;
    spe = init_SPE(spe);

    root = SPE_to_tree(spe);
    bottom_up(root);
    top_down(root);

    global_cost = calculate_cost(root);
    
    myFile.open(outputname, ios::out);
    write_file(myFile);
    myFile.close();

    cout << endl << fixed << setprecision(2) << "global best : " << root->blks[0].aspect.width * root->blks[0].aspect.height << endl;
    postorder(root);
    cout << endl;
    cout<< "**********************************************************" << endl;
    cout << endl;
    delete_tree(root);
    

    while((double(clock()-start)/CLOCKS_PER_SEC) < 7199) {
        spe = SA(spe);
        // cout << "time = " << double(clock()-start)/CLOCKS_PER_SEC << " s" << endl;
    }
}

void Floorplanning::postorder(Node *ptr) {
    if(ptr) {
        postorder(ptr->left);
        postorder(ptr->right);
        cout << ptr->data << " ";
    }
}

vector<string> Floorplanning::init_SPE(vector<string> &spe) {
    spe.clear();
    spe.push_back("0");
    spe.push_back("1");
    spe.push_back("V");
    for(int i = 2; i < moduleCount; i++) {
        string x = to_string(i);
        spe.push_back(x);
        if(i % 3 == 0)
            spe.push_back("V");
        else if(i % 3 == 1)
            spe.push_back("V");
        else
            spe.push_back("V");
    }
    return spe;
}

vector<string> Floorplanning::SA(vector<string> &spe) {

    vector<string> best_SPE;
    Node *temp_root = NULL, *iptr = NULL, *jptr = NULL;
    double T0, T, w, curr_cost, new_cost, delta_cost, best_cost = 1.79769e+308;
    int Move_type = 0, MoveCount = 0, uphill = 0, N, reject = 0, i, j;

    SPE = spe;

    N = iter_T * moduleCount;
    T0 = init_temp();
    T = T0;

    SPE = spe;

    // cout << "T0 is " << T0 << endl;

    root = SPE_to_tree(SPE);
    bottom_up(root);
    top_down(root);

    best_cost = calculate_cost(root);
    best_SPE = SPE;

    cout << "-----------------------------------------------------------" << endl << endl;

    do {
        MoveCount = 0; uphill = 0; reject = 0;
        update_cooling_factor(&w, T);
        
        do {
            curr_cost = calculate_cost(root);
            Move_type = select_move();    

            // cout << endl << "Move type is " << Move_type << endl;

            switch(Move_type) {
                case 1:
                    M1(true, &iptr, &jptr, &i, &j);
                    new_cost = calculate_cost(root);
                    break;
                case 2:
                    M2(true, &iptr, &i, &j);
                    new_cost = calculate_cost(root);
                    break;
                case 3:
                    temp_root = M3(&i, &j);
                    if(temp_root) 
                        new_cost = calculate_cost(temp_root);
                    else
                        new_cost = curr_cost;
                    break;
            }
            delta_cost = new_cost - curr_cost;
            MoveCount++;
            
            if(is_movable(delta_cost, T)) {
                if(Move_type == 3 && temp_root) {
                    delete_tree(root);
                    root = temp_root;
                }
                if(delta_cost > 0)
                    uphill++;
                    
                if(new_cost < best_cost) {
                    cout << "Move_type is " << Move_type << endl;
                    best_cost = new_cost;
                    best_SPE = SPE;

                    cout << fixed << setprecision(2) << "best : " << root->blks[0].aspect.width * root->blks[0].aspect.height << endl;
                    postorder(root);
                    cout << endl << endl;

                    myFile.open(outputname, ios::out);
                    write_file(myFile);
                    myFile.close();
    
                }
            }
            else {
                switch(Move_type) {
                    case 1:
                        M1(false, &iptr, &jptr, &i, &j);
                        break;
                    case 2:
                        M2(false, &iptr, &i, &j);
                        break;
                    case 3:
                        if(temp_root)
                            delete_tree(temp_root);                        
                        swap(i, j);
                        break;
                }
                reject++;
            }
        }
        while((uphill < N) && (MoveCount < 2 * N));
        cout << "T= " << T  << " time = " << double(clock()-start)/CLOCKS_PER_SEC << " s"<<endl;
        T = w * T;
    }
    while(((double)reject/MoveCount) < 0.95 && (T > freezing) && (double(clock()-start)/CLOCKS_PER_SEC) < 7199);
    
    delete_tree(root);
    return best_SPE;
}

void Floorplanning::update_cooling_factor(double *w, double T) {
    if(T > 2000) *w = 0.85;
    else if (T <= 2000 && T > 1000) *w = 0.9;
    else if (T <= 1000 && T > 500) *w = 0.95;
    else if (T <= 500 && T > 200) *w = 0.96;
    else if (T <= 200 && T > 10) *w = 0.97;
    else if (T <= 10 && T > 0.1) *w = 0.98;
}

double Floorplanning::calculate_cost(Node *ptr) {
    double cost;
    return cost = cost_f * (ptr->blks[0].aspect.width * ptr->blks[0].aspect.height)/total_area;
    // return cost = (ptr->blks[0].aspect.width * ptr->blks[0].aspect.height);
}

void Floorplanning::M1(bool control, Node **iptr, Node **jptr, int *i, int *j) {

    int x;

    if(control){
        x = rand() % SPE.size();
        if(SPE[x] == "V" || SPE[x] == "H") 
            (*i) = find_int(x);
        else 
            (*i) = x;

        (*j) = find_int((*i));
            
        find_node_ij(&(*iptr), &(*jptr), (*i), (*j));
    }
    swap((*i), (*j));

    Node *parent_i = (*iptr)->parent;
    Node *parent_j = (*jptr)->parent;

    if(parent_i != parent_j) {
        if(parent_i->left == (*iptr))
            parent_i->left = (*jptr);
        else
            parent_i->right = (*jptr);
        if(parent_j->right == (*jptr))
            parent_j->right = (*iptr);
        else
            parent_j->left = (*iptr);
    }
    else {
        if(parent_i->left == (*iptr)){
            parent_i->left = (*jptr);
            parent_i->right = (*iptr);
        }
        else {
            parent_i->right = (*jptr);
            parent_i->left = (*iptr);
        }
    }

    (*iptr)->parent = parent_j;
    (*jptr)->parent = parent_i;
    
    for(Node *temp = (*iptr)->parent; temp; temp = temp->parent)
        temp->update = false;
    
    for(Node *temp = (*jptr)->parent; temp; temp = temp->parent) {
        temp->update = false;
        if(temp->parent && temp->parent->update == false)
            break;
    }

    bottom_up(root);
    top_down(root);
}

void Floorplanning::M2(bool control, Node **iptr, int *i, int *j) {

    int x;
    if(control){
        x = rand() % SPE.size();
    
        if(SPE[x] != "V" && SPE[x] != "H")
            x = find_VH(x);

        for(int k = x; k < SPE.size(); k++) {
            if(SPE[k] == "V" || SPE[k] == "H") 
                (*j) = k;
            else
                break;
        }
        for(int k = x; k >= 0; k--) {
            if(SPE[k] == "V" || SPE[k] == "H") 
                (*i) = k;
            else
                break;
        }
    }

    find_node_i(&(*iptr), (*i), (*j));

    for(int k = (*i); k <= (*j); k++) {
        if(SPE[k] == "V")
            SPE[k] = "H";
        else
            SPE[k] = "V";
    }

    for(Node *temp = (*iptr); temp; temp = temp->parent) 
        temp->update = false;

    bottom_up(root);
    top_down(root);
}

Node * Floorplanning::M3(int *i, int *j) {
    
    int x, counter = 0; 
    Node *temp = NULL;

    while(counter < 10) {
        x = rand() % (SPE.size() - 1);
        find_int_VH(x, &(*i), &(*j));
        swap((*i), (*j));
        if(!legal_check((*j))) 
            swap((*i), (*j));
        else{
            break;
        }
        counter++;
    }
    if(counter != 10) { 
        temp = SPE_to_tree(SPE);
        bottom_up(temp);
        top_down(temp);
        return temp;
    }
    return NULL;
}

int Floorplanning::select_move() {
    return rand() % 3 + 1;;
}

double Floorplanning::init_temp() {
    int Move_type, n = 0, uphill = 0, i, j;
    double total_cost = 0, curr_cost = 0, new_cost = 0, ave_cost = 0;
    Node *iptr = NULL, *jptr = NULL, *temp_root = NULL;

    SPE = init_SPE(SPE);
    root = SPE_to_tree(SPE);
    bottom_up(root);
    top_down(root);

    curr_cost = calculate_cost(root);

    while(n < iter_temp) {
        double diff = 0;
        Move_type = select_move();
        switch(Move_type) {
            case 1:
                M1(true, &iptr, &jptr, &i, &j);
                new_cost = calculate_cost(root);
                break;
            case 2:
                M2(true, &iptr, &i, &j);
                new_cost = calculate_cost(root);
                break;
            case 3:
                temp_root = M3(&i, &j);

                if(temp_root){
                    new_cost = calculate_cost(temp_root);
                    delete_tree(root);
                    root = temp_root;
                    bottom_up(root);
                    top_down(root);
                }
                else
                    new_cost = curr_cost;
                break;
        }

        diff = new_cost - curr_cost;

        if(diff > 0) {
            total_cost += diff;
            uphill++;
            curr_cost = new_cost;
        }
        n++;
    }

    ave_cost = total_cost / uphill;

    delete_tree(root);
    return t0 * ave_cost / log(init_P);
}

void Floorplanning::bottom_up(Node *ptr) {
    if(!ptr)
        return; 

    bottom_up(ptr->left);
    bottom_up(ptr->right);

    vector<Block> blks;
    Block temp;

    if((ptr->data == "V" || ptr->data == "H") && ptr->update == false) {
        vector<vector<Size>> all_size(ptr->left->blks.size()+2, vector<Size>(ptr->right->blks.size()+2));

        if(ptr->data == "V") {
            for(int i = 1; i <= ptr->left->blks.size(); i++) {
                for(int j = 1; j <= ptr->right->blks.size(); j++) {
                    all_size[i][j].width = ptr->left->blks[i-1].aspect.width + ptr->right->blks[j-1].aspect.width;
                    all_size[i][j].height = max(ptr->left->blks[i-1].aspect.height, ptr->right->blks[j-1].aspect.height);

                    if(!((is_equal(all_size[i][j].width, all_size[i-1][j].width) && all_size[i][j].height > all_size[i-1][j].height) || (is_equal(all_size[i][j].height, all_size[i-1][j].height) && all_size[i][j].width > all_size[i-1][j].width) || (is_equal(all_size[i][j].width, all_size[i][j-1].width) && all_size[i][j].height > all_size[i][j-1].height) || (is_equal(all_size[i][j].height, all_size[i][j-1].height) && all_size[i][j].width > all_size[i][j-1].width))) {
                        temp.aspect = all_size[i][j];
                        temp.Lindex = i-1;
                        temp.Rindex = j-1;
                        blks.push_back(temp);
                    }
                }
            }
        }
        else {
            for(int i = ptr->left->blks.size(); i >= 1; i--) {
                for(int j = ptr->right->blks.size(); j >= 1; j--) {
                    all_size[i][j].height = ptr->left->blks[i-1].aspect.height + ptr->right->blks[j-1].aspect.height;
                    all_size[i][j].width = max(ptr->left->blks[i-1].aspect.width, ptr->right->blks[j-1].aspect.width);

                    if(!((is_equal(all_size[i][j].width, all_size[i+1][j].width) && all_size[i][j].height > all_size[i+1][j].height) || (is_equal(all_size[i][j].height, all_size[i+1][j].height) && all_size[i][j].width > all_size[i+1][j].width) || (is_equal(all_size[i][j].width, all_size[i][j+1].width) && all_size[i][j].height > all_size[i][j+1].height) || (is_equal(all_size[i][j].height, all_size[i][j+1].height) && all_size[i][j].width > all_size[i][j+1].width))) {
                        temp.aspect = all_size[i][j];
                        temp.Lindex = i-1;
                        temp.Rindex = j-1;
                        blks.push_back(temp);
                    }
                }
            }
        }
        ptr->blks.clear();

        sort(blks.begin(), blks.begin()+blks.size(), mycompare);

        if(blks.size() > pts) 
            for(int i = 0 ; i < pts; i++)     // pts or blks.size(), this condition is changlable
                ptr->blks.push_back(blks[i]);
        else 
            for(int i = 0 ; i < blks.size(); i++)     // pts or blks.size(), this condition is changlable
                ptr->blks.push_back(blks[i]);
        
    }
}

void Floorplanning::top_down(Node *ptr) {

    queue<Node *> q;

    ptr->choosen = 0;
    ptr->update = true;

    q.push(ptr);

    while(true){        
        if(!q.empty()) {
            ptr = q.front();
            q.pop();
        }
        else 
            break;

        if(ptr->left && ptr->right) {
                ptr->left->choosen = ptr->blks[ptr->choosen].Lindex;
                ptr->right->choosen = ptr->blks[ptr->choosen].Rindex;
                ptr->left->update = true;
                ptr->right->update = true;
                q.push(ptr->left);
                q.push(ptr->right);
        }
        else {
            module[stoi(ptr->data)].aspect = ptr->blks[ptr->choosen].aspect;
            ptr->update = true;
        }
    }
}

void Floorplanning::write_file(fstream& myFile) {
    
    myFile << fixed << setprecision(10) << root->blks[0].aspect.width << " " << fixed << setprecision(10) << root->blks[0].aspect.height << " " << fixed << setprecision(10) << root->blks[0].aspect.width * root->blks[0].aspect.height << endl;
    for(int i = 0; i < module.size(); i++)
        myFile << fixed << setprecision(10) << module[i].aspect.width << " " << fixed << setprecision(10) << module[i].aspect.height << endl;
        
    for(int i = 0; i < SPE.size(); i++) 
        myFile << SPE[i] << " ";

}

void Floorplanning::print(Node *ptr) {
    
    cout << fixed << setprecision(10) << ptr->blks[0].aspect.width << " " << fixed << setprecision(10) << ptr->blks[0].aspect.height << " " << fixed << setprecision(10) << fixed << setprecision(2) << ptr->blks[0].aspect.width * ptr->blks[0].aspect.height << endl;
    for(int i = 0; i < module.size(); i++)
        cout << fixed << setprecision(10) << module[i].aspect.width << " " << fixed << setprecision(10) << module[i].aspect.height << endl;
        
    for(int i = 0; i < SPE.size(); i++) 
        cout << SPE[i] << " ";

    cout << endl;
}

bool Floorplanning::legal_check(int j) {
    int operandCount = 0, operatorCount = 0;

    if(j == SPE.size()-1) {
        if(SPE[j-2] == SPE[j-1]) 
            return false;
    }
    else if(j == 1) {
        if(SPE[j] == SPE[j+1]) 
            return false;
    }
    else {
        if(SPE[j-2] == SPE[j-1] || SPE[j] == SPE[j+1]) 
            return false;
    }
        
    for(int i = 0; i <= j; i++) {
        if(SPE[i] == "V" || SPE[i] == "H")
            operatorCount++;
        else
            operandCount++;
        if(operatorCount >= operandCount) {
            return false;      
        }  
    }
    return true;
}

bool Floorplanning::is_movable(double delta_cost, double T) {
    if(delta_cost <= 0)
        return true;
    
    double x = (double) rand() / (RAND_MAX + 1.0);

    if(x < exp(-(delta_cost/T)))
        return true;

    return false;
}

void Floorplanning::swap(int i, int j) {
    string tmp = SPE[i];
    SPE[i] = SPE[j];
    SPE[j] = tmp;
}

void Floorplanning::find_node_ij(Node **iptr, Node **jptr, int i, int j) {

    stack<Node *> s1, s2;
    Node *temp = NULL;
    int flag = 0;

    s1.push(root);
    while(!s1.empty()) {
        temp = s1.top();
        s1.pop();
        s2.push(temp);
        if(temp->left) 
            s1.push(temp->left);
        if(temp->right) 
            s1.push(temp->right);
    }
    while(!s2.empty()) {
        temp = s2.top();
        s2.pop();

        if(temp->data == SPE[i]) {
            (*iptr) = temp;
            if(!flag)
                flag = 1;
            else 
                break;
        }
        if(temp->data == SPE[j]) {
            (*jptr) = temp;
            if(!flag)
                flag = 1;
            else 
                break;
        }
    }
}

void Floorplanning::find_node_i(Node **iptr, int i, int j) {

    stack<Node *> s1, s2;
    Node *temp = NULL;
    int flag = 0;

    s1.push(root);
    while(!s1.empty()) {
        temp = s1.top();
        s1.pop();
        s2.push(temp);
        if(temp->left) 
            s1.push(temp->left);
        if(temp->right) 
            s1.push(temp->right);
    }
    while(!s2.empty()) {
        temp = s2.top();
        s2.pop();

        if(flag && i-j != 1) {
            if(temp->data == "V")
                temp->data = "H";
            else
                temp->data = "V";
            j--;
        }
        
        if(i - j == 1)
            break;

        if(temp->data == SPE[i-1]) {
            (*iptr) = temp->parent;
            flag = 1;
        }
    }

}

void Floorplanning::find_int_VH(int x, int *i, int *j) {
    
    if(SPE[x] == "V" || SPE[x] == "H") {
        for(int k = x; k < SPE.size()-1; k++) {
            if(SPE[k+1] != "V" && SPE[k+1] != "H") {
                (*i) = k;
                (*j) = k+1;
                return;
            }  
        }
        
        for(int k = x; k > 0; k--) {
            if(SPE[k-1] != "V" && SPE[k-1] != "H") {
                (*i) = k-1;
                (*j) = k;
                return;
            }  
            
        }
    }
    else {
        for(int k = x; k < SPE.size()-1; k++) {
            if(SPE[k+1] == "V" || SPE[k+1] == "H") {
                (*i) = k;
                (*j) = k+1;
                return;
            }  
        }
        for(int k = x; k > 0; k--) {
            if(SPE[k-1] == "V" || SPE[k-1] == "H") {
                (*i) = k-1;
                (*j) = k;
                return;
            }  
        }
    }
}

int Floorplanning::find_int(int x) {
    for(int k = x; k < SPE.size(); k++) {
        if(k == x) 
            continue;
        if(SPE[k] != "V" && SPE[k] != "H") {
            return k;
        }
    }
    for(int k = x; k >= 0; k--) {
        if(k == x) 
            continue;
        if(SPE[k] != "V" && SPE[k] != "H") {
            return k;
        }
    }
    return -1;
}

int Floorplanning::find_VH(int x) {
    for(int k = x; k < SPE.size(); k++) {
        if(k == x)
            continue;
        if(SPE[k] == "V" || SPE[k] == "H") {
            return k;
        }    
    }
    for(int k = x; k >= 0; k--) {
        if(k == x) 
            continue;
        if(SPE[k] == "V" || SPE[k] == "H") {
            return k;
        }
    }
    return -1;
}

void Floorplanning::choose_possible_size(int index, Node *ptr) { // no need to **!!!!!!!!!!!!!!!!
    
    double left, right;
    Block temp;

    left = sqrt(module[index].area / 1.9999999);
    right = sqrt(1.9999999 * module[index].area);
    
    for(int j = 0; j < pts; j++) {
        temp.aspect.width = left + (j * ((right-left)/(pts-1)));
        temp.aspect.height = module[index].area / temp.aspect.width;
        ptr->blks.push_back(temp);
    }
}

bool mycompare(Block a, Block b){
   return a.aspect.width * a.aspect.height < b.aspect.width * b.aspect.height;
}

Node *Floorplanning::creat_node(string data, bool is_avail) {
    Node *temp;
    if(!is_avail) 
        temp = new Node();
    else {
        temp = avail;
        avail = avail->right;
    }  
    temp->data = data;
    temp->left = NULL;
    temp->right = NULL;
    temp->update = false;
    return temp;
}

double Floorplanning::max(double a, double b) {
    if(a > b)
        return a;
    else
        return b;
}

bool Floorplanning::is_equal(double a, double b) {
    if((a-b) < 0.00000001 && (a-b) > -0.00000001)
        return true;
    else
        return false;
}

Node * Floorplanning::SPE_to_tree(vector<string> SPE) {

    stack<Node *> s;
    Node *temp = NULL;

    for(int i = 0; i < SPE.size(); i++) {
        if(SPE[i] == "V" || SPE[i] == "H") {
            if(SPE[i] == "V") 
                    temp = creat_node("V", true);
            else 
                    temp = creat_node("H", true);
            temp->right = s.top();
            temp->right->parent = temp;
            s.pop();

            temp->left = s.top();
            temp->left->parent = temp;
            s.pop();
            
            s.push(temp);
        }
        else { 
            temp = creat_node(SPE[i], true);
            choose_possible_size(stoi(SPE[i]), temp);
            s.push(temp);
        }
    }
    temp = s.top();
    temp->parent = NULL;
    s.pop();
    return temp;
}

void Floorplanning::delete_tree(Node *ptr) {
    if(ptr){
        delete_tree(ptr->left);
        delete_tree(ptr->right);
        if(ptr->parent) {
            if(ptr->parent->left == ptr)
                ptr->parent->left = NULL;
            else
                ptr->parent->right = NULL;
            ptr->parent = NULL;
        }
        ptr->blks.clear();
        ptr->right = avail;
        avail = ptr;
    }
}

// vector<string> Floorplanning::init_cond(int length) {
//     vector<string> temp;
//     if(length/4==0)
//         return temp;

//     vector<string> PE = init_cond(length / 4);
//     int result = length / 4;
//     int remainder = length % 4;
    
//     switch(remainder) {
//         case 1:
//             PE.push_back("Num");
//             break;
//         case 2:
//             PE.push_back("Num");
//             PE.push_back("Num");
//             PE.push_back("V");
//             break;
//         case 3:
//             PE.push_back("Num");
//             PE.push_back("Num");
//             PE.push_back("V");
//             PE.push_back("Num");
//             PE.push_back("H");
//             break;

//     }
// }