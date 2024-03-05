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
        void get_predecessor();
        int get_latency();
        void get_successor();
        void extend_vertex_number();
        void creat_node(int, int);
        bool ALAP();
        void schedule();
        void sorting();
        void print_graph();
    };

int main() {

    fstream myFile;
    Scheduling S;

    myFile.open("testcase2S", ios::in);
    S.read_file(myFile);
    myFile.close();

    myFile.open("output.txt", ios::out);
    S.write_file(myFile);
    myFile.close();
    // cout << "end main";
    return 0;
}

Scheduling::Scheduling() {
    ADDCount = 0;
    MULCount = 0;
}

void Scheduling::read_file(fstream& myFile) {

    while (getline(myFile, line)) {
        // cout << line << endl;
        // this line is for workstation (.txt need, but others don't)
        // line = line.substr(0, line.length()-1); 
        process_text();
    }
    get_predecessor();
    if(ALAP()) 
        schedule();
    // print_graph();
}

void Scheduling::process_text() {
    if(line.find("Latency constrain") != string::npos){
        latency = get_latency();
        vector<int> temp;
        for(int i = 0; i < latency+1; i++)
            output.push_back(temp);
    }
    else if(line[0] >= 48  && line[0] <= 57) 
        get_successor();

}

int Scheduling::get_latency() {
    int i, j, sum = 0;
    string token;

    while(line[0] < 48  || line[0] > 57) 
        line = line.substr(line.find(" ")+1, line.length()-line.find(" "));

    token = line.substr(0, line.find(" "));

    for(int i = 0; i < token.length(); i++) 
        sum = sum * 10 + line[i] - 48; 

    return sum;
}


void Scheduling::get_successor() {

    NodePtr temp = NULL;
    NodePtr NOP = NULL;
    string suc_str = " ";

    int pre = 0;

    for(int i = 0; i < line.find(" "); i++) 
            pre = pre * 10 + line[i] - 48;

    suc_str = line.substr(line.find(" ")+3, line.length()-line.find(" ")-3) + "  ";

    while(suc_str[0] != ' ' && line[line.find(" ")+1] != 'o') {
        int suc = 0;
        for(int i = 0; i < suc_str.find(" "); i++) 
            suc = suc * 10 + suc_str[i] - 48;

        suc_str =  suc_str.substr(suc_str.find(" ")+1, suc_str.length() - suc_str.find(" ")-1);
        
        if(sucHead.size() < suc+1) 
            for(int i = sucHead.size(); i < suc+1; i++)
                extend_vertex_number();
        
        sucCount[pre]++;
        creat_node(pre, suc);
    }

    oper[pre] = line[line.find(" ")+1];
    // if(oper[pre] == 'i')
    //     latest[pre] = 0;

}

void Scheduling::extend_vertex_number() {
    sucCount.push_back(0);
    preCount.push_back(0);
    sucHead.push_back(NULL);
    preHead.push_back(NULL);
    oper.push_back(' ');
    valid.push_back(true);
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
                temp->cycle = 1;
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
        if(!sucCount[i]) {
            // cout << "add " << i << " into stack" << endl;
            s.push(i);
        }

    for(int i = 1; i <= sucHead.size()-1; i++) {

        // cout << endl << "stack size is : " << s.size() << endl;

        index = s.top();
        s.pop();

        // cout << "index is : " << index << endl;

        if(oper[index]!='i')
            // cout <<"("<< preHead[index]->pre <<", "<<preHead[index]->suc <<")"<< endl;

        for(NodePtr ptr = preHead[index]; ptr; ptr = ptr->prePtr) {
            // cout << "ptr->pre is : " << ptr->pre << endl;
            sucCount[ptr->pre]--;
            if(!sucCount[ptr->pre]){
                // cout << "add " << ptr->pre << " into stack" << endl;
                s.push(ptr->pre);
            }
            if(latest[index] - ptr->cycle < latest[ptr->pre]){
                // cout <<endl<< "lastest["<<ptr->pre<<"]from " << latest[ptr->pre] << " to " ;
                // cout << "latest[index] - ptr->cycle = " << latest[index] <<" - " << ptr->cycle << " = " << latest[index] - ptr->cycle << endl;
                latest[ptr->pre] = latest[index] - ptr->cycle;
                if(latest[ptr->pre] < 0) {
                    cout << "It's impossible to solve in this latency constrain" << endl;
                    cout << ptr->pre << "  " << latest[ptr->pre] << endl;
                    return false;
                }
            }
        }
    }
    cout << endl;
    for(int i = 1; i <= sucHead.size()-1; i++) {
        cout << "latest[" << i << "] is :"<< latest[i] << endl;
    }

    return true;

}

void Scheduling::schedule() {
    queue<int> ready;
    int index, slack;
    int readyCount = 0;

    for(int level = 0; level <= latency; level++) {
        
        int currentADDCount = 0;
        int currentMULCount = 0;
        int availMULCount = 0;

        for(int i = 1; i <= preHead.size()-1; i++) 
            if(!preCount[i] && valid[i]) 
                ready.push(i);
        readyCount = ready.size();
        
        // cout << endl<< endl << "_________level is " << level << "_________"<< endl<< endl;
        // cout << "ready count is " << readyCount << endl << endl << endl;

        for(int i = 0; i < readyCount; i++) {
            // cout << "i = " << i << endl;
            index = ready.front();
            ready.pop();
            // cout << "latest[" << index << "] is " << latest[index] << endl;
            
            slack = latest[index] - level;
            if(slack == 0) {
                // cout << "slack is 0" << endl;

                valid[index] = false;
                // cout << "vertex " << index << " is scheduled\n";

                if(oper[index] != 'i'){
                    if(oper[index] == '+') {
                        currentADDCount++;
                        output[level].push_back(index);
                    }
                    else if(oper[index] == '*') {
                        currentMULCount++;
                        for(int j = level; j < level+3; j++) 
                            output[j].push_back(index);
                    }
                }

                for(NodePtr ptr = sucHead[index]; ptr; ptr = ptr->sucPtr) {
                    preCount[ptr->suc]--;
                    // cout << "preCount[" << ptr->suc << "] from " << preCount[ptr->suc]+1 << " to " << preCount[ptr->suc] << endl;
                }
                // cout << endl << endl;
            }
            
        }
        
        if(ADDCount < currentADDCount){
            // cout << "test0" << endl;
            ADDCount = currentADDCount;
        }
        for(int i = 0; i < MULCount; i++) {
            if(!MULState[i]){
                // cout << "test1" << endl;
                availMULCount++;
            }
        }
        if(availMULCount < currentMULCount) { 
            // cout << "test2" << endl;
            for(int i = 0; i < currentMULCount - availMULCount; i++){
                MULState.push_back(3);
            }
        }
        MULCount = MULState.size();

        for(int i = 0; i < MULCount; i++) {
            if(MULState[i]) {
                // cout << "test3" << endl;
                MULState[i]--;
            }
        }
        
    }
    
    // for(int i = 1; i <= latency; i++) {
    //     for(int j = 0; j < output[i].size(); j++)
    //         cout << output[i][j] << " ";
    //     cout << endl;
    // }
}

void Scheduling::sorting() {
    for(int i = 1; i <= latency; i++) 
        sort(output[i].begin(), output[i].begin() + output[i].size());
}

void Scheduling::print_graph() {
    NodePtr temp;

    cout << endl << "this is print_graph()"<<endl;
    cout << "node number is : "<< sucHead.size() << endl << endl;
    for(int i = 1; i < sucHead.size(); i++) {
        cout << "vertex "<< i << endl;
        cout << "oper is : " << oper[i] << endl;
        if(oper[i] != 'o')
            cout << "cycle is : " << sucHead[i]->cycle << endl;
        // cout << "sucCount is : " << sucCount[i] << endl;
        cout << "preCount is : " << preCount[i] << endl;
        // cout << "sucs are :    ";
        // temp = sucHead[i];
        // while(temp) {
        //     cout << temp->suc << " ";
        //     temp = temp->sucPtr;
        // }
        // cout << endl;
        cout << "pres are :    ";
        temp = preHead[i];
        while(temp) {
            cout << temp->pre << " ";
            temp = temp->prePtr;
        }
        cout << endl << endl;
    }

}
void Scheduling::write_file(fstream& myFile) {
    myFile << ADDCount << endl << MULCount << endl;
    for(int i = 1; i <= latency; i++) {
        //myFile << "----------Timing is : " << i << "----------" << endl;
        for(int j = 0; j < output[i].size(); j++)
            myFile << output[i][j] << " ";
        myFile << endl;
    }
    cout << "---------- adder count ----------" << endl;
    cout << ADDCount << endl;
    cout << "---------- multiplier count ----------" << endl;
    cout << MULCount << endl;
    for(int i = 1; i <= latency; i++) {
        cout << "---------- Timing is : " << i << " ----------" << endl;
        for(int j = 0; j < output[i].size(); j++)
            cout << output[i][j] << " ";
        cout << endl;
    }
}
