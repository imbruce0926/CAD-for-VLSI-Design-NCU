#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

    const int MAX_SIZE = 2000, MAX_PORTS = 10;
    
    struct gate_info {
        int port_cnt;
        string port[MAX_PORTS];
        string type;
    };
    typedef struct gate_info Gate_info;
    
    void delete_blank(string *);
    void func_judgement(string *, string *);
    void process_txt(string *, string *, string *, string *, string *, int *, int *, int *, int *, Gate_info *);
    void get_IO(string *, string *, int *);
    void comma_or_semicolon(string *, string *, int *, string);
    void get_port(string *, Gate_info *, int *, string);
    void print_result(fstream &, string, int, int, int *, int, string *, string *, Gate_info *);
    

int main(int argc, char *argv[]) {

    string line, func = "none", name, input[MAX_SIZE], output[MAX_SIZE];
    int input_cnt = 0, output_cnt = 0,gate_cnt = 0 , type_cnt[8] = {};
    Gate_info gate[MAX_SIZE];
    fstream myFile;

    myFile.open(argv[1], ios::in);

    while (getline(myFile, line)) {

        delete_blank(&line);

        func_judgement(&line, &func);

        process_txt(&line, &func, &name, input, output, &input_cnt, &output_cnt, &gate_cnt, type_cnt, gate);
            
    }

    myFile.close();

    myFile.open(argv[2], ios::out);

    print_result(myFile, name, input_cnt, output_cnt, type_cnt, gate_cnt, input, output, gate);

    myFile.close();

    return 0;
}

void delete_blank(string *line) {

    while((*line).find(" ") != string::npos) {
        if((*line).find(" ") == 0)
            (*line) = (*line).substr(1, (*line).length() - 1);
        else if((*line).find(" ") == (*line).length() - 1)
            (*line) = (*line).substr(0, (*line).length() - 1);
        else
            (*line) = (*line).substr(0, (*line).find(" ")) + (*line).substr((*line).find(" ") + 1, (*line).length() - (*line).find(" ") - 1);
    }

}

void func_judgement(string *line, string *func) {

    if((*line).find("module") != string::npos && (*line).find("endmodule") == string::npos) 
        (*func) = "module";
    if((*line).find("input") != string::npos) 
        (*func) = "input";
    if((*line).find("output") != string::npos) 
        (*func) = "output";
    if((*line).find("wire") != string::npos) 
        (*func) = "none";
    if((*line).find("and") != string::npos && (*line).find("nand") == string::npos) 
        (*func) = "and";
    if((*line).find("nand") != string::npos) 
        (*func) = "nand";
    if((*line).find("or") != string::npos && (*line).find("nor") == string::npos && (*line).find("xor") == string::npos && (*line).find("xnor") == string::npos) 
        (*func) = "or";
    if((*line).find("nor") != string::npos && (*line).find("xnor") == string::npos) 
        (*func) = "nor";
    if((*line).find("xor") != string::npos) 
        (*func) = "xor";
    if((*line).find("xnor") != string::npos) 
        (*func) = "xnor";
    if((*line).find("buf") != string::npos) 
        (*func) = "buf";
    if((*line).find("not") != string::npos) 
        (*func) = "not";

}

void process_txt(string *line, string *func, string *name, string *input, string *output, int *input_cnt, int *output_cnt,
                    int *gate_cnt, int *type_cnt, Gate_info *gate) {

        if((*func) == "module") {
            (*name) = (*line).substr(6, (*line).find("(")-6);
            (*func) = "none";
        }
        if ((*func) == "input") 
            get_IO(&(*line), input, &(*input_cnt));
        if ((*func) == "output") 
            get_IO(&(*line), output, &(*output_cnt));
        if ((*func) == "and") {
            type_cnt[0]++;
            get_port(&(*line), gate, &(*gate_cnt), (*func));
            (*func) = "none";
        }
        if ((*func) == "nand") {
            type_cnt[1]++;
            get_port(&(*line), gate, &(*gate_cnt), (*func));
            (*func) = "none";
        }
        if ((*func) == "or") {
            type_cnt[2]++;
            get_port(&(*line), gate, &(*gate_cnt), (*func));
            (*func) = "none";
        }
        if ((*func) == "nor") {
            type_cnt[3]++;
            get_port(&(*line), gate, &(*gate_cnt), (*func));
            (*func) = "none";
        }
        if ((*func) == "xor") {
            type_cnt[4]++;
            get_port(&(*line), gate, &(*gate_cnt), (*func));
            (*func) = "none";
        }
        if ((*func) == "xnor") {
            type_cnt[5]++;
            get_port(&(*line), gate, &(*gate_cnt), (*func));
            (*func) = "none";
        }
        if ((*func) == "buf") {
            type_cnt[6]++;
            get_port(&(*line), gate, &(*gate_cnt), (*func));
            (*func) = "none";
        }
        if ((*func) == "not") {
            type_cnt[7]++;
            get_port(&(*line), gate, &(*gate_cnt), (*func));
            (*func) = "none";
        }

        if((*line).find(";") != string::npos) 
            (*func) = "none";

}

void get_IO(string *line, string *IOput, int *cnt) {

    while((*line).find("N") != string::npos) {

        if((*line).find(",") != string::npos) 
            comma_or_semicolon(&(*line), IOput, &(*cnt), ",");
        else 
            comma_or_semicolon(&(*line), IOput, &(*cnt), ";");

    }

}

void comma_or_semicolon(string *line, string *IOput, int *cnt, string item) {

    IOput[(*cnt)++] = (*line).substr((*line).find("N") + 1, (*line).find(item) - (*line).find("N") - 1);
    (*line) = (*line).substr((*line).find(item) + 1, (*line).length() - (*line).find(item) - 1);

}

void get_port(string *line, Gate_info *gate, int *gate_cnt, string func) {

    (*line) = (*line).substr((*line).find("(") + 1, (*line).find(")") - (*line).find("(") - 1) + ",";

    while((*line).find("N") != string::npos) {

        gate[(*gate_cnt)].port[gate[(*gate_cnt)].port_cnt++] = (*line).substr((*line).find("N") + 1, (*line).find(",") - (*line).find("N") - 1);
        (*line) = (*line).substr((*line).find(",") + 1, (*line).length() - (*line).find(",") - 1);

    }
    gate[(*gate_cnt)++].type = func;

}

void print_result(fstream &myFile, string name, int input_cnt, int output_cnt, 
                    int *type_cnt, int gate_cnt, string *input, string *output, Gate_info *gate) {

    myFile << "# " << name << endl;
    myFile << "# " << input_cnt; 

    if(input_cnt == 0 || input_cnt == 1)
        myFile << " input" << endl;
    else
        myFile << " inputs" << endl;

    myFile << "# " << output_cnt ;

    if(output_cnt == 0 || output_cnt == 1)
        myFile << " output" << endl;
    else
        myFile << " outputs" << endl;

    myFile << "# " << type_cnt[7];

    if(type_cnt[7] == 0 || type_cnt[7] == 1)
        myFile << " inverter" << endl;
    else
        myFile << " inverters" << endl;

    myFile << "# " << gate_cnt - type_cnt[7] << " gates ";
    myFile << "( ";

    for(int i = 0; i < 7; i++) {

        if(0 == type_cnt[i])
            continue;
        else {
            if(0 == i) 
                myFile << type_cnt[i] << " " << "ANDs";
            if(1 == i)
                myFile << type_cnt[i] << " " << "NANDs";
            if(2 == i)
                myFile << type_cnt[i] << " " << "ORs";
            if(3 == i)
                myFile << type_cnt[i] << " " << "NORs";
            if(4 == i)
                myFile << type_cnt[i] << " " << "XORs";
            if(5 == i)
                myFile << type_cnt[i] << " " << "XNORs";
            if(6 == i)
                myFile << type_cnt[i] << " " << "buffers";

            for(int j = i + 1; j < 7; j++) {
                if(type_cnt[j] != 0){
                    myFile << " + ";
                    break;
                }
            }
        }

    }
    
    myFile << " )" << endl;
    myFile << endl;

    for(int i = 0; i < input_cnt; i++) 
        myFile << "INPUT(" << input[i] << ")" << endl;

    myFile << endl; 

    for(int i = 0; i < output_cnt; i++) 
        myFile << "OUTPUT(" << output[i] << ")" << endl;

    myFile << endl;

    for(int i = 0; i < gate_cnt; i++) {
        for(int j = 0; j < gate[i].port_cnt; j++) {

            if(0 == j) {
                myFile << gate[i].port[j] << " = ";
                if("and" == gate[i].type)
                    myFile << "AND(";
                if("nand" == gate[i].type)
                    myFile << "NAND(";
                if("or" == gate[i].type)
                    myFile << "OR(";
                if("nor" == gate[i].type)
                    myFile << "NOR(";
                if("xor" == gate[i].type)
                    myFile << "XOR(";
                if("xnor" == gate[i].type)
                    myFile << "XNOR(";
                if("not" == gate[i].type)
                    myFile << "NOT(";
                if("buf" == gate[i].type)
                    myFile << "BUFF(";
            }
            else if(j == gate[i].port_cnt - 1) 
                myFile << gate[i].port[j] << ")";
            else 
                myFile << gate[i].port[j] << ", ";
                
        }

        myFile << endl;
        
    } 

}