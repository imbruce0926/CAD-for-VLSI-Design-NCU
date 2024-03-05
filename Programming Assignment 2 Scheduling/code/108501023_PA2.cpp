// PA2_Scheduling
// Input file: latency constrain, info of node connection
// Output file: number of ADD & MUL used, executing vertex in each timing
// This program use List Scheduling algorithm under MR-LCS restrition
// 2023/3/31 Version4
// Author: Pin Hsien Lee

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>

using namespace std;
    
    struct node {
        int suc, pre, cycle;
        struct node *sucPtr, *prePtr;
    };
    typedef struct node Node;
    typedef Node *NodePtr;

    struct vertexInfo {
        int latest, sucCount, preCount;
        char oper;
        bool valid;
    };
    typedef struct vertexInfo VertexInfo;

    class Scheduling { 
    public:
        Scheduling();
        void read_file(fstream &);
        void write_file(fstream &);

    private:
        int latency, ADDCount, MULCount, vertexNum;
        string line;
        vector<NodePtr> sucHead, preHead;
        vector<VertexInfo> vertex;
        vector<vector<int>> output;
        vector<vector<int>> MULstate;

        void process_text();
        int str_to_int(string);
        void get_predecessor();
        int get_latency();
        void get_successor();
        void extend_vertex_count();
        void creat_node(int, int);
        bool ALAP();
        void List_Scheduling();
        void schedule_vertex(int, int, char, vector<int> &);
        void decrement_preCount(int);
        void sorting();
    };

int main(int argc, char *argv[]) {

    fstream myFile;
    Scheduling S;
    string intputFile = argv[1]; 
    string outputFile = intputFile + ".out";

    myFile.open(intputFile, ios::in);
    if(!myFile) {
        cerr << "Error opening file" << endl;
        return 1;
    }
    else {
        S.read_file(myFile);
        myFile.close();
    }

    myFile.open(outputFile, ios::out);
    S.write_file(myFile);
    myFile.close();

    return 0;
}

Scheduling::Scheduling() {
    ADDCount = MULCount = latency = vertexNum = 0;
}

void Scheduling::read_file(fstream& myFile) {

    // 1. read the file line by line, constructure the 1-D data structure 
    // 2. reconstruct it from 1-D to 2-D data structure 
    // 3. do the list scheduling algorithm
    // 4. sort each output line in ascending order

    while (getline(myFile, line)) 
        process_text();
    
    get_predecessor();
    List_Scheduling();
    sorting();
}

void Scheduling::process_text() {

    // get the latency constrain and each node infomation

    if(line.find("Latency constrain") != string::npos){
        vector<int> temp;

        latency = get_latency();
        
        for(int i = 0; i < latency+1; i++)
            output.push_back(temp);
    }
    else if(line[0] >= 48  && line[0] <= 57) 
        get_successor();
}

int Scheduling::str_to_int(string str) {

    // translate it from string to int

    int sum = 0;

    for(int i = 0; i < str.find(" "); i++) 
        sum = sum * 10 + str[i] - 48; 

    return sum;
}

int Scheduling::get_latency() {

    // process string to get latency

    line = line + "  ";

    while(line[0] < 48  || line[0] > 57) 
        line = line.substr(line.find(" ")+1, line.length()-line.find(" "));

    int latency = str_to_int(line);

    return latency;
}

void Scheduling::get_successor() {

    // process string to constructure 1-D data structure
    // each sucHead[vertex_index] will point to its successors

    string suc_str = " ";
    int pre = str_to_int(line);
    
    suc_str = line.substr(line.find(" ")+3, line.length()-line.find(" ")-3) + "  ";

    while(suc_str[0] != ' ' && line[line.find(" ")+1] != 'o') {

        int suc = str_to_int(suc_str);
        suc_str =  suc_str.substr(suc_str.find(" ")+1, suc_str.length() - suc_str.find(" ")-1);
        
        if(sucHead.size() < suc+1) 
            for(int i = sucHead.size(); i < suc+1; i++)
                extend_vertex_count();
        
        vertex[pre].sucCount++;
        creat_node(pre, suc);
    }

    vertex[pre].valid = true;
    vertex[pre].oper = line[line.find(" ")+1];
}

void Scheduling::extend_vertex_count() {

    // create a vertex

    VertexInfo temp;
    temp.latest = latency+1;
    temp.oper = ' ';
    temp.preCount = temp.sucCount = 0;
    temp.valid = false;

    vertex.push_back(temp);
    sucHead.push_back(NULL);
    preHead.push_back(NULL);
}

void Scheduling::creat_node(int pre, int suc) {

    // create a node to store edge and cycle of predecessor

    NodePtr new_node = new Node();

    new_node->pre = pre;
    new_node->suc = suc;
    new_node->cycle = 0;
    new_node->prePtr = NULL;
    new_node->sucPtr = sucHead[pre];
    sucHead[pre] = new_node;
}

void Scheduling::get_predecessor() {

    // reconstructure it to 2-D data structure
    // each preHead[vertex_index] will point to its predeccessors
    // e.g. edge(0, 1) will be pointed by sucHead[0] and preHead[1]

    NodePtr temp;

    vertexNum = sucHead.size()-1;

    for(int i = 1; i <= vertexNum; i++) {

        temp = sucHead[i];

        while(temp) {
            vertex[temp->suc].preCount++;

            if(vertex[i].oper == '+') 
                temp->cycle = 1;
            else if(vertex[i].oper == '*') 
                temp->cycle = 3;
            else if(vertex[i].oper == 'i')
                temp->cycle = 1;

            temp->prePtr = preHead[temp->suc];
            preHead[temp->suc] = temp;
            temp = temp->sucPtr;
        }
    }
}

bool Scheduling::ALAP() {

    // use topology sort to get latest starting time of each vertex

    stack<int> s;
    int index;
    
    for(int i = 1; i <= vertexNum; i++) 
        if(!vertex[i].sucCount && vertex[i].valid) 
            s.push(i);
        
    for(int i = 1; i <= vertexNum; i++) {
        index = s.top();
        s.pop();

        for(NodePtr ptr = preHead[index]; ptr; ptr = ptr->prePtr) {

            vertex[ptr->pre].sucCount--;

            if(!vertex[ptr->pre].sucCount && vertex[i].valid)
                s.push(ptr->pre);
            
            if(vertex[index].latest - ptr->cycle < vertex[ptr->pre].latest){

                vertex[ptr->pre].latest = vertex[index].latest - ptr->cycle;

                if(vertex[ptr->pre].latest < 0) 
                    return false;
            }
        }
    }
    return true;
}

void Scheduling::List_Scheduling() {

    // if latest < 0 while doing ALAP, can't be scheduling
    // using list scheduling algorithm to schedule vertexs

    if(!ALAP()) {
        cout << "It's impossible to solve in this latency constrain" << endl;
        return;
    }

    queue<int> ready;
    stack<int> remain;
    int index, slack;
    int readyCount = 0, remainCount = 0;
    vector<int> currentADDCount(latency+1, 0);
    vector<int> currentMULCount(latency+1, 0);

    // vertex of preCount==0 and valid==true is chosen to ready queue
    // notice that the executing '*' vertex is with false valid bit

    for(int level = 0; level <= latency; level++) {

        for(int i = 1; i <= vertexNum; i++) 
            if(!vertex[i].preCount && vertex[i].valid) 
                ready.push(i);

        readyCount = ready.size();

        for(int i = 0; i < readyCount; i++) {

            index = ready.front();
            ready.pop();
            
            slack = vertex[index].latest - level;

            if(vertex[index].oper == 'i') 
                decrement_preCount(index);
            else if(slack == 0) {
                if(vertex[index].oper == '+') 
                    schedule_vertex(index, level, '+', currentADDCount);
                else if(vertex[index].oper == '*') 
                    schedule_vertex(index, level, '*', currentMULCount);
            }
            else    
                remain.push(index);
        }

        // vertex of preCount==0, valid==true and slack!=0 is chosen to remain stack

        remainCount = remain.size();

        if(ADDCount > currentADDCount[level] || MULCount > currentMULCount[level]){
            for(int i = 0; i < remainCount; i++) {

                index = remain.top();
                remain.pop();

                if(vertex[index].oper == '+' && ADDCount > currentADDCount[level]) 
                    schedule_vertex(index, level, '+', currentADDCount);
                else if(vertex[index].oper == '*' && MULCount > currentMULCount[level]) 
                    schedule_vertex(index, level, '*', currentMULCount);
            }
        }
        
        vector<vector<int>> temp_;
        for(int i = 0; i < MULstate.size(); i++) {
            if(--MULstate[i][1] == 0) 
                decrement_preCount(MULstate[i][0]);
            else 
                temp_.push_back(MULstate[i]);
        }
        MULstate = temp_;

        if(ADDCount < currentADDCount[level])
            ADDCount = currentADDCount[level];
        if(MULCount < currentMULCount[level])
            MULCount = currentMULCount[level];
    }

}

void Scheduling::schedule_vertex(int index, int level, char oper, vector<int> &currentCount) {

    // schedule vertex according to its type
    // notice decrement_preCount() can't be called with '*' op instantly due to 3 cycle 
    // but the valid unset first, thus it will not be selected to ready queue

    if(oper == '+') {
        output[level].push_back(index);
        currentCount[level]++;
        decrement_preCount(index);
    }
    else if(oper == '*') {
        for(int j = level; j < level+3; j++) {
            vertex[index].valid = false;
            output[j].push_back(index);
            currentCount[j]++;
        }
        vector<int> temp;
        MULstate.push_back(temp);
        MULstate.back().push_back(index);
        MULstate.back().push_back(3);
    }
}

void Scheduling::decrement_preCount(int index) {

    // called at the end cycle of operation

    vertex[index].valid = false;
    for(NodePtr ptr = sucHead[index]; ptr; ptr = ptr->sucPtr) 
        vertex[ptr->suc].preCount--;
}

void Scheduling::sorting() {

    // sort each vertex in increasing order with a time slot

    for(int i = 1; i <= latency; i++) 
        sort(output[i].begin(), output[i].begin() + output[i].size());
}

void Scheduling::write_file(fstream& myFile) {

    myFile << ADDCount << endl << MULCount << endl;

    for(int i = 1; i <= latency; i++) {
        for(int j = 0; j < output[i].size(); j++) {
            myFile << output[i][j];
            if(j != output[i].size()-1)
              myFile << " ";
        }
          myFile << endl;
    }
}