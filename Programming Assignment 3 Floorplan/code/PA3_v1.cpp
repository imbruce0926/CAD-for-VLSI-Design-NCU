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
        vector<Size> aspect;  
    };

    class Node {
    public:
        class Node *left, *right;
        char data;
        double merged_area;
        vector<Size> aspect;
    };

    class Floorplanning { 
    public:
        Floorplanning();
        void read_file(fstream &);
        void write_file(fstream &);
        void choose_possible_size(int, Node **);
        void init();
        Node *creat_node(char data);
        void postorder(Node *);
        void postorder2(Node *);
        double max(double, double);

    private:
        int moduleCount, pts;
        Node *root;
        vector<Module> module;
        string SPE;
        
        vector<Size> valid_size;

    };

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

    return 0;
}

Floorplanning::Floorplanning() {
    SPE = "";
    pts = 5;
    
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

    init();
    postorder(root);
    cout << "--------------------------------------" << endl;
    postorder2(root);
    cout << SPE << endl;
}

void Floorplanning::choose_possible_size(int index, Node **ptr) {
    
    double left, right;
    Size temp;

    left = sqrt(module[index].area / 2);
    right = sqrt(2 * module[index].area);
    
    for(int j = 0; j < pts; j++) {
        temp.width = left + (j * ((right-left)/(pts-1)));
        temp.height = module[index].area / temp.width;
        (*ptr)->aspect.push_back(temp);
        module[index].aspect.push_back(temp);
    }
}

void Floorplanning::init() {

    root = creat_node('V');
    Node *temp = root;
    
    for(int i = moduleCount-1; i > 0; i--) {
        temp->left = creat_node('V');
        temp->right = creat_node(i + 48);
        choose_possible_size(i, &(temp->right));
        temp = temp->left;
    }
    temp->data = 0 + 48;
    choose_possible_size(0, &(temp));

    

}

Node *Floorplanning::creat_node(char data) {
    Node *temp = new Node();
    temp->data = data;
    temp->left = NULL;
    temp->right = NULL;
    return temp;
}

void Floorplanning::postorder(Node *ptr) {
    if(ptr) {
        postorder(ptr->left);
        postorder(ptr->right);
        SPE = SPE + ptr->data;
            
        if(ptr->data == 'V') {
            Size temp;
            temp.width = 0; temp.height = 0;

            vector<vector<Size>> all_size(ptr->left->aspect.size()+1, vector<Size>(ptr->right->aspect.size()+1));

            // cout << "left size is " << ptr->left->aspect.size() << " right size is " << ptr->right->aspect.size() << endl;
            // cout << all_size.size() << " " << all_size[0].size() << endl;

            for(int i = 0; i < ptr->left->aspect.size()+1; i++) {
                // cout << i << endl;
                all_size[i][0].width = 0;   all_size[i][0].height = 0;
                // cout << "suc"<<endl;
            }
            for(int j = 0; j < ptr->right->aspect.size()+1; j++) {
                // cout << j;
                all_size[0][j].width = 0;   all_size[0][j].height = 0;
            }
            
            for(int i = 1; i <= ptr->left->aspect.size(); i++) {
                // cout <<"i is "<< i << endl;
                for(int j = 1; j <= ptr->right->aspect.size(); j++) {
                    // cout <<"j is "<< j << endl;

                    all_size[i][j].width = ptr->left->aspect[i-1].width + ptr->right->aspect[j-1].width;
                    all_size[i][j].height = max(ptr->left->aspect[i-1].height, ptr->right->aspect[j-1].height);
                    // cout << i << " " << j << " " << all_size[i][j].width << " " << all_size[i][j].height << endl;

                    if(!(((all_size[i][j].width - all_size[i-1][j].width) < 0.00000001 && (all_size[i][j].width - all_size[i-1][j].width) > -0.00000001 && all_size[i][j].height > all_size[i-1][j].height) || 
                        ((all_size[i][j].height - all_size[i-1][j].height) < 0.00000001 && (all_size[i][j].height - all_size[i-1][j].height) > -0.00000001 && all_size[i][j].width > all_size[i-1][j].width) ||
                        ((all_size[i][j].width - all_size[i][j-1].width) < 0.00000001 && (all_size[i][j].width - all_size[i][j-1].width) > -0.00000001 && all_size[i][j].height > all_size[i][j-1].height) ||
                        ((all_size[i][j].height - all_size[i][j-1].height) < 0.00000001 && (all_size[i][j].height - all_size[i][j-1].height) > -0.00000001 && all_size[i][j].width > all_size[i][j-1].width))) {
                        
                        // cout << all_size[i][j].width << "   " << all_size[i][j].height << endl;
                        ptr->aspect.push_back(all_size[i][j]);
                        // cout << "success" << endl;
                        
                        
                    }
                    
                    // else if(all_size[i][j].height - all_size[i-1][j].height < 0.00000001 && all_size[i][j].width > all_size[i-1][j].width){
                    //     cout << "hehe" << endl;
                    //     cout << all_size[i][j].height << "   " << all_size[i-1][j].height << endl;
                    // }
                    


                }
                // cout << endl;
            }
            


        }
        else if(ptr->data == 'H') {

        }
        else {
            // cout << "area: " << module[ptr->data-48].area << endl;
            // for(int i = 0; i < ptr->aspect.size(); i++){
            //     cout << "= " <<  ptr->aspect[i].width << " * " << ptr->aspect[i].height;
            //     cout << " / " << ptr->aspect[i].height / ptr->aspect[i].width << endl;
            // }
            // cout << endl;
        }
    }
}

void Floorplanning::postorder2(Node *ptr) {
    if(ptr) {
        postorder2(ptr->left);
        postorder2(ptr->right);
        // SPE = SPE + ptr->data;
        cout << "It's " << ptr->data << endl;
        cout << ptr->aspect.size() << endl;
        if(ptr->data == 'V') {
            for(int i = 0; i < ptr->aspect.size(); i++)
                cout << ptr->aspect[i].width << " " << ptr->aspect[i].height << endl;
            cout << endl;
        }

    }
}

double Floorplanning::max(double a, double b) {
    if(a > b)
        return a;
    else
        return b;
}

// void Floorplanning::get_successor() {


// }

// void Floorplanning::extend_vertex_count() {


// }



// void Floorplanning::get_predecessor() {

// }

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