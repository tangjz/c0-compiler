#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#include "common.h"
#include "error.h"
#include "pseudo.h"

enum KIND {CONST, VARIABLE, ARRAY, FUNCTION, PARAMETER, TEMPORARY};
enum TYPE {VOID, INT, CHAR};
struct SYMBOLTABLE {
    char name[TOKEN_MAX];
    enum KIND kind; // const, var, arr, funct, para, temp
    enum TYPE type;
    bool isGlobal; // only two level: global, function
    int value; // const: int/char(value); array: (elementSize); function: (parameterCount);
    int storeSize; // 4 bytes per int, 1 byte per int, function: (localVariableStoreSize);
    int preIndex; // previous available index
} ;

extern int symbolCount, lastSymbolIndex;
extern int headerIndex, temporaryBaseIndex, temporaryCount, temporaryLimit;
extern int checkingIndex;
extern bool checkingResult;
extern SYMBOLTABLE symbolTable[TABLE_SIZE];

extern int findSymbol(const char *name);
extern int insertSymbol(const char *name, KIND kind = VARIABLE, TYPE type = VOID, bool isGlobal = false, int value = 0);
extern int generateTemporarySymbol(TYPE type);
extern int generateTemporarySymbol(TYPE type, int value); // with immediate
extern void revokeTemporarySymbol(int index);
extern void formatterTemporarySymbol(int &index, TYPE type);

#endif // TABLE_H_INCLUDED
