# Loxpreter


A feature-complete tree-walk interpreter for the Lox programming language, implemented in C++ following Robert Nystrom's [Crafting Interpreters](https://craftinginterpreters.com/). This interpreter demonstrates fundamental compiler and interpreter concepts including lexical analysis, parsing, semantic analysis, and runtime evaluation.

## About Lox

Lox is a dynamically-typed scripting language designed for educational purposes. It features:
- C-style syntax with familiar control flow structures
- First-class functions with closures
- Object-oriented programming with classes and inheritance
- Dynamic typing with automatic memory management

## Features

### Language Support
- **Data Types**: Numbers (double-precision floating point), strings, booleans, and nil
- **Operators**: Arithmetic (`+`, `-`, `*`, `/`), comparison (`<`, `<=`, `>`, `>=`, `==`, `!=`), logical (`and`, `or`, `!`)
- **Variables**: Dynamic variable declaration and assignment with lexical scoping
- **Control Flow**: `if`/`else` statements, `while` loops, and `for` loops
- **Functions**: First-class functions with closures, recursion support, and `return` statements
- **Classes**: Object-oriented programming with class declarations, methods, constructors (`init`), and field access
- **Inheritance**: Single inheritance with `super` keyword for accessing parent class methods
- **Built-in Functions**: Native `clock()` function for timing operations

### Interpreter Architecture
- **Lexical Scanner**: Hand-written tokenizer that recognizes all Lox language tokens, handles comments, and detects lexical errors
- **Recursive Descent Parser**: Builds an Abstract Syntax Tree (AST) from tokens, implementing Lox's complete grammar with proper precedence
- **Semantic Analyzer**: Static resolver that performs variable resolution and binding before interpretation, catching semantic errors early
- **Tree-Walk Interpreter**: Evaluates the AST with support for nested scopes, closures, and object-oriented features
- **Environment Management**: Implements lexical scoping with environment chains for variable and function lookups

### Error Handling
- Comprehensive error detection and reporting for:
  - Lexical errors (unterminated strings, unexpected characters)
  - Syntax errors (malformed expressions, missing tokens)
  - Runtime errors (type mismatches, undefined variables, invalid operations)
  - Semantic errors (variable shadowing issues, invalid return statements)

## Implementation Details

The interpreter is structured in multiple phases:

1. **Tokenization**: The source code is scanned character-by-character to produce a stream of tokens, handling both single-character (`(`, `)`, `{`, `}`, `;`) and multi-character tokens (`==`, `!=`, `<=`, `>=`)

2. **Parsing**: A recursive descent parser builds an AST using expression and statement node types, implementing operator precedence from lowest (assignment, logical) to highest (unary, primary)

3. **Resolution**: A static analysis pass walks the AST to resolve variable bindings and detect semantic errors before runtime, storing scope depth information for each variable reference

4. **Interpretation**: The AST is evaluated using the Visitor pattern, maintaining runtime environments for variable storage and supporting dynamic dispatch for method calls

The codebase uses C++ structs for AST nodes (expressions and statements), a custom `Environment` class for scope management, and polymorphic callable types (`LoxCallable`, `LoxFunction`, `LoxClass`) for function and class support.

## Getting Started

### Prerequisites
- CMake (build system)
- C++ compiler with C++11 support or later
- vcpkg (for dependency management, optional)

### Building the Project

1. Clone the repository:
```sh
git clone <repository-url>
cd loxpreter
```

2. Build using the provided script:
```sh
./your_program.sh tokenize <lox-file>    # Tokenize and display tokens
./your_program.sh parse <lox-file>       # Parse and display AST
./your_program.sh evaluate <lox-file>    # Evaluate expressions
./your_program.sh run <lox-file>         # Execute Lox program
```

### Usage Examples

**Variables and Arithmetic:**
```lox
var a = 10;
var b = 20;
print a + b;  // 30
```

**Functions and Closures:**
```lox
fun makeCounter() {
    var i = 0;
    fun count() {
        i = i + 1;
        print i;
    }
    return count;
}

var counter = makeCounter();
counter();  // 1
counter();  // 2
```

**Classes and Inheritance:**
```lox
class Animal {
    init(name) {
        this.name = name;
    }
    
    speak() {
        print this.name + " makes a sound.";
    }
}

class Dog < Animal {
    speak() {
        print this.name + " barks.";
    }
}

var dog = Dog("Rex");
dog.speak();  // "Rex barks."
```

## Project Structure

```
loxpreter/
├── src/
│   └── main.cpp           # Complete interpreter implementation
├── CMakeLists.txt         # CMake build configuration
├── vcpkg.json            # Package dependencies
└── your_program.sh       # Build and run script
```

## Development

This project was developed as part of the CodeCrafters "Build your own Interpreter" challenge, implementing chapters 4-13 of *Crafting Interpreters*. The implementation follows the tree-walk interpreter approach (jlox) rather than the bytecode VM approach (clox).

## References

- [Crafting Interpreters](https://craftinginterpreters.com/) by Robert Nystrom
- [Lox Language Specification](https://craftinginterpreters.com/the-lox-language.html)
- [CodeCrafters Interpreter Challenge](https://app.codecrafters.io/courses/interpreter/overview)
