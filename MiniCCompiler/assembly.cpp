// ============================================================
// assembly.cpp
// Implementation of Phase 11 : Assembly Code Generation
// ============================================================
#include "assembly.h"
#include "utils.h"
#include <unordered_map>
#include <regex>

namespace {

    bool isTempName(const std::string &name) {
        static const std::regex tempPattern("^t[0-9]+$");
        return std::regex_match(name, tempPattern);
    }

    // Very small linear register allocator: every distinct name
    // (variable, temporary, or constant literal) gets its own
    // register the first time it is needed, reused on every
    // subsequent reference.
    class RegisterAllocator {
    public:
        std::string get(const std::string &name) {
            auto it = assigned.find(name);
            if (it != assigned.end()) return it->second;
            std::string reg = "R" + std::to_string(nextReg++);
            assigned[name] = reg;
            return reg;
        }

    private:
        std::unordered_map<std::string, std::string> assigned;
        int nextReg = 1;
    };
}

namespace Assembly {

std::vector<std::string> generate(const std::vector<TACInstr> &code) {
    std::vector<std::string> lines;
    RegisterAllocator regs;

    for (const auto &instr : code) {
        std::string rt = regs.get(instr.result);

        if (instr.op == "=") {
            // Plain assignment / constant load: MOV Rt, <source>
            if (Utils::isNumberString(instr.arg1)) {
                lines.push_back("MOV " + rt + ", #" + instr.arg1);
            } else {
                std::string rsrc = regs.get(instr.arg1);
                lines.push_back("MOV " + rt + ", " + rsrc);
            }
        } else {
            // Binary arithmetic: load arg1 into Rt, then apply op with arg2.
            if (Utils::isNumberString(instr.arg1)) {
                lines.push_back("MOV " + rt + ", #" + instr.arg1);
            } else {
                std::string r1 = regs.get(instr.arg1);
                lines.push_back("MOV " + rt + ", " + r1);
            }

            std::string operand2;
            if (Utils::isNumberString(instr.arg2)) {
                operand2 = "#" + instr.arg2;
            } else {
                operand2 = regs.get(instr.arg2);
            }

            if (instr.op == "+") lines.push_back("ADD " + rt + ", " + operand2);
            else if (instr.op == "-") lines.push_back("SUB " + rt + ", " + operand2);
            else if (instr.op == "*") lines.push_back("MUL " + rt + ", " + operand2);
            else if (instr.op == "/") lines.push_back("DIV " + rt + ", " + operand2);
        }

        // If the destination is a real (non-temporary) variable, the
        // computed value represents a program result and must be
        // written back to its memory location.
        if (!isTempName(instr.result)) {
            lines.push_back("MOV [" + instr.result + "], " + rt);
        }
    }

    return lines;
}

std::vector<std::string> generateConditionalExample(const std::string &left,
                                                      const std::string &op,
                                                      const std::string &right,
                                                      const std::string &label) {
    std::vector<std::string> lines;
    RegisterAllocator regs;
    std::string r1 = regs.get(left);
    std::string r2 = regs.get(right);

    lines.push_back("MOV " + r1 + ", " + left);
    lines.push_back("MOV " + r2 + ", " + right);
    lines.push_back("CMP " + r1 + ", " + r2);

    // Choose the conditional jump mnemonic matching the source operator.
    std::string jumpMnemonic = "JMP";
    if (op == ">")  jumpMnemonic = "JG";
    else if (op == "<")  jumpMnemonic = "JL";
    else if (op == ">=") jumpMnemonic = "JGE";
    else if (op == "<=") jumpMnemonic = "JLE";
    else if (op == "==") jumpMnemonic = "JE";
    else if (op == "!=") jumpMnemonic = "JNE";

    lines.push_back(jumpMnemonic + " " + label);
    return lines;
}

void printAssembly(const std::vector<std::string> &lines, const std::string &title) {
    Logger::println(title);
    Logger::divider();
    for (const auto &line : lines) {
        Logger::println("  " + line);
    }
    Logger::println();
}

} // namespace Assembly
