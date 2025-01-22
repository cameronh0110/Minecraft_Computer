#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

//global control variables
double speed;

//global component variables
ifstream romFile;
int cache[15];
int memory[255];

//global bus variables
int readAdrA;
int readAdrB;
int writeAdr;
int readA;
int readB;
int write;

int ALUOP;
int ALUout;

int gpaioBus;
int gpaioAdr;

void loop(){

}

int main(int argc, char **argv){
    romFile = argv
}