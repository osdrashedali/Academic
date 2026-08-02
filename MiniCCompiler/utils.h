// ============================================================
// utils.h
// Common utility helpers shared across every phase of the
// Mini C Compiler. Currently provides a Logger that writes
// every printed line simultaneously to the console AND to
// output.txt, so the user gets a permanent record of the run.
// ============================================================
#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>

// ------------------------------------------------------------
// Logger
// A tiny "tee" utility: anything streamed into a Logger object
// with << is written to std::cout AND appended to output.txt.
// Used everywhere instead of raw std::cout so every stage of
// the compiler is captured in the report file automatically.
// ------------------------------------------------------------
class Logger {
public:
    // Opens output.txt in the given mode (default: truncate at start of run)
    static void init(const std::string &path = "output.txt") {
        outFile().open(path, std::ios::out | std::ios::trunc);
    }

    static void close() {
        if (outFile().is_open())
            outFile().close();
    }

    // Print a line to both console and file (adds newline)
    static void println(const std::string &line = "") {
        std::cout << line << std::endl;
        if (outFile().is_open()) {
            outFile() << line << std::endl;
        }
    }

    // Print without forcing newline (caller controls formatting)
    static void print(const std::string &text) {
        std::cout << text;
        if (outFile().is_open()) {
            outFile() << text;
        }
    }

    // Convenience: prints a big banner for a new stage
    static void stageHeader(int stageNumber, const std::string &title) {
        println();
        println("========================================================");
        println("STAGE " + std::to_string(stageNumber) + " : " + title);
        println("========================================================");
    }

    // Convenience: prints a thin section divider
    static void divider() {
        println("--------------------------------------------------------");
    }

private:
    static std::ofstream &outFile() {
        static std::ofstream file;
        return file;
    }
};

// ------------------------------------------------------------
// Small generic helpers used by several modules
// ------------------------------------------------------------
namespace Utils {

    // Trim leading/trailing whitespace from a string
    inline std::string trim(const std::string &s) {
        size_t start = s.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = s.find_last_not_of(" \t\r\n");
        return s.substr(start, end - start + 1);
    }

    // Split a string by a single-character delimiter, skipping empty tokens
    inline std::vector<std::string> splitWhitespace(const std::string &s) {
        std::vector<std::string> tokens;
        std::istringstream iss(s);
        std::string tok;
        while (iss >> tok) tokens.push_back(tok);
        return tokens;
    }

    inline bool isNumberString(const std::string &s) {
        if (s.empty()) return false;
        size_t i = 0;
        bool dot = false;
        if (s[0] == '-' ) i = 1;
        if (i >= s.size()) return false;
        for (; i < s.size(); i++) {
            if (s[i] == '.') {
                if (dot) return false;
                dot = true;
            } else if (!isdigit((unsigned char)s[i])) {
                return false;
            }
        }
        return true;
    }
}

#endif // UTILS_H
