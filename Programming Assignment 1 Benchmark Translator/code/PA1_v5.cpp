#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
    
    struct gate_info {
        int port_cnt = 0;
        vector<string> port;
        string type = "none";
    };
    typedef struct gate_info Gate_info;

    class Benchmark_translator { 
    public:
        Benchmark_translator();
        void read_file(fstream &);
        void write_file(fstream &);

    private:
        string line_, func_, name_;
        vector<string> input_, output_;
        int inputCount_, outputCount_, gateCount_, typeCount_[8];
        vector<Gate_info> gates_;

        void delete_blank();
        void func_judgement();
        void process_txt();
        void get_name();
        void get_IO(vector<string> &, int *);
        void process_IO(vector<string> &, int *, string);
        void get_port();

    };

int main(int argc, char *argv[]) {

    fstream myFile;
    Benchmark_translator BT;

    myFile.open(argv[1], ios::in);
    BT.read_file(myFile);
    myFile.close();

    myFile.open(argv[2], ios::out);
    BT.write_file(myFile);
    myFile.close();

    return 0;
}

Benchmark_translator::Benchmark_translator() {

    // initial variables

    func_ = "none";
    inputCount_ = outputCount_ = gateCount_ = 0;
    for(int i = 0; i < 8; i++)
        typeCount_[i] = 0;
}

void Benchmark_translator::read_file(fstream& myFile) {

    // 1. read file line by line
    // 2. delete all the blank in the string
    // 3. judge what state it is when recieve the key word
    // 4. process the text by following the current state

    while (getline(myFile, line_)) {
        
        delete_blank();
        func_judgement();
        process_txt();
        //myFile << line_ << endl;
    }
}

void Benchmark_translator::delete_blank() {

    // delete all the blank in the string

        while(line_.find(" ") != string::npos) {
            if(line_.find(" ") == 0)
                line_ = line_.substr(1, line_.length() - 1);
            else if(line_.find(" ") == line_.length() - 1)
                line_ = line_.substr(0, line_.length() - 1);
            else
                line_ = line_.substr(0, line_.find(" ")) + line_.substr(line_.find(" ") + 1, line_.length() - line_.find(" ") - 1);
        }
}

void Benchmark_translator::func_judgement() {

    // function like a finite state machine
    // change state when recieve the key word

    if(line_.find("module") != string::npos && line_.find("endmodule") == string::npos) 
        func_ = "module";
    else if(line_.find("input") != string::npos) 
        func_ = "input";
    else if(line_.find("output") != string::npos) 
        func_ = "output";
    else if(line_.find("wire") != string::npos) 
        func_ = "none";
    else if(line_.find("and") != string::npos && line_.find("nand") == string::npos) 
        func_ = "and";
    else if(line_.find("nand") != string::npos) 
        func_ = "nand";
    else if(line_.find("or") != string::npos && line_.find("nor") == string::npos && line_.find("xor") == string::npos && line_.find("xnor") == string::npos) 
        func_ = "or";
    else if(line_.find("nor") != string::npos && line_.find("xnor") == string::npos) 
        func_ = "nor";
    else if(line_.find("xor") != string::npos) 
        func_ = "xor";
    else if(line_.find("xnor") != string::npos) 
        func_ = "xnor";
    else if(line_.find("buf") != string::npos) 
        func_ = "buf";
    else if(line_.find("not") != string::npos) 
        func_ = "not";
}

void Benchmark_translator::process_txt() {

    // process the text depending on the current state

    if(func_ == "module") 
        get_name();
    else if (func_ == "input") 
        get_IO(input_, &inputCount_);
    else if (func_ == "output") 
        get_IO(output_, &outputCount_);
    else if (func_ == "and") {
        typeCount_[0]++;
        get_port();
    }
    else if (func_ == "nand") {
        typeCount_[1]++;
        get_port();
    }
    else if (func_ == "or") {
        typeCount_[2]++;
        get_port();
    }
    else if (func_ == "nor") {
        typeCount_[3]++;
        get_port();
    }
    else if (func_ == "xor") {
        typeCount_[4]++;
        get_port();
    }
    else if (func_ == "xnor") {
        typeCount_[5]++;
        get_port();
    }
    else if (func_ == "buf") {
        typeCount_[6]++;
        get_port();
    }
    else if (func_ == "not") {
        typeCount_[7]++;
        get_port();
    }


}

void Benchmark_translator::get_name() {

    // receive the name in "module" state

    name_ = line_.substr(6, line_.find("(") - 6);
    func_ = "none";
}

void Benchmark_translator::get_IO(vector<string> &inout, int *inoutCount) {

    // receive input/output ports in "input"/"output state

    while(line_.find("N") != string::npos) {

        if(line_.find(",") != string::npos) 
            process_IO(inout, &(*inoutCount), ",");
        else 
            process_IO(inout, &(*inoutCount), ";");

    }
}

void Benchmark_translator::process_IO(vector<string> &IOput, int *IOCount, string item) {

    // "N" may be follow by "," or ";"
    // need to be judge in different way

    (*IOCount)++;
    IOput.push_back(line_.substr(line_.find("N") + 1, line_.find(item) - line_.find("N") - 1));
    line_ = line_.substr(line_.find(item) + 1, line_.length() - line_.find(item) - 1);

    if(line_.find(";") != string::npos) 
        func_ = "none";
}

void Benchmark_translator::get_port() {

    // receive ports in logic gate state 

    line_ = line_.substr(line_.find("(") + 1, line_.find(")") - line_.find("(") - 1) + ",";

    Gate_info temp;

    gates_.push_back(temp);

    while(line_.find("N") != string::npos) {

        gates_[gateCount_].port.push_back(line_.substr(line_.find("N") + 1, line_.find(",") - line_.find("N") - 1));
        gates_[gateCount_].port_cnt++;
        
        line_ = line_.substr(line_.find(",") + 1, line_.length() - line_.find(",") - 1);

    }
    gates_[gateCount_++].type = func_;
    func_ = "none";
}

void Benchmark_translator::write_file(fstream& myFile) {

    // print the result

    myFile << "# " << name_ << endl;
    myFile << "# " << inputCount_; 

    if(inputCount_ == 0 || inputCount_ == 1)
        myFile << " input" << endl;
    else
        myFile << " inputs" << endl;

    myFile << "# " << outputCount_ ;

    if(outputCount_ == 0 || outputCount_ == 1)
        myFile << " output" << endl;
    else
        myFile << " outputs" << endl;

    myFile << "# " << typeCount_[7];

    if(typeCount_[7] == 0 || typeCount_[7] == 1)
        myFile << " inverter" << endl;
    else
        myFile << " inverters" << endl;

    myFile << "# " << gateCount_ - typeCount_[7] << " gates ";
    myFile << "( ";

    for(int i = 0; i < 7; i++) {

        if(0 == typeCount_[i])
            continue;
        else {
            if(0 == i) 
                myFile << typeCount_[i] << " " << "ANDs";
            else if(1 == i)
                myFile << typeCount_[i] << " " << "NANDs";
            else if(2 == i)
                myFile << typeCount_[i] << " " << "ORs";
            else if(3 == i)
                myFile << typeCount_[i] << " " << "NORs";
            else if(4 == i)
                myFile << typeCount_[i] << " " << "XORs";
            else if(5 == i)
                myFile << typeCount_[i] << " " << "XNORs";
            else if(6 == i)
                myFile << typeCount_[i] << " " << "buffers";

            for(int j = i + 1; j < 7; j++) {
                if(typeCount_[j] != 0){
                    myFile << " + ";
                    break;
                }
            }
        }

    }
    
    myFile << " )" << endl;
    myFile << endl;

    for(int i = 0; i < inputCount_; i++) 
        myFile << "INPUT(" << input_[i] << ")" << endl;

    myFile << endl; 

    for(int i = 0; i < outputCount_; i++) 
        myFile << "OUTPUT(" << output_[i] << ")" << endl;

    myFile << endl;

    for(int i = 0; i < gateCount_; i++) {
        for(int j = 0; j < gates_[i].port_cnt; j++) {

            if(0 == j) {
                myFile << gates_[i].port[j] << " = ";
                if("and" == gates_[i].type)
                    myFile << "AND(";
                else if("nand" == gates_[i].type)
                    myFile << "NAND(";
                else if("or" == gates_[i].type)
                    myFile << "OR(";
                else if("nor" == gates_[i].type)
                    myFile << "NOR(";
                else if("xor" == gates_[i].type)
                    myFile << "XOR(";
                else if("xnor" == gates_[i].type)
                    myFile << "XNOR(";
                else if("not" == gates_[i].type)
                    myFile << "NOT(";
                else if("buf" == gates_[i].type)
                    myFile << "BUFF(";
            }
            else if(j == gates_[i].port_cnt - 1) 
                myFile << gates_[i].port[j] << ")";
            else 
                myFile << gates_[i].port[j] << ", ";   

        }
        myFile << endl;
    } 
}
