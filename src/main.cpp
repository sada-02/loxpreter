#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
using namespace std;

string read_file_contents(const string& filename);

string formatNumber(const string& numStr) {
    if(numStr.find('.') == string::npos) {
        return numStr + ".0";
    }
    
    string result = numStr;
    while(result.back() == '0' && result.find('.') != string::npos) {
        result.pop_back();
    }
    if(result.back() == '.') {
        result += "0";
    }
    return result;
}

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
    bool insideComment = false , insideString = false , insideNum = false;
    string currStr = "" , currNum = "";
    bool firstDec = false;
    for(int i=0 ;i<file.size() ;i++) {
        if(insideComment) {
            if(file[i] == '\n') {
                insideComment = false;
                lineNo++;
            }
            continue;
        }
        if(insideString) {
            if(file[i] == '\"') {
                insideString = false;
                Tokens.push_back({"STRING","\""+currStr+"\"",currStr});
                currStr = "";
            }
            else {
                currStr += file[i];
            }
            continue;
        }
        if(insideNum) {
            if(file[i] <= '9' && file[i]>='0') {
                currNum += file[i];
                continue;
            } 
            else if(file[i] == '.' && !firstDec) {
                currNum += file[i];
                firstDec = true;
                continue;
            }
            else {
                Tokens.push_back({"NUMBER",currNum,formatNumber(currNum)});
                currNum = "";
                insideNum = false;
                firstDec = false;
            }
        }
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
        else if(file[i] == '\t'|| file[i] == ' ') {
            continue;
        }
        else if(file[i] == '\"') {
            insideString = true;
        }
        else if(file[i]>='0' && file[i]<='9') {
            insideNum = true;
            currNum += file[i];
        }
        else if(file[i] == '/') {
            bool flag = true;
            if(i+1 < file.size()) {
                if(file[i+1] == '/') {
                    insideComment = true;
                    flag = false;
                }
            }

            if(flag) {
                Tokens.push_back({"SLASH","/","null"});
            }
            else {
                i++;
            }
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
        else if(file[i] == '!') {
            bool flag = true;
            if(i+1 < file.size()) {
                if(file[i+1] == '=') {
                    Tokens.push_back({"BANG_EQUAL","!=","null"});
                    flag = false;
                }
            }

            if(flag) {
                Tokens.push_back({"BANG","!","null"});
            }
            else {
                i++;
            }
        }
        else if(file[i] == '<') {
            bool flag = true;
            if(i+1 < file.size()) {
                if(file[i+1] == '=') {
                    Tokens.push_back({"LESS_EQUAL","<=","null"});
                    flag = false;
                }
            }

            if(flag) {
                Tokens.push_back({"LESS","<","null"});
            }
            else {
                i++;
            }
        }
        else if(file[i] == '>') {
            bool flag = true;
            if(i+1 < file.size()) {
                if(file[i+1] == '=') {
                    Tokens.push_back({"GREATER_EQUAL",">=","null"});
                    flag = false;
                }
            }

            if(flag) {
                Tokens.push_back({"GREATER",">","null"});
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
            errors.push_back("[line " + to_string(lineNo) + "] Error: Unexpected character: "+file[i]);
        }
    }

    if(insideString) {
        Tokens.push_back({"error","",""});
        errors.push_back("[line "+ to_string(lineNo) +"] Error: Unterminated string.");
    }
    if(insideNum) {
        Tokens.push_back({"NUMBER",currNum,formatNumber(currNum)});
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
