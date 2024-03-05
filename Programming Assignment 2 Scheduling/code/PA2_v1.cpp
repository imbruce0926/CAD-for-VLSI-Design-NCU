#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
    
    struct node {
        int pre;
        struct node *nodePtr;
    };
    typedef struct node Node;
    typedef Node *NodePtr;

    struct headnode {
        int sucCount;
        int preCount;
        char oper;
        struct node *nodePtr;
    };
    typedef struct headnode Headnode;

    class Scheduling { 
    public:
        Scheduling();
        void read_file(fstream &);
        void write_file(fstream &);

    private:
        int latency, ADDCount, MULCount, nodeNum;
        string line;
        vector<Headnode> h_node;
        vector<int> latest;

        void get_successor();
        int get_latency();
        void get_successor();
        void creat_node(int, int);
        void print_graph();
    };

int main() {

    fstream myFile;
    Scheduling S;

    myFile.open("case1.txt", ios::in);
    S.read_file(myFile);
    myFile.close();

    myFile.open("output.txt", ios::out);
    S.write_file(myFile);
    myFile.close();

    return 0;
}

Scheduling::Scheduling() {

    // initial variables

    nodeNum = 0;

}

void Scheduling::read_file(fstream& myFile) {

    while (getline(myFile, line)) {
        // cout << line << endl;
        // line = line.substr(0, line.length()-1); // this line is for workstation
        get_successor();
    }
    
    print_graph();
}

void Scheduling::get_successor() {
    if(line.find("Latency constrain") != string::npos)
        latency = get_latency();
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

    Headnode temp;
    NodePtr NOP;
    string suc_str = " ";
    temp.preCount = temp.sucCount = 0; temp.oper = ' '; temp.nodePtr = NULL;
    nodeNum++;

    
    if(line[line.find(" ")+1] != 'o'){
        //cout << "test1" << endl;
        if(line.find("//") != string::npos)
            suc_str = line.substr(line.find(" ")+3, line.find("/") - line.find(" ")-2) + "  ";
        else   
            suc_str = line.substr(line.find(" ")+3, line.length() - line.find(" ")-1) + "  ";
    }
    while(suc_str[0] != ' ') {
        int suc = 0;
        for(int i = 0; i < suc_str.find(" "); i++) 
            suc = suc * 10 + suc_str[i] - 48;
        suc_str =  suc_str.substr(suc_str.find(" ")+1, suc_str.length() - suc_str.find(" ")-1);
        
        if(h_node.size() < suc+1) 
            for(int i = h_node.size(); i < suc+1; i++)
                h_node.push_back(temp);

        h_node[nodeNum].sucCount++;
        h_node[suc].preCount++;
        creat_node(nodeNum, suc);

    }

    h_node[nodeNum].oper = line[line.find(" ")+1];
    if(h_node[nodeNum].oper == 'i')
        creat_node(0, nodeNum);
    // cout << h_node[nodeNum].oper << endl;
    
}

void Scheduling::creat_node(int pre, int succ) {
    NodePtr new_node = new Node();
    new_node->pre = pre;
    new_node->nodePtr = h_node[succ].nodePtr;
    h_node[succ].nodePtr = new_node;
}

void Scheduling::print_graph() {
    NodePtr temp;

    cout << "this is print_graph()"<<endl;
    cout << "node number is : "<< h_node.size() << endl;
    for(int i = 1; i < h_node.size(); i++) {
        cout << "vertex "<< i << endl;
        cout << "oper is : " << h_node[i].oper << endl;
        cout << "sucCount is : " << h_node[i].sucCount << endl;
        cout << "preCount is : " << h_node[i].preCount << "   // use it to find who could in ready state" << endl;
        cout << "pres are :    ";
        temp = h_node[i].nodePtr;
        while(temp) {
            cout << temp->pre << " ";
            temp = temp->nodePtr;
        }
        cout << endl << endl;
    }

}

void Scheduling::write_file(fstream& myFile) {

    // print the result


}
