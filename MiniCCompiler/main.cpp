// ============================================================
// main.cpp
// Mini C Compiler -- University Compiler Lab Project
//
// Drives all eleven phases of the mini-compiler pipeline in
// sequence, printing every stage's output to the console AND
// to output.txt via the Logger utility.
//
//   Stage 1  : Comment Removal
//   Stage 2  : Lexical Analysis
//   Stage 3  : Grammar Processing
//   Stage 4  : Left Recursion Removal
//   Stage 5  : FIRST Set Computation
//   Stage 6  : FOLLOW Set Computation
//   Stage 7  : LL(1) Parsing Table Construction
//   Stage 8  : Predictive Parsing
//   Stage 9  : Three Address Code Generation
//   Stage 10 : Code Optimization
//   Stage 11 : Assembly Code Generation
// ============================================================

#include "utils.h"
#include "comment_removal.h"
#include "tokenizer.h"
#include "grammar.h"
#include "left_recursion.h"
#include "first_follow.h"
#include "parsing_table.h"
#include "parser.h"
#include "tac.h"
#include "optimizer.h"
#include "assembly.h"

#include <iostream>
#include <vector>
#include <string>

// ------------------------------------------------------------
// Helper: converts a small hand-written expression string such
// as "a = ( b + c ) * d" into a stream of grammar TERMINAL
// symbols (ID, NUM, "=", "+", "-", "*", "/", "(", ")") so it can
// be fed straight into the Phase 8 predictive parser.
// ------------------------------------------------------------
static std::vector<std::string> toTerminalStream(const std::string &expr) {
    std::vector<std::string> terminals;
    std::vector<std::string> rawTokens = Utils::splitWhitespace(expr);

    for (const auto &tok : rawTokens) {
        if (tok == "=" || tok == "+" || tok == "-" || tok == "*" ||
            tok == "/" || tok == "(" || tok == ")") {
            terminals.push_back(tok);
        } else if (Utils::isNumberString(tok)) {
            terminals.push_back("NUM");
        } else {
            terminals.push_back("ID");
        }
    }
    return terminals;
}

int main() {
    Logger::init("output.txt");

    Logger::println("############################################################");
    Logger::println("#              MINI C COMPILER -- LAB PROJECT             #");
    Logger::println("############################################################");

    // ============================================================
    // STAGE 1 : Comment Removal
    // ============================================================
    Logger::stageHeader(1, "Comment Removal");
    std::string rawSource;
    std::string cleanedSource;
    try {
        rawSource = CommentRemoval::readFile("input.c");
        cleanedSource = CommentRemoval::removeComments(rawSource);
        Logger::println("Cleaned source code (comments removed):");
        Logger::divider();
        Logger::println(cleanedSource);
    } catch (const std::exception &e) {
        Logger::println(std::string("ERROR in Stage 1: ") + e.what());
        Logger::close();
        return 1;
    }

    // ============================================================
    // STAGE 2 : Lexical Analysis
    // ============================================================
    Logger::stageHeader(2, "Lexical Analysis (Tokenization)");
    std::vector<Token> tokens = Tokenizer::tokenize(cleanedSource);
    Tokenizer::printTokenTable(tokens);

    // ============================================================
    // STAGE 3 : Grammar Processing
    // ============================================================
    Logger::stageHeader(3, "Grammar Processing");
    Grammar originalGrammar;
    try {
        originalGrammar.loadFromFile("grammar.txt");
        originalGrammar.display("Original Grammar (as read from grammar.txt)");
    } catch (const std::exception &e) {
        Logger::println(std::string("ERROR in Stage 3: ") + e.what());
        Logger::close();
        return 1;
    }

    // ============================================================
    // STAGE 4 : Left Recursion Removal
    // ============================================================
    Logger::stageHeader(4, "Left Recursion Removal");
    Grammar cleanGrammar = LeftRecursion::removeLeftRecursion(originalGrammar);
    cleanGrammar.display("Transformed Grammar (left recursion eliminated)");

    // ============================================================
    // STAGE 5 : FIRST Set Computation
    // ============================================================
    Logger::stageHeader(5, "FIRST Set Computation");
    FirstFollow::SetTable firstSets = FirstFollow::computeFirstSets(cleanGrammar);
    FirstFollow::printSets(firstSets, "FIRST Sets", "FIRST");

    // ============================================================
    // STAGE 6 : FOLLOW Set Computation
    // ============================================================
    Logger::stageHeader(6, "FOLLOW Set Computation");
    FirstFollow::SetTable followSets = FirstFollow::computeFollowSets(cleanGrammar, firstSets);
    FirstFollow::printSets(followSets, "FOLLOW Sets", "FOLLOW");

    // ============================================================
    // STAGE 7 : LL(1) Parsing Table Construction
    // ============================================================
    Logger::stageHeader(7, "LL(1) Parsing Table Construction");
    ParsingTable::BuildResult tableResult = ParsingTable::build(cleanGrammar, firstSets, followSets);
    ParsingTable::print(tableResult, cleanGrammar);

    // ============================================================
    // STAGE 8 : Predictive Parsing
    // ============================================================
    Logger::stageHeader(8, "Predictive Parsing");

    Logger::println("Test Case 1 (valid assignment expression): a = ( b + c ) * d");
    Logger::println("");
    std::vector<std::string> validInput = toTerminalStream("ID = ( ID + ID ) * ID");
    Parser::parse(cleanGrammar, tableResult.table, validInput);

    Logger::println("");
    Logger::println("Test Case 2 (invalid expression, missing operand): a = ( b + )");
    Logger::println("");
    std::vector<std::string> invalidInput = toTerminalStream("ID = ( ID + )");
    Parser::parse(cleanGrammar, tableResult.table, invalidInput);

    // ============================================================
    // STAGE 9 : Three Address Code Generation
    // ============================================================
    Logger::stageHeader(9, "Three Address Code (TAC) Generation");
    std::string sourceExpr = "a=(b+c)*(d-e)/2";
    Logger::println("Source expression: " + sourceExpr);
    Logger::println("");

    std::string targetVar;
    std::shared_ptr<ASTNode> ast = TAC::parseAssignment(sourceExpr, targetVar);
    std::vector<TACInstr> tacCode = TAC::generate(ast, targetVar);
    TAC::printTAC(tacCode, "Generated Three Address Code");

    // ============================================================
    // STAGE 10 : Code Optimization
    // ============================================================
    Logger::stageHeader(10, "Code Optimization");

    Logger::println("(A) Optimizing the TAC generated in Stage 9");
    Logger::println("");
    std::vector<TACInstr> optimizedMain = Optimizer::optimize(tacCode);
    TAC::printTAC(optimizedMain, "Optimized TAC for: " + sourceExpr);

    Logger::println("(B) Extended demonstration covering all four optimizations");
    Logger::println("    (constant folding, copy propagation, common subexpression");
    Logger::println("     elimination, and dead code elimination) on a hand-built");
    Logger::println("     TAC sequence with clear opportunities for each:");
    Logger::println("");

    std::vector<TACInstr> demoCode;
    // t1 = 4 + 6            -> constant folding candidate
    demoCode.push_back({"t1", "4", "+", "6"});
    // t2 = a                -> copy, propagated forward
    demoCode.push_back({"t2", "a", "=", ""});
    // x  = t2                -> becomes x = a after copy propagation
    demoCode.push_back({"x", "t2", "=", ""});
    // t3 = b + c
    demoCode.push_back({"t3", "b", "+", "c"});
    // t4 = b + c            -> common subexpression of t3
    demoCode.push_back({"t4", "b", "+", "c"});
    // y  = t3 + t4          -> after CSE, effectively t3 + t3
    demoCode.push_back({"y", "t3", "+", "t4"});
    // t5 = 100              -> dead code, never used afterwards
    demoCode.push_back({"t5", "100", "=", ""});
    // z  = x * y
    demoCode.push_back({"z", "x", "*", "y"});

    TAC::printTAC(demoCode, "Original Demonstration TAC");

    std::vector<TACInstr> afterFold = Optimizer::constantFold(demoCode);
    TAC::printTAC(afterFold, "Step 1 -- After Constant Folding (4 + 6 -> 10)");

    std::vector<TACInstr> afterCopy = Optimizer::propagateCopies(afterFold);
    TAC::printTAC(afterCopy, "Step 2 -- After Copy Propagation (x = t2 -> x = a)");

    std::vector<TACInstr> afterCSE = Optimizer::eliminateCommonSubexpressions(afterCopy);
    TAC::printTAC(afterCSE, "Step 3 -- After Common Subexpression Elimination (t4 reuses t3)");

    std::vector<TACInstr> afterDCE = Optimizer::eliminateDeadCode(afterCSE);
    TAC::printTAC(afterDCE, "Step 4 -- After Dead Code Elimination (t1, t2, t5 dropped)");

    // ============================================================
    // STAGE 11 : Assembly Code Generation
    // ============================================================
    Logger::stageHeader(11, "Assembly Code Generation");

    Logger::println("(A) Assembly generated from the optimized TAC of Stage 9:");
    Logger::println("");
    std::vector<std::string> assemblyLines = Assembly::generate(optimizedMain);
    Assembly::printAssembly(assemblyLines, "Generated Assembly Code (" + sourceExpr + ")");

    Logger::println("(B) CMP / JMP demonstration for a conditional such as");
    Logger::println("    the grading logic used in input.c ('if (units > 500)'):");
    Logger::println("");
    std::vector<std::string> condLines = Assembly::generateConditionalExample("units", ">", "500", "L_HEAVY_USAGE");
    Assembly::printAssembly(condLines, "Generated Assembly Code (conditional branch)");

    Logger::println("############################################################");
    Logger::println("#                 COMPILATION PIPELINE COMPLETE           #");
    Logger::println("############################################################");

    Logger::close();
    return 0;
}
