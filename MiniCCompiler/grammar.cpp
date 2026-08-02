// ============================================================
// grammar.cpp
// Implementation of Phase 3 : Grammar Processing
// ============================================================
#include "grammar.h"
#include "utils.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

std::string Production::toString() const {
    std::string s = lhs + " -> ";
    for (size_t i = 0; i < rhs.size(); i++) {
        s += rhs[i];
        if (i + 1 < rhs.size()) s += " ";
    }
    return s;
}

void Grammar::loadFromFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open grammar file: " + filename);
    }

    productions.clear();
    nonTerminals.clear();
    terminals.clear();
    startSymbol.clear();

    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = Utils::trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue; // skip blank/comment lines

        size_t arrowPos = trimmed.find("->");
        if (arrowPos == std::string::npos) continue; // malformed line, skip

        std::string lhs = Utils::trim(trimmed.substr(0, arrowPos));
        std::string rhsPart = Utils::trim(trimmed.substr(arrowPos + 2));

        // Support "A -> alpha | beta | gamma" on one line as a convenience,
        // in addition to one-production-per-line format.
        std::stringstream altStream(rhsPart);
        std::string alt;
        while (std::getline(altStream, alt, '|')) {
            std::vector<std::string> rhsSymbols = Utils::splitWhitespace(Utils::trim(alt));
            if (rhsSymbols.empty()) rhsSymbols.push_back(EPSILON_SYM);
            productions.push_back(Production(lhs, rhsSymbols));

            if (startSymbol.empty()) startSymbol = lhs;

            if (std::find(nonTerminals.begin(), nonTerminals.end(), lhs) == nonTerminals.end()) {
                nonTerminals.push_back(lhs);
            }
        }
    }

    classifySymbols();
}

void Grammar::classifySymbols() {
    // Any RHS symbol that never appears as an LHS is a terminal
    // (excluding EPSILON, which is not a real terminal symbol).
    for (const auto &p : productions) {
        for (const auto &sym : p.rhs) {
            if (sym == EPSILON_SYM) continue;
            bool isNT = std::find(nonTerminals.begin(), nonTerminals.end(), sym) != nonTerminals.end();
            if (!isNT) {
                if (std::find(terminals.begin(), terminals.end(), sym) == terminals.end()) {
                    terminals.push_back(sym);
                }
            }
        }
    }
}

std::vector<Production> Grammar::productionsFor(const std::string &nt) const {
    std::vector<Production> result;
    for (const auto &p : productions) {
        if (p.lhs == nt) result.push_back(p);
    }
    return result;
}

bool Grammar::isNonTerminal(const std::string &symbol) const {
    return std::find(nonTerminals.begin(), nonTerminals.end(), symbol) != nonTerminals.end();
}

void Grammar::display(const std::string &title) const {
    Logger::println(title + " (Start Symbol = " + startSymbol + ")");
    Logger::divider();
    for (const auto &p : productions) {
        Logger::println("  " + p.toString());
    }
    Logger::println();

    std::string ntLine = "Non-terminals: ";
    for (const auto &nt : nonTerminals) ntLine += nt + " ";
    Logger::println(ntLine);

    std::string tLine = "Terminals    : ";
    for (const auto &t : terminals) tLine += t + " ";
    Logger::println(tLine);
}
