// ============================================================
// comment_removal.h
// Phase 1: Comment Removal
// Removes // single-line comments and /* ... */ multi-line
// comments from raw C source, while being careful NOT to touch
// characters that merely LOOK like comment markers when they
// occur inside string literals ("...") or character literals
// ('...').
// ============================================================
#ifndef COMMENT_REMOVAL_H
#define COMMENT_REMOVAL_H

#include <string>

namespace CommentRemoval {

    // Reads the whole content of a file into a single string.
    // Throws std::runtime_error if the file cannot be opened.
    std::string readFile(const std::string &filename);

    // Returns a copy of `source` with all comments stripped out,
    // respecting string/character literal boundaries.
    std::string removeComments(const std::string &source);

}

#endif // COMMENT_REMOVAL_H
