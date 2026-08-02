# Mini C Compiler — Compiler Lab Project

A complete, modular Mini C Compiler written in C++ that performs all classic
front-end and back-end compiler phases on a sample C source file and a
hand-written context-free grammar, printing (and saving to `output.txt`)
a full trace of every stage.

## Phases implemented

| Stage | Phase                                   | Module(s)                        |
|------:|------------------------------------------|-----------------------------------|
| 1     | Comment Removal                          | `comment_removal.h/.cpp`          |
| 2     | Lexical Analysis (Tokenizer)             | `tokenizer.h/.cpp`                |
| 3     | Grammar Processing                       | `grammar.h/.cpp`                  |
| 4     | Left Recursion Removal                   | `left_recursion.h/.cpp`           |
| 5     | FIRST Set Computation                    | `first_follow.h/.cpp`             |
| 6     | FOLLOW Set Computation                   | `first_follow.h/.cpp`             |
| 7     | LL(1) Parsing Table Construction         | `parsing_table.h/.cpp`            |
| 8     | Predictive (Table-Driven) Parsing        | `parser.h/.cpp`                   |
| 9     | Three Address Code (TAC) Generation      | `tac.h/.cpp`                      |
| 10    | Code Optimization                        | `optimizer.h/.cpp`                |
| 11    | Assembly Code Generation                 | `assembly.h/.cpp`                 |

`main.cpp` wires all eleven stages together in sequence. `utils.h` provides
a small `Logger` that duplicates every printed line to both the console and
`output.txt`, so a permanent record of each run is always available.

## Project files

```
main.cpp
utils.h
comment_removal.h / comment_removal.cpp
tokenizer.h       / tokenizer.cpp
grammar.h         / grammar.cpp
left_recursion.h  / left_recursion.cpp
first_follow.h    / first_follow.cpp
parsing_table.h   / parsing_table.cpp
parser.h          / parser.cpp
tac.h             / tac.cpp
optimizer.h       / optimizer.cpp
assembly.h        / assembly.cpp
input.c              <- sample C source (Phase 1 & 2 input)
grammar.txt          <- sample CFG, deliberately left-recursive (Phase 3 & 4 input)
output.txt           <- sample output from a full run (regenerated every run)
.vscode/tasks.json    <- one-click build task for VS Code
.vscode/launch.json   <- one-click debug config for VS Code
```

## Building and running in Visual Studio Code (Windows, g++)

### Prerequisites
- [Visual Studio Code](https://code.visualstudio.com/)
- A working **MinGW-w64 g++** toolchain on your `PATH` (e.g. via
  [MSYS2](https://www.msys2.org/) or the
  [MinGW-w64 installer](https://www.mingw-w64.org/)). Verify with:
  ```
  g++ --version
  ```
- The C/C++ extension for VS Code (`ms-vscode.cpptools`) is recommended but
  not required to simply build and run.

### Option A — One-click build task (recommended)
1. Open this folder in VS Code (`File > Open Folder...`).
2. Press `Ctrl+Shift+B` to run the default build task
   ("Build Mini C Compiler (g++)"). This compiles every `.cpp` file into
   `minicc.exe` in the project root.
3. Open a terminal in VS Code (`` Ctrl+` ``) and run:
   ```
   .\minicc.exe
   ```
4. Watch the 11 stages print to the terminal, and check the freshly
   generated `output.txt` for the same content saved to disk.

### Option B — Manual command line
From a terminal inside the project folder:
```
g++ -std=c++17 -Wall -o minicc.exe main.cpp comment_removal.cpp tokenizer.cpp grammar.cpp left_recursion.cpp first_follow.cpp parsing_table.cpp parser.cpp tac.cpp optimizer.cpp assembly.cpp
minicc.exe
```

### Debugging
Press `F5` (with the C/C++ extension installed) to build and launch under
`gdb` using the provided `launch.json`.

## Customizing the inputs

- **`input.c`** — replace with any C source file to see Phases 1–2 tokenize
  different code. Keep the filename `input.c`, or update the path passed to
  `CommentRemoval::readFile(...)` in `main.cpp`.
- **`grammar.txt`** — replace with any context-free grammar in the format:
  ```
  LHS -> SYM1 SYM2 SYM3
  LHS -> SYM4
  ```
  One production per line (or `|`-separated alternatives on one line).
  Use the literal token `EPSILON` for empty productions. The first
  non-terminal that appears becomes the start symbol. Left recursion,
  FIRST/FOLLOW, and the LL(1) table are all computed automatically —
  nothing about the grammar's symbols is hard-coded in the compiler.

## Notes on design choices

- **Left recursion removal (Phase 4)** implements the standard textbook
  transformation (`A -> Aα | β` ⟹ `A -> βA'`, `A' -> αA' | ε`) generically
  over whatever grammar is loaded — it is not special-cased to the shipped
  `grammar.txt`.
- **FIRST/FOLLOW/parsing table (Phases 5–7)** are computed on the
  *left-recursion-free* grammar produced by Phase 4, which is the version
  actually usable for LL(1) parsing.
- **Predictive parsing (Phase 8)** demonstrates both a successful parse
  (`a = ( b + c ) * d` → Accepted) and a deliberately malformed input
  (`a = ( b + )` → Syntax Error) so both outcomes are visible in one run.
- **TAC/Optimization/Assembly (Phases 9–11)** use a small, independent
  expression parser (precedence-climbing over `+ - * /` and parentheses)
  purpose-built for generating three-address code from arithmetic
  assignment statements — this keeps the LL(1) demonstration in Phases
  5–8 general-purpose while keeping code generation straightforward.
- **Code Optimization (Phase 10)** runs four passes in order: constant
  folding, copy propagation, common subexpression elimination, and dead
  code elimination. `main.cpp` prints the TAC after *each* individual pass
  on a hand-built example so every optimization's effect is visible on its
  own, in addition to running the full pipeline on the Phase 9 output.

## Regenerating `output.txt`

`output.txt` is overwritten every time `minicc.exe` is run — it is not a
static file you need to edit by hand. The copy shipped in this project is
simply a saved sample from one run, useful for reports/screenshots.
