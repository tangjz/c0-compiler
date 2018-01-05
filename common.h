#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <bits/stdc++.h>
using namespace std;

extern FILE *fin, *fout, *ferr;

#define TOKEN_MAX   255 // limit for string
#define BUF_SIZE    2047 // limit for each line
#define ERROR_MAX   101
#define TABLE_SIZE  65535
#define ARRAY_MAX   1048575
#define OPER_MAX    31 // limit for operator length
#define CODE_MAX    65535
#define REG_MAX     32
#define OFFSET_MAX  32767

#define BYTE_PER_INT    4
#define BYTE_PER_CHAR   1

// switches for debugging
// #define SCANNER_DEBUG // to fout
// #define SYNTAX_DEBUG // to ferr
// #define SEMANTIC_DEBUG // to fout
// #define FOURCODE_DEBUG // to ferr
#define TRANSFORM_DEBUG // to fout

// switches for MARS
// MARS -> Settings -> Popup dialog for input syscalls: ON
// MARS -> Settings -> Delayed branching: OFF
// MARS -> Settings -> Memory Configuration...: Default
#define OUTPUT_EOLN

// switches for optimization
//#define COMMON_SUBEXPRESSION_ELIMINATION // compulsory
//#define CONSTANT_FOLDING_PROPAGAION
//#define VARIABLE_COPY_PROPAGAION // compulsory
//#define PEEPHOLE_OPTIMIZATION
//#define DEAD_CODE_ELIMINATION
//#define GLOBAL_REGISTER_ALLOCATION // compulsory

#endif // COMMON_H_INCLUDED
