// ============================================================
// tac.cpp
// Implementation of Phase 9 : Three Address Code Generation
// ============================================================
#include "tac.h"
#include "utils.h"
#include <cctype>
#include <stdexcept>

std::string TACInstr::toString() const {
    if (op == "=") {
        return result + " = " + arg1;
    }
    return result + " = " + arg1 + " " + op + " " + arg2;
}

// ------------------------------------------------------------
// A tiny internal tokenizer + recursive-descent parser for a
// single arithmetic expression (used only by this phase; it is
// independent from the main lexer used in Phase 2).
//
//   expr   := term (('+' | '-') term)*
//   term   := factor (('*' | '/') factor)*
//   factor := NUMBER | IDENTIFIER | '(' expr ')'
// ------------------------------------------------------------
namespace {

struct ExprToken {
    enum class Kind { NUMBER, IDENTIFIER, PLUS, MINUS, STAR, SLASH, LPAREN, RPAREN, END } kind;
    std::string text;
};

std::vector<ExprToken> tokenizeExpr(const std::string &s) {
    std::vector<ExprToken> tokens;
    size_t i = 0, n = s.size();
    while (i < n) {
        char c = s[i];
        if (isspace((unsigned char)c)) { i++; continue; }
        if (isdigit((unsigned char)c) || c == '.') {
            size_t start = i;
            while (i < n && (isdigit((unsigned char)s[i]) || s[i] == '.')) i++;
            tokens.push_back({ExprToken::Kind::NUMBER, s.substr(start, i - start)});
            continue;
        }
        if (isalpha((unsigned char)c) || c == '_') {
            size_t start = i;
            while (i < n && (isalnum((unsigned char)s[i]) || s[i] == '_')) i++;
            tokens.push_back({ExprToken::Kind::IDENTIFIER, s.substr(start, i - start)});
            continue;
        }
        switch (c) {
            case '+': tokens.push_back({ExprToken::Kind::PLUS, "+"}); break;
            case '-': tokens.push_back({ExprToken::Kind::MINUS, "-"}); break;
            case '*': tokens.push_back({ExprToken::Kind::STAR, "*"}); break;
            case '/': tokens.push_back({ExprToken::Kind::SLASH, "/"}); break;
            case '(': tokens.push_back({ExprToken::Kind::LPAREN, "("}); break;
            case ')': tokens.push_back({ExprToken::Kind::RPAREN, ")"}); break;
            default:
                throw std::runtime_error(std::string("TAC: unexpected character '") + c + "' in expression");
        }
        i++;
    }
    tokens.push_back({ExprToken::Kind::END, ""});
    return tokens;
}

class ExprParser {
public:
    explicit ExprParser(const std::vector<ExprToken> &toks) : tokens(toks), pos(0) {}

    std::shared_ptr<ASTNode> parseExpr() {
        auto node = parseTerm();
        while (tokens[pos].kind == ExprToken::Kind::PLUS || tokens[pos].kind == ExprToken::Kind::MINUS) {
            std::string op = tokens[pos].text;
            pos++;
            auto right = parseTerm();
            node = makeBinOp(op, node, right);
        }
        return node;
    }

private:
    const std::vector<ExprToken> &tokens;
    size_t pos;

    std::shared_ptr<ASTNode> makeBinOp(const std::string &op, std::shared_ptr<ASTNode> l, std::shared_ptr<ASTNode> r) {
        auto node = std::make_shared<ASTNode>();
        node->kind = ASTNode::Kind::BINOP;
        node->value = op;
        node->left = l;
        node->right = r;
        return node;
    }

    std::shared_ptr<ASTNode> parseTerm() {
        auto node = parseFactor();
        while (tokens[pos].kind == ExprToken::Kind::STAR || tokens[pos].kind == ExprToken::Kind::SLASH) {
            std::string op = tokens[pos].text;
            pos++;
            auto right = parseFactor();
            node = makeBinOp(op, node, right);
        }
        return node;
    }

    std::shared_ptr<ASTNode> parseFactor() {
        const ExprToken &t = tokens[pos];
        if (t.kind == ExprToken::Kind::NUMBER) {
            pos++;
            auto node = std::make_shared<ASTNode>();
            node->kind = ASTNode::Kind::NUMBER;
            node->value = t.text;
            return node;
        }
        if (t.kind == ExprToken::Kind::IDENTIFIER) {
            pos++;
            auto node = std::make_shared<ASTNode>();
            node->kind = ASTNode::Kind::IDENTIFIER;
            node->value = t.text;
            return node;
        }
        if (t.kind == ExprToken::Kind::LPAREN) {
            pos++;
            auto node = parseExpr();
            if (tokens[pos].kind != ExprToken::Kind::RPAREN) {
                throw std::runtime_error("TAC: expected ')' in expression");
            }
            pos++;
            return node;
        }
        throw std::runtime_error("TAC: unexpected token while parsing factor");
    }
};

} // anonymous namespace

namespace TAC {

std::shared_ptr<ASTNode> parseAssignment(const std::string &statement, std::string &targetVar) {
    size_t eq = statement.find('=');
    if (eq == std::string::npos) {
        throw std::runtime_error("TAC: statement is not an assignment (no '=' found)");
    }
    targetVar = Utils::trim(statement.substr(0, eq));
    std::string rhs = statement.substr(eq + 1);

    auto tokens = tokenizeExpr(rhs);
    ExprParser parser(tokens);
    return parser.parseExpr();
}

static std::string genTACRecursive(const std::shared_ptr<ASTNode> &node,
                                    std::vector<TACInstr> &code,
                                    int &tempCounter) {
    if (node->kind == ASTNode::Kind::NUMBER || node->kind == ASTNode::Kind::IDENTIFIER) {
        return node->value; // leaf: no instruction needed, value used directly
    }

    // BINOP: recursively generate code for both children first.
    std::string leftPlace = genTACRecursive(node->left, code, tempCounter);
    std::string rightPlace = genTACRecursive(node->right, code, tempCounter);

    std::string temp = "t" + std::to_string(tempCounter++);
    TACInstr instr;
    instr.result = temp;
    instr.arg1 = leftPlace;
    instr.op = node->value;
    instr.arg2 = rightPlace;
    code.push_back(instr);

    return temp;
}

std::vector<TACInstr> generate(const std::shared_ptr<ASTNode> &root, const std::string &targetVar) {
    std::vector<TACInstr> code;
    int tempCounter = 1;

    std::string finalPlace = genTACRecursive(root, code, tempCounter);

    // Final assignment of the computed value into the target variable.
    TACInstr assign;
    assign.result = targetVar;
    assign.arg1 = finalPlace;
    assign.op = "=";
    code.push_back(assign);

    return code;
}

void printTAC(const std::vector<TACInstr> &code, const std::string &title) {
    Logger::println(title);
    Logger::divider();
    for (const auto &instr : code) {
        Logger::println("  " + instr.toString());
    }
    Logger::println();
}

} // namespace TAC
