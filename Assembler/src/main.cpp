#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <bitset>

using namespace std;

//map for instructions variables
unordered_map<string, string> inst_map;
unordered_map<string, function<string(vector<string>&)>> asm_map;

//map for %tags
unordered_map<string, int> tag_map;

//returns binary string format of given number
string ToBinary(string input, int digits){
    bitset<8> binary(stoi(input));
    string ret = binary.to_string().substr(8 - digits);
    return ret;
}

/*
Processes ALU 2 op instructions
*/
string Alu2op(vector<string> &tokens){
    string ret;
    
    if(tokens.size() != 4){cout << "ERROR: Incorrect Number of arguments" << endl; return "ERR";}

    //convert instruction into binary machine code
    ret = inst_map[tokens.at(0)] + ToBinary(tokens.at(1), 4) + " " + ToBinary(tokens.at(2), 4) + " " + ToBinary(tokens.at(3), 4) + " ";

    return ret;
}

/*
Processes all instructions not involving alu dual read
*/
string GenericOp(vector<string> &tokens){
    string ret;
    
    //insert blank space for shifts and flow instructions
    if(tokens.at(0) == "SHL" || tokens.at(0) == "SHR" || tokens.at(0) == "GOTO-PTR"){
        tokens.insert(tokens.begin() + 1, "0");
    }
    if(tokens.at(0) == "BRC"){
        tokens.push_back("0");
    }
    if(tokens.at(0) == "END"){
        tokens.push_back("0");
        tokens.push_back("0");
    }

    if(tokens.size() != 3){cout << "ERROR: Incorrect number of arguments" << endl; return "ERR";}

    //convert instruction into binary machine code
    ret = inst_map[tokens.at(0)] + ToBinary(tokens.at(1), 8).substr(0, 4) + " " + ToBinary(tokens.at(1), 8).substr(4) + " " + ToBinary(tokens.at(2), 4) + " ";

    return ret;
}

//initializes a map to convert opcodes to binary
void Init_Inst_Map(){
    //Alu 2 op
    inst_map["ADD"]         = "0000 ";
    inst_map["SUB"]         = "0001 ";
    inst_map["XOR"]         = "0010 ";
    inst_map["AND"]         = "0011 ";
    //Alu immediate
    inst_map["ADDI"]        = "0100 ";
    inst_map["SUBI"]        = "0101 ";
    inst_map["SHL"]         = "0110 ";
    inst_map["SHR"]         = "0111 ";
    //Memory
    inst_map["LDUR-ADR"]    = "1000 ";
    inst_map["STUR-ADR"]    = "1001 ";
    inst_map["LDUR-PTR"]    = "1010 ";
    inst_map["STUR-PTR"]    = "1011 ";
    //Flow
    inst_map["GOTO"]        = "1100 ";
    inst_map["GOTO-PTR"]    = "1101 ";
    inst_map["BRC"]         = "1110 ";
    inst_map["END"]         = "1111 ";
}

void Init_Assembler_Map(){
    //ALU 2 Op
    asm_map["ADD"]      = Alu2op;
    asm_map["SUB"]      = Alu2op;
    asm_map["XOR"]      = Alu2op;
    asm_map["AND"]      = Alu2op;
    //ALU immediate
    asm_map["ADDI"]     = GenericOp;
    asm_map["SUBI"]     = GenericOp;
    asm_map["SHL"]      = GenericOp;
    asm_map["SHR"]      = GenericOp;
    //Memory
    asm_map["LDUR-ADR"] = GenericOp;
    asm_map["STUR-ADR"] = GenericOp;
    asm_map["LDUR-PTR"] = GenericOp;
    asm_map["STUR-PTR"] = GenericOp;
    //Flow
    asm_map["GOTO"]     = GenericOp;
    asm_map["GOTO-PTR"] = GenericOp;
    asm_map["BRC"]      = GenericOp;
    asm_map["END"]      = GenericOp;
}

/*opens file and saves each line with an instruction to a vector of strings
*/
vector<string> GenerateLineList(string filename){
    vector<string> ret; // vector to return
    string input;       // input string

    ifstream fin;
    fin.open(filename);
    if(!fin.is_open()){
        cout << "ERROR: could not open input file " << filename << endl;
        return ret; // return empty vector if failed
    }

    while(!fin.eof()){
        //read line
        getline(fin, input);
        if(!input.empty() && input[0] != '\n' && input[0] != '#'){
            //input += "\n";
            ret.push_back(input);
        }
    }
    return ret;
}

vector<string> DeComment(vector<string> input){
    vector<string> ret;
    for(int i = 0; i < input.size(); i++){
        ret.push_back(input.at(i).substr(0, input.at(i).find('#')));
    }
    return ret;
}

vector<string> ReadTags(vector<string> input){
    vector<string> ret;

    //add tags to the map, and remove them from the beginning of the tag map
    for(int i = 0; i < input.size(); i++){
        if(input.at(i)[0] == '%'){
            tag_map[input.at(i).substr(0, input.at(i).find(' '))] = i;
            input.at(i) = input.at(i).substr(input.at(i).find(' '), input.at(i).length());
        }
    }
    return input;
}

//converts vector of line strings to a vector of token vectors, and processes tag arguments
vector<vector<string>> Tokenize(vector<string> input){
    vector<vector<string>> ret;
    string read;
    for(int i = 0; i < input.size(); i++){
        vector<string> tokens;
        read = input.at(i);
        istringstream stream(read);
        //tokenize line
        while(stream >> read){
            //replace tag with mapped value
            if(tag_map[read]){
                read = to_string(tag_map[read]);
            }
            tokens.push_back(read);
        }
        ret.push_back(tokens);
    }
    return ret;
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
    Init maps
    */
    Init_Inst_Map();
    Init_Assembler_Map();

    /*
    Generate line list
    */
    vector<string> lines = GenerateLineList(inputFileName);
    lines = DeComment(lines);
    lines = ReadTags(lines);
    vector<vector<string>> tokenizedLines = Tokenize(lines);

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
    int outLine = 0;        //line of the output file
    int inLine = 0;         //line from input file being process
    bool success = true;    //bool for success
    vector<string> errors;  //stores errors to output after completion
    cout << "___Human Transferable Output___________________________________" << endl;
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
        string assembled;   //assembled instruction
        string error;       //error code
        if(tokens.size()){
            //assemble line or return error
            if(asm_map.find(tokens.at(0)) != asm_map.end()){
                assembled = asm_map[tokens.at(0)](tokens);
            } else {
                assembled = "XXXX XXXX XXXX XXXX ";
                error = "Error at line " + to_string(inLine) + " Invalid Instruction";
                errors.push_back(error);
                success = false;
            }
            fout << assembled << endl;
            cout << setw(3) << setfill('0') << outLine << " | " << assembled << "| ";
            for(int i = 0; i < tokens.size(); i++){
                cout << setw(8) << setfill(' ') << tokens.at(i) << " ";
            };
            cout << endl;
            
            outLine++;
        }
    }
    
    /*
    print diagnostic info
    */
    cout << endl;
    for(int i = 0; i < errors.size(); i++){
        cout << errors.at(i) << endl;
    }
    if(!success){
        cout << endl << "Assembly failed, check log for details" << endl << endl;
    } else {
        cout << "Successfully compiled to " << outputFileName << endl << endl;
    }
    return 1;

}