// ============================================================
// tokenizer.cpp
// Implementation of Phase 2 : Lexical Analysis
// ============================================================
#include "tokenizer.h"
#include "utils.h"
#include <unordered_set>
#include <cctype>
#include <iomanip>
#include <sstream>

// ------------------------------------------------------------
// Reserved keyword set (standard C keywords used in this lab)
// ------------------------------------------------------------
static const std::unordered_set<std::string> KEYWORDS = {
    "auto", "break", "case", "char", "const", "continue", "default",
    "do", "double", "else", "enum", "extern", "float", "for", "goto",
    "if", "int", "long", "register", "return", "short", "signed",
    "sizeof", "static", "struct", "switch", "typedef", "union",
    "unsigned", "void", "volatile", "while"
};

// Multi-character operators, longest first so the scanner can try
// them before falling back to single-character operators.
static const std::vector<std::string> MULTI_CHAR_OPERATORS = {
    "<<=", ">>=",
    "==", "!=", "<=", ">=", "&&", "||", "++", "--",
    "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=",
    "<<", ">>", "->"
};

static const std::string SINGLE_CHAR_OPERATORS = "+-*/%=<>!&|^~";
static const std::string DELIMITERS = "(){}[];,.:";

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD:        return "KEYWORD";
        case TokenType::IDENTIFIER:     return "IDENTIFIER";
        case TokenType::INT_CONST:      return "INT_CONST";
        case TokenType::FLOAT_CONST:    return "FLOAT_CONST";
        case TokenType::CHAR_CONST:     return "CHAR_CONST";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::OPERATOR:       return "OPERATOR";
        case TokenType::DELIMITER:      return "DELIMITER";
        default:                        return "UNKNOWN";
    }
}

namespace Tokenizer {

bool isKeyword(const std::string &word) {
    return KEYWORDS.find(word) != KEYWORDS.end();
}

std::vector<Token> tokenize(const std::string &source) {
    std::vector<Token> tokens;
    size_t n = source.size();
    size_t i = 0;
    int line = 1;

    while (i < n) {
        char c = source[i];

        // ---- whitespace & line tracking ----
        if (c == '\n') { line++; i++; continue; }
        if (isspace((unsigned char)c)) { i++; continue; }

        // ---- preprocessor directives (#include, #define, ...) ----
        // Treated as a single DELIMITER-ish token so they don't confuse
        // the rest of the pipeline; the whole line is captured.
        if (c == '#') {
            size_t start = i;
            while (i < n && source[i] != '\n') i++;
            tokens.push_back(Token(TokenType::DELIMITER, source.substr(start, i - start), line));
            continue;
        }

        // ---- string literal ----
        if (c == '"') {
            size_t start = i;
            i++; // consume opening quote
            while (i < n && source[i] != '"') {
                if (source[i] == '\\' && i + 1 < n) i++; // skip escaped char
                if (source[i] == '\n') line++;
                i++;
            }
            i++; // consume closing quote
            tokens.push_back(Token(TokenType::STRING_LITERAL, source.substr(start, i - start), line));
            continue;
        }

        // ---- character literal ----
        if (c == '\'') {
            size_t start = i;
            i++; // consume opening quote
            while (i < n && source[i] != '\'') {
                if (source[i] == '\\' && i + 1 < n) i++; // skip escaped char
                i++;
            }
            i++; // consume closing quote
            tokens.push_back(Token(TokenType::CHAR_CONST, source.substr(start, i - start), line));
            continue;
        }

        // ---- number: integer or floating constant ----
        if (isdigit((unsigned char)c)) {
            size_t start = i;
            bool isFloat = false;
            while (i < n && isdigit((unsigned char)source[i])) i++;
            if (i < n && source[i] == '.') {
                isFloat = true;
                i++;
                while (i < n && isdigit((unsigned char)source[i])) i++;
            }
            // optional exponent part, e.g. 1e10, 2.5E-3
            if (i < n && (source[i] == 'e' || source[i] == 'E')) {
                size_t save = i;
                size_t j = i + 1;
                if (j < n && (source[j] == '+' || source[j] == '-')) j++;
                if (j < n && isdigit((unsigned char)source[j])) {
                    isFloat = true;
                    i = j;
                    while (i < n && isdigit((unsigned char)source[i])) i++;
                } else {
                    i = save; // not a valid exponent, leave as is
                }
            }
            // optional suffix like f, L, u, U (kept as part of lexeme)
            while (i < n && (source[i] == 'f' || source[i] == 'F' ||
                              source[i] == 'l' || source[i] == 'L' ||
                              source[i] == 'u' || source[i] == 'U')) {
                i++;
            }
            std::string lex = source.substr(start, i - start);
            tokens.push_back(Token(isFloat ? TokenType::FLOAT_CONST : TokenType::INT_CONST, lex, line));
            continue;
        }

        // ---- identifier or keyword ----
        if (isalpha((unsigned char)c) || c == '_') {
            size_t start = i;
            while (i < n && (isalnum((unsigned char)source[i]) || source[i] == '_')) i++;
            std::string lex = source.substr(start, i - start);
            TokenType type = isKeyword(lex) ? TokenType::KEYWORD : TokenType::IDENTIFIER;
            tokens.push_back(Token(type, lex, line));
            continue;
        }

        // ---- multi-character operators ----
        bool matchedMulti = false;
        for (const auto &op : MULTI_CHAR_OPERATORS) {
            size_t len = op.size();
            if (i + len <= n && source.compare(i, len, op) == 0) {
                tokens.push_back(Token(TokenType::OPERATOR, op, line));
                i += len;
                matchedMulti = true;
                break;
            }
        }
        if (matchedMulti) continue;

        // ---- single-character operator ----
        if (SINGLE_CHAR_OPERATORS.find(c) != std::string::npos) {
            tokens.push_back(Token(TokenType::OPERATOR, std::string(1, c), line));
            i++;
            continue;
        }

        // ---- delimiter ----
        if (DELIMITERS.find(c) != std::string::npos) {
            tokens.push_back(Token(TokenType::DELIMITER, std::string(1, c), line));
            i++;
            continue;
        }

        // ---- anything else is unrecognized ----
        tokens.push_back(Token(TokenType::UNKNOWN, std::string(1, c), line));
        i++;
    }

    return tokens;
}

// Long lexemes (e.g. string literals) are clipped with "..." so the
// table columns stay aligned; the full lexeme is never lost from the
// source, only from this particular printed table.
static std::string clip(const std::string &s, size_t maxLen) {
    if (s.size() <= maxLen) return s;
    return s.substr(0, maxLen - 3) + "...";
}

void printTokenTable(const std::vector<Token> &tokens) {
    const int LEXEME_WIDTH = 28;

    std::ostringstream header;
    header << std::left << std::setw(6)  << "No."
           << std::setw(LEXEME_WIDTH) << "Lexeme"
           << std::setw(18) << "Token Type"
           << std::setw(6)  << "Line";
    Logger::println(header.str());
    Logger::divider();

    int index = 1;
    for (const auto &tok : tokens) {
        std::ostringstream row;
        row << std::left << std::setw(6) << index++
            << std::setw(LEXEME_WIDTH) << clip(tok.lexeme, LEXEME_WIDTH - 2)
            << std::setw(18) << tokenTypeToString(tok.type)
            << std::setw(6) << tok.line;
        Logger::println(row.str());
    }
    Logger::println();
    Logger::println("Total tokens recognized: " + std::to_string(tokens.size()));
}

} // namespace Tokenizer
