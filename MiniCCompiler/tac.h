// ============================================================
// tac.h
// Phase 9: Three Address Code (TAC) Generation
// Parses a single arithmetic assignment statement such as
//      a = ( b + c ) * ( d - e ) / 2
// into an expression tree, then linearizes that tree into a
// sequence of three-address instructions using fresh temporary
// variables (t1, t2, t3, ...).
// ============================================================
#ifndef TAC_H
#define TAC_H

#include <string>
#include <vector>
#include <memory>

// One three-address instruction:  result = arg1 op arg2
// For a plain copy (e.g. "x = y"), op is "=" and arg2 is empty.
struct TACInstr {
    std::string result;
    std::string arg1;
    std::string op;    // "+", "-", "*", "/", or "=" for a plain copy
    std::string arg2;  // empty when op == "="

    std::string toString() const;
};

// A minimal expression AST node.
struct ASTNode {
    enum class Kind { NUMBER, IDENTIFIER, BINOP } kind;
    std::string value;                 // number literal, identifier name, or operator
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;
};

namespace TAC {

    // Parses "target = expression" into an AST for the RHS expression,
    // returning the AST root and (via out-parameter) the assigned variable.
    std::shared_ptr<ASTNode> parseAssignment(const std::string &statement, std::string &targetVar);

    // Walks the AST and emits three-address code, ending with an
    // instruction that assigns the final temporary to `targetVar`.
    std::vector<TACInstr> generate(const std::shared_ptr<ASTNode> &root, const std::string &targetVar);

    // Prints a list of TAC instructions to the Logger.
    void printTAC(const std::vector<TACInstr> &code, const std::string &title);
}

#endif // TAC_H
