#ifndef FOUR_H_INCLUDED
#define FOUR_H_INCLUDED

#include "common.h"
#include "table.h"
#include "scanner.h"

struct QUADCODE {
    char op[OPER_MAX], lft[TOKEN_MAX], rht[TOKEN_MAX], dst[TOKEN_MAX];
    // lft, rht may be identifier or number, dst must be identifier
};

extern int codeCount, labelCount;
extern QUADCODE codeList[CODE_MAX];
extern char labelTable[TABLE_SIZE][TOKEN_MAX];

extern int findLabel(char token[TOKEN_MAX]);
extern int findFunctionLabel(int functionIndex, int order);
extern int generateLabel(SYMBOL type, int order, int typeCount, int superLabel = -1);
extern int generateLabel(int functionIndex = -1, int order = 1);

extern int arithmeticOpeation(SYMBOL oper, int leftIndex, int rightIndex, int destinationIndex); // operator, left, right, destination
extern int defineElement(int index); // def kind, type, value, name
extern int setLabel(int index); // label, labelString, ,
extern int jumpLabel(int index); // j (or jal), , , labelString
extern int branchLabel(SYMBOL oper, int leftIndex, int rightIndex, int falseLabelIndex); // branchOperator, left, right, falseLabel
extern int startOfFunction(int functionIndex); // function, name, type,
extern int endOfFunction(int functionIndex); // endFunction, name, type,
extern int sysCall(int type, int src = -1, int dst = -1); // syscall, serviceNumber, source, destination
extern int userCall(int calleeIndex, int returnIndex = -1); // call, function, temporaryCount, result
extern int userReturn(int returnIndex = -1); // return, result, ,
extern int storeImmediate(int index, int value); // =, value, , variable
extern int storeArrayElement(int arrayIndex, int offsetIndex, int sourceIndex); // []=, source, offset, array
extern int loadArrayElement(int arrayIndex, int offsetIndex, int destinationIndex); // =[], array, offset, destination
extern int pushParameter(int index); // pushPara, (type), name,
extern void printCodeList();

#endif // FOUR_H_INCLUDED
