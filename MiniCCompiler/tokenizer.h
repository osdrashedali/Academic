// ============================================================
// tokenizer.h
// Phase 2: Lexical Analysis
// Defines the Token structure and the tokenizer that converts
// cleaned C source code into a stream of classified tokens.
// ============================================================
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>

// Categories of tokens the lexer can recognize.
enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    INT_CONST,
    FLOAT_CONST,
    CHAR_CONST,
    STRING_LITERAL,
    OPERATOR,
    DELIMITER,
    UNKNOWN
};

// Converts a TokenType to a human readable string for printing.
std::string tokenTypeToString(TokenType type);

// A single lexical token.
struct Token {
    TokenType type;
    std::string lexeme;
    int line;

    Token(TokenType t, const std::string &lex, int ln)
        : type(t), lexeme(lex), line(ln) {}
};

namespace Tokenizer {

    // Scans `source` (already comment-free) and returns the full
    // list of recognized tokens in order of appearance.
    std::vector<Token> tokenize(const std::string &source);

    // Pretty-prints the token table (index, lexeme, type, line).
    void printTokenTable(const std::vector<Token> &tokens);

    // Returns true if `word` is one of the reserved C keywords.
    bool isKeyword(const std::string &word);
}

#endif // TOKENIZER_H
