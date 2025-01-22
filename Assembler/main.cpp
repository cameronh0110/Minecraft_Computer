#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

using namespace std;

string assemble(vector<string> tokens){
    string ret;
    string inst = tokens.at(0);
    
    return ret;
}

//initializes a map to convert opcodes to binary
unordered_map<string, string> Init_Inst_Map(){
    unordered_map<string, string> inst_map;
    
    //alu 2 op
    inst_map["ADD"]         = "0000 ";
    inst_map["SUB"]         = "0001 ";
    inst_map["XOR"]         = "0010 ";
    inst_map["AND"]         = "0011 ";
    //alu immediate
    inst_map["ADDI"]        = "0100 ";
    inst_map["SUBI"]        = "0101 ";
    inst_map["SHL"]         = "0110 ";
    inst_map["SHR"]         = "0111 ";
    //memory
    inst_map["LDUR-ADR"]    = "1000 ";
    inst_map["SDUR-ADR"]    = "1001 ";
    inst_map["LDUR-ADR"]    = "1010 ";
    inst_map["SDUR-ADR"]    = "1011 ";
    //flow
    inst_map["GOTO"]        = "1100 ";
    inst_map["GOTO-PTR"]    = "1101 ";
    inst_map["BRC"]         = "1110 ";
    inst_map["END"]         = "1111 ";

    return inst_map;
}

int main(int argc, char **argv){
    /*
    Gaurd cases and filename processing
    */
    //check if file provided
    if(!argc){
        cout << "ERROR: No File Provided\n";
        return 0;
    }
    //check file extension
    string inputFileName = argv[1];
    if(inputFileName.substr(inputFileName.find('.'), inputFileName.length()) != ".mcasm"){
        cout << "ERROR: Filename must end in .mcasm\n";
        return 0;
    }
    //use output file name if provided, or create a name based on the input file ending with .mbin
    string outputFileName;
    if(argc > 2){
        outputFileName = argv[2];
    } else {
        outputFileName = inputFileName.substr(0, inputFileName.find('.')) + ".mcbin";
    }
    
    /*
    File management
    */
    ifstream fin;
    ofstream fout;
    string read;
    string out;

    fin.open(inputFileName);
    fout.open(outputFileName);

    if(!fin.is_open()){
        cout << "ERROR: could not open input file " << inputFileName << endl;
        return 0;
    }
    if(!fout.is_open()){
        cout << "ERROR: could not open output file " << outputFileName << endl;
        return 0;
    }

    /*
    Read and process file
    */
    int outLine = 0;    //line of the output file
    int inLine = 0;     //line from input file being process
    while(!fin.eof()){
        //read line and create a vector of tokens
        getline(fin, read);
        inLine++;

        vector<string> tokens;
        istringstream stream(read);

        //tokenize line, rejecting blank lines, and comments (anything after a '#')
        while(stream >> read && read.at(0) != '#' && read != ""){
            tokens.push_back(read);
        }

        

        //if there are valid tokens, process and output them. Output is a string, as the binary code must be manually copied to ROM in game
        if(tokens.size()){
            cout << setw(3) << setfill('0') << outLine << ": 0000 0000 0000 0000 ";
            for(int i = 0; i < tokens.size(); i++){
                cout << tokens.at(i) << " ";
            };
            cout << endl;
            
            outLine++;
        }
        
    }
    return 1;

}