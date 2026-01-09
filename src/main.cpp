#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;
string read_file_contents(const string& filename);

int main(int argc, char *argv[]) {
    cout << unitbuf;
    cerr << unitbuf;

    cerr << "Logs from your program will appear here!" << endl;

    if (argc < 3) {
        cerr << "Usage: ./your_program tokenize <filename>" << endl;
        return 1;
    }

    const string command = argv[1];

    if (command == "tokenize") {
        string file_contents = read_file_contents(argv[2]);
        
        if (!file_contents.empty()) {
            cerr << "Scanner not implemented" << endl;
            return 1;
        }
        cout << "EOF  null" << endl; 
    } 
    else {
        cerr << "Unknown command: " << command << endl;
        return 1;
    }

    return 0;
}

string read_file_contents(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error reading file: " << filename << endl;
        exit(1);
    }

    stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}
