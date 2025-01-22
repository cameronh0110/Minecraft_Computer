#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

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
    int line = 0;
    while(!fin.eof()){
        getline(fin, read);

        vector<string> tokens;
        istringstream stream(read);

        while(stream >> read && read.at(0) != '#' && read != ""){
            tokens.push_back(read);
        }
        if(tokens.size()){
            cout << line << ": 0000 0000 0000 0000 ";
            for(int i = 0; i < tokens.size(); i++){
                cout << tokens.at(i) << "\t";
            };
            cout << endl;
            
            line++;
        }
        
    }
    return 1;

}