// ============================================================
// assembly.h
// Phase 11: Assembly Code Generation
// Translates optimized Three Address Code into a simple,
// register-based pseudo-assembly using MOV / ADD / SUB / MUL /
// DIV, and stores final results back to memory (the original
// variable names). CMP and JMP are also supported by this
// module for completeness (used for conditional / loop
// constructs elsewhere in a full compiler), even though the
// pure arithmetic sample program in this lab does not need them.
// ============================================================
#ifndef ASSEMBLY_H
#define ASSEMBLY_H

#include "tac.h"
#include <vector>
#include <string>

namespace Assembly {

    // Generates assembly instructions for a list of (optimized) TAC
    // instructions. Returns the assembly program as a list of lines.
    std::vector<std::string> generate(const std::vector<TACInstr> &code);

    // Demonstrates CMP/JMP generation for a simple conditional,
    // e.g. "if (a > b) goto L1" -- included so the module shows it
    // supports the full requested instruction set.
    std::vector<std::string> generateConditionalExample(const std::string &left,
                                                          const std::string &op,
                                                          const std::string &right,
                                                          const std::string &label);

    void printAssembly(const std::vector<std::string> &lines, const std::string &title);
}

#endif // ASSEMBLY_H
