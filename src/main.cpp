#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

string read_file_contents(const string& filename);

struct tok{
    string type;
    string lexeme;
    string literal;

    tok(string t , string lex , string lit) : type {t} , lexeme {lex} , literal {lex} {}
};

vector<tok> Tokens;

void handleTokenisation(string& file) {
    for(char c : file) {
        if(c == '(') {
            Tokens.push_back({"LEFT_PAREN","(","NULL"});
        }
        else if(c == ')') {
            Tokens.push_back({"RIGHT_PAREN",")","NULL"});
        }
    }

    Tokens.push_back({"EOF","","NULL"});
}

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
        string file = read_file_contents(argv[2]);
        
        if (!file.empty()) {
            cerr << "Scanner not implemented" << endl;
            return 1;
        }

        for(auto& t : Tokens) {
            cout<<t.type<<" "<<t.lexeme<<" "<<t.literal<<endl;
        }
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
