// ============================================================
// parser.h
// Phase 8: Predictive (Table-Driven) Parsing
// Uses the LL(1) parsing table built in Phase 7 to parse a
// stream of terminal symbols, printing the Stack / Input /
// Action trace at every step, and finally reporting
// "Accepted" or "Syntax Error".
// ============================================================
#ifndef PARSER_H
#define PARSER_H

#include "grammar.h"
#include "parsing_table.h"
#include <vector>
#include <string>

namespace Parser {

    // Runs the predictive parsing algorithm.
    // `inputTerminals` must be a sequence of terminal symbols as they
    // appear in the grammar (e.g. "ID", "=", "(", "NUM", ...), WITHOUT
    // the trailing end marker -- it is added automatically.
    // Returns true if the input was accepted, false on syntax error.
    bool parse(const Grammar &g, const ParsingTable::Table &table,
               const std::vector<std::string> &inputTerminals);

}

#endif // PARSER_H
