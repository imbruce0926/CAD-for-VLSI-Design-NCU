// PA4 Channel Routing
// Input file: 2 vector of pins
// Output file: connection of each pins
// This program use constrainted left-edge algorithm
// 2023/6/3 Version 1
// Author: Pin Hsien Lee

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

    class Interval {
    public:
        int pinNum;
        int left, right;
        bool update;
    };

    class Node {
    public:
        int pinNum;
        class Node *ptr;
    };

    class Pin {
    public:
        bool valid;
        int preCount;
        int trackNum;
        Interval I;
        vector<int> TOP_loc, BOT_loc;
        class Node *ptr;
    };

    class Routing { 
    public:
        Routing();
        void read_file(fstream &);
        void write_file(fstream &);
        void left_edge();

    private:
        Node *creat_node(int);
        void build_graph();
        void get_interval();

        vector<int> BOT;
        vector<int> TOP;
        vector<vector<Interval>> track;
        vector<Interval> I;
        vector<Pin> pin;
        int columnCount;
        int ICount;
    };

    bool mycompare(Interval, Interval);

int main(int argc, char *argv[]) {

    fstream myFile;
    Routing R;
    string intputFile = argv[1]; 
    string outputFile = argv[2];

    myFile.open(intputFile, ios::in);
    R.read_file(myFile);
    myFile.close();

    R.left_edge();

    myFile.open(outputFile, ios::out);
    R.write_file(myFile);
    myFile.close();
    
    return 0;
}

Routing::Routing() {
    Pin temp;
    temp.valid = false;
    temp.preCount = 0;
    temp.ptr = NULL;
    temp.trackNum = 0;

    for(int i = 0; i < 65536; i++) 
        pin.push_back(temp);
    
}

void Routing::read_file(fstream& myFile) {

    int tmp;
    vector<int> temp;

    while(myFile >> tmp) 
        temp.push_back(tmp);
    
    for(int i = 0; i < temp.size() / 2; i++) {
        TOP.push_back(temp[i]);
        BOT.push_back(temp[i + (temp.size() / 2)]);
    }
    columnCount = TOP.size();

}

void Routing::build_graph() {

    for(int i = 0; i < columnCount; i++) {
        if(TOP[i]) {
            pin[TOP[i]].TOP_loc.push_back(i);
            pin[TOP[i]].valid = true;
        }
        if(BOT[i]) {
            pin[BOT[i]].BOT_loc.push_back(i);
            pin[BOT[i]].valid = true;
        }
        if(TOP[i] && BOT[i]) {
            Node *temp = creat_node(BOT[i]);
            temp->ptr = pin[TOP[i]].ptr;
            pin[TOP[i]].ptr = temp;
            pin[BOT[i]].preCount++;
        }
    }
}

void Routing::get_interval() {
    Interval temp;
    
    for(int i = 0; i < 65536; i++) {
        if(pin[i].valid) {
            if(pin[i].TOP_loc.size() && pin[i].BOT_loc.size()){
                temp.left = min(pin[i].TOP_loc[0], pin[i].BOT_loc[0]);
                temp.right = max(pin[i].TOP_loc[pin[i].TOP_loc.size()-1], pin[i].BOT_loc[pin[i].BOT_loc.size()-1]);
            }
            else if(pin[i].BOT_loc.size()) {
                temp.left = pin[i].BOT_loc[0];
                temp.right = pin[i].BOT_loc[pin[i].BOT_loc.size()-1];
            }
            else if(pin[i].TOP_loc.size()) {
                temp.left = pin[i].TOP_loc[0];
                temp.right = pin[i].TOP_loc[pin[i].TOP_loc.size()-1];
            }
            temp.update = false;
            temp.pinNum = i;
            pin[i].I = temp;
            I.push_back(temp);
        }
    }
    ICount = I.size();
}

Node *Routing::creat_node(int data) {
    Node *temp = new Node();
    temp->pinNum = data;
    temp->ptr = NULL;
    return temp;
}

void Routing::left_edge() {

    build_graph();
    get_interval();

    sort(I.begin(), I.begin()+I.size(), mycompare);

    while(ICount > 0) {
        
        vector<Interval>temp;
        track.push_back(temp);

        int watermark = -1;
        for(int i = 0; i < I.size(); i++) {
            
            if(!I[i].update && !pin[I[i].pinNum].preCount && I[i].left > watermark) {
                for(Node *ptr = pin[I[i].pinNum].ptr; ptr; ptr = ptr->ptr) 
                    pin[ptr->pinNum].preCount--;
                
                pin[I[i].pinNum].trackNum = track.size()-1;
                I[i].update = true;
                ICount--;
                
                track.back().push_back(I[i]);
                watermark = I[i].right;
            }
        }
    }
}

void Routing::write_file(fstream& myFile) {
    for(int i = 0; i < 65536; i++) {
        if(pin[i].valid) {
            myFile << ".begin " << i << endl;
            myFile << ".H " << pin[i].I.left << " " << track.size() - pin[i].trackNum << " " << pin[i].I.right << endl;
            if(pin[i].TOP_loc.size()) {
                for(int k = 0; k < pin[i].TOP_loc.size(); k++) {
                    myFile << ".V " << pin[i].TOP_loc[k] << " " << track.size() - pin[i].trackNum << " " << track.size()+1 << endl;
                }
            }
            if(pin[i].BOT_loc.size()) {
                for(int k = 0; k < pin[i].BOT_loc.size(); k++) {
                    myFile << ".V " << pin[i].BOT_loc[k] << " 0 " << track.size() - pin[i].trackNum << endl;
                }
            }
            myFile << ".end" << endl;
        }
    }
}

bool mycompare(Interval a, Interval b){
   return a.left < b.left;
}
