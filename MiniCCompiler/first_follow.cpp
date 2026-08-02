// ============================================================
// first_follow.cpp
// Implementation of Phase 5 (FIRST) and Phase 6 (FOLLOW)
// ============================================================
#include "first_follow.h"
#include "utils.h"

namespace FirstFollow {

SymbolSet firstOfSequence(const std::vector<std::string> &seq,
                           const SetTable &firstSets,
                           const Grammar &g) {
    SymbolSet result;

    if (seq.empty() || (seq.size() == 1 && seq[0] == EPSILON_SYM)) {
        result.insert(EPSILON_SYM);
        return result;
    }

    bool allNullableSoFar = true;
    for (const auto &sym : seq) {
        if (!allNullableSoFar) break;

        if (!g.isNonTerminal(sym)) {
            // Terminal (or EPSILON literal appearing mid-sequence, which
            // should not normally happen, but handle gracefully anyway).
            if (sym != EPSILON_SYM) {
                result.insert(sym);
            }
            allNullableSoFar = false; // terminal is never nullable
        } else {
            auto it = firstSets.find(sym);
            SymbolSet firstOfSym = (it != firstSets.end()) ? it->second : SymbolSet();

            for (const auto &t : firstOfSym) {
                if (t != EPSILON_SYM) result.insert(t);
            }

            if (firstOfSym.find(EPSILON_SYM) == firstOfSym.end()) {
                allNullableSoFar = false; // this symbol cannot vanish, stop here
            }
        }
    }

    if (allNullableSoFar) {
        // Every symbol in the sequence could derive epsilon.
        result.insert(EPSILON_SYM);
    }

    return result;
}

SetTable computeFirstSets(const Grammar &g) {
    SetTable firstSets;
    for (const auto &nt : g.nonTerminals) firstSets[nt] = SymbolSet();

    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto &p : g.productions) {
            SymbolSet rhsFirst = firstOfSequence(p.rhs, firstSets, g);

            size_t before = firstSets[p.lhs].size();
            for (const auto &sym : rhsFirst) {
                firstSets[p.lhs].insert(sym);
            }
            if (firstSets[p.lhs].size() != before) changed = true;
        }
    }

    return firstSets;
}

SetTable computeFollowSets(const Grammar &g, const SetTable &firstSets) {
    SetTable followSets;
    for (const auto &nt : g.nonTerminals) followSets[nt] = SymbolSet();

    // Rule 1: $ is in FOLLOW(start symbol)
    if (!g.startSymbol.empty()) {
        followSets[g.startSymbol].insert(END_SYM);
    }

    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto &p : g.productions) {
            const auto &rhs = p.rhs;
            for (size_t i = 0; i < rhs.size(); i++) {
                const std::string &B = rhs[i];
                if (!g.isNonTerminal(B)) continue; // only care about non-terminals

                // beta = everything after B in this production
                std::vector<std::string> beta(rhs.begin() + i + 1, rhs.end());
                SymbolSet firstBeta = firstOfSequence(beta, firstSets, g);

                size_t before = followSets[B].size();

                // Rule 2: add FIRST(beta) \ {epsilon} to FOLLOW(B)
                for (const auto &t : firstBeta) {
                    if (t != EPSILON_SYM) followSets[B].insert(t);
                }

                // Rule 3: if beta is empty or nullable, add FOLLOW(A) to FOLLOW(B)
                if (beta.empty() || firstBeta.find(EPSILON_SYM) != firstBeta.end()) {
                    for (const auto &t : followSets[p.lhs]) {
                        followSets[B].insert(t);
                    }
                }

                if (followSets[B].size() != before) changed = true;
            }
        }
    }

    return followSets;
}

void printSets(const SetTable &table, const std::string &title, const std::string &setName) {
    Logger::println(title);
    Logger::divider();
    for (const auto &entry : table) {
        std::string line = setName + "(" + entry.first + ") = { ";
        bool first = true;
        for (const auto &sym : entry.second) {
            if (!first) line += ", ";
            line += sym;
            first = false;
        }
        line += " }";
        Logger::println(line);
    }
    Logger::println();
}

} // namespace FirstFollow
