// ============================================================
// parsing_table.cpp
// Implementation of Phase 7 : LL(1) Parsing Table Construction
// ============================================================
#include "parsing_table.h"
#include "utils.h"
#include <iomanip>
#include <sstream>

namespace ParsingTable {

BuildResult build(const Grammar &g, const FirstFollow::SetTable &firstSets,
                   const FirstFollow::SetTable &followSets) {
    BuildResult result;

    for (const auto &p : g.productions) {
        FirstFollow::SymbolSet firstOfRhs = FirstFollow::firstOfSequence(p.rhs, firstSets, g);

        // For every terminal a in FIRST(rhs), set table[LHS][a] = production
        for (const auto &a : firstOfRhs) {
            if (a == EPSILON_SYM) continue;

            auto &cell = result.table[p.lhs];
            auto existing = cell.find(a);
            if (existing != cell.end() && !(existing->second.lhs == p.lhs && existing->second.rhs == p.rhs)) {
                result.conflicts.push_back(
                    "Conflict at table[" + p.lhs + "][" + a + "] between '" +
                    existing->second.toString() + "' and '" + p.toString() + "'");
            }
            cell[a] = p;
        }

        // If EPSILON in FIRST(rhs), add production for every terminal in FOLLOW(LHS)
        if (firstOfRhs.find(EPSILON_SYM) != firstOfRhs.end()) {
            auto followIt = followSets.find(p.lhs);
            if (followIt != followSets.end()) {
                for (const auto &b : followIt->second) {
                    auto &cell = result.table[p.lhs];
                    auto existing = cell.find(b);
                    if (existing != cell.end() && !(existing->second.lhs == p.lhs && existing->second.rhs == p.rhs)) {
                        result.conflicts.push_back(
                            "Conflict at table[" + p.lhs + "][" + b + "] between '" +
                            existing->second.toString() + "' and '" + p.toString() + "'");
                    }
                    cell[b] = p;
                }
            }
        }
    }

    return result;
}

void print(const BuildResult &result, const Grammar &g) {
    Logger::println("LL(1) Parsing Table");
    Logger::divider();

    const int COL_WIDTH = 24;

    // Header row: terminals + end marker
    std::ostringstream header;
    header << std::left << std::setw(10) << "NT";
    for (const auto &t : g.terminals) header << std::setw(COL_WIDTH) << t;
    header << std::setw(COL_WIDTH) << END_SYM;
    Logger::println(header.str());

    for (const auto &nt : g.nonTerminals) {
        std::ostringstream row;
        row << std::left << std::setw(10) << nt;

        auto ntIt = result.table.find(nt);
        for (const auto &t : g.terminals) {
            std::string cellText = "-";
            if (ntIt != result.table.end()) {
                auto cellIt = ntIt->second.find(t);
                if (cellIt != ntIt->second.end()) cellText = cellIt->second.toString();
            }
            row << std::setw(COL_WIDTH) << cellText;
        }
        // end marker column
        std::string cellText = "-";
        if (ntIt != result.table.end()) {
            auto cellIt = ntIt->second.find(END_SYM);
            if (cellIt != ntIt->second.end()) cellText = cellIt->second.toString();
        }
        row << std::setw(COL_WIDTH) << cellText;

        Logger::println(row.str());
    }

    Logger::println();
    if (result.conflicts.empty()) {
        Logger::println("No FIRST/FIRST or FIRST/FOLLOW conflicts detected. Grammar is LL(1).");
    } else {
        Logger::println("Conflicts detected (grammar is NOT strictly LL(1)):");
        for (const auto &c : result.conflicts) Logger::println("  " + c);
    }
}

} // namespace ParsingTable
