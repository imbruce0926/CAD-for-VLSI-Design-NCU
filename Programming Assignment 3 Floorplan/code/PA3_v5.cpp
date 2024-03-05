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
        void read_file(fstream &);
        void write_file(fstream &);
        void choose_possible_size(int, Node **);
        Node *  SPE_to_tree(vector<string>);
        Node * creat_node(string data);
        void bottom_up(Node *);
        void postorder(Node *);
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
        void SA();
        void update_cooling_factor(double *, double);
        int select_move();
        void postorder2(Node *);
        bool is_movable(double, double);
        void delete_tree(Node *);
        void swap(int, int);



    private:
        int moduleCount, pts;
        double total_area;
        Node *root;
        vector<Module> module;
        vector<string> SPE;
        vector<Size> valid_size;

    };

    bool mycompare(Block, Block);

int main(int argc, char *argv[]) {

    fstream myFile;
    Floorplanning F;
    string intputFile = argv[1]; 
    string outputFile = intputFile.substr(0, intputFile.find(".")) + "_out.txt";
    
    myFile.open(intputFile, ios::in);
    if(!myFile) {
        cerr << "Error opening file" << endl;
        return 1;
    }
    else {
        F.read_file(myFile);
        myFile.close();
    }
    
    myFile.open(outputFile, ios::out);
    // F.write_file(myFile);
    myFile.close();
    cout << endl << "end main";
    return 0;
}

Floorplanning::Floorplanning() {
    root = NULL;
    SPE.push_back("0");
    SPE.push_back("1");
    SPE.push_back("V");
    pts = 10;
    srand(7);
    total_area = 0;
}

void Floorplanning::read_file(fstream& myFile) {

    double temp;
    Module temp_; 

    myFile >> moduleCount;

    while(myFile >> temp) {
        myFile >> temp;
        module.push_back(temp_);
        module.back().area = temp;
        total_area += temp;
    }
    cout << total_area << endl;

    for(int i = 2; i < moduleCount; i++) {
        string x = to_string(i);
        SPE.push_back(x);
        SPE.push_back("V");
    }

    SA();

    // Node *iptr, *jptr;
    // int i, j;
    // root = SPE_to_tree(SPE);
    // bottom_up(root);
    // top_down(root);
    // postorder2(root);
    // cout << endl;
    
    // for(int k = 0; k < 3; k++) {    
    //     cout << endl;
    //     M2(true, &iptr, &i, &j);
    //     postorder2(root);
    //     cout << endl;
    //     // cout << "-----------------------------------------------------------------" << endl;
    //     // print();
    //     // M2(false, &iptr, &i, &j);
    //     // postorder2(root);
    //     // cout << endl;
    // }
    // for(int k = 0; k < 3; k++) {    
    //     cout << endl;
    //     M3();
    //     postorder2(root);
    //     cout << endl;
    //     cout << "-----------------------------------------------------------------" << endl;
    //     // print();
    //     // M2(false, &iptr, &i, &j);
    //     // postorder2(root);
    //     // cout << endl;
    // }

    
}

Node * Floorplanning::SPE_to_tree(vector<string> SPE) {

    stack<Node *> s;
    Node *temp = NULL;

    for(int i = 0; i < SPE.size(); i++) {
        if(SPE[i] == "V" || SPE[i] == "H") {
            if(SPE[i] == "V")
                temp = creat_node("V");
            else    
                temp = creat_node("H");

            temp->right = s.top();
            temp->right->parent = temp;
            s.pop();

            temp->left = s.top();
            temp->left->parent = temp;
            s.pop();
            
            s.push(temp);
        }
        else {
            temp = creat_node(SPE[i]);
            choose_possible_size(stoi(SPE[i]), &(temp));
            s.push(temp);
        }
    }
    temp = s.top();
    temp->parent = NULL;
    s.pop();
    return temp;
}

void Floorplanning::bottom_up(Node *ptr) {
    if(ptr) {
        bottom_up(ptr->left);
        bottom_up(ptr->right);
        // cout << "it's " << ptr->data << endl; 

        vector<Block> blks;
        Block temp;

        if((ptr->data == "V" || ptr->data == "H") && ptr->update == false) {
            // cout << "data is " << ptr->left->data << " & " << ptr->right->data << endl;
            // cout << "size is "<< ptr->left->blks.size() << " * " << ptr->right->blks.size() << endl;
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
            
            
            // for(int i = 0; i < pts; i++) {
            //     cout << ptr->blks[i].aspect.width << " * " << ptr->blks[i].aspect.height << " = " << ptr->blks[i].aspect.width * ptr->blks[i].aspect.height << endl;
            // }
            // cout << endl;
        }
        else {
            // cout << "area: " << module[ptr->data-48].area << endl;
            // for(int i = 0; i < ptr->blks.size(); i++){
            //     cout << "= " <<  ptr->blks[i].aspect.width << " * " << ptr->blks[i].aspect.height;
            //     cout << " / " << ptr->blks[i].aspect.height / ptr->blks[i].aspect.width << endl;
            // }
            // cout << endl;
        }
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
            if(ptr->left->update == false) {
                ptr->left->choosen = ptr->blks[ptr->choosen].Lindex;
                ptr->left->update = true;
                q.push(ptr->left);
            }
            if(ptr->right->update == false){
                ptr->right->choosen = ptr->blks[ptr->choosen].Rindex;
                ptr->right->update = true;
                q.push(ptr->right);
            }
        }
        else 
            module[stoi(ptr->data)].aspect = ptr->blks[ptr->choosen].aspect;
    }
}

void Floorplanning::SA() {

    vector<Module> best_module;
    vector<string> best_SPE;
    Node *temp_root = NULL, *best_root = NULL, *iptr, *jptr;
    double T0 = 30000, freezing = 0.1, T = T0, w, curr_cost, new_cost, delta_cost, best_cost = 1.79769e+308;
    int Move_type = 0, MoveCount = 0, uphill = 0, N = 5 * moduleCount, reject = 0, i, j;

    root = SPE_to_tree(SPE);
    bottom_up(root);
    top_down(root);

    int test;

    do {

        MoveCount = 0; uphill = 0; reject = 0;
        update_cooling_factor(&w, T);
        curr_cost = 100*(root->blks[0].aspect.width * root->blks[0].aspect.height)/total_area;

        do {
            test = Move_type;
            Move_type = select_move();
            // cout << endl;
            // postorder2(root);
            // cout << endl;
            // for(int k = 0; k <SPE.size(); k++)
            //     cout << SPE[k] << " ";
            
            // if(Move_type!=3)
            //     cout << endl;
            switch(Move_type) {
                case 1:
                    M1(true, &iptr, &jptr, &i, &j);
                    // cout << "end M1 "<< endl;
                    // print();
                    new_cost = 100*(root->blks[0].aspect.width * root->blks[0].aspect.height)/total_area;
                    // postorder(root);
                    // cout << endl;
                    break;
                case 2:
                    M2(true, &iptr, &i, &j);
                    // print();
                    new_cost = 100*(root->blks[0].aspect.width * root->blks[0].aspect.height)/total_area;
                    // postorder(root);
                    // cout << endl;
                    break;
                case 3:
                    // cout << endl << "Move_type is " << Move_type << " after " << test << endl;
                    // postorder2(root);
                    // cout << endl;
                    // for(int k = 0; k <SPE.size(); k++)
                    //     cout << SPE[k] << " ";
                    // cout << endl;
                    temp_root = M3(&i, &j);
                    // if(temp_root) 
                    //     cout << "take + ";
                    // else 
                    //     cout << "not take + ";
                    // cout << "123";
                    if(temp_root)
                        new_cost = 100*(temp_root->blks[0].aspect.width * temp_root->blks[0].aspect.height)/total_area;
                    else
                        new_cost = curr_cost;
                    break;
            }
            delta_cost = new_cost - curr_cost;
            MoveCount++;
            
            if(is_movable(delta_cost, T)) {
                // if(Move_type ==3)
                // cout << "moveable" << endl;
                // cout << "new : " << new_cost << " best : " << best_cost;
                if(delta_cost > 0)
                    uphill++;
                if(new_cost < best_cost) {
                    cout << "Move_type is " << Move_type << endl;
                    best_cost = new_cost;
                    best_SPE = SPE;
                    best_module = module;
                    // cout << root->blks[0].aspect.width << " " << root->blks[0].aspect.height << " ";
                    cout << fixed << setprecision(2) << "best : " << best_cost*total_area/100 << endl;
                    // cout << endl;
                    // postorder(best_root);
                    // cout << endl;
                    if(Move_type == 3 && temp_root)
                        print(temp_root);
                    else
                        print(root);
                    cout << endl;
                }
                
                if(Move_type == 3 && temp_root) {
                    delete_tree(root);
                    root = temp_root;

                    // postorder2(root);
                    // cout << endl;
                    // for(int k = 0; k <SPE.size(); k++)
                    //     cout << SPE[k] << " ";
                    // cout << endl;
                }
                // postorder2(root);
                // cout << endl;
                // for(int k = 0; k <SPE.size(); k++)
                //     cout << SPE[k] << " ";
                // cout << endl;
            }
            else {
                // if(Move_type ==3)
                // cout << "not moveable " ;
                switch(Move_type) {
                    case 1:
                        M1(false, &iptr, &jptr, &i, &j);
                        break;
                    case 2:
                        M2(false, &iptr, &i, &j);
                        break;
                    case 3:
                        // delete_tree(temp_root);
                        // cout << endl;
                        // postorder2(root);
                        // cout << endl;
                        // for(int k = 0; k <SPE.size(); k++)
                        //     cout << SPE[k] << " ";
                        

                        swap(i, j);
                        // cout << i << " " << j << endl;

                        
                        break;
                }
                reject++;
            }
            // cout << " " << uphill << " " << MoveCount << " " << N << " " << T <<endl;;
        }
        while((uphill < N) && (MoveCount < 2*N));
        
        T = w * T;
        // cout << "------------------------------------------------------" << endl;
        // cout << T << " " << ((double)reject/MoveCount) << endl;
    }
    while(((double)reject/MoveCount) < 0.95 && (T > freezing));

}


void Floorplanning::delete_tree(Node *ptr) {
    if(ptr){
        delete_tree(ptr->left);
        delete_tree(ptr->right);
        delete ptr;
    }
}

bool Floorplanning::is_movable(double delta_cost, double T) {
    if(delta_cost <= 0)
        return true;
    
    double x = (double) rand() / (RAND_MAX + 1.0);

    if(x < exp(-(delta_cost/T)))
        return true;

    return false;
}

void Floorplanning::update_cooling_factor(double *w, double T) {
    if(T <= 30000 && T > 2000) *w = 0.85;
    else if (T <= 2000 && T > 1000) *w = 0.9;
    else if (T <= 1000 && T > 500) *w = 0.95;
    else if (T <= 500 && T > 200) *w = 0.96;
    else if (T <= 200 && T > 10) *w = 0.97;
    else if (T <= 10 && T > 0.1) *w = 0.98;

}

int Floorplanning::select_move() {
    int x = rand() % 3 + 1;
    if(x == 1)
        return 1;
    else if(x == 2)
        return 2;
    else
        return 3;
}

void Floorplanning::M1(bool control, Node **iptr, Node **jptr, int *i, int *j) {

    int x;

    if(control){
        do {
            x = rand() % SPE.size();
            if(SPE[x] == "V" || SPE[x] == "H") 
                (*i) = find_int(x);
            else 
                (*i) = x;

            (*j) = find_int((*i));
        }
        while((*i)-(*j) == 1 || (*i)-(*j) == -1);

        // cout << endl << "x, i, j are: " << x << " " << *i << " " << *j << " " << endl;

        find_node_ij(&(*iptr), &(*jptr), (*i), (*j));
    }

    // cout << "iptr->data is " << (*iptr)->data << endl;
    // cout << "jptr->data is " << (*jptr)->data << endl;

    swap((*i), (*j));

    Node *parent_i = (*iptr)->parent;
    Node *parent_j = (*jptr)->parent;

    if(parent_i->left == (*iptr))
        parent_i->left = (*jptr);
    else
        parent_i->right = (*jptr);

    if(parent_j->left == (*jptr))
        parent_j->left = (*iptr);
    else
        parent_j->right = (*iptr);

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

void Floorplanning::find_node_ij(Node **iptr, Node **jptr, int i, int j) {

    stack<Node *> s1, s2;
    Node *temp;
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

void Floorplanning::M2(bool control, Node **iptr, int *i, int *j) {

    int x;

    if(control){
        x = rand() % SPE.size();

        // cout << endl << x;
    
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

        // cout << endl << x << " " << i << " " << j << endl;
    }

    find_node_i(&(*iptr), (*i), (*j));
    

    // cout << iptr->data << endl;

    for(int k = (*i); k <= (*j); k++) {
        if(SPE[k] == "V")
            SPE[k] = "H";
        else
            SPE[k] = "V";
    }

    for(Node *temp = (*iptr); temp; temp = temp->parent) 
        temp->update = false;

    // cout << "not yet update" << endl;

    // postorder(root);
    // cout << endl;
    bottom_up(root);
    top_down(root);
}

void Floorplanning::find_node_i(Node **iptr, int i, int j) {

    stack<Node *> s1, s2;
    Node *temp;
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

Node * Floorplanning::M3(int *i, int *j) {
    
    int x, counter = 0; 
    Node *temp;

    while(counter < 10) {
        // cout << counter << " ";
        x = rand() % SPE.size();
        find_int_VH(x, &(*i), &(*j));
        
        // cout << x << " " << (*i) << " " << (*j) << " " << endl;
        swap((*i), (*j));
        // for(int k = 0; k < SPE.size(); k++)
            // cout << SPE[k] << " " ;
        // cout << endl;
        if(!legal_check((*j))) 
            swap((*i), (*j));
        else{
            // cout << endl << x << " " << (*i) << " " << (*j) << " " << endl;
            break;
        }
        counter++;
    }
    // cout << endl;

    if(counter != 10) { 
        temp = SPE_to_tree(SPE);
        bottom_up(temp);
        top_down(temp);
        return temp;
    }
    return NULL;
}

bool Floorplanning::legal_check(int j) {
    int operandCount = 0, operatorCount = 0;

    if(j == SPE.size()) {
        if(SPE[j-2] == SPE[j-1]) {
            // cout << "test 1" << endl;
            return false;
        }
    }
    else if(j == 1) {
        if(SPE[j] == SPE[j+1]) {
            // cout << "test 2" << endl;
            return false;
        }
    }
    else {
        if(SPE[j-2] == SPE[j-1] || SPE[j] == SPE[j+1]) {
            // cout << "test 3" << endl;
            // cout << "j-2, j-1, j, j+1 is :" << j-2 << " " << j-1 << " " << j << " " << j+1 << endl;
            // cout << "SPE[j-2], SPE[j-1], SPE[j], SPE[j+1] is :" << SPE[j-2] << " " << SPE[j-1] << " " << SPE[j] << " " << SPE[j+1] << endl;
            return false;
        }
    }
    
    for(int i = 0; i <= j; i++) {
        if(SPE[i] == "V" || SPE[i] == "H")
            operatorCount++;
        else
            operandCount++;
        if(operatorCount >= operandCount) {
            // cout << "test 4" << endl;
            // cout << "operator, operand count is " << operatorCount << " " << operandCount << endl;
            return false;      
        }  
    }
    // cout << "test 5" << endl;
    return true;
}

void Floorplanning::swap(int i, int j) {
    string tmp = SPE[i];
    SPE[i] = SPE[j];
    SPE[j] = tmp;
}

void Floorplanning::find_int_VH(int x, int *i, int *j) {
    
    if(SPE[x] == "V" || SPE[x] == "H") {
        
        if(x != SPE.size()){
            for(int k = x; k < SPE.size()-1; k++) {
                if(SPE[k+1] != "V" && SPE[k+1] != "H") {
                    (*i) = k;
                    (*j) = k+1;
                    return;
                }  
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
        if(x != SPE.size()){
            for(int k = x; k < SPE.size()-1; k++) {
                if(SPE[k+1] == "V" || SPE[k+1] == "H") {
                    (*i) = k;
                    (*j) = k+1;
                    return;
                }  
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

void Floorplanning::postorder(Node *ptr) {
    if(ptr) {
        postorder(ptr->left);
        postorder(ptr->right);
        if(ptr->parent)
            cout << "It's " << ptr->data << " my parent is " << ptr->parent->data << " my parents' children are " << ptr->parent->left->data << " and " << ptr->parent->right->data << endl;
        else
            cout << "It's " << ptr->data << " I have no child "<< endl;
        // cout << "It's " << ptr->data << "  " << ptr->update << endl;
        cout << ptr->data << " " << endl;
        // cout << ptr->blks.size() << endl;
        // if(ptr->data == 'V' || ptr->data == 'H') {
            if(!ptr->parent)
                    cout << ptr->blks[0].aspect.width * ptr->blks[0].aspect.height << endl;
            for(int i = 0; i < ptr->blks.size(); i++){
                cout << ptr->blks[i].aspect.width << " " << ptr->blks[i].aspect.height << endl;
            }
            cout << endl;
        // }
    }
}

void Floorplanning::postorder2(Node *ptr) {
    if(ptr) {
        postorder2(ptr->left);
        postorder2(ptr->right);
        cout << ptr->data << " ";
    }
}

int Floorplanning::find_int(int x) {
    for(int k = x; k < SPE.size(); k++) {
        if(k == x) 
            continue;
        if(SPE[k] != "V" && SPE[k] != "H") {
            // cout << "+SPE[k], k are " << SPE[k] << " " << k;
            return k;
        }
    }
    for(int k = x; k >= 0; k--) {
        if(k == x) 
            continue;
        if(SPE[k] != "V" && SPE[k] != "H") {
            // cout << "-SPE[k], k are " << SPE[k] << " " << k;
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

void Floorplanning::choose_possible_size(int index, Node **ptr) { // no need to **!!!!!!!!!!!!!!!!
    
    double left, right;
    Block temp;

    left = sqrt(module[index].area / 2);
    right = sqrt(2 * module[index].area);
    
    for(int j = 0; j < pts; j++) {
        temp.aspect.width = left + (j * ((right-left)/(pts-1)));
        temp.aspect.height = module[index].area / temp.aspect.width;
        (*ptr)->blks.push_back(temp);
    }
}

bool mycompare(Block a, Block b){
   return a.aspect.width * a.aspect.height < b.aspect.width * b.aspect.height;
}

Node *Floorplanning::creat_node(string data) {
    Node *temp = new Node();
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

void Floorplanning::write_file(fstream& myFile) {
    
    cout << root->blks[0].aspect.width << " " << root->blks[0].aspect.height << " " << root->blks[0].aspect.width * root->blks[0].aspect.height << endl;
    for(int i = 0; i < module.size(); i++)
        cout << module[i].aspect.width << " " << module[i].aspect.height << endl;
        
    for(int i = 0; i < SPE.size(); i++) 
        cout << SPE[i] << " ";

}

void Floorplanning::print(Node *ptr) {
    
    cout << endl << ptr->blks[0].aspect.width << " " << ptr->blks[0].aspect.height << " " << fixed << setprecision(2) << ptr->blks[0].aspect.width * ptr->blks[0].aspect.height << endl;
    for(int i = 0; i < module.size(); i++)
        cout << module[i].aspect.width << " " << module[i].aspect.height << endl;
        
    for(int i = 0; i < SPE.size(); i++) 
        cout << SPE[i] << " ";

    cout << endl;
}