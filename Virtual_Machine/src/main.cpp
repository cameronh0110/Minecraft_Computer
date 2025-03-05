#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <bitset>
#include <stdlib.h>

using namespace std;

//global control variables
int speed;
bool step = false;

//global component variables
ifstream fin;
string ROM[255];
uint8_t cache[16];
uint8_t memory[256];
int cacheUpdate[16];
int memoryUpdate[256];

//global bus variables
uint8_t readAdrA;
uint8_t readAdrB;
uint8_t writeAdr;
uint8_t readA;
uint8_t readB;
uint8_t write;

uint8_t ALUOP;
uint8_t ALUA;
uint8_t ALUB;
uint8_t ALUout;
uint8_t ALUflag;

uint8_t gpaioBusIn;
uint8_t gpaioBusOut;
uint8_t gpaioAdr;

uint8_t pc = 0;
uint8_t opcode;
uint8_t instArgA;
uint8_t instArgB;
uint8_t instArgC;

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

    gpaioBusIn = 0;
    gpaioBusOut = 0;
    gpaioAdr = 0;

    opcode = 0;
    instArgA = 0;
    instArgB = 0;
    instArgC = 0;

    cache[0] = 0;

    //memory failure test code
    //memory[12] = 85;
}

void printState(){
    string color = "\033[0m";
    cout << "Cache Status: " << "Read Adr A: " << setw(2) << setfill('0') << (uint)readAdrA << " \t Read Adr B: " << setw(2) << setfill('0') << (uint)readAdrB << " \t Write Adr: " << setw(2) << setfill('0') << (uint)writeAdr << endl;
    cout << "              Read A:    " << setw(3) << setfill('0') << (uint)readA << " \t Read B:    " << setw(3) << setfill('0') << (uint)readB << " \t Write:    " << setw(3) << setfill('0') << (uint)write << endl;
    cout << endl;
    cout << "ALU Status:   " << "OP:          " << (uint)ALUOP << " \t Output:    " << setw(3) << setfill('0') << (uint)ALUout << " \t Flag:     " << setw(3) << setfill('0') << (uint)ALUflag << endl;
    cout << "              OP A:      " << setw(3) << setfill('0') << (uint)ALUA << " \t OP B:      " << setw(3) << setfill('0') << (uint)ALUB << endl;
    cout << endl;
    cout << "GPAIO Status: Value In:  " << setw(3) << setfill('0') << (uint)gpaioBusIn << "\tValueOut:   " << setw(3) << setfill('0') << (uint)gpaioBusOut << "\t\t Address:   " << setw(3) << setfill('0') << (uint)gpaioAdr << endl;
    cout << "Inst " << setw(3) << setfill('0') << (uint)pc << ": " << setw(2) << setfill('0') <<  (uint)opcode << " " << setw(2) << setfill('0') <<  (uint)instArgA << " " << setw(2) << setfill('0') <<  (uint)instArgB << " " << setw(2) << setfill('0') <<  (uint)instArgC << endl;
    cout << endl;
    cout << "Cache   | RAM" << endl;
    cout << "________|_______________________________________________________________________________________________________________________________________________" << endl;
    for(int i = 0; i < 16; i++){
        if(cacheUpdate[i] == 1){
            color = "\033[33m";
            cacheUpdate[i] = 2;
        } else if (cacheUpdate[i] == 2){
            color = "\033[32m";
        } else {
            color = "\033[0m";
        }
        cout << "\033[90m" << setw(2) << setfill('0') << i << "| " << color << setw(3) << setfill('0') << (uint)cache[i] << "\033[0m | ";
        for(int j = 0; j < 16; j++){
            if(memoryUpdate[(16*j) + i] == 1){
                color = "\033[33m";
                memoryUpdate[(16*j) + i] = 2;
            } else if(memoryUpdate[(16*j) + i] == 2){
                color = "\033[32m";
            } else {
                color = "\033[0m";
            }
            cout << "\033[90m" << setw(3) << setfill('0') << ((16*j) + i) << ":" << color << setw(3) << setfill('0') << (uint)memory[(16*j) + i] << "\033[0m" << "  ";
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
    //ALU Immediate
    } else if(opcode <= 7){
        readAdrA = instArgA;
        readAdrB = instArgC;
        readA = cache[readAdrA];
        readB = cache[readAdrB];
        writeAdr = instArgC;
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
    if(ALUOP == 2){ALUout = ALUA ^ ALUB; write = ALUout;}
    if(ALUOP == 3){ALUout = ALUA & ALUB; write = ALUout;}
    if(ALUOP == 6){ALUout = ALUB << 1; write = ALUout;}
    if(ALUOP == 7){ALUout = ALUB >> 1; write = ALUout;}
    //set condition flags for arithmetic operations
    if(opcode <= 7){
        ALUflag = 0;
        if(ALUout == 0){
            ALUflag += 4;
        }
        if(ALUB < ALUA){
            ALUflag += 1;
        }
        if(ALUout > 255 || ALUout < 0){
            ALUflag += 2;
        }
    }
    
}

void Flow(){
    if(opcode == 12){   //GOTO
        pc = ALUout;
    } else if(opcode == 14){ //BRC
        //if flag is true, skip next instruction
        if(instArgB & ALUflag){
            pc++;
        }
        pc++;
    } else if(opcode == 15){
        return;
    } else {
        pc++;
    }
}

void Write(){
    if(opcode <= 7){
        cache[writeAdr] = ALUout;
        cacheUpdate[writeAdr] = 1;
    } else if(opcode == 13){
        cache[writeAdr] = ALUout;
        cacheUpdate[writeAdr] = 1;
    }

    cache[0] = 0;
}

void RamLoad(){
    cache[instArgC] = gpaioBusOut;
    cacheUpdate[instArgC] = 1;
}

void RamStore(){
    memory[gpaioAdr] = gpaioBusIn;
    memoryUpdate[gpaioAdr] = 1;
}

//Handles GPAIO Bus, assumes all addresses are for RAM, with both read and write access
void GPAIO(){
    if(opcode == 8){
        gpaioAdr = instArgA*16 + instArgB;
        gpaioBusOut = memory[gpaioAdr];
        RamLoad();
    } else if(opcode == 9){
        gpaioAdr = instArgA*16 + instArgB;
        gpaioBusIn = cache[instArgC];
        RamStore();
    } else if(opcode == 10){
        gpaioAdr = cache[instArgB];
        gpaioBusOut = memory[gpaioAdr];
        RamLoad();
    } else if(opcode == 11){
        gpaioAdr = cache[instArgB];
        gpaioBusIn = cache[instArgC];
        RamStore();
    }
}

void loop(){
    pc = 0;
    while(true){
        auto start = chrono::high_resolution_clock::now();
        system("clear");
        cout << "PC: " << (uint)pc << endl;
        zero();

        InstructionProcessor();
        SetAddrLines();
        BusIn();
        ALU();
        GPAIO();
        Write();
        Flow();

        printState();
        cout << "Iteration: " << iteration << endl;
        iteration++;
        if(opcode == 15){
            //printState();
            cout << "END signal received from code: exiting..." << endl;
            return;
        }
        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::nanoseconds>(stop - start);
        this_thread::sleep_for(chrono::nanoseconds(speed - duration.count()));
        if(step){
            cin.get();
        }
        
    }
    
}

void Randomize(){
    srand(time(0));
    for(int i = 0; i < 16; i++){    
        cache[i] = rand() % 255;
    }
    for(int i = 0; i < 256; i++){    
        memory[i] = rand() % 256;
    }
}

int main(int argc, char **argv){
    char input;
    if(!argc){
        cout << "Error: expected filename for rom" << endl;
        return 1;
    }
    string fileName = argv[1];
    string inputFileName = argv[1];
    if(inputFileName.substr(inputFileName.find('.'), inputFileName.length()) != ".mcrom"){
        cout << "ERROR: Filename must end in .mcrom\n";
        return 1;
    }
    fin.open(fileName);
    if(!fin.is_open()){
        cout << "Error: Could not open file" << endl;
    }
    LoadROM();
    cout << "ROM Loaded: " << fileName << endl;

    cout << "Enter max clock speed for simulation: ";
    cin >> speed;
    cout << "Randomize memory? (y/n): ";
    cin >> input;
    if(tolower(input) == 'y'){
        Randomize();
    }
    cout << "Step by step? (y/n): ";
    cin >> input;
    if(tolower(input) == 'y'){
        step = true;
        speed = 2000;
    }
    speed = 1000000000/speed;

    //run program
    auto start = chrono::high_resolution_clock::now();
    loop();
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::nanoseconds>(stop - start);
    cout << "Program finished after: " << (float)duration.count()/1000000000 << " seconds" << endl;
    cout << "Actual Execution rate:  " << (float)iteration/((float)duration.count()/1000000000) << " Hrtz" << endl;

    this_thread::sleep_for(chrono::seconds(2));

    return 0;
}