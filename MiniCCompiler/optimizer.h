// ============================================================
// optimizer.h
// Phase 10: Code Optimization
// Applies four classic, simple optimizations to a list of
// Three Address Code instructions, in this order:
//   1. Constant Folding                (compile-time arithmetic)
//   2. Copy Propagation                (replace copies with their source)
//   3. Common Subexpression Elimination (basic, straight-line)
//   4. Dead Code Elimination           (drop unused temporaries)
// ============================================================
#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "tac.h"
#include <vector>

namespace Optimizer {

    std::vector<TACInstr> constantFold(const std::vector<TACInstr> &code);
    std::vector<TACInstr> propagateCopies(const std::vector<TACInstr> &code);
    std::vector<TACInstr> eliminateCommonSubexpressions(const std::vector<TACInstr> &code);
    std::vector<TACInstr> eliminateDeadCode(const std::vector<TACInstr> &code);

    // Runs all four passes in sequence and returns the final optimized code.
    std::vector<TACInstr> optimize(const std::vector<TACInstr> &code);
}

#endif // OPTIMIZER_H
