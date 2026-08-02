// ============================================================
// left_recursion.h
// Phase 4: Left Recursion Removal
// Automatically detects immediate left recursion of the form
//      A -> A alpha | beta
// and rewrites it, using the standard transformation, into
//      A  -> beta A'
//      A' -> alpha A' | EPSILON
// The algorithm works generically on ANY grammar loaded into a
// Grammar object -- nothing is hard-coded for a specific input.
// ============================================================
#ifndef LEFT_RECURSION_H
#define LEFT_RECURSION_H

#include "grammar.h"

namespace LeftRecursion {

    // Returns a NEW grammar equivalent to `g` but with all immediate
    // left recursion eliminated. The original grammar is left untouched.
    Grammar removeLeftRecursion(const Grammar &g);

}

#endif // LEFT_RECURSION_H
