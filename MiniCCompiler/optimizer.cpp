// ============================================================
// optimizer.cpp
// Implementation of Phase 10 : Code Optimization
// ============================================================
#include "optimizer.h"
#include "utils.h"
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <cmath>

namespace {

    bool isTemp(const std::string &name) {
        static const std::regex tempPattern("^t[0-9]+$");
        return std::regex_match(name, tempPattern);
    }

    double applyOp(double a, double b, const std::string &op) {
        if (op == "+") return a + b;
        if (op == "-") return a - b;
        if (op == "*") return a * b;
        if (op == "/") return b != 0.0 ? a / b : 0.0;
        return 0.0;
    }

    // Formats a folded numeric constant, dropping a trailing ".0" for
    // whole numbers so integer-looking results stay readable.
    std::string formatNumber(double value) {
        if (value == static_cast<long long>(value)) {
            return std::to_string(static_cast<long long>(value));
        }
        std::string s = std::to_string(value);
        return s;
    }
}

namespace Optimizer {

// ------------------------------------------------------------
// Pass 1: Constant Folding
// If both operands of an arithmetic instruction are numeric
// literals, compute the result now and turn it into a copy.
// ------------------------------------------------------------
std::vector<TACInstr> constantFold(const std::vector<TACInstr> &code) {
    std::vector<TACInstr> result;
    for (const auto &instr : code) {
        if (instr.op != "=" && Utils::isNumberString(instr.arg1) && Utils::isNumberString(instr.arg2)) {
            double a = std::stod(instr.arg1);
            double b = std::stod(instr.arg2);
            double folded = applyOp(a, b, instr.op);

            TACInstr newInstr;
            newInstr.result = instr.result;
            newInstr.arg1 = formatNumber(folded);
            newInstr.op = "=";
            newInstr.arg2 = "";
            result.push_back(newInstr);
        } else {
            result.push_back(instr);
        }
    }
    return result;
}

// ------------------------------------------------------------
// Pass 2: Copy Propagation
// Whenever we see "x = y" (a plain copy), remember that x now
// stands for y, and substitute y in place of x everywhere x is
// used afterwards -- as long as x has not been reassigned.
// ------------------------------------------------------------
std::vector<TACInstr> propagateCopies(const std::vector<TACInstr> &code) {
    std::unordered_map<std::string, std::string> copyOf;
    std::vector<TACInstr> result;

    auto resolve = [&](const std::string &name) {
        std::string current = name;
        // Follow the copy chain in case of x = y, y = z style chains.
        std::unordered_set<std::string> seen;
        while (copyOf.count(current) && seen.insert(current).second) {
            current = copyOf[current];
        }
        return current;
    };

    for (auto instr : code) {
        if (!instr.arg1.empty()) instr.arg1 = resolve(instr.arg1);
        if (!instr.arg2.empty()) instr.arg2 = resolve(instr.arg2);

        result.push_back(instr);

        // Update the copy table AFTER emitting, based on the (already
        // resolved) instruction we just produced.
        if (instr.op == "=") {
            copyOf[instr.result] = instr.arg1;
        } else {
            // instr.result is redefined by a real computation now,
            // so any previous copy record for it is no longer valid.
            copyOf.erase(instr.result);
        }
    }

    return result;
}

// ------------------------------------------------------------
// Pass 3: Common Subexpression Elimination (basic)
// If "a op b" was already computed earlier into some temp T and
// neither operand has changed since, reuse T instead of
// recomputing the same expression.
// ------------------------------------------------------------
std::vector<TACInstr> eliminateCommonSubexpressions(const std::vector<TACInstr> &code) {
    std::vector<TACInstr> result;
    std::unordered_map<std::string, std::string> seenExpr; // "a op b" -> result var

    for (auto instr : code) {
        // Before doing anything else: if this instruction is about to
        // redefine a variable that some cached expression currently
        // depends on being unchanged, drop that stale cache entry.
        // (Conservative safety net for reused, non-temporary variables;
        // temporaries in this compiler are single-assignment so this
        // rarely triggers, but it keeps the pass correct in general.)
        for (auto it = seenExpr.begin(); it != seenExpr.end(); ) {
            if (it->second == instr.result) {
                it = seenExpr.erase(it);
            } else {
                ++it;
            }
        }

        if (instr.op != "=") {
            // Build a canonical key. + and * are commutative, so normalize
            // operand order for those to catch e.g. (a+b) and (b+a).
            std::string x = instr.arg1, y = instr.arg2;
            if ((instr.op == "+" || instr.op == "*") && x > y) std::swap(x, y);
            std::string key = x + " " + instr.op + " " + y;

            auto it = seenExpr.find(key);
            if (it != seenExpr.end()) {
                // Reuse previous computation: turn this into a copy.
                TACInstr copyInstr;
                copyInstr.result = instr.result;
                copyInstr.arg1 = it->second;
                copyInstr.op = "=";
                copyInstr.arg2 = "";
                result.push_back(copyInstr);
                continue;
            } else {
                seenExpr[key] = instr.result;
            }
        }
        result.push_back(instr);
    }
    return result;
}

// ------------------------------------------------------------
// Pass 4: Dead Code Elimination
// A temporary is "live" if some later instruction still needs
// its value. Assignments to real (non-temporary) variables are
// always kept since they are externally observable results.
// ------------------------------------------------------------
std::vector<TACInstr> eliminateDeadCode(const std::vector<TACInstr> &code) {
    std::unordered_set<std::string> needed;

    // First pass (backwards): decide which instructions to keep.
    std::vector<bool> keep(code.size(), false);
    for (int i = static_cast<int>(code.size()) - 1; i >= 0; i--) {
        const TACInstr &instr = code[i];
        bool mustKeep = !isTemp(instr.result) || needed.count(instr.result) > 0;

        if (mustKeep) {
            keep[i] = true;
            needed.erase(instr.result);
            if (!instr.arg1.empty() && !Utils::isNumberString(instr.arg1)) needed.insert(instr.arg1);
            if (!instr.arg2.empty() && !Utils::isNumberString(instr.arg2)) needed.insert(instr.arg2);
        }
    }

    std::vector<TACInstr> result;
    for (size_t i = 0; i < code.size(); i++) {
        if (keep[i]) result.push_back(code[i]);
    }
    return result;
}

std::vector<TACInstr> optimize(const std::vector<TACInstr> &code) {
    std::vector<TACInstr> step1 = constantFold(code);
    std::vector<TACInstr> step2 = propagateCopies(step1);
    std::vector<TACInstr> step3 = eliminateCommonSubexpressions(step2);
    std::vector<TACInstr> step4 = eliminateDeadCode(step3);
    return step4;
}

} // namespace Optimizer
