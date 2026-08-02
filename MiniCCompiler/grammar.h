// ============================================================
// grammar.h
// Phase 3: Grammar Processing
// Defines the data structures used to represent a context-free
// grammar (CFG) read from grammar.txt, plus loading/printing.
// ============================================================
#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <string>
#include <vector>
#include <set>

// The special symbol used to represent an epsilon (empty) production.
const std::string EPSILON_SYM = "EPSILON";
// The special end-of-input marker used in FOLLOW sets / parsing table.
const std::string END_SYM = "$";

// A single grammar production:  lhs -> rhs[0] rhs[1] ... rhs[k]
struct Production {
    std::string lhs;
    std::vector<std::string> rhs;

    Production() {}
    Production(const std::string &l, const std::vector<std::string> &r) : lhs(l), rhs(r) {}

    // Human readable form, e.g. "EXPR -> EXPR + TERM"
    std::string toString() const;
};

// Represents an entire context-free grammar: an ordered list of
// productions plus derived sets of non-terminals / terminals.
class Grammar {
public:
    std::vector<Production> productions;   // all productions, in file order
    std::vector<std::string> nonTerminals;  // in first-seen order
    std::vector<std::string> terminals;     // in first-seen order
    std::string startSymbol;

    // Loads a grammar description from a text file. Each non-blank,
    // non '#' line must have the form:  LHS -> SYM1 SYM2 ... SYMn
    // Multiple lines may share the same LHS (grammar alternatives).
    void loadFromFile(const std::string &filename);

    // Returns every production whose LHS equals `nt`.
    std::vector<Production> productionsFor(const std::string &nt) const;

    // True if `symbol` is a non-terminal of this grammar.
    bool isNonTerminal(const std::string &symbol) const;

    // Prints the grammar productions to the Logger.
    void display(const std::string &title = "Grammar") const;

private:
    void classifySymbols();
};

#endif // GRAMMAR_H
