#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
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
vector<string> reservedWord = {"and","class","else","false","for","fun","if","nil","or","print",
    "return","super","this","true","var","while"};
int lineNo ;

string upperCase(string str) {
    transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return std::toupper(c); });
    return str;
}

void handleTokenisation(string& file) {
    bool insideComment = false , insideString = false , insideNum = false , insideWord = false;
    string currStr = "" , currNum = "" , currWord = "";
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
        if(insideWord) {
            if((file[i]>='a' && file[i]<='z') || (file[i]>='A' && file[i]<='Z') || file[i]=='_' || 
            (file[i]>='0' && file[i]<='9')) {
                currWord += file[i];
                continue;
            }
            else {
                insideWord = false;
                bool flag = true;

                for(string& temp : reservedWord) {
                    if(temp == currWord) {
                        flag = false;
                        break;
                    }
                }

                if(flag) {
                    Tokens.push_back({"IDENTIFIER",currWord,"null"});
                }
                else {
                    Tokens.push_back({upperCase(currWord),currWord,"null"});
                }

                currWord = "";
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
        else if((file[i]<='z' && file[i]>='a') || (file[i]<='Z' && file[i]>='A') || file[i]=='_') {
            currWord += file[i];
            insideWord = true;
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
    if(insideWord) {
        insideWord = false;
        bool flag = true;

        for(string& temp : reservedWord) {
            if(temp == currWord) {
                flag = false;
                break;
            }
        }

        if(flag) {
            Tokens.push_back({"IDENTIFIER",currWord,"null"});
        }
        else {
            Tokens.push_back({upperCase(currWord),currWord,"null"});
        }
    }
    Tokens.push_back({"EOF","","null"});
}

class Parser {
    private:
    vector<tok>& tokens;
    int curr;
    bool hadError;
    string errorMsg;
    
    tok peek() {
        return tokens[curr];
    }
    
    tok previous() {
        return tokens[curr - 1];
    }
    
    bool isAtEnd() {
        return peek().type == "EOF";
    }
    
    tok advance() {
        if (!isAtEnd()) curr++;
        return previous();
    }
    
    bool check(const string& type) {
        if (isAtEnd()) return false;
        return peek().type == type;
    }
    
    bool match(const vector<string>& types) {
        for (const string& type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }
    
    void error(const string& message) {
        hadError = true;
        tok token = peek();
        if (token.type == "EOF") {
            errorMsg = "[line " + to_string(lineNo) + "] Error at end: " + message;
        } else {
            errorMsg = "[line " + to_string(lineNo) + "] Error at '" + token.lexeme + "': " + message;
        }
    }
    
    tok consume(const string& type, const string& message) {
        if (check(type)) return advance();
        error(message);
        return peek();
    }
    
    void synchronize() {
        advance();
        
        while (!isAtEnd()) {
            if (previous().type == "SEMICOLON") return;
            
            if (peek().type == "CLASS" || peek().type == "FUN" || 
                peek().type == "VAR" || peek().type == "FOR" ||
                peek().type == "IF" || peek().type == "WHILE" ||
                peek().type == "PRINT" || peek().type == "RETURN") {
                return;
            }
            
            advance();
        }
    }
    
    string primary() {
        if (match({"FALSE"})) return "false";
        if (match({"TRUE"})) return "true";
        if (match({"NIL"})) return "nil";
        
        if (match({"NUMBER"})) {
            return previous().literal;
        }
        
        if (match({"STRING"})) {
            return previous().literal;
        }
        
        if (match({"LEFT_PAREN"})) {
            string expr = expression();
            consume("RIGHT_PAREN", "Expect ')' after expression.");
            return "(group " + expr + ")";
        }
        
        error("Expect expression.");
        return "";
    }
    
    string unary() {
        if (match({"BANG", "MINUS"})) {
            string op = previous().lexeme;
            string right = unary();
            return "(" + op + " " + right + ")";
        }
        
        return primary();
    }
    
    string factor() {
        string expr = unary();
        
        while (match({"SLASH", "STAR"})) {
            string op = previous().lexeme;
            string right = unary();
            expr = "(" + op + " " + expr + " " + right + ")";
        }
        
        return expr;
    }
    
    string term() {
        string expr = factor();
        
        while (match({"MINUS", "PLUS"})) {
            string op = previous().lexeme;
            string right = factor();
            expr = "(" + op + " " + expr + " " + right + ")";
        }
        
        return expr;
    }
    
    string comparison() {
        string expr = term();
        
        while (match({"GREATER", "GREATER_EQUAL", "LESS", "LESS_EQUAL"})) {
            string op = previous().lexeme;
            string right = term();
            expr = "(" + op + " " + expr + " " + right + ")";
        }
        
        return expr;
    }
    
    string equality() {
        string expr = comparison();
        
        while (match({"BANG_EQUAL", "EQUAL_EQUAL"})) {
            string op = previous().lexeme;
            string right = comparison();
            expr = "(" + op + " " + expr + " " + right + ")";
        }
        
        return expr;
    }
    
    string expression() {
        return equality();
    }
    
    public:
    Parser(vector<tok>& tokens) : tokens(tokens), curr(0), hadError(false), errorMsg("") {}
    
    string parse() {
        try {
            return expression();
        } catch (...) {
            return "";
        }
    }
    
    bool hasError() {
        return hadError;
    }
    
    string getError() {
        return errorMsg;
    }
};

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
    else if(command == "parse") {
        lineNo = 1;
        string file = read_file_contents(argv[2]);
        handleTokenisation(file);
        
        vector<tok> validTokens;
        for(auto& t : Tokens) {
            if(t.type != "error") {
                validTokens.push_back(t);
            }
        }
        
        Parser parser(validTokens);
        string result = parser.parse();
        
        if (parser.hasError()) {
            cerr << parser.getError() << endl;
            exitCode = 65;
        } 
        else {
            cout << result << endl;
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
