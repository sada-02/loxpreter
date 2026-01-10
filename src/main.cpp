#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <map>
#include <chrono>
using namespace std;
string read_file_contents(const string& filename);

string doubleToString(double value) {
    ostringstream oss;
    oss << fixed << setprecision(6) << value;
    string result = oss.str();

    if (result.find('.') != string::npos) {
        result.erase(result.find_last_not_of('0') + 1, string::npos);
        if (result.back() == '.') result.pop_back();
    }

    return result;
}

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

struct VariableExpr : Expr {
    string name;
    
    VariableExpr(string n) : name(n) {}
    
    string toString() override {
        return name;
    }
};

struct AssignExpr : Expr {
    string name;
    Expr* value;
    
    AssignExpr(string n, Expr* v) : name(n), value(v) {}
    
    ~AssignExpr() { delete value; }
    
    string toString() override {
        return "(= " + name + " " + value->toString() + ")";
    }
};

struct LogicalExpr : Expr {
    Expr* left;
    string op;
    Expr* right;
    
    LogicalExpr(Expr* l, string o, Expr* r) : left(l), op(o), right(r) {}
    
    ~LogicalExpr() {
        delete left;
        delete right;
    }
    
    string toString() override {
        return "(" + op + " " + left->toString() + " " + right->toString() + ")";
    }
};

struct CallExpr : Expr {
    Expr* callee;
    vector<Expr*> arguments;
    
    CallExpr(Expr* c, vector<Expr*> args) : callee(c), arguments(args) {}
    
    ~CallExpr() {
        delete callee;
        for (auto arg : arguments) {
            delete arg;
        }
    }
    
    string toString() override {
        string result = "(call " + callee->toString();
        for (auto arg : arguments) {
            result += " " + arg->toString();
        }
        result += ")";
        return result;
    }
};

struct Stmt {
    virtual ~Stmt() {}
};

struct ExprStmt : Stmt {
    Expr* expression;
    
    ExprStmt(Expr* e) : expression(e) {}
    
    ~ExprStmt() { delete expression; }
};

struct PrintStmt : Stmt {
    Expr* expression;
    
    PrintStmt(Expr* e) : expression(e) {}
    
    ~PrintStmt() { delete expression; }
};

struct VarStmt : Stmt {
    string name;
    Expr* initializer;
    
    VarStmt(string n, Expr* init) : name(n), initializer(init) {}
    
    ~VarStmt() { if (initializer) delete initializer; }
};

struct BlockStmt : Stmt {
    vector<Stmt*> statements;
    
    BlockStmt(vector<Stmt*> stmts) : statements(stmts) {}
    
    ~BlockStmt() {
        for (auto stmt : statements) {
            delete stmt;
        }
    }
};

struct IfStmt : Stmt {
    Expr* condition;
    Stmt* thenBranch;
    Stmt* elseBranch;
    
    IfStmt(Expr* cond, Stmt* thenBr, Stmt* elseBr) 
        : condition(cond), thenBranch(thenBr), elseBranch(elseBr) {}
    
    ~IfStmt() {
        delete condition;
        delete thenBranch;
        if (elseBranch) delete elseBranch;
    }
};

struct WhileStmt : Stmt {
    Expr* condition;
    Stmt* body;
    
    WhileStmt(Expr* cond, Stmt* b) : condition(cond), body(b) {}
    
    ~WhileStmt() {
        delete condition;
        delete body;
    }
};

struct FunStmt : Stmt {
    string name;
    vector<string> params;
    vector<Stmt*> body;
    
    FunStmt(string n, vector<string> p, vector<Stmt*> b) 
        : name(n), params(p), body(b) {}
    
    ~FunStmt() {
        for (auto stmt : body) {
            delete stmt;
        }
    }
};

struct ReturnStmt : Stmt {
    Expr* value;
    
    ReturnStmt(Expr* v) : value(v) {}
    
    ~ReturnStmt() { if (value) delete value; }
};

struct ClassStmt : Stmt {
    string name;
    vector<FunStmt*> methods;
    
    ClassStmt(string n, vector<FunStmt*> m) : name(n), methods(m) {}
    
    ~ClassStmt() {
        for (auto method : methods) {
            delete method;
        }
    }
};

struct GetExpr : Expr {
    Expr* object;
    string name;
    
    GetExpr(Expr* obj, string n) : object(obj), name(n) {}
    
    ~GetExpr() { delete object; }
    
    string toString() override {
        return "(get " + object->toString() + " " + name + ")";
    }
};

struct SetExpr : Expr {
    Expr* object;
    string name;
    Expr* value;
    
    SetExpr(Expr* obj, string n, Expr* v) : object(obj), name(n), value(v) {}
    
    ~SetExpr() {
        delete object;
        delete value;
    }
    
    string toString() override {
        return "(set " + object->toString() + " " + name + " " + value->toString() + ")";
    }
};

struct ThisExpr : Expr {
    ThisExpr() {}
    
    string toString() override {
        return "this";
    }
};

class Environment {
    private:
    map<string, pair<string, string>> values;
    Environment* enclosing;
    
    public:
    Environment() : enclosing(nullptr) {}
    Environment(Environment* enc) : enclosing(enc) {}
    
    void define(const string& name, const string& value, const string& type) {
        values[name] = {value, type};
    }
    
    pair<string, string> get(const string& name) {
        if (values.find(name) != values.end()) {
            return values[name];
        }
        if (enclosing != nullptr) {
            return enclosing->get(name);
        }
        throw runtime_error("Undefined variable '" + name + "'.");
    }
    
    void assign(const string& name, const string& value, const string& type) {
        if (values.find(name) != values.end()) {
            values[name] = {value, type};
            return;
        }
        if (enclosing != nullptr) {
            enclosing->assign(name, value, type);
            return;
        }
        throw runtime_error("Undefined variable '" + name + "'.");
    }
    
    Environment* ancestor(int distance) {
        Environment* environment = this;
        for (int i = 0; i < distance; i++) {
            if (environment) {
                environment = environment->enclosing;
            }
        }
        return environment;
    }
    
    pair<string, string> getAt(int distance, const string& name) {
        return ancestor(distance)->values[name];
    }
    
    void assignAt(int distance, const string& name, const string& value, const string& type) {
        ancestor(distance)->values[name] = {value, type};
    }
};

class Interpreter;

struct LoxCallable {
    virtual ~LoxCallable() {}
    virtual int arity() = 0;
    virtual pair<string, string> call(Interpreter* interpreter, vector<pair<string, string>>& arguments) = 0;
    virtual string toString() = 0;
};

struct LoxFunction : LoxCallable {
    FunStmt* declaration;
    Environment* closure;
    bool isInitializer;
    
    LoxFunction(FunStmt* decl, Environment* clos, bool init = false) 
        : declaration(decl), closure(clos), isInitializer(init) {}
    
    LoxFunction* bind(const string& instanceId, const string& instanceType, Interpreter* interpreter);
    
    int arity() override {
        return declaration->params.size();
    }
    
    pair<string, string> call(Interpreter* interpreter, vector<pair<string, string>>& arguments) override;
    
    string toString() override {
        return "<fn " + declaration->name + ">";
    }
};

struct ClockNative : LoxCallable {
    int arity() override { return 0; }
    
    pair<string, string> call(Interpreter* interpreter, vector<pair<string, string>>& arguments) override {
        auto now = chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto millis = chrono::duration_cast<chrono::milliseconds>(duration).count();
        double seconds = millis / 1000.0;
        return {doubleToString(seconds), "number"};
    }
    
    string toString() override {
        return "<native fn>";
    }
};

class LoxInstance;

struct LoxClass : LoxCallable {
    string name;
    map<string, LoxFunction*> methods;
    
    LoxClass(string n, map<string, LoxFunction*> m) : name(n), methods(m) {}
    
    int arity() override {
        LoxFunction* initializer = findMethod("init");
        if (initializer != nullptr) {
            return initializer->arity();
        }
        return 0;
    }
    
    LoxFunction* findMethod(const string& name) {
        auto it = methods.find(name);
        if (it != methods.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    pair<string, string> call(Interpreter* interpreter, vector<pair<string, string>>& arguments) override;
    
    string toString() override {
        return name;
    }
};

class LoxInstance {
public:
    LoxClass* klass;
    map<string, pair<string, string>> fields;
    
    LoxInstance(LoxClass* k) : klass(k) {}
    
    pair<string, string> get(const string& name, class Interpreter* interpreter);
    
    void set(const string& name, const string& value, const string& type) {
        fields[name] = {value, type};
    }
    
    string toString() {
        return klass->name + " instance";
    }
};

class Resolver {
private:
    vector<map<string, bool>> scopes;
    bool hadError;
    string errorMsg;
    map<Expr*, int> locals;
    
    enum FunctionType {
        NONE,
        FUNCTION,
        METHOD,
        INITIALIZER
    };
    
    enum ClassType {
        NONE_CLASS,
        CLASS
    };
    
    FunctionType currentFunction;
    ClassType currentClass;
    
    void beginScope() {
        scopes.push_back(map<string, bool>());
    }
    
    void endScope() {
        if (!scopes.empty()) {
            scopes.pop_back();
        }
    }
    
    void declare(const string& name) {
        if (scopes.empty()) return;
        
        auto& scope = scopes.back();
        if (scope.find(name) != scope.end()) {
            hadError = true;
            errorMsg = "Already a variable with this name in this scope.";
            return;
        }
        
        scope[name] = false;
    }
    
    void define(const string& name) {
        if (scopes.empty()) return;
        scopes.back()[name] = true;
    }
    
    void resolveLocal(Expr* expr, const string& name) {
        for (int i = scopes.size() - 1; i >= 0; i--) {
            if (scopes[i].find(name) != scopes[i].end()) {
                locals[expr] = scopes.size() - 1 - i;
                return;
            }
        }
    }
    
    void resolveExpr(Expr* expr) {
        if (auto* lit = dynamic_cast<LiteralExpr*>(expr)) {
            
        }
        else if (auto* var = dynamic_cast<VariableExpr*>(expr)) {
            if (!scopes.empty()) {
                auto& scope = scopes.back();
                if (scope.find(var->name) != scope.end() && scope[var->name] == false) {
                    hadError = true;
                    errorMsg = "Can't read local variable in its own initializer.";
                    return;
                }
            }
            resolveLocal(var, var->name);
        }
        else if (auto* assign = dynamic_cast<AssignExpr*>(expr)) {
            resolveExpr(assign->value);
            resolveLocal(assign, assign->name);
        }
        else if (auto* binary = dynamic_cast<BinaryExpr*>(expr)) {
            resolveExpr(binary->left);
            resolveExpr(binary->right);
        }
        else if (auto* unary = dynamic_cast<UnaryExpr*>(expr)) {
            resolveExpr(unary->right);
        }
        else if (auto* group = dynamic_cast<GroupingExpr*>(expr)) {
            resolveExpr(group->expression);
        }
        else if (auto* logical = dynamic_cast<LogicalExpr*>(expr)) {
            resolveExpr(logical->left);
            resolveExpr(logical->right);
        }
        else if (auto* call = dynamic_cast<CallExpr*>(expr)) {
            resolveExpr(call->callee);
            for (Expr* arg : call->arguments) {
                resolveExpr(arg);
            }
        }
        else if (auto* get = dynamic_cast<GetExpr*>(expr)) {
            resolveExpr(get->object);
        }
        else if (auto* set = dynamic_cast<SetExpr*>(expr)) {
            resolveExpr(set->value);
            resolveExpr(set->object);
        }
        else if (auto* thisExpr = dynamic_cast<ThisExpr*>(expr)) {
            if (currentClass == NONE_CLASS) {
                hadError = true;
                errorMsg = "Can't use 'this' outside of a class.";
                return;
            }
            resolveLocal(thisExpr, "this");
        }
    }
    
    void resolveStmt(Stmt* stmt) {
        if (auto* exprStmt = dynamic_cast<ExprStmt*>(stmt)) {
            resolveExpr(exprStmt->expression);
        }
        else if (auto* printStmt = dynamic_cast<PrintStmt*>(stmt)) {
            resolveExpr(printStmt->expression);
        }
        else if (auto* varStmt = dynamic_cast<VarStmt*>(stmt)) {
            declare(varStmt->name);
            if (varStmt->initializer != nullptr) {
                resolveExpr(varStmt->initializer);
            }
            define(varStmt->name);
        }
        else if (auto* blockStmt = dynamic_cast<BlockStmt*>(stmt)) {
            beginScope();
            for (Stmt* s : blockStmt->statements) {
                resolveStmt(s);
            }
            endScope();
        }
        else if (auto* ifStmt = dynamic_cast<IfStmt*>(stmt)) {
            resolveExpr(ifStmt->condition);
            resolveStmt(ifStmt->thenBranch);
            if (ifStmt->elseBranch != nullptr) {
                resolveStmt(ifStmt->elseBranch);
            }
        }
        else if (auto* whileStmt = dynamic_cast<WhileStmt*>(stmt)) {
            resolveExpr(whileStmt->condition);
            resolveStmt(whileStmt->body);
        }
        else if (auto* funStmt = dynamic_cast<FunStmt*>(stmt)) {
            declare(funStmt->name);
            define(funStmt->name);
            
            FunctionType enclosingFunction = currentFunction;
            currentFunction = FUNCTION;
            
            beginScope();
            for (const string& param : funStmt->params) {
                declare(param);
                define(param);
            }
            for (Stmt* bodyStmt : funStmt->body) {
                resolveStmt(bodyStmt);
            }
            endScope();
            
            currentFunction = enclosingFunction;
        }
        else if (auto* classStmt = dynamic_cast<ClassStmt*>(stmt)) {
            ClassType enclosingClass = currentClass;
            currentClass = CLASS;
            
            declare(classStmt->name);
            define(classStmt->name);
            
            beginScope();
            scopes.back()["this"] = true;
            
            for (FunStmt* method : classStmt->methods) {
                FunctionType declaration = METHOD;
                if (method->name == "init") {
                    declaration = INITIALIZER;
                }
                
                FunctionType enclosingFunction = currentFunction;
                currentFunction = declaration;
                
                beginScope();
                for (const string& param : method->params) {
                    declare(param);
                    define(param);
                }
                for (Stmt* bodyStmt : method->body) {
                    resolveStmt(bodyStmt);
                }
                endScope();
                
                currentFunction = enclosingFunction;
            }
            
            endScope();
            currentClass = enclosingClass;
        }
        else if (auto* returnStmt = dynamic_cast<ReturnStmt*>(stmt)) {
            if (currentFunction == NONE) {
                hadError = true;
                errorMsg = "Can't return from top-level code.";
                return;
            }
            
            if (returnStmt->value != nullptr) {
                if (currentFunction == INITIALIZER) {
                    hadError = true;
                    errorMsg = "Can't return a value from an initializer.";
                    return;
                }
                resolveExpr(returnStmt->value);
            }
        }
    }
    
    public:
    Resolver() : hadError(false), currentFunction(NONE), currentClass(NONE_CLASS) {}
    
    void resolve(const vector<Stmt*>& statements) {
        for (Stmt* stmt : statements) {
            resolveStmt(stmt);
            if (hadError) return;
        }
    }
    
    bool hasError() const { return hadError; }
    string getError() const { return errorMsg; }
    
    int getDepth(Expr* expr) {
        auto it = locals.find(expr);
        if (it != locals.end()) {
            return it->second;
        }
        return -1; 
    }
    
    const map<Expr*, int>& getLocals() const {
        return locals;
    }
};

class Interpreter {
    public:
    bool isReturning;
    pair<string, string> returnValue;
    
    private:
    bool hadRuntimeError;
    string runtimeErrorMsg;
    Environment* environment;
    Environment* globals;
    map<string, LoxCallable*> functions;
    map<string, LoxInstance*> instances;
    int nextFunctionId;
    int nextInstanceId;
    map<Expr*, int> locals;
    
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
    
    Interpreter() : hadRuntimeError(false), runtimeErrorMsg(""), isReturning(false), nextFunctionId(0), nextInstanceId(0) {
        globals = new Environment();
        environment = globals;
        
        // Define native functions
        functions["clock"] = new ClockNative();
        globals->define("clock", "clock", "function");
    }
    
    ~Interpreter() {
        delete globals;
        for (auto& pair : functions) {
            delete pair.second;
        }
    }
    
    void setLocals(const map<Expr*, int>& resolvedLocals) {
        locals = resolvedLocals;
    }
    
    string createInstance(LoxInstance* instance) {
        string instanceId = "__instance_" + to_string(nextInstanceId++);
        instances[instanceId] = instance;
        return instanceId;
    }
    
    LoxInstance* getInstance(const string& instanceId) {
        auto it = instances.find(instanceId);
        if (it != instances.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    string storeFunction(LoxCallable* function) {
        string functionId = "__fn_" + to_string(nextFunctionId++);
        functions[functionId] = function;
        return functionId;
    }
    
    string findInstanceId(LoxInstance* instance) {
        for (const auto& pair : instances) {
            if (pair.second == instance) {
                return pair.first;
            }
        }
        return "";
    }
    
    Value lookupVariable(const string& name, Expr* expr) {
        auto it = locals.find(expr);
        if (it != locals.end()) {
            int distance = it->second;
            auto result = environment->getAt(distance, name);
            return {result.first, result.second};
        } else {
            auto result = globals->get(name);
            return {result.first, result.second};
        }
    }
    
    Value evaluate(Expr* expr) {
        if (auto* lit = dynamic_cast<LiteralExpr*>(expr)) {
            return {lit->value, lit->type};
        }
        else if (auto* var = dynamic_cast<VariableExpr*>(expr)) {
            try {
                return lookupVariable(var->name, var);
            }
             catch (const runtime_error& e) {
                runtimeError(e.what());
                return {"", "nil"};
            }
        }
        else if (auto* assign = dynamic_cast<AssignExpr*>(expr)) {
            Value value = evaluate(assign->value);
            if (hadRuntimeError) return {"", "nil"};
            
            try {
                auto it = locals.find(assign);
                if (it != locals.end()) {
                    int distance = it->second;
                    environment->assignAt(distance, assign->name, value.val, value.type);
                } 
                else {
                    globals->assign(assign->name, value.val, value.type);
                }
                return value;
            }
             catch (const runtime_error& e) {
                runtimeError(e.what());
                return {"", "nil"};
            }
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
                return {doubleToString(-num), "number"};
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
                return {doubleToString(result), "number"};
            }
            else if (binary->op == "+") {
                if (left.type == "number" && right.type == "number") {
                    double result = stod(left.val) + stod(right.val);
                    return {doubleToString(result), "number"};
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
        else if (auto* logical = dynamic_cast<LogicalExpr*>(expr)) {
            Value left = evaluate(logical->left);
            
            if (logical->op == "or") {
                if (isTruthy(left.val, left.type)) return left;
            } else {
                if (!isTruthy(left.val, left.type)) return left;
            }
            
            return evaluate(logical->right);
        }
        else if (auto* call = dynamic_cast<CallExpr*>(expr)) {
            Value callee = evaluate(call->callee);
            
            vector<pair<string, string>> arguments;
            for (Expr* arg : call->arguments) {
                Value argValue = evaluate(arg);
                arguments.push_back({argValue.val, argValue.type});
            }
            
            if (callee.type != "function") {
                runtimeError("Can only call functions and classes.");
                return {"", "nil"};
            }
            
            LoxCallable* function = functions[callee.val];
            if (function == nullptr) {
                runtimeError("Undefined function '" + callee.val + "'.");
                return {"", "nil"};
            }
            
            if (arguments.size() != function->arity()) {
                runtimeError("Expected " + to_string(function->arity()) + 
                           " arguments but got " + to_string(arguments.size()) + ".");
                return {"", "nil"};
            }
            
            auto result = function->call(this, arguments);
            return {result.first, result.second};
        }
        else if (auto* get = dynamic_cast<GetExpr*>(expr)) {
            Value object = evaluate(get->object);
            if (hadRuntimeError) return {"", "nil"};
            
            if (object.type == "instance") {
                LoxInstance* instance = instances[object.val];
                if (instance != nullptr) {
                    auto result = instance->get(get->name, this);
                    return {result.first, result.second};
                }
            }
            
            runtimeError("Only instances have properties.");
            return {"", "nil"};
        }
        else if (auto* set = dynamic_cast<SetExpr*>(expr)) {
            Value object = evaluate(set->object);
            if (hadRuntimeError) return {"", "nil"};
            
            if (object.type != "instance") {
                runtimeError("Only instances have fields.");
                return {"", "nil"};
            }
            
            Value value = evaluate(set->value);
            if (hadRuntimeError) return {"", "nil"};
            
            LoxInstance* instance = instances[object.val];
            if (instance != nullptr) {
                instance->set(set->name, value.val, value.type);
            }
            
            return value;
        }
        else if (auto* thisExpr = dynamic_cast<ThisExpr*>(expr)) {
            return lookupVariable("this", thisExpr);
        }
        
        return {"", "nil"};
    }
    
    void execute(Stmt* stmt) {
        if (auto* exprStmt = dynamic_cast<ExprStmt*>(stmt)) {
            evaluate(exprStmt->expression);
        }
        else if (auto* printStmt = dynamic_cast<PrintStmt*>(stmt)) {
            Value value = evaluate(printStmt->expression);
            if (!hadRuntimeError) {
                if (value.type == "number") {
                    double num = stod(value.val);
                    if (num == floor(num)) {
                        cout << static_cast<long long>(num) << endl;
                    } else {
                        cout << doubleToString(num) << endl;
                    }
                }
                 else if (value.type == "string") {
                    cout << value.val << endl;
                }
                 else if (value.type == "function") {
                    LoxCallable* func = functions[value.val];
                    if (func != nullptr) {
                        cout << func->toString() << endl;
                    } else {
                        cout << value.val << endl;
                    }
                }
                 else if (value.type == "instance") {
                    LoxInstance* inst = instances[value.val];
                    if (inst != nullptr) {
                        cout << inst->toString() << endl;
                    } else {
                        cout << value.val << endl;
                    }
                }
                 else {
                    cout << value.val << endl;
                }
            }
        }
        else if (auto* varStmt = dynamic_cast<VarStmt*>(stmt)) {
            Value value = {"nil", "nil"};
            if (varStmt->initializer != nullptr) {
                value = evaluate(varStmt->initializer);
            }
            if (!hadRuntimeError) {
                environment->define(varStmt->name, value.val, value.type);
            }
        }
        else if (auto* blockStmt = dynamic_cast<BlockStmt*>(stmt)) {
            executeBlock(blockStmt->statements, new Environment(environment));
        }
        else if (auto* ifStmt = dynamic_cast<IfStmt*>(stmt)) {
            Value condition = evaluate(ifStmt->condition);
            if (hadRuntimeError) return;
            
            if (isTruthy(condition.val, condition.type)) {
                execute(ifStmt->thenBranch);
            } else if (ifStmt->elseBranch != nullptr) {
                execute(ifStmt->elseBranch);
            }
        }
        else if (auto* whileStmt = dynamic_cast<WhileStmt*>(stmt)) {
            while (true) {
                Value condition = evaluate(whileStmt->condition);
                if (hadRuntimeError) return;
                
                if (!isTruthy(condition.val, condition.type)) break;
                
                execute(whileStmt->body);
                if (hadRuntimeError || isReturning) return;
            }
        }
        else if (auto* funStmt = dynamic_cast<FunStmt*>(stmt)) {
            LoxFunction* function = new LoxFunction(funStmt, environment);
            string functionId = "__fn_" + to_string(nextFunctionId++);
            functions[functionId] = function;
            environment->define(funStmt->name, functionId, "function");
        }
        else if (auto* classStmt = dynamic_cast<ClassStmt*>(stmt)) {
            map<string, LoxFunction*> methods;
            for (FunStmt* method : classStmt->methods) {
                bool isInit = (method->name == "init");
                LoxFunction* function = new LoxFunction(method, environment, isInit);
                methods[method->name] = function;
            }
            
            LoxClass* klass = new LoxClass(classStmt->name, methods);
            string classId = "__class_" + to_string(nextFunctionId++);
            functions[classId] = klass;
            environment->define(classStmt->name, classId, "function");
        }
        else if (auto* returnStmt = dynamic_cast<ReturnStmt*>(stmt)) {
            if (returnStmt->value != nullptr) {
                Value value = evaluate(returnStmt->value);
                returnValue = {value.val, value.type};
            } 
            else {
                returnValue = {"nil", "nil"};
            }
            isReturning = true;
        }
    }
    
    void executeBlock(const vector<Stmt*>& statements, Environment* newEnv) {
        Environment* previous = environment;
        environment = newEnv;
        
        for (Stmt* stmt : statements) {
            execute(stmt);
            if (hadRuntimeError || isReturning) break;
        }
        
        environment = previous;
    }
    
    void interpret(const vector<Stmt*>& statements) {
        for (Stmt* stmt : statements) {
            execute(stmt);
            if (hadRuntimeError) break;
        }
    }
    
    bool hasError() { return hadRuntimeError; }
    string getError() { return runtimeErrorMsg; }
};

pair<string, string> LoxFunction::call(Interpreter* interpreter, vector<pair<string, string>>& arguments) {
    Environment* environment = new Environment(closure);
    
    for (size_t i = 0; i < declaration->params.size(); i++) {
        environment->define(declaration->params[i], arguments[i].first, arguments[i].second);
    }
    
    interpreter->isReturning = false;
    interpreter->executeBlock(declaration->body, environment);
    
    if (interpreter->isReturning) {
        interpreter->isReturning = false;
        if (isInitializer) {
            return closure->getAt(0, "this");
        }
        return interpreter->returnValue;
    }
    
    if (isInitializer) {
        return closure->getAt(0, "this");
    }
    
    return {"nil", "nil"};
}

LoxFunction* LoxFunction::bind(const string& instanceId, const string& instanceType, Interpreter* interpreter) {
    Environment* environment = new Environment(closure);
    environment->define("this", instanceId, instanceType);
    return new LoxFunction(declaration, environment, isInitializer);
}

pair<string, string> LoxClass::call(Interpreter* interpreter, vector<pair<string, string>>& arguments) {
    LoxInstance* instance = new LoxInstance(this);
    string instanceId = interpreter->createInstance(instance);
    
    LoxFunction* initializer = findMethod("init");
    if (initializer != nullptr) {
        LoxFunction* bound = initializer->bind(instanceId, "instance", interpreter);
        bound->call(interpreter, arguments);
    }
    
    return {instanceId, "instance"};
}

pair<string, string> LoxInstance::get(const string& name, Interpreter* interpreter) {
    auto it = fields.find(name);
    if (it != fields.end()) {
        return it->second;
    }
    
    LoxFunction* method = klass->findMethod(name);
    if (method != nullptr) {
        string instanceId = interpreter->findInstanceId(this);      
        LoxFunction* bound = method->bind(instanceId, "instance", interpreter);
        string functionId = interpreter->storeFunction(bound);
        
        return {functionId, "function"};
    }
    
    throw runtime_error("Undefined property '" + name + "'.");
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
        
        if (match({"THIS"})) {
            return new ThisExpr();
        }
        
        if (match({"IDENTIFIER"})) {
            return new VariableExpr(previous().lexeme);
        }
        
        if (match({"LEFT_PAREN"})) {
            Expr* expr = expressionExpr();
            consume("RIGHT_PAREN", "Expect ')' after expression.");
            return new GroupingExpr(expr);
        }
        
        error("Expect expression.");
        advance();  
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
    
    Expr* callExpr() {
        Expr* expr = primaryExpr();
        
        while (true) {
            if (match({"LEFT_PAREN"})) {
                vector<Expr*> arguments;
                if (!check("RIGHT_PAREN")) {
                    do {
                        if (arguments.size() >= 255) {
                            error("Can't have more than 255 arguments.");
                            break;
                        }
                        arguments.push_back(expressionExpr());
                    } while (match({"COMMA"}));
                }
                consume("RIGHT_PAREN", "Expect ')' after arguments.");
                expr = new CallExpr(expr, arguments);
            }
            else if (match({"DOT"})) {
                tok name = consume("IDENTIFIER", "Expect property name after '.'.");
                expr = new GetExpr(expr, name.lexeme);
            }
            else {
                break;
            }
        }
        
        return expr;
    }
    
    Expr* unaryExpr() {
        if (match({"BANG", "MINUS"})) {
            string op = previous().lexeme;
            Expr* right = unaryExpr();
            return new UnaryExpr(op, right);
        }
        
        return callExpr();
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
    
    Expr* orExpr() {
        Expr* expr = andExpr();
        
        while (match({"OR"})) {
            string op = previous().lexeme;
            Expr* right = andExpr();
            expr = new LogicalExpr(expr, op, right);
        }
        
        return expr;
    }
    
    Expr* andExpr() {
        Expr* expr = equalityExpr();
        
        while (match({"AND"})) {
            string op = previous().lexeme;
            Expr* right = equalityExpr();
            expr = new LogicalExpr(expr, op, right);
        }
        
        return expr;
    }
    
    Expr* assignmentExpr() {
        Expr* expr = orExpr();
        
        if (match({"EQUAL"})) {
            tok equals = previous();
            Expr* value = assignmentExpr();
            
            if (auto* var = dynamic_cast<VariableExpr*>(expr)) {
                return new AssignExpr(var->name, value);
            }
            else if (auto* get = dynamic_cast<GetExpr*>(expr)) {
                return new SetExpr(get->object, get->name, value);
            }
            
            delete expr;
            error("Invalid assignment target.");
        }
        
        return expr;
    }
    
    string expression() {
        return equality();
    }
    
    Expr* expressionExpr() {
        return assignmentExpr();
    }
    
    Stmt* statement() {
        if (match({"FOR"})) return forStatement();
        if (match({"IF"})) return ifStatement();
        if (match({"PRINT"})) return printStatement();
        if (match({"RETURN"})) return returnStatement();
        if (match({"WHILE"})) return whileStatement();
        if (match({"LEFT_BRACE"})) return new BlockStmt(block());
        return expressionStatement();
    }
    
    Stmt* returnStatement() {
        Expr* value = nullptr;
        if (!check("SEMICOLON")) {
            value = expressionExpr();
        }
        consume("SEMICOLON", "Expect ';' after return value.");
        return new ReturnStmt(value);
    }
    
    Stmt* printStatement() {
        Expr* value = expressionExpr();
        consume("SEMICOLON", "Expect ';' after value.");
        return new PrintStmt(value);
    }
    
    Stmt* ifStatement() {
        consume("LEFT_PAREN", "Expect '(' after 'if'.");
        Expr* condition = expressionExpr();
        consume("RIGHT_PAREN", "Expect ')' after if condition.");
        
        Stmt* thenBranch = statement();
        Stmt* elseBranch = nullptr;
        if (match({"ELSE"})) {
            elseBranch = statement();
        }
        
        return new IfStmt(condition, thenBranch, elseBranch);
    }
    
    Stmt* whileStatement() {
        consume("LEFT_PAREN", "Expect '(' after 'while'.");
        Expr* condition = expressionExpr();
        consume("RIGHT_PAREN", "Expect ')' after condition.");
        Stmt* body = statement();
        
        return new WhileStmt(condition, body);
    }
    
    Stmt* forStatement() {
        consume("LEFT_PAREN", "Expect '(' after 'for'.");
        
        Stmt* initializer;
        if (match({"SEMICOLON"})) {
            initializer = nullptr;
        } 
        else if (match({"VAR"})) {
            initializer = varDeclaration();
        } 
        else {
            initializer = expressionStatement();
        }
        
        Expr* condition = nullptr;
        if (!check("SEMICOLON")) {
            condition = expressionExpr();
        }
        consume("SEMICOLON", "Expect ';' after loop condition.");
        
        Expr* increment = nullptr;
        if (!check("RIGHT_PAREN")) {
            increment = expressionExpr();
        }
        consume("RIGHT_PAREN", "Expect ')' after for clauses.");
        
        Stmt* body = statement();
        
        if (increment != nullptr) {
            vector<Stmt*> bodyStatements;
            bodyStatements.push_back(body);
            bodyStatements.push_back(new ExprStmt(increment));
            body = new BlockStmt(bodyStatements);
        }
        
        if (condition == nullptr) {
            condition = new LiteralExpr("true", "boolean");
        }
        body = new WhileStmt(condition, body);
        
        if (initializer != nullptr) {
            vector<Stmt*> statements;
            statements.push_back(initializer);
            statements.push_back(body);
            body = new BlockStmt(statements);
        }
        
        return body;
    }
    
    Stmt* expressionStatement() {
        Expr* expr = expressionExpr();
        consume("SEMICOLON", "Expect ';' after expression.");
        return new ExprStmt(expr);
    }
    
    Stmt* varDeclaration() {
        tok name = consume("IDENTIFIER", "Expect variable name.");
        
        Expr* initializer = nullptr;
        if (match({"EQUAL"})) {
            initializer = expressionExpr();
        }
        
        consume("SEMICOLON", "Expect ';' after variable declaration.");
        return new VarStmt(name.lexeme, initializer);
    }
    
    vector<Stmt*> block() {
        vector<Stmt*> statements;
        
        while (!check("RIGHT_BRACE") && !isAtEnd()) {
            statements.push_back(declaration());
        }
        
        consume("RIGHT_BRACE", "Expect '}' after block.");
        return statements;
    }
    
    Stmt* declaration() {
        try {
            if (match({"CLASS"})) return classDeclaration();
            if (match({"FUN"})) return function("function");
            if (match({"VAR"})) return varDeclaration();
            return statement();
        }
         catch (...) {
            synchronize();
            return nullptr;
        }
    }
    
    Stmt* classDeclaration() {
        tok name = consume("IDENTIFIER", "Expect class name.");
        consume("LEFT_BRACE", "Expect '{' before class body.");
        
        vector<FunStmt*> methods;
        while (!check("RIGHT_BRACE") && !isAtEnd()) {
            methods.push_back((FunStmt*)function("method"));
        }
        
        consume("RIGHT_BRACE", "Expect '}' after class body.");
        return new ClassStmt(name.lexeme, methods);
    }
    
    Stmt* function(const string& kind) {
        tok name = consume("IDENTIFIER", "Expect " + kind + " name.");
        consume("LEFT_PAREN", "Expect '(' after " + kind + " name.");
        
        vector<string> parameters;
        if (!check("RIGHT_PAREN")) {
            do {
                if (parameters.size() >= 255) {
                    error("Can't have more than 255 parameters.");
                }
                parameters.push_back(consume("IDENTIFIER", "Expect parameter name.").lexeme);
            } while (match({"COMMA"}));
        }
        consume("RIGHT_PAREN", "Expect ')' after parameters.");
        
        consume("LEFT_BRACE", "Expect '{' before " + kind + " body.");
        vector<Stmt*> body = block();
        
        return new FunStmt(name.lexeme, parameters, body);
    }
    
    public:
    Parser(vector<tok>& tokens) : tokens(tokens), curr(0), hadError(false), errorMsg("") {}
    
    string parse() {
        try {
            return expression();
        }
         catch (...) {
            return "";
        }
    }
    
    Expr* parseExpr() {
        try {
            return expressionExpr();
        }
         catch (...) {
            return nullptr;
        }
    }
    
    vector<Stmt*> parseProgram() {
        vector<Stmt*> statements;
        while (!isAtEnd()) {
            Stmt* stmt = declaration();
            if (stmt != nullptr) {
                statements.push_back(stmt);
            }
        }
        return statements;
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
                        cout << doubleToString(num) << endl;
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
    else if(command == "run") {
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
        vector<Stmt*> statements = parser.parseProgram();
        
        if (parser.hasError()) {
            cerr << parser.getError() << endl;
            exitCode = 65;
            
            for (Stmt* stmt : statements) {
                delete stmt;
            }
        } 
        else {
            Resolver resolver;
            resolver.resolve(statements);
            
            if (resolver.hasError()) {
                cerr << resolver.getError() << endl;
                exitCode = 65;
            }
            else {
                Interpreter interpreter;
                interpreter.setLocals(resolver.getLocals());
                try {
                    interpreter.interpret(statements);
                    
                    if (interpreter.hasError()) {
                        cerr << interpreter.getError() << endl;
                        exitCode = 70;
                    }
                } catch (const runtime_error& e) {
                    cerr << e.what() << endl;
                    exitCode = 70;
                }
            }
            
            for (Stmt* stmt : statements) {
                delete stmt;
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
