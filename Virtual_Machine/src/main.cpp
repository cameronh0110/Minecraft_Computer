#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <bitset>

using namespace std;

//global control variables
int speed;

//global component variables
ifstream fin;
string ROM[255];
int cache[16];
int memory[255];

//global bus variables
int readAdrA;
int readAdrB;
int writeAdr;
int readA;
int readB;
int write;

int ALUOP;
int ALUA;
int ALUB;
int ALUout;

int gpaioBus;
int gpaioAdr;

int pc;
int opcode;
int instArgA;
int instArgB;
int instArgC;

int iteration = 0;

void zero(){
    readAdrA = 0;
    readAdrB = 0;
    writeAdr = 0;
    readA = 0;
    readB = 0;
    write = 0;

    ALUOP = 0;
    ALUA = 0;
    ALUB = 0;
    ALUout = 0;

    gpaioBus = 0;
    gpaioAdr = 0;

    opcode = 0;
    instArgA = 0;
    instArgB = 0;
    instArgC = 0;
}

void printState(){
    cout << "Cache Status: " << "Read Adr A: " << setw(2) << setfill('0') << readAdrA << " \t Read Adr B: " << setw(2) << setfill('0') << readAdrB << " \t Write Adr: " << setw(2) << setfill('0') << writeAdr << endl;
    cout << "              Read A:    " << setw(3) << setfill('0') << readA << " \t Read B:    " << setw(3) << setfill('0') << readB << " \t Write:    " << setw(3) << setfill('0') << write << endl;
    cout << endl;
    cout << "ALU Status:   " << "OP:          " << ALUOP << " \t Output:    " << setw(3) << setfill('0') << ALUout << endl;
    cout << "              OP A:      " << setw(3) << setfill('0') << ALUA << " \t OP B:      " << setw(3) << setfill('0') << ALUB << endl;
    cout << endl;
    cout << "GPAIO Status: Value:     " << setw(3) << setfill('0') << gpaioBus << "\t Address:   " << setw(3) << setfill('0') << gpaioAdr << endl;
    cout << "Inst " << setw(3) << setfill('0') << pc << ": " << setw(2) << setfill('0') <<  opcode << " " << setw(2) << setfill('0') <<  instArgA << " " << setw(2) << setfill('0') <<  instArgB << " " << setw(2) << setfill('0') <<  instArgC << endl;
    cout << endl;
    cout << "Cache   | RAM" << endl;
    cout << "________|_______________________________________________________________________________________________________________________________________________" << endl;
    for(int i = 0; i < 16; i++){
        cout << "\033[90m" << setw(2) << setfill('0') << i << "|\033[0m " << setw(3) << setfill('0') << cache[i] << " | ";
        for(int j = 0; j < 16; j++){
                cout << "\033[90m" << setw(3) << setfill('0') << (16*j) + i << ":\033[0m" << setw(3) << setfill('0') << memory[(16*j) + i] << "  ";
            }
        cout << endl;
    }
}

void LoadROM(){
    string input;
    int i = 0;
    while(getline(fin, input)){
        ROM[i] = input;
        i++;
    }
}

int StringBinaryToInt(string in){
    string input = in;
    //cout << input;
    bitset<32> bin(input);
    int ret = static_cast<int>(bin.to_ulong());
    return ret;
}

void InstructionProcessor(){
    string input = ROM[pc];
    stringstream str;
    str << input;
    str >> input;
    opcode = StringBinaryToInt(input);
    str >> input;
    instArgA = StringBinaryToInt(input);
    str >> input;
    instArgB = StringBinaryToInt(input);
    str >> input;
    instArgC = StringBinaryToInt(input);
}

void SetAddrLines(){
    //ALU 2 Op
    if(opcode <= 3){
        readAdrA = instArgA;
        readAdrB = instArgB;
        readA = cache[readAdrA];
        readB = cache[readAdrB];
        writeAdr = instArgC;
    } else if(opcode <= 7){
        readAdrA = instArgA;
        readAdrB = instArgC;
        readA = cache[readAdrA];
        readB = cache[readAdrB];
        writeAdr = instArgC;
    } else if(opcode <= 11){
    } else if(opcode == 12){
        readAdrA = instArgA;
        readAdrB = instArgC;
        readA = cache[readAdrA];
        readB = cache[readAdrB];
        writeAdr = instArgC;
    } else if(opcode == 13){
        readAdrA = 0;
        readAdrB = 0;
        readA = cache[readAdrA];
        readB = cache[readAdrB];
        writeAdr = instArgC;
    } else if(opcode == 14){
        writeAdr = instArgC;
    } else if(opcode == 15){
        writeAdr = instArgC;
    }
}

void BusIn(){
    if(opcode <= 3){
        ALUA = readA;
        ALUB = readB;
        ALUOP = opcode;
    } else if(opcode <= 7){
        ALUA = instArgA*16 + instArgB;
        ALUB = readB;
        ALUOP = opcode;
    } else if(opcode <= 11){
        //TODO
    } else if(opcode == 12){
        ALUA = instArgA*16 + instArgB;
        ALUB = readB;
        writeAdr = instArgC;
    } else if(opcode == 13){
        ALUA = readA;
        ALUB = pc;
        writeAdr = instArgC;
    } else if(opcode == 14){
        ALUA = readA;
        ALUB = readB;
        writeAdr = instArgC;
    } else if(opcode == 15){
        ALUA = readA;
        ALUB = readB;
        writeAdr = instArgC;
    }
}

void ALU(){
    if(ALUOP == 0 || ALUOP == 4){ALUout =  ALUA + ALUB; write = ALUout;}
    if(ALUOP == 1 || ALUOP == 5){ALUout = -ALUA + ALUB; write = ALUout;}
}

void Flow(){
    if(opcode == 12){
        pc = ALUout;
    } else {
        pc++;
    }
}

void Write(){
    if(opcode <= 7){
        cache[writeAdr] = ALUout;
    } else if(opcode == 13){
        cache[writeAdr] = ALUout;
    }

    cache[0] = 0;
}

void loop(){
    pc = 0;
    while(true){
        system("clear");
        zero();

        InstructionProcessor();
        SetAddrLines();
        BusIn();
        ALU();
        Write();
        Flow();

        printState();
        cout << "Iteration: " << iteration << endl;
        iteration++;
        this_thread::sleep_for(chrono::milliseconds(speed));
        //cin.get();
    }
    
}

int main(int argc, char **argv){
    if(!argc){
        cout << "Error: expected filename for rom" << endl;
        return 1;
    }
    string fileName = argv[1];
    fin.open(fileName);
    if(!fin.is_open()){
        cout << "Error: Could not open file" << endl;
    }
    LoadROM();
    cout << "ROM Loaded: " << fileName << endl;

    cout << "Enter max clock speed for simulation: ";
    cin >> speed;
    speed = 1000/speed;

    loop();

    return 1;
}