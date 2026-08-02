// ============================================================
// comment_removal.cpp
// Implementation of Phase 1 : Comment Removal
// ============================================================
#include "comment_removal.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace CommentRemoval {

std::string readFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// A small state machine walks the source character by character.
// States:
//   NORMAL        - ordinary code
//   IN_STRING     - inside "..."
//   IN_CHAR       - inside '...'
//   IN_LINE_COMM  - inside // ...  (until newline)
//   IN_BLOCK_COMM - inside /* ... */
std::string removeComments(const std::string &source) {
    enum class State { NORMAL, IN_STRING, IN_CHAR, IN_LINE_COMM, IN_BLOCK_COMM };
    State state = State::NORMAL;

    std::string result;
    result.reserve(source.size());

    size_t n = source.size();
    for (size_t i = 0; i < n; i++) {
        char c = source[i];
        char next = (i + 1 < n) ? source[i + 1] : '\0';

        switch (state) {

            case State::NORMAL:
                if (c == '/' && next == '/') {
                    state = State::IN_LINE_COMM;
                    i++; // skip the second '/'
                } else if (c == '/' && next == '*') {
                    state = State::IN_BLOCK_COMM;
                    i++; // skip the '*'
                } else if (c == '"') {
                    state = State::IN_STRING;
                    result += c;
                } else if (c == '\'') {
                    state = State::IN_CHAR;
                    result += c;
                } else {
                    result += c;
                }
                break;

            case State::IN_STRING:
                result += c;
                if (c == '\\' && i + 1 < n) {
                    // Escaped character inside string, e.g. \" or \\ -
                    // copy the following character verbatim too so we
                    // don't mistake an escaped quote for the closing quote.
                    result += next;
                    i++;
                } else if (c == '"') {
                    state = State::NORMAL;
                }
                break;

            case State::IN_CHAR:
                result += c;
                if (c == '\\' && i + 1 < n) {
                    result += next;
                    i++;
                } else if (c == '\'') {
                    state = State::NORMAL;
                }
                break;

            case State::IN_LINE_COMM:
                // Everything is discarded until the newline; the newline
                // itself is preserved so line numbers in the remaining
                // source stay meaningful.
                if (c == '\n') {
                    result += c;
                    state = State::NORMAL;
                }
                break;

            case State::IN_BLOCK_COMM:
                if (c == '\n') {
                    // Keep line breaks so later phases still see roughly
                    // the same line structure as the original file.
                    result += c;
                } else if (c == '*' && next == '/') {
                    state = State::NORMAL;
                    i++; // skip the '/'
                }
                break;
        }
    }

    return result;
}

} // namespace CommentRemoval
