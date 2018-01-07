#include "table.h"

//enum KIND {CONST, VARIABLE, ARRAY, FUNCTION, PARAMETER, TEMPORARY};
//enum TYPE {VOID, INT, CHAR};
//struct SYMBOLTABLE {
//    char name[TOKEN_MAX];
//    enum KIND kind; // const, var, arr, funct, para, temp
//    enum TYPE type;
//    bool isGlobal; // only two level: global, function
//    bool isHidden; // only be hidden from finding but for parameter checking
//    int value; // const: int/char(value); array: (elementSize); function: (parameterCount);
//    int storeSize; // 4 bytes per int, 1 byte per int
//} ;

int symbolCount = 0, lastSymbolIndex = -1;
int headerIndex = -1, temporaryBaseIndex = -1, temporaryCount = 0, temporaryLimit = 0;
int checkingIndex = -1;
bool checkingResult = false;
SYMBOLTABLE symbolTable[TABLE_SIZE];

int findSymbol(const char *name) {
    int index;
    for(index = lastSymbolIndex; index >= 0; index = symbolTable[index].preIndex)
        if(strcmp(name, symbolTable[index].name) == 0)
            break;
    checkingResult |= index == checkingIndex;
    return index; // real index or -1
}
void recalculateStoreSize(int index) {
    SYMBOLTABLE &cur = symbolTable[index];
    if(cur.kind == CONST || cur.kind == FUNCTION || cur.type == VOID) {
        cur.storeSize = 0;
    } else if(cur.kind == ARRAY) {
        if(cur.type == INT) {
            cur.storeSize = cur.value * BYTE_PER_INT;
        } else { // cur.type == CHAR
            cur.storeSize = cur.value * BYTE_PER_CHAR;
        }
    } else if(cur.type == INT) {
        cur.storeSize = BYTE_PER_INT;
    } else { // cur.type == CHAR
        cur.storeSize = BYTE_PER_CHAR;
    }
    // address alignment
    int remain = cur.storeSize % BYTE_PER_INT;
    if(remain != 0)
        cur.storeSize += BYTE_PER_INT - remain;
}
int insertSymbol(const char *name, KIND kind, TYPE type, bool isGlobal, int value) {
    int realIndex = findSymbol(name);
    assert(realIndex == -1 || (symbolTable[realIndex].isGlobal && !isGlobal));
    if(symbolCount >= TABLE_SIZE) { // fatal error
        puts("Symbol table is full.");
        error(0);
        return -1;
    }
    SYMBOLTABLE &cur = symbolTable[symbolCount];
    strcpy(cur.name, name);
    cur.kind = kind;
    cur.type = type;
    cur.isGlobal = isGlobal;
    cur.value = value;
    cur.preIndex = lastSymbolIndex;
    recalculateStoreSize(symbolCount);
#ifdef SEMANTIC_DEBUG
    fprintf(fout, "insert symbol #%d: \"%s\", %s, %s, %s, %d, #%d, %d byte(s)\n", symbolCount, cur.name,
        cur.kind == CONST ? "constant" :
        cur.kind == VARIABLE ? "variable" :
        cur.kind == ARRAY ? "array" :
        cur.kind == FUNCTION ? "function" :
        cur.kind == PARAMETER ? "parameter variable" : "temporary variable",
        cur.type == VOID ? "void" :
        cur.type == INT ? "int" : "char",
        cur.isGlobal ? "global" : "local", cur.value, cur.preIndex, cur.storeSize);
#endif
    return lastSymbolIndex = symbolCount++;
}
int generateTemporarySymbol(TYPE type) {
    static char temporaryToken[TOKEN_MAX];
    if(temporaryCount == temporaryLimit) {
        sprintf(temporaryToken, "%s@T%d", symbolTable[headerIndex].name, ++temporaryLimit);
        int index = insertSymbol(temporaryToken, TEMPORARY, type, false);
        defineElement(index);
    } // now, temporaryCount < temporaryLimit
    ++temporaryCount;
    int index = temporaryBaseIndex + temporaryCount;
    symbolTable[index].type = type;
#ifdef SEMANTIC_DEBUG
    fprintf(fout, "%s start to be used.\n", symbolTable[temporaryBaseIndex + temporaryCount].name);
#endif
    return index;
}
int generateTemporarySymbol(TYPE type, int value) {
    static char temporaryToken[TOKEN_MAX];
    if(temporaryCount == temporaryLimit) {
        sprintf(temporaryToken, "%s@T%d", symbolTable[headerIndex].name, ++temporaryLimit);
        int index = insertSymbol(temporaryToken, TEMPORARY, type, false, value);
        defineElement(index);
    } // now, temporaryCount < temporaryLimit
    ++temporaryCount;
    int index = temporaryBaseIndex + temporaryCount;
    storeImmediate(index, value);
    symbolTable[index].type = type;
#ifdef SEMANTIC_DEBUG
    fprintf(fout, "%s start to be used.\n", symbolTable[temporaryBaseIndex + temporaryCount].name);
#endif
    return index;
}
void revokeTemporarySymbol(int index) {
    if(index == -1 || symbolTable[index].kind != TEMPORARY) {
        return;
    }
#ifdef SEMANTIC_DEBUG
    fprintf(fout, "%s became old.\n", symbolTable[temporaryBaseIndex + temporaryCount].name);
#endif
    assert(index == temporaryBaseIndex + temporaryCount);
    --temporaryCount;
}
void formatterTemporarySymbol(int &index, TYPE type) {
    if(symbolTable[index].kind != TEMPORARY) {
        int index2 = generateTemporarySymbol(type);
//        if(symbolTable[index].kind == CONST) {
//            storeImmediate(index2, symbolTable[index].value);
//        } else {
            arithmeticOpeation(ASSIGNSY, index, -1, index2);
//        }
        index = index2;
    } else if(symbolTable[index].type != type) {
        symbolTable[index].type = type;
        recalculateStoreSize(index);
    }
}
