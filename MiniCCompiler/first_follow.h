// ============================================================
// first_follow.h
// Phase 5 & 6: FIRST and FOLLOW set computation
// Generic fixed-point algorithms that operate on any Grammar
// object, correctly handling epsilon productions and (indirect)
// recursive productions.
// ============================================================
#ifndef FIRST_FOLLOW_H
#define FIRST_FOLLOW_H

#include "grammar.h"
#include <map>
#include <set>
#include <vector>
#include <string>

namespace FirstFollow {

    using SymbolSet = std::set<std::string>;
    using SetTable = std::map<std::string, SymbolSet>;

    // Computes FIRST(X) for every non-terminal X in the grammar.
    SetTable computeFirstSets(const Grammar &g);

    // Computes FIRST of an arbitrary symbol string (sequence of RHS
    // symbols), given already-computed FIRST sets for non-terminals.
    // Used internally, but exposed because the parsing table builder
    // needs the same logic.
    SymbolSet firstOfSequence(const std::vector<std::string> &seq,
                               const SetTable &firstSets,
                               const Grammar &g);

    // Computes FOLLOW(X) for every non-terminal X in the grammar.
    SetTable computeFollowSets(const Grammar &g, const SetTable &firstSets);

    // Pretty-printers. `setName` is printed as e.g. "FIRST(EXPR) = { ... }"
    void printSets(const SetTable &table, const std::string &title, const std::string &setName);
}

#endif // FIRST_FOLLOW_H
