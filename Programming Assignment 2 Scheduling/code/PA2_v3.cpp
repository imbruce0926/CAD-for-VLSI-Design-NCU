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
        int suc;
        int pre;
        int cycle;
        struct node *sucPtr;
        struct node *prePtr;
    };
    typedef struct node Node;
    typedef Node *NodePtr;


    class Scheduling { 
    public:
        Scheduling();
        void read_file(fstream &);
        void write_file(fstream &);

    private:
        int latency, ADDCount, MULCount;
        string line;
        vector<NodePtr> sucHead, preHead;
        vector<int> latest;
        vector<int> sucCount;
        vector<int> preCount;
        vector<char> oper;
        vector<bool> valid;
        vector<vector<int>> output;
        vector<int> MULState;

        void process_text();
        int str_to_int(string);
        void get_predecessor();
        int get_latency();
        void get_successor();
        void extend_vertex_number();
        void creat_node(int, int);
        bool ALAP();
        void schedule();
        void sorting();
    };

int main() {

    fstream myFile;
    Scheduling S;

    myFile.open("testcase3", ios::in);
    S.read_file(myFile);
    myFile.close();

    myFile.open("output3.txt", ios::out);
    S.write_file(myFile);
    myFile.close();

    return 0;
}

Scheduling::Scheduling() {

    ADDCount = 0;
    MULCount = 0;
}

void Scheduling::read_file(fstream& myFile) {

    while (getline(myFile, line)) 
        process_text();
    
    get_predecessor();
    if(ALAP()) {
        schedule();
        sorting();
    }
}

void Scheduling::process_text() {

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

    int sum = 0;

    for(int i = 0; i < str.find(" "); i++) 
        sum = sum * 10 + str[i] - 48; 

    return sum;
}

int Scheduling::get_latency() {

    line = line + "  ";

    while(line[0] < 48  || line[0] > 57) 
        line = line.substr(line.find(" ")+1, line.length()-line.find(" "));

    int latency = str_to_int(line);

    return latency;
}


void Scheduling::get_successor() {

    NodePtr temp = NULL;
    NodePtr NOP = NULL;
    string suc_str = " ";
    int pre = str_to_int(line);
    

    suc_str = line.substr(line.find(" ")+3, line.length()-line.find(" ")-3) + "  ";

    while(suc_str[0] != ' ' && line[line.find(" ")+1] != 'o') {

        int suc = str_to_int(suc_str);
        suc_str =  suc_str.substr(suc_str.find(" ")+1, suc_str.length() - suc_str.find(" ")-1);
        
        if(sucHead.size() < suc+1) 
            for(int i = sucHead.size(); i < suc+1; i++)
                extend_vertex_number();
        
        sucCount[pre]++;
        creat_node(pre, suc);
    }

    valid[pre] = true;

    oper[pre] = line[line.find(" ")+1];

    if(oper[pre] == 'i')
        latest[pre] = 0;
}

void Scheduling::extend_vertex_number() {
    sucCount.push_back(0);
    preCount.push_back(0);
    sucHead.push_back(NULL);
    preHead.push_back(NULL);
    oper.push_back(' ');
    valid.push_back(false);
    latest.push_back(latency+1);
}

void Scheduling::creat_node(int pre, int suc) {
    NodePtr new_node = new Node();
    new_node->pre = pre;
    new_node->suc = suc;
    new_node->cycle = 0;
    new_node->prePtr = NULL;
    new_node->sucPtr = sucHead[pre];
    sucHead[pre] = new_node;
}

void Scheduling::get_predecessor() {

    NodePtr temp;

    for(int i = 1; i <= sucHead.size()-1; i++) {

        temp = sucHead[i];

        while(temp) {
            preCount[temp->suc]++;

            if(oper[i] == '+') 
                temp->cycle = 1;
            else if(oper[i] == '*') 
                temp->cycle = 3;
            else if(oper[i] == 'i')
                temp->cycle = 0;

            temp->prePtr = preHead[temp->suc];
            preHead[temp->suc] = temp;
            temp = temp->sucPtr;
        }
    }
}

bool Scheduling::ALAP() {

    stack<int> s;
    int index;
    
    for(int i = 1; i <= sucHead.size()-1; i++) 
        if(!sucCount[i]) 
            s.push(i);
        
    for(int i = 1; i <= sucHead.size()-1; i++) {
        index = s.top();
        s.pop();

        for(NodePtr ptr = preHead[index]; ptr; ptr = ptr->prePtr) {

            sucCount[ptr->pre]--;

            if(!sucCount[ptr->pre])
                s.push(ptr->pre);
            
            if(latest[index] - ptr->cycle < latest[ptr->pre]){
                latest[ptr->pre] = latest[index] - ptr->cycle;
                if(latest[ptr->pre] < 0) {
                    cout << "It's impossible to solve in this latency constrain" << endl;
                    return false;
                }
            }
        }
    }
    return true;
}

void Scheduling::schedule() {
    queue<int> ready;
    int index, slack;
    int readyCount = 0;

    for(int level = 0; level <= latency; level++) {

        for(int i = 1; i <= preHead.size()-1; i++) 
            if(!preCount[i] && valid[i]) 
                ready.push(i);

        readyCount = ready.size();

        for(int i = 0; i < readyCount; i++) {

            index = ready.front();
            ready.pop();
            
            slack = latest[index] - level;

            if(slack == 0) {
                valid[index] = false;

                if(oper[index] != 'i'){
                    if(oper[index] == '+') 
                        output[level].push_back(index);
                    else if(oper[index] == '*') 
                        for(int j = level; j < level+3; j++) 
                            output[j].push_back(index);
                }

                for(NodePtr ptr = sucHead[index]; ptr; ptr = ptr->sucPtr) 
                    preCount[ptr->suc]--;
            } 
        }
        
    }
    for(int i = 1; i <= latency; i++) {
        int currentADDCount = 0;
        int currentMULCount = 0;
        for(int j = 0; j < output[i].size(); j++){
            if(oper[output[i][j]] == '+')
                currentADDCount++;
            else if(oper[output[i][j]] == '*')
                currentMULCount++;
        }
        if(ADDCount < currentADDCount)
            ADDCount = currentADDCount;
        if(MULCount < currentMULCount)
            MULCount = currentMULCount;
    }
}

void Scheduling::sorting() {
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
