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

    pc = 0;
    opcode = 0;
    instArgA = 0;
    instArgB = 0;
    instArgC = 0;
}

void printState(){
    cout << "Cache Status: " << "Read Adr A: " << setw(2) << setfill('0') << readAdrA << " \t Read Adr B: " << setw(2) << setfill('0') << readAdrB << " \t Write Adr: " << setw(2) << setfill('0') << readAdrB << endl;
    cout << "              Read A:    " << setw(3) << setfill('0') << readA << " \t Read B:    " << setw(3) << setfill('0') << readB << " \t Write:    " << setw(3) << setfill('0') << write << endl;
    cout << endl;
    cout << "ALU Status:   " << "OP:          " << ALUOP << " \t Output:    " << setw(3) << setfill('0') << ALUout << endl;
    cout << "              OP A:      " << setw(3) << setfill('0') << ALUA << " \t OP B:      " << setw(3) << setfill('0') << ALUB << endl;
    cout << endl;
    cout << "GPAIO Status: Value:     " << setw(3) << setfill('0') << gpaioBus << "\t Address:   " << setw(3) << setfill('0') << gpaioAdr << endl;
    cout << "Inst " << setw(3) << setfill('0') << pc << ": " << instArgA << " " << instArgB << " " << instArgC << endl;
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

int StringBinaryToInt(string input){

}

void InstructionProcessor(){
    string input;
}

void loop(){
    while(true){
        system("clear");
        zero();

        printState();
        cout << "Iteration: " << iteration << endl;
        iteration++;
        this_thread::sleep_for(chrono::milliseconds(speed));
    }
    
}

int main(int argc, char **argv){
    if(!argc){
        cout << "Error: expected filename for rom" << endl;
        return 1;
    }

    fin.open(argv[0]);
    if(!fin.is_open()){
        cout << "Error: Could not open file" << endl;
    }

    cout << "Enter max clock speed for simulation: ";
    cin >> speed;
    speed = 1000/speed;

    loop();

    return 1;
}