// ============================================================
// parser.cpp
// Implementation of Phase 8 : Predictive Parsing
// ============================================================
#include "parser.h"
#include "utils.h"
#include <deque>
#include <iomanip>
#include <sstream>

namespace Parser {

// Builds a printable string of the stack (top on the right, as
// conventionally shown in predictive-parsing trace tables).
static std::string stackToString(const std::deque<std::string> &stack) {
    std::string s;
    for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
        s += *it + " ";
    }
    return s;
}

// Builds a printable string of the remaining input, left to right.
static std::string inputToString(const std::vector<std::string> &input, size_t pos) {
    std::string s;
    for (size_t i = pos; i < input.size(); i++) s += input[i] + " ";
    return s;
}

bool parse(const Grammar &g, const ParsingTable::Table &table,
           const std::vector<std::string> &inputTerminals) {

    std::vector<std::string> input = inputTerminals;
    input.push_back(END_SYM);

    std::deque<std::string> stack;
    stack.push_back(END_SYM);
    stack.push_back(g.startSymbol);

    size_t pos = 0;

    const int STACK_WIDTH = 42;
    const int INPUT_WIDTH = 28;
    const int ACTION_WIDTH = 34;

    // Table header
    std::ostringstream header;
    header << std::left << std::setw(STACK_WIDTH) << "Stack"
           << std::setw(INPUT_WIDTH) << "Input"
           << std::setw(ACTION_WIDTH) << "Action";
    Logger::println(header.str());
    Logger::divider();

    int step = 0;
    const int MAX_STEPS = 1000; // safety guard against infinite loops on bad tables

    while (!stack.empty()) {
        if (++step > MAX_STEPS) {
            Logger::println("Parsing aborted: exceeded maximum step count.");
            return false;
        }

        std::string top = stack.back();
        std::string currentInput = (pos < input.size()) ? input[pos] : END_SYM;

        std::string action;

        if (top == END_SYM && currentInput == END_SYM) {
            action = "Accept";
            std::ostringstream row;
            row << std::left << std::setw(STACK_WIDTH) << stackToString(stack)
                << std::setw(INPUT_WIDTH) << inputToString(input, pos)
                << std::setw(ACTION_WIDTH) << action;
            Logger::println(row.str());
            Logger::println();
            Logger::println("Result: Accepted");
            return true;
        }

        if (!g.isNonTerminal(top)) {
            // Top of stack is a terminal (or $): it must match the current input.
            if (top == currentInput) {
                action = "Match '" + top + "'";
                stack.pop_back();
                pos++;
            } else {
                action = "ERROR: expected '" + top + "' but found '" + currentInput + "'";
                std::ostringstream row;
                row << std::left << std::setw(STACK_WIDTH) << stackToString(stack)
                    << std::setw(INPUT_WIDTH) << inputToString(input, pos)
                    << std::setw(ACTION_WIDTH) << action;
                Logger::println(row.str());
                Logger::println();
                Logger::println("Result: Syntax Error");
                return false;
            }
        } else {
            // Top of stack is a non-terminal: consult the parsing table.
            auto ntIt = table.find(top);
            if (ntIt == table.end() || ntIt->second.find(currentInput) == ntIt->second.end()) {
                action = "ERROR: no rule for [" + top + ", " + currentInput + "]";
                std::ostringstream row;
                row << std::left << std::setw(STACK_WIDTH) << stackToString(stack)
                    << std::setw(INPUT_WIDTH) << inputToString(input, pos)
                    << std::setw(ACTION_WIDTH) << action;
                Logger::println(row.str());
                Logger::println();
                Logger::println("Result: Syntax Error");
                return false;
            }

            const Production &prod = ntIt->second.at(currentInput);
            action = "Apply " + prod.toString();

            stack.pop_back();
            if (!(prod.rhs.size() == 1 && prod.rhs[0] == EPSILON_SYM)) {
                for (auto it = prod.rhs.rbegin(); it != prod.rhs.rend(); ++it) {
                    stack.push_back(*it);
                }
            }
        }

        std::ostringstream row;
        row << std::left << std::setw(STACK_WIDTH) << stackToString(stack)
            << std::setw(INPUT_WIDTH) << inputToString(input, pos)
            << std::setw(ACTION_WIDTH) << action;
        Logger::println(row.str());
    }

    Logger::println("Result: Syntax Error (stack emptied unexpectedly)");
    return false;
}

} // namespace Parser
