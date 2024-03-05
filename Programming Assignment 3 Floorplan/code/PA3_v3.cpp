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
        int i, j;
        Size aspect;
    };

    class Node {
    public:
        class Node *left, *right;
        char data;
        int choosen;
        vector<int> Lindex, Rindex;
        vector<Size> aspect;
    };

    class Floorplanning { 
    public:
        Floorplanning();
        void read_file(fstream &);
        void write_file(fstream &);
        void choose_possible_size(int, Node **);
        void SPE_to_tree(string);
        Node *creat_node(char data);
        void bottom_up(Node *);
        void postorder(Node *);
        double max(double, double);
        bool is_equal(double, double);
        int choose_min_area(Node *);
        void top_down(Node *);
        


    private:
        int moduleCount, pts;
        Node *root;
        vector<Module> module;
        string SPE;
        
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
    F.write_file(myFile);
    myFile.close();
    cout << "end main";
    return 0;
}

Floorplanning::Floorplanning() {
    root = NULL;
    SPE = "01V2V3V4V5V6V7V8V9V";
    pts = 4;
}

void Floorplanning::read_file(fstream& myFile) {

    double temp;
    Module temp_; 

    myFile >> moduleCount;

    while(myFile >> temp) {
        myFile >> temp;
        module.push_back(temp_);
        module.back().area = temp;
    }

    SPE_to_tree(SPE);
    bottom_up(root);
    // postorder(root);

    // cout << root->aspect.size();
    cout << "--------------------------------------" << endl << endl;

    top_down(root);
    // postorder(root);

    // cout << SPE << endl;
    cout << "Total area : " << root->aspect[0].width * root->aspect[0].height << endl;
    for(int i = 0; i < module.size(); i++)
        cout << module[i].aspect.width << " " << module[i].aspect.height << endl;
}

void Floorplanning::SPE_to_tree(string SPE) {

    stack<Node *> s;
    Node *temp = NULL;

    for(int i = 0; i < SPE.length(); i++) {
        if(SPE[i] == 'V' || SPE[i] == 'H') {
            if(SPE[i] == 'V')
                temp = creat_node('V');
            else    
                temp = creat_node('H');

            temp->right = s.top();
            s.pop();
            temp->left = s.top();
            s.pop();
            s.push(temp);
        }
        else {
            temp = creat_node(SPE[i]);
            choose_possible_size(SPE[i] - 48, &(temp));
            s.push(temp);
        }
    }

    root = s.top();
    s.pop();

}

void Floorplanning::choose_possible_size(int index, Node **ptr) { // no need to **!!!!!!!!!!!!!!!!
    
    double left, right;
    Size temp;

    left = sqrt(module[index].area / 2);
    right = sqrt(2 * module[index].area);
    
    for(int j = 0; j < pts; j++) {
        temp.width = left + (j * ((right-left)/(pts-1)));
        temp.height = module[index].area / temp.width;
        (*ptr)->aspect.push_back(temp);
        // module[index].aspect.push_back(temp);
    }
}

Node *Floorplanning::creat_node(char data) {
    Node *temp = new Node();
    temp->data = data;
    temp->left = NULL;
    temp->right = NULL;
    return temp;
}



void Floorplanning::bottom_up(Node *ptr) {
    if(ptr) {
        bottom_up(ptr->left);
        bottom_up(ptr->right);
        cout << "it's " << ptr->data << endl; 

        vector<Block> blks;
        Block temp;

        if(ptr->data == 'V' || ptr->data == 'H') {
            cout << "data is " << ptr->left->data << " & " << ptr->right->data << endl;
            cout << "size is "<< ptr->left->aspect.size() << " * " << ptr->right->aspect.size() << endl;
            for(int i = 0; i < ptr->left->aspect.size(); i++) {
                for(int j = 0; j < ptr->right->aspect.size(); j++) {
                    if(ptr->data == 'V') {
                        temp.aspect.width = ptr->left->aspect[i].width + ptr->right->aspect[j].width;
                        temp.aspect.height = max(ptr->left->aspect[i].height, ptr->right->aspect[j].height);
                    }
                    else {
                        temp.aspect.height = ptr->left->aspect[i].height + ptr->right->aspect[j].height;
                        temp.aspect.width = max(ptr->left->aspect[i].width, ptr->right->aspect[j].width);
                    }
                    temp.i = i;
                    temp.j = j;
                    blks.push_back(temp);
                }
            }
            sort(blks.begin(), blks.begin()+blks.size(), mycompare);
            int cnt = 0, i = 0;
            while(cnt < pts) {
                if((blks[i].aspect.width != blks[i-1].aspect.width && blks[i].aspect.height != blks[i-1].aspect.height) || cnt == 0){
                    ptr->aspect.push_back(blks[i].aspect);
                    ptr->Lindex.push_back(blks[i].i);
                    ptr->Rindex.push_back(blks[i].j);
                    cnt++;
                }
                i++;
            }
            for(int i = 0; i < pts; i++) {
                cout << ptr->aspect[i].width << " * " << ptr->aspect[i].height << " = " << ptr->aspect[i].width * ptr->aspect[i].height << endl;
            }
            cout << endl;
        }
        else {
            cout << "area: " << module[ptr->data-48].area << endl;
            for(int i = 0; i < ptr->aspect.size(); i++){
                cout << "= " <<  ptr->aspect[i].width << " * " << ptr->aspect[i].height;
                cout << " / " << ptr->aspect[i].height / ptr->aspect[i].width << endl;
            }
            cout << endl;
        }
    }

    
}

bool mycompare(Block a, Block b){
   return a.aspect.width * a.aspect.height < b.aspect.width * b.aspect.height;
}

void Floorplanning::top_down(Node *ptr) {

    queue<Node *> q;

    ptr->choosen = 0;

    q.push(ptr);

    while(true){
        
        if(!q.empty()) {
            ptr = q.front();
            q.pop();
        }
        else 
            break;

        if(ptr->left && ptr->right) {
            ptr->left->choosen = ptr->Lindex[ptr->choosen];
            ptr->right->choosen = ptr->Rindex[ptr->choosen];
            q.push(ptr->left);
            q.push(ptr->right);
        }
        else 
            module[ptr->data-48].aspect = ptr->aspect[ptr->choosen];
        
                            

    }
}

void Floorplanning::postorder(Node *ptr) {
    if(ptr) {
        postorder(ptr->left);
        postorder(ptr->right);
        cout << "It's " << ptr->data << endl;
        cout << ptr->aspect.size() << endl;
        // if(ptr->data == 'V' || ptr->data == 'H') {
            for(int i = 0; i < ptr->aspect.size(); i++)
                cout << ptr->aspect[i].width << " " << ptr->aspect[i].height << endl;
            cout << endl;
        // }
    }
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






// bool Floorplanning::ALAP() {


// }

// void Floorplanning::List_Scheduling() {


// }

// void Floorplanning::schedule_vertex(int index, int level, char oper, vector<int> &currentCount) {


// }

// void Floorplanning::decrement_preCount(int index) {


// }

// void Floorplanning::sorting() {


// }

void Floorplanning::write_file(fstream& myFile) {


}