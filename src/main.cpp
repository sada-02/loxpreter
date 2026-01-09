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
vector<string> errors;
int lineNo ;

void handleTokenisation(string& file) {
    for(int i=0 ;i<file.size() ;i++) {
        if(file[i] == '(') {
            Tokens.push_back({"LEFT_PAREN","(","null"});
        }
        else if(file[i] == ')') {
            Tokens.push_back({"RIGHT_PAREN",")","null"});
        }
        else if(file[i] == '{') {
            Tokens.push_back({"LEFT_BRACE","{","null"});
        }
        else if(file[i] == '}') {
            Tokens.push_back({"RIGHT_BRACE","}","null"});
        }
        else if(file[i] == '*') {
            Tokens.push_back({"STAR","*","null"});
        }
        else if(file[i] == '+') {
            Tokens.push_back({"PLUS","+","null"});
        }
        else if(file[i] == '.') {
            Tokens.push_back({"DOT",".","null"});
        }
        else if(file[i] == ',') {
            Tokens.push_back({"COMMA",",","null"});
        }
        else if(file[i] == '-') {
            Tokens.push_back({"MINUS","-","null"});
        }
        else if(file[i] == ';') {
            Tokens.push_back({"SEMICOLON",";","null"});
        }
        else if(file[i] == '=') {
            bool flag = true;
            if(i+1 < file.size()) {
                if(file[i+1] == '=') {
                    Tokens.push_back({"EQUAL_EQUAL","==","null"});
                    flag = false;
                }
            }

            if(flag) {
                Tokens.push_back({"EQUAL","=","null"});
            }
            else {
                i++;
            }
        }
        else if(file[i] == '\n') {
            lineNo++;
        }
        else {
            Tokens.push_back({"error","",""});
            errors.push_back("[line " + to_string(lineNo) + "] Error: Unexpected character: "+s[i]);
        }
    }

    Tokens.push_back({"EOF","","null"});
}

int main(int argc, char *argv[]) {
    cout << unitbuf;
    cerr << unitbuf;

    int exitCode = 0;
    if (argc < 3) {
        cerr << "Usage: ./your_program tokenize <filename>" << endl;
        exitCode = 1;
        return exitCode;
    }

    const string command = argv[1];

    if (command == "tokenize") {
        lineNo = 1;
        string file = read_file_contents(argv[2]);

        handleTokenisation(file);
        int errorCnt = 0;
        for(auto& t : Tokens) {
            if(t.type != "error") {
                cout<<t.type<<" "<<t.lexeme<<" "<<t.literal<<endl;
            }
            else {
                cerr<<errors[errorCnt]<<endl;
                errorCnt++;
                exitCode = 65;
            }
        }
    } 
    else {
        cerr << "Unknown command: " << command << endl;
        exitCode = 1;
    }

    return exitCode;
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
