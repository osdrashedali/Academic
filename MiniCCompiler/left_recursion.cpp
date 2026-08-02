// ============================================================
// left_recursion.cpp
// Implementation of Phase 4 : Left Recursion Removal
// ============================================================
#include "left_recursion.h"
#include "utils.h"
#include <algorithm>

namespace LeftRecursion {

Grammar removeLeftRecursion(const Grammar &g) {
    Grammar result;
    result.startSymbol = g.startSymbol;

    // We rebuild non-terminals / terminals / productions from scratch
    // as we go, so classification stays consistent with the new grammar.
    std::vector<std::string> newNonTerminals;

    for (const auto &nt : g.nonTerminals) {
        std::vector<Production> prods = g.productionsFor(nt);

        std::vector<std::vector<std::string>> recursive;    // alpha parts (A -> A alpha)
        std::vector<std::vector<std::string>> nonRecursive; // beta parts (A -> beta)

        for (const auto &p : prods) {
            if (!p.rhs.empty() && p.rhs[0] == nt) {
                // A -> A alpha  : strip the leading A, keep alpha
                std::vector<std::string> alpha(p.rhs.begin() + 1, p.rhs.end());
                recursive.push_back(alpha);
            } else {
                nonRecursive.push_back(p.rhs);
            }
        }

        if (recursive.empty()) {
            // No immediate left recursion on this non-terminal: copy as-is.
            for (const auto &beta : nonRecursive) {
                result.productions.push_back(Production(nt, beta));
            }
            if (std::find(newNonTerminals.begin(), newNonTerminals.end(), nt) == newNonTerminals.end())
                newNonTerminals.push_back(nt);
            continue;
        }

        // Left recursion detected on `nt`: apply the standard rewrite.
        std::string ntPrime = nt + "'";

        // A -> beta A'   (for every non-recursive beta)
        for (auto beta : nonRecursive) {
            if (beta.size() == 1 && beta[0] == EPSILON_SYM) {
                // A -> EPSILON  becomes  A -> A'
                std::vector<std::string> rhs = { ntPrime };
                result.productions.push_back(Production(nt, rhs));
            } else {
                beta.push_back(ntPrime);
                result.productions.push_back(Production(nt, beta));
            }
        }

        // A' -> alpha A'   (for every recursive alpha)
        for (auto alpha : recursive) {
            alpha.push_back(ntPrime);
            result.productions.push_back(Production(ntPrime, alpha));
        }

        // A' -> EPSILON
        result.productions.push_back(Production(ntPrime, { EPSILON_SYM }));

        if (std::find(newNonTerminals.begin(), newNonTerminals.end(), nt) == newNonTerminals.end())
            newNonTerminals.push_back(nt);
        newNonTerminals.push_back(ntPrime);
    }

    result.nonTerminals = newNonTerminals;

    // Re-derive the terminal set for the transformed grammar.
    for (const auto &p : result.productions) {
        for (const auto &sym : p.rhs) {
            if (sym == EPSILON_SYM) continue;
            bool isNT = std::find(result.nonTerminals.begin(), result.nonTerminals.end(), sym) != result.nonTerminals.end();
            if (!isNT && std::find(result.terminals.begin(), result.terminals.end(), sym) == result.terminals.end()) {
                result.terminals.push_back(sym);
            }
        }
    }

    return result;
}

} // namespace LeftRecursion
