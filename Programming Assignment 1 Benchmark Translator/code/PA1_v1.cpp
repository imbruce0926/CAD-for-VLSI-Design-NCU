#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

const int MAX_SIZE = 2000, MAX_PORTS = 10;

int main(int argc, char *argv[]) {
    string line, func = "none", name, temp, input[MAX_SIZE], output[MAX_SIZE], gate[MAX_SIZE][MAX_PORTS], type[MAX_SIZE];
    int input_cnt = 0, output_cnt = 0, and_cnt = 0, cnt[8] = {};
    int gate_cnt = 0, port_cnt[MAX_SIZE] = {};

    fstream myFile;
    myFile.open(argv[1], ios::in);

    while (getline(myFile, line)) {

        while(line.find(" ") != string::npos) {
            if(line.find(" ") == 0)
                line = line.substr(1, line.length() - 1);
            else if(line.find(" ") == line.length() - 1)
                line = line.substr(0, line.length() - 1);
            else
                line = line.substr(0, line.find(" ")) + line.substr(line.find(" ") + 1, line.length() - line.find(" ") - 1);
        }

        if(line.find("module") != string::npos) {
            func = "module";
            line = line.substr(line.find("module") + 6, line.length() - 6);
        }
        if(line.find("input") != string::npos) {
            func = "input";
            line = line.substr(line.find("input") + 5, line.length() - 5);
        }
        if(line.find("output") != string::npos) {
            func = "output";
            line = line.substr(line.find("output") + 6, line.length() - 6);
        }
        if(line.find("wire") != string::npos) {
            func = "wire";
            line = line.substr(line.find("wire") + 4, line.length() - 4);
        }
        if(line.find("and") != string::npos && line.find("nand") == string::npos) {
            func = "and";
            line = line.substr(line.find("(") + 1, line.find(")") - line.find("(") - 1) + ",";
        }
        if(line.find("nand") != string::npos) {
            func = "nand";
            line = line.substr(line.find("(") + 1, line.find(")") - line.find("(") - 1) + ",";
        }
        if(line.find("or") != string::npos && line.find("nor") == string::npos && line.find("xor") == string::npos && line.find("xnor") == string::npos) {
            func = "or";
            line = line.substr(line.find("(") + 1, line.find(")") - line.find("(") - 1) + ",";
        }
        if(line.find("nor") != string::npos && line.find("xnor") == string::npos) {
            func = "nor";
            line = line.substr(line.find("(") + 1, line.find(")") - line.find("(") - 1) + ",";
        }
        if(line.find("xor") != string::npos) {
            func = "xor";
            line = line.substr(line.find("(") + 1, line.find(")") - line.find("(") - 1) + ",";
        }
        if(line.find("xnor") != string::npos) {
            func = "xnor";
            line = line.substr(line.find("(") + 1, line.find(")") - line.find("(") - 1) + ",";
        }
        if(line.find("buf") != string::npos) {
            func = "buf";
            line = line.substr(line.find("(") + 1, line.find(")") - line.find("(") - 1) + ",";
        }
        if(line.find("not") != string::npos) {
            func = "not";
            line = line.substr(line.find("(") + 1, line.find(")") - line.find("(") - 1) + ",";
        }

        if(func == "module") {
            if(line.find("(") != string::npos) 
                name = line.substr(0, line.find("("));
            else {
                // NXX need to be handled?
            }
        }
        else if (func == "input") {
            while(line.find("N") != string::npos) {
                if(line.find(",") != string::npos) {
                    input[input_cnt++] = line.substr(line.find("N") + 1, line.find(",") - line.find("N") - 1);
                    line = line.substr(line.find(",") + 1, line.length() - line.find(",") - 1);
                }
                else {
                    input[input_cnt++] = line.substr(line.find("N") + 1, line.find(";") - line.find("N") - 1);
                    line = line.substr(line.find(";") + 1, line.length() - line.find(";") - 1);
                }
            }
        }
        else if (func == "output") {
            while(line.find("N") != string::npos) {
                if(line.find(",") != string::npos) {
                    output[output_cnt++] = line.substr(line.find("N") + 1, line.find(",") - line.find("N") - 1);
                    line = line.substr(line.find(",") + 1, line.length() - line.find(",") - 1);
                }
                else {
                    output[output_cnt++] = line.substr(line.find("N") + 1, line.find(";") - line.find("N") - 1);
                    line = line.substr(line.find(";") + 1, line.length() - line.find(";") - 1);
                }
            }
        }
        else if (func == "and") {
            cnt[0]++;
            while(line.find("N") != string::npos) {
                gate[gate_cnt][port_cnt[gate_cnt]++] = line.substr(line.find("N") + 1, line.find(",") - line.find("N") - 1);
                line = line.substr(line.find(",") + 1, line.length() - line.find(",") - 1);
            }
            type[gate_cnt] = func;
            gate_cnt++;
            func = "none";
        }
        else if (func == "nand") {
            cnt[1]++;
            while(line.find("N") != string::npos) {
                gate[gate_cnt][port_cnt[gate_cnt]++] = line.substr(line.find("N") + 1, line.find(",") - line.find("N") - 1);
                line = line.substr(line.find(",") + 1, line.length() - line.find(",") - 1);
            }
            type[gate_cnt] = func;
            gate_cnt++;
            func = "none";
        }
        else if (func == "or") {
            cnt[2]++;
            while(line.find("N") != string::npos) {
                gate[gate_cnt][port_cnt[gate_cnt]++] = line.substr(line.find("N") + 1, line.find(",") - line.find("N") - 1);
                line = line.substr(line.find(",") + 1, line.length() - line.find(",") - 1);
            }
            type[gate_cnt] = func;
            gate_cnt++;
            func = "none";
        }
        else if (func == "nor") {
            cnt[3]++;
            while(line.find("N") != string::npos) {
                gate[gate_cnt][port_cnt[gate_cnt]++] = line.substr(line.find("N") + 1, line.find(",") - line.find("N") - 1);
                line = line.substr(line.find(",") + 1, line.length() - line.find(",") - 1);
            }
            type[gate_cnt] = func;
            gate_cnt++;
            func = "none";
        }
        else if (func == "xor") {
            cnt[4]++;
            while(line.find("N") != string::npos) {
                gate[gate_cnt][port_cnt[gate_cnt]++] = line.substr(line.find("N") + 1, line.find(",") - line.find("N") - 1);
                line = line.substr(line.find(",") + 1, line.length() - line.find(",") - 1);
            }
            type[gate_cnt] = func;
            gate_cnt++;
            func = "none";
        }
        else if (func == "xnor") {
            cnt[5]++;
            while(line.find("N") != string::npos) {
                gate[gate_cnt][port_cnt[gate_cnt]++] = line.substr(line.find("N") + 1, line.find(",") - line.find("N") - 1);
                line = line.substr(line.find(",") + 1, line.length() - line.find(",") - 1);
            }
            type[gate_cnt] = func;
            gate_cnt++;
            func = "none";
        }
        else if (func == "buf") {
            cnt[6]++;
            while(line.find("N") != string::npos) {
                gate[gate_cnt][port_cnt[gate_cnt]++] = line.substr(line.find("N") + 1, line.find(",") - line.find("N") - 1);
                line = line.substr(line.find(",") + 1, line.length() - line.find(",") - 1);
            }
            type[gate_cnt] = func;
            gate_cnt++;
            func = "none";
        }
        else if (func == "not") {
            cnt[7]++;
            while(line.find("N") != string::npos) {
                gate[gate_cnt][port_cnt[gate_cnt]++] = line.substr(line.find("N") + 1, line.find(",") - line.find("N") - 1);
                line = line.substr(line.find(",") + 1, line.length() - line.find(",") - 1);
            }
            type[gate_cnt] = func;
            gate_cnt++;
            func = "none";
        }
        else {
            //cout << "func == none" << endl;
        }

        if(line.find(";") != string::npos) 
            func = "none";
            
        //cout << line << endl;
    }

    // cout << "input_cnt is: " << input_cnt << endl;
    // for(int i = 0; i < input_cnt; i++) {
    //     cout << input[i] << endl;
    // }
    // cout << "output_cnt is: " << output_cnt << endl;
    // for(int i = 0; i < output_cnt; i++) {
    //     cout << output[i] << endl;
    // }
    // cout << not_cnt << " " << and_cnt << " " << nand_cnt << " " << or_cnt << " " << nor_cnt << " " << buf_cnt << endl;

    // for(int i = 0; i < gate_cnt; i++) {
    //     for(int j = 0; j < port_cnt[i]; j++) {
    //         //cout << "i is : " << i << "   " << "j is : " << j << endl;
    //         cout << gate[i][j] << " ";
    //     }
    //     cout << endl;
    // }

    myFile.close();

    for(int i = 0; i < 8; i++) {
        myFile << cnt[i] << " ";
    }


    
    myFile.open(argv[2], ios::out);

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

    myFile << "# " << cnt[7];
    if(cnt[7] == 0 || cnt[7] == 1)
        myFile << " inverter" << endl;
    else
        myFile << " inverters" << endl;
    myFile << "# " << gate_cnt - cnt[7] << " gates ";
    myFile << "( ";
    for(int i = 0; i < 7; i++) {
        if(cnt[i] == 0)
            continue;
        else {
            switch(i) {
                case 0:
                    myFile << cnt[i] << " " << "ANDs";
                    break;
                case 1:
                    myFile << cnt[i] << " " << "NANDs";
                    break;
                case 2:
                    myFile << cnt[i] << " " << "ORs";
                    break;
                case 3:
                    myFile << cnt[i] << " " << "NORs";
                    break;
                case 4:
                    myFile << cnt[i] << " " << "XORs";
                    break;
                case 5:
                    myFile << cnt[i] << " " << "XNORs";
                    break;
                case 6:
                    myFile << cnt[i] << " " << "buffers";
                    break;
            }
            for(int j = i + 1; j < 7; j++) {
                if(cnt[j] != 0){
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
        for(int j = 0; j < port_cnt[i]; j++) {
            if(j == 0) {
                myFile << gate[i][j] << " = ";
                if(type[i] == "and")
                    myFile << "AND(";
                if(type[i] == "nand")
                    myFile << "NAND(";
                if(type[i] == "or")
                    myFile << "OR(";
                if(type[i] == "nor")
                    myFile << "NOR(";
                if(type[i] == "xor")
                    myFile << "XOR(";
                if(type[i] == "xnor")
                    myFile << "XNOR(";
                if(type[i] == "not")
                    myFile << "NOT(";
                if(type[i] == "buf")
                    myFile << "BUFF(";
            }
            else if(j == port_cnt[i] - 1) {
                myFile << gate[i][j] << ")";
            }
            else {
                myFile << gate[i][j] << ", ";
            }
        }
        myFile << endl;
    } 

    myFile.close();

    return 0;
}
