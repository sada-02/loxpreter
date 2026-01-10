#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cmath>
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

struct Expr {
    virtual ~Expr() {}
    virtual string toString() = 0;
};

struct LiteralExpr : Expr {
    string value;
    string type; 
    
    LiteralExpr(string v, string t) : value(v), type(t) {}
    
    string toString() override {
        if (type == "nil") return "nil";
        if (type == "boolean") return value;
        return value;
    }
};

struct UnaryExpr : Expr {
    string op;
    Expr* right;
    
    UnaryExpr(string o, Expr* r) : op(o), right(r) {}
    
    ~UnaryExpr() { delete right; }
    
    string toString() override {
        return "(" + op + " " + right->toString() + ")";
    }
};

struct BinaryExpr : Expr {
    Expr* left;
    string op;
    Expr* right;
    
    BinaryExpr(Expr* l, string o, Expr* r) : left(l), op(o), right(r) {}
    
    ~BinaryExpr() { 
        delete left; 
        delete right; 
    }
    
    string toString() override {
        return "(" + op + " " + left->toString() + " " + right->toString() + ")";
    }
};

struct GroupingExpr : Expr {
    Expr* expression;
    
    GroupingExpr(Expr* e) : expression(e) {}
    
    ~GroupingExpr() { delete expression; }
    
    string toString() override {
        return "(group " + expression->toString() + ")";
    }
};

class Interpreter {
    private:
    bool hadRuntimeError;
    string runtimeErrorMsg;
    
    void runtimeError(const string& message) {
        hadRuntimeError = true;
        runtimeErrorMsg = message;
    }
    
    bool isTruthy(const string& value, const string& type) {
        if (type == "nil") return false;
        if (type == "boolean") return value == "true";
        return true;
    }
    
    bool isEqual(const string& left, const string& leftType, 
                 const string& right, const string& rightType) {
        if (leftType == "nil" && rightType == "nil") return true;
        if (leftType == "nil" || rightType == "nil") return false;
        if (leftType != rightType) return false;
        
        if (leftType == "number") {
            double leftNum = stod(left);
            double rightNum = stod(right);
            return leftNum == rightNum;
        }
        
        return left == right;
    }
    
    public:
    struct Value {
        string val;
        string type; 
    };
    
    Interpreter() : hadRuntimeError(false), runtimeErrorMsg("") {}
    
    Value evaluate(Expr* expr) {
        if (auto* lit = dynamic_cast<LiteralExpr*>(expr)) {
            return {lit->value, lit->type};
        }
        else if (auto* group = dynamic_cast<GroupingExpr*>(expr)) {
            return evaluate(group->expression);
        }
        else if (auto* unary = dynamic_cast<UnaryExpr*>(expr)) {
            Value right = evaluate(unary->right);
            
            if (unary->op == "-") {
                if (right.type != "number") {
                    runtimeError("Operand must be a number.");
                    return {"", "nil"};
                }
                double num = stod(right.val);
                return {to_string(-num), "number"};
            }
            else if (unary->op == "!") {
                bool truthiness = isTruthy(right.val, right.type);
                return {truthiness ? "false" : "true", "boolean"};
            }
        }
        else if (auto* binary = dynamic_cast<BinaryExpr*>(expr)) {
            Value left = evaluate(binary->left);
            Value right = evaluate(binary->right);
            
            if (binary->op == "-" || binary->op == "/" || binary->op == "*") {
                if (left.type != "number" || right.type != "number") {
                    runtimeError("Operands must be numbers.");
                    return {"", "nil"};
                }
                double leftNum = stod(left.val);
                double rightNum = stod(right.val);
                double result;
                
                if (binary->op == "-") result = leftNum - rightNum;
                else if (binary->op == "*") result = leftNum * rightNum;
                else { 
                    if (rightNum == 0) {
                        runtimeError("Division by zero.");
                        return {"", "nil"};
                    }
                    result = leftNum / rightNum;
                }
                return {to_string(result), "number"};
            }
            else if (binary->op == "+") {
                if (left.type == "number" && right.type == "number") {
                    double result = stod(left.val) + stod(right.val);
                    return {to_string(result), "number"};
                }
                else if (left.type == "string" && right.type == "string") {
                    return {left.val + right.val, "string"};
                }
                else {
                    runtimeError("Operands must be two numbers or two strings.");
                    return {"", "nil"};
                }
            }
            else if (binary->op == ">" || binary->op == ">=" || 
                     binary->op == "<" || binary->op == "<=") {
                if (left.type != "number" || right.type != "number") {
                    runtimeError("Operands must be numbers.");
                    return {"", "nil"};
                }
                double leftNum = stod(left.val);
                double rightNum = stod(right.val);
                bool result;
                
                if (binary->op == ">") result = leftNum > rightNum;
                else if (binary->op == ">=") result = leftNum >= rightNum;
                else if (binary->op == "<") result = leftNum < rightNum;
                else result = leftNum <= rightNum;
                
                return {result ? "true" : "false", "boolean"};
            }
            else if (binary->op == "==") {
                bool result = isEqual(left.val, left.type, right.val, right.type);
                return {result ? "true" : "false", "boolean"};
            }
            else if (binary->op == "!=") {
                bool result = !isEqual(left.val, left.type, right.val, right.type);
                return {result ? "true" : "false", "boolean"};
            }
        }
        
        return {"", "nil"};
    }
    
    bool hasError() { return hadRuntimeError; }
    string getError() { return runtimeErrorMsg; }
};

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
    
    Expr* primaryExpr() {
        if (match({"FALSE"})) return new LiteralExpr("false", "boolean");
        if (match({"TRUE"})) return new LiteralExpr("true", "boolean");
        if (match({"NIL"})) return new LiteralExpr("nil", "nil");
        
        if (match({"NUMBER"})) {
            return new LiteralExpr(previous().literal, "number");
        }
        
        if (match({"STRING"})) {
            return new LiteralExpr(previous().literal, "string");
        }
        
        if (match({"LEFT_PAREN"})) {
            Expr* expr = expressionExpr();
            consume("RIGHT_PAREN", "Expect ')' after expression.");
            return new GroupingExpr(expr);
        }
        
        error("Expect expression.");
        return new LiteralExpr("nil", "nil");
    }
    
    string unary() {
        if (match({"BANG", "MINUS"})) {
            string op = previous().lexeme;
            string right = unary();
            return "(" + op + " " + right + ")";
        }
        
        return primary();
    }
    
    Expr* unaryExpr() {
        if (match({"BANG", "MINUS"})) {
            string op = previous().lexeme;
            Expr* right = unaryExpr();
            return new UnaryExpr(op, right);
        }
        
        return primaryExpr();
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
    
    Expr* factorExpr() {
        Expr* expr = unaryExpr();
        
        while (match({"SLASH", "STAR"})) {
            string op = previous().lexeme;
            Expr* right = unaryExpr();
            expr = new BinaryExpr(expr, op, right);
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
    
    Expr* termExpr() {
        Expr* expr = factorExpr();
        
        while (match({"MINUS", "PLUS"})) {
            string op = previous().lexeme;
            Expr* right = factorExpr();
            expr = new BinaryExpr(expr, op, right);
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
    
    Expr* comparisonExpr() {
        Expr* expr = termExpr();
        
        while (match({"GREATER", "GREATER_EQUAL", "LESS", "LESS_EQUAL"})) {
            string op = previous().lexeme;
            Expr* right = termExpr();
            expr = new BinaryExpr(expr, op, right);
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
    
    Expr* equalityExpr() {
        Expr* expr = comparisonExpr();
        
        while (match({"BANG_EQUAL", "EQUAL_EQUAL"})) {
            string op = previous().lexeme;
            Expr* right = comparisonExpr();
            expr = new BinaryExpr(expr, op, right);
        }
        
        return expr;
    }
    
    string expression() {
        return equality();
    }
    
    Expr* expressionExpr() {
        return equalityExpr();
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
    
    Expr* parseExpr() {
        try {
            return expressionExpr();
        } catch (...) {
            return nullptr;
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
    else if(command == "evaluate") {
        lineNo = 1;
        string file = read_file_contents(argv[2]);
        handleTokenisation(file);
        
        int errorCnt = 0;
        for(auto& t : Tokens) {
            if(t.type == "error") {
                cerr<<errors[errorCnt]<<endl;
                errorCnt++;
                exitCode = 65;
            }
        }
        
        if (exitCode != 0) {
            return exitCode;
        }
        
        vector<tok> validTokens;
        for(auto& t : Tokens) {
            if(t.type != "error") {
                validTokens.push_back(t);
            }
        }
        
        Parser parser(validTokens);
        Expr* ast = parser.parseExpr();
        
        if (parser.hasError()) {
            cerr << parser.getError() << endl;
            exitCode = 65;

            if (ast) delete ast;
        } 
        else if (ast) {
            Interpreter interpreter;
            Interpreter::Value result = interpreter.evaluate(ast);
            
            if (interpreter.hasError()) {
                cerr << interpreter.getError() << endl;
                exitCode = 70;
            } 
            else {
                if (result.type == "number") {
                    double num = stod(result.val);
                    if (num == floor(num)) {
                        cout << static_cast<long long>(num) << endl;
                    } else {
                        cout << num << endl;
                    }
                } 
                else if (result.type == "string") {
                    cout << result.val << endl;
                } 
                else {
                    cout << result.val << endl;
                }
            }
            
            delete ast;
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
