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

    tok(string t , string lex , string lit) : type {t} , lexeme {lex} , literal {lit} {}
};

vector<tok> Tokens;

void handleTokenisation(string& file) {
    for(char c : file) {
        if(c == '(') {
            Tokens.push_back({"LEFT_PAREN","(","null"});
        }
        else if(c == ')') {
            Tokens.push_back({"RIGHT_PAREN",")","null"});
        }
        else if(c == '{') {
            Tokens.push_back({"LEFT_BRACE","{","null"});
        }
        else if(c == '}') {
            Tokens.push_back({"RIGHT_BRACE","}","null"});
        }
        else if(c == '*') {
            Tokens.push_back({"STAR","*","null"});
        }
        else if(c == '+') {
            Tokens.push_back({"PLUS","+","null"});
        }
        else if(c == '.') {
            Tokens.push_back({"DOT",".","null"});
        }
        else if(c == ',') {
            Tokens.push_back({"COMMA",",","null"});
        }
    }

    Tokens.push_back({"EOF","","null"});
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

        handleTokenisation(file);
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
