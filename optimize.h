#ifndef OPTIMIZE_H_INCLUDED
#define OPTIMIZE_H_INCLUDED

#include "common.h"
#include "table.h"
#include "pseudo.h"

extern void divideBlocks(); // with dead code elimination
extern void commonSubexpressionElimination(); // with constant folding propagation
extern void activeVariableAnalysis(); // just initialization
extern bool hasConfliction(int index1, int index2); // check for global/local variables (but not temporary)

//TODO: global register allocation should be implemented in transform.cpp
//TODO: peephole optimization also ...

#endif // OPTIMIZE_H_INCLUDED
