#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <bits/stdc++.h>
using namespace std;

extern FILE *fin, *fout, *ferr;

const int TOKEN_MAX = 255; // limit for string
const int BUF_SIZE = 2047; // limit for each line
const int ERROR_MAX = 101;
const int TABLE_SIZE = 65535;
const int ARRAY_MAX = 1048575;
const int OPER_MAX = 31; // limit for operator length
const int CODE_MAX = 65535;
const int REG_MAX = 32;
const int OFFSET_MAX = 32767;
const int BLOCK_MAX = 2047; // limit for code blocks (depend on call/jump/branch)

const int BYTE_PER_INT = 4;
const int BYTE_PER_CHAR = 1;

// switches for debugging
#define ERROR_DEBUG // to ferr
// #define SCANNER_DEBUG // to fout
// #define SYNTAX_DEBUG // to ferr
// #define SEMANTIC_DEBUG // to fout
// #define QUADCODE_DEBUG // to ferr
#define QUADCODE_OUTPUT // to ferr
#define TRANSFORM_DEBUG // to fout
#define OPTIMIZE_DEBUG // to ferr

// switches for MARS
// MARS -> Settings -> Popup dialog for input syscalls: ON
// MARS -> Settings -> Delayed branching: OFF
// MARS -> Settings -> Memory Configuration...: Default
#define OUTPUT_EOLN
#define CONSTANT_EXPRESS

// switches for optimization
// #define OPTIMIZATION
#ifdef OPTIMIZATION
    #define COMMON_SUBEXPRESSION_ELIMINATION // compulsory
    #define CONSTANT_FOLDING_PROPAGAION
    #define PEEPHOLE_OPTIMIZATION
    #define DEAD_CODE_ELIMINATION
    #define GLOBAL_REGISTER_ALLOCATION // compulsory
#endif

#endif // COMMON_H_INCLUDED
