// ============================================================
// parsing_table.h
// Phase 7: LL(1) Parsing Table Construction
// Builds table[NonTerminal][Terminal] = Production automatically
// from the FIRST and FOLLOW sets of a (left-recursion-free)
// grammar, and reports any FIRST/FIRST or FIRST/FOLLOW conflicts.
// ============================================================
#ifndef PARSING_TABLE_H
#define PARSING_TABLE_H

#include "grammar.h"
#include "first_follow.h"
#include <map>
#include <string>

namespace ParsingTable {

    // table[NonTerminal][Terminal] -> the Production to apply.
    using Table = std::map<std::string, std::map<std::string, Production>>;

    struct BuildResult {
        Table table;
        std::vector<std::string> conflicts; // human readable conflict descriptions
    };

    // Builds the LL(1) parsing table for grammar `g` using its
    // precomputed FIRST and FOLLOW sets.
    BuildResult build(const Grammar &g, const FirstFollow::SetTable &firstSets,
                       const FirstFollow::SetTable &followSets);

    // Nicely prints the table as rows (non-terminals) x columns (terminals).
    void print(const BuildResult &result, const Grammar &g);
}

#endif // PARSING_TABLE_H
