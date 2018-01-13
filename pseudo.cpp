#include "pseudo.h"

//struct QUADCODE {
//    char op[OPER_MAX], lft[TOKEN_MAX], rht[TOKEN_MAX], dst[TOKEN_MAX];
//    // lft, rht may be identifier or number, dst must be identifier
//};

int codeCount = 0, labelCount = 0;
QUADCODE codeList[CODE_MAX];
char labelTable[TABLE_SIZE][TOKEN_MAX];

bool checkLabelLimit() {
    if(labelCount == TABLE_SIZE) {
        puts("Label table is full.");
        error(0);
        return false;
    }
    return true;
}
bool checkCodeLimit() {
    if(codeCount == CODE_MAX) {
        puts("Code list is full.");
        error(0);
        return false;
    }
    return true;
}

int findLabel(char token[TOKEN_MAX]) {
    int index;
    for(index = labelCount - 1; index >= 0; --index)
        if(strcmp(token, labelTable[index]) == 0)
            break;
    return index;
}
int findFunctionLabel(int functionIndex, int order) {
    static char labelToken[TOKEN_MAX];
    assert(functionIndex >= 0 && functionIndex < symbolCount && symbolTable[functionIndex].kind == FUNCTION);
    assert(order >= 1 && order <= 2);
    sprintf(labelToken, "%s@%s", symbolTable[functionIndex].name, order == 1 ? "startFunction" : "endFunction");
    return findLabel(labelToken);
}
int generateLabel(SYMBOL type, int order, int typeIndex, int superLabel) {
    if(!checkLabelLimit())
        return -1;
    char *cur = labelTable[labelCount];
    assert(headerIndex >= 0 && headerIndex < symbolCount && symbolTable[headerIndex].kind == FUNCTION);
    switch(type) {
        case IFSY: sprintf(cur, "%s@%s%d", symbolTable[headerIndex].name,
            order == 1 ? "else" : "endIf", typeIndex); break;
        case FORSY: sprintf(cur, "%s@loop%s%d", symbolTable[headerIndex].name,
            order == 1 ? "Condition" :
            order == 2 ? "Iteration" :
            order == 3 ? "Block" : "End", typeIndex); break;
        case SWITCHSY: sprintf(cur, "%s@switch%d", symbolTable[headerIndex].name, typeIndex); break;
        case CASESY: sprintf(cur, "%s@case%d", labelTable[superLabel], typeIndex); break;
        default: assert(false);
    }
    assert(findLabel(cur) == -1);
    return labelCount++;
}
int generateLabel(int functionIndex, int order) {
    if(!checkLabelLimit())
        return -1;
    char *cur = labelTable[labelCount];
    if(functionIndex == -1) { // global
        strcpy(cur, "@globalEnd");
    } else { // function
        assert(functionIndex >= 0 && functionIndex < symbolCount && symbolTable[functionIndex].kind == FUNCTION);
        assert(order >= 1 && order <= 2);
        sprintf(cur, "%s@%s", symbolTable[functionIndex].name, order == 1 ? "startFunction" : "endFunction");
    }
    assert(findLabel(cur) == -1);
    return labelCount++;
}

int arithmeticOpeation(SYMBOL oper, int leftIndex, int rightIndex, int destinationIndex) { // operator, left, right, destination
    if(!checkCodeLimit())
        return -1;
    QUADCODE &cur = codeList[codeCount];
    switch(oper) {
        case ASSIGNSY: strcpy(cur.op, "="); break;
        case PLUSSY: strcpy(cur.op, "add"); break;
        case MINUSSY: strcpy(cur.op, "sub"); break;
        case MULTISY: strcpy(cur.op, "mult"); break;
        case DIVISY: strcpy(cur.op, "div"); break;
        default: assert(false);
    }
    if(oper == MINUSSY && leftIndex == -1) { // self-negative
        strcpy(cur.lft, "0");
    } else {
        assert(leftIndex >= 0 && leftIndex < symbolCount && hasValue(leftIndex));
//        if(symbolTable[leftIndex].kind == CONST) {
//            sprintf(cur.lft, "%d", symbolTable[leftIndex].value);
//        } else {
            strcpy(cur.lft, symbolTable[leftIndex].name);
//        }
    }
    if(oper != ASSIGNSY) {
        assert(rightIndex >= 0 && rightIndex < symbolCount && hasValue(rightIndex));
//        if(symbolTable[rightIndex].kind == CONST) {
//            sprintf(cur.rht, "%d", symbolTable[rightIndex].value);
//        } else {
            strcpy(cur.rht, symbolTable[rightIndex].name);
//        }
    } else {
        strcpy(cur.rht, "");
    }
    assert(destinationIndex >= 0 && destinationIndex < symbolCount && isVariable(destinationIndex));
    strcpy(cur.dst, symbolTable[destinationIndex].name);
#ifdef FOURCODE_DEBUG
    fprintf(ferr, "#%d: %s, %s, %s, %s\n", codeCount, cur.op, cur.lft, cur.rht, cur.dst);
#endif
    return codeCount++;
}

int defineElement(int index) { // def kind, type, value, name
    if(!checkCodeLimit())
        return -1;
    QUADCODE &cur = codeList[codeCount];
    assert(index >= 0 && index < symbolCount && symbolTable[index].kind != FUNCTION);
    switch(symbolTable[index].kind) {
        case CONST: strcpy(cur.op, "defConstant"); break;
        case VARIABLE: strcpy(cur.op, "defVariable"); break;
        case PARAMETER: strcpy(cur.op, "defParameterVariable"); break;
        case TEMPORARY: strcpy(cur.op, "defTemporaryVariable"); break;
        case ARRAY: strcpy(cur.op, "defArrayVariable"); break;
        default: assert(false);
    }
    strcpy(cur.lft, symbolTable[index].type == VOID ? "void" : (symbolTable[index].type == INT ? "int" : "char"));
    if(isVariable(index)) {
        strcpy(cur.rht, "");
    } else {
        sprintf(cur.rht, "%d", symbolTable[index].value);
    }
    strcpy(cur.dst, symbolTable[index].name);
#ifdef FOURCODE_DEBUG
    fprintf(ferr, "#%d: %s, %s, %s, %s\n", codeCount, cur.op, cur.lft, cur.rht, cur.dst);
#endif
    return codeCount++;
}

int setLabel(int index) { // label, labelString, ,
    if(!checkCodeLimit())
        return -1;
    QUADCODE &cur = codeList[codeCount];
    strcpy(cur.op, "label");
    assert(index >= 0 && index < labelCount);
    strcpy(cur.lft, labelTable[index]);
    strcpy(cur.rht, ""); // optional
    strcpy(cur.dst, ""); // optional
#ifdef FOURCODE_DEBUG
    fprintf(ferr, "#%d: %s, %s, %s, %s\n", codeCount, cur.op, cur.lft, cur.rht, cur.dst);
#endif
    return codeCount++;
}

int jumpLabel(int index) { // j, , , labelString
    if(!checkCodeLimit())
        return -1;
    QUADCODE &cur = codeList[codeCount];
    strcpy(cur.op, "j");
    strcpy(cur.lft, ""); // optional
    strcpy(cur.rht, ""); // optional
    assert(index >= 0 && index < labelCount);
    strcpy(cur.dst, labelTable[index]);
#ifdef FOURCODE_DEBUG
    fprintf(ferr, "#%d: %s, %s, %s, %s\n", codeCount, cur.op, cur.lft, cur.rht, cur.dst);
#endif
    return codeCount++;
}

int branchLabel(SYMBOL oper, int leftIndex, int rightIndex, int falseLabelIndex) { // branchOperator, left, right, falseLabel
    if(!checkCodeLimit())
        return -1;
    QUADCODE &cur = codeList[codeCount];
    switch(oper) {
        case EQSY: strcpy(cur.op, "bne"); break;
        case NEQSY: strcpy(cur.op, "beq"); break;
        case LESSY: strcpy(cur.op, "bge"); break;
        case LEQSY: strcpy(cur.op, "bgt"); break;
        case GRESY: strcpy(cur.op, "ble"); break;
        case GEQSY: strcpy(cur.op, "blt"); break;
        default: assert(oper == NOTSY); strcpy(cur.op, "beq");
    }
    assert(leftIndex >= 0 && leftIndex < symbolCount && hasValue(leftIndex));
//    if(symbolTable[leftIndex].kind == CONST) {
//        sprintf(cur.lft, "%d", symbolTable[leftIndex].value);
//    } else {
        strcpy(cur.lft, symbolTable[leftIndex].name);
//    }
    if(oper != NOTSY) {
        assert(rightIndex >= 0 && rightIndex < symbolCount && hasValue(rightIndex));
//        if(symbolTable[rightIndex].kind == CONST) {
//            sprintf(cur.rht, "%d", symbolTable[rightIndex].value);
//        } else {
            strcpy(cur.rht, symbolTable[rightIndex].name);
//        }
    } else {
        strcpy(cur.rht, "0");
    }
    assert(falseLabelIndex >= 0 && falseLabelIndex < labelCount);
    strcpy(cur.dst, labelTable[falseLabelIndex]);
#ifdef FOURCODE_DEBUG
    fprintf(ferr, "#%d: %s, %s, %s, %s\n", codeCount, cur.op, cur.lft, cur.rht, cur.dst);
#endif
    return codeCount++;
}

int startOfFunction(int functionIndex) { // function, name, type,
    if(!checkCodeLimit())
        return -1;
    QUADCODE &cur = codeList[codeCount];
    strcpy(cur.op, "function");
    assert(functionIndex >= 0 && functionIndex < symbolCount && symbolTable[functionIndex].kind == FUNCTION);
    strcpy(cur.lft, symbolTable[functionIndex].name);
    strcpy(cur.rht, symbolTable[functionIndex].type == VOID ? "void" : (symbolTable[functionIndex].type == INT ? "int" : "char"));
    strcpy(cur.dst, ""); // optional
#ifdef FOURCODE_DEBUG
    fprintf(ferr, "#%d: %s, %s, %s, %s\n", codeCount, cur.op, cur.lft, cur.rht, cur.dst);
#endif
    return codeCount++;
}

int endOfFunction(int functionIndex) { // endFunction, name, type,
    if(!checkCodeLimit())
        return -1;
    QUADCODE &cur = codeList[codeCount];
    strcpy(cur.op, "endFunction");
    assert(functionIndex >= 0 && functionIndex < symbolCount && symbolTable[functionIndex].kind == FUNCTION);
    strcpy(cur.lft, symbolTable[functionIndex].name);
    strcpy(cur.rht, symbolTable[functionIndex].type == VOID ? "void" : (symbolTable[functionIndex].type == INT ? "int" : "char"));
    strcpy(cur.dst, ""); // optional
#ifdef FOURCODE_DEBUG
    fprintf(ferr, "#%d: %s, %s, %s, %s\n", codeCount, cur.op, cur.lft, cur.rht, cur.dst);
#endif
    return codeCount++;
}

int sysCall(int type, int src, int dst) { // syscall, serviceNumber, source, destination
    if(!checkCodeLimit())
        return -1;
    QUADCODE &cur = codeList[codeCount];
    strcpy(cur.op, "syscall");
    sprintf(cur.lft, "%d", type);
    if(type == 4) { // string
        assert(symbol == STRSY);
        strcpy(cur.rht, token);
    } else if(type == 1 || type == 11) { // output
        if(type == 11 && src == -1) {
            sprintf(cur.rht, "%d", '\n');
        } else {
            assert(src >= 0 && src < symbolCount);
            strcpy(cur.rht, symbolTable[src].name);
        }
    } else if(type == 5 || type == 12) { // input
        assert(dst >= 0 && dst < symbolCount);
        strcpy(cur.dst, symbolTable[dst].name);
    } else {
        assert(false);
    }
#ifdef FOURCODE_DEBUG
    if(type == 4) {
        fprintf(ferr, "#%d: %s, %s, \"", codeCount, cur.op, cur.lft);
        for(int i = 0; cur.rht[i]; ++i) {
            if(cur.rht[i] == '\\') {
                fputc('\\', ferr);
            }
            fputc(cur.rht[i], ferr);
        }
        fprintf(ferr, "\", %s\n", cur.dst);
    } else {
        fprintf(ferr, "#%d: %s, %s, %s, %s\n", codeCount, cur.op, cur.lft, cur.rht, cur.dst);
    }
#endif
    return codeCount++;
}

int userCall(int calleeIndex, int returnIndex) { // call, function, temporaryCount, result
    if(!checkCodeLimit())
        return -1;
    QUADCODE &cur = codeList[codeCount];
    strcpy(cur.op, "call");
    assert(calleeIndex >= 0 && calleeIndex < symbolCount && symbolTable[calleeIndex].kind == FUNCTION);
    strcpy(cur.lft, symbolTable[calleeIndex].name);
    sprintf(cur.rht, "%d", temporaryCount);
    if(returnIndex >= 0) {
        assert(returnIndex < symbolCount && isVariable(returnIndex) && symbolTable[returnIndex].type == symbolTable[calleeIndex].type);
        strcpy(cur.dst, symbolTable[returnIndex].name);
    } else {
        strcpy(cur.dst, "");
    }
#ifdef FOURCODE_DEBUG
    fprintf(ferr, "#%d: %s, %s, %s, %s\n", codeCount, cur.op, cur.lft, cur.rht, cur.dst);
#endif
    return codeCount++;
}

int userReturn(int returnIndex) { // return, result, ,
    if(!checkCodeLimit())
        return -1;
    QUADCODE &cur = codeList[codeCount];
    strcpy(cur.op, "return");
    if(returnIndex >= 0) {
        assert(returnIndex < symbolCount && isVariable(returnIndex));
        strcpy(cur.lft, symbolTable[returnIndex].name);
    } else {
        strcpy(cur.lft, "");
    }
    strcpy(cur.rht, ""); // optional
    strcpy(cur.dst, ""); // optional
#ifdef FOURCODE_DEBUG
    fprintf(ferr, "#%d: %s, %s, %s, %s\n", codeCount, cur.op, cur.lft, cur.rht, cur.dst);
#endif
    return codeCount++;
}

int storeImmediate(int index, int value) { // =v, value, , variable
    if(!checkCodeLimit())
        return -1;
    QUADCODE &cur = codeList[codeCount];
    strcpy(cur.op, "=");
    sprintf(cur.lft, "%d", value);
    assert(index >= 0 && index < symbolCount && isVariable(index));
    strcpy(cur.dst, symbolTable[index].name);
#ifdef FOURCODE_DEBUG
    fprintf(ferr, "#%d: %s, %s, %s, %s\n", codeCount, cur.op, cur.lft, cur.rht, cur.dst);
#endif
    return codeCount++;
}

int storeArrayElement(int arrayIndex, int offsetIndex, int sourceIndex) { // []=, source, offset, array
    if(!checkCodeLimit())
        return -1;
    QUADCODE &cur = codeList[codeCount];
    strcpy(cur.op, "[]=");
    assert(sourceIndex >= 0 && sourceIndex < symbolCount && hasValue(sourceIndex));
//    if(symbolTable[sourceIndex].kind == CONST) {
//        sprintf(cur.lft, "%d", symbolTable[sourceIndex].value);
//    } else {
        strcpy(cur.lft, symbolTable[sourceIndex].name);
//    }
    assert(offsetIndex >= 0 && offsetIndex < symbolCount && hasValue(offsetIndex));
//    if(symbolTable[offsetIndex].kind == CONST) {
//        sprintf(cur.rht, "%d", symbolTable[offsetIndex].value);
//    } else {
        strcpy(cur.rht, symbolTable[offsetIndex].name);
//    }
    assert(arrayIndex >= 0 && arrayIndex < symbolCount && symbolTable[arrayIndex].kind == ARRAY);
    strcpy(cur.dst, symbolTable[arrayIndex].name);
#ifdef FOURCODE_DEBUG
    fprintf(ferr, "#%d: %s, %s, %s, %s\n", codeCount, cur.op, cur.lft, cur.rht, cur.dst);
#endif
    return codeCount++;
}

int loadArrayElement(int arrayIndex, int offsetIndex, int destinationIndex) { // =[], array, offset, destination
    if(!checkCodeLimit())
        return -1;
    QUADCODE &cur = codeList[codeCount];
    strcpy(cur.op, "=[]");
    assert(arrayIndex >= 0 && arrayIndex < symbolCount && symbolTable[arrayIndex].kind == ARRAY);
    strcpy(cur.lft, symbolTable[arrayIndex].name);
    assert(offsetIndex >= 0 && offsetIndex < symbolCount && hasValue(offsetIndex));
//    if(symbolTable[offsetIndex].kind == CONST) {
//        sprintf(cur.rht, "%d", symbolTable[offsetIndex].value);
//    } else {
        strcpy(cur.rht, symbolTable[offsetIndex].name);
//    }
    assert(destinationIndex >= 0 && destinationIndex < symbolCount && isVariable(destinationIndex));
    strcpy(cur.dst, symbolTable[destinationIndex].name);
#ifdef FOURCODE_DEBUG
    fprintf(ferr, "#%d: %s, %s, %s, %s\n", codeCount, cur.op, cur.lft, cur.rht, cur.dst);
#endif
    return codeCount++;
}

int pushParameter(int index) { // pushPara, (type), name,
    if(!checkCodeLimit())
        return -1;
    QUADCODE &cur = codeList[codeCount];
    strcpy(cur.op, "pushPara");
    assert(index >= 0 && index < symbolCount && hasValue(index));
    strcpy(cur.lft, symbolTable[index].type == INT ? "int" : "char"); // optional
//    if(symbolTable[index].kind == CONST) {
//        sprintf(cur.rht, "%d", symbolTable[index].value);
//    } else {
        strcpy(cur.rht, symbolTable[index].name);
//    }
    strcpy(cur.dst, ""); // optional
#ifdef FOURCODE_DEBUG
    fprintf(ferr, "#%d: %s, %s, %s, %s\n", codeCount, cur.op, cur.lft, cur.rht, cur.dst);
#endif
    return codeCount++;
}

void printCodeList() {
    for(int index = 0; index < codeCount; ++index) {
        QUADCODE &cur = codeList[index];
        if(strcmp(cur.op, "syscall") == 0 && strcmp(cur.lft, "4") == 0) {
            fprintf(fout, "#%d: %s, %s, \"", index, cur.op, cur.lft);
            for(char *ptr = cur.rht; *ptr; ++ptr) {
                if(*ptr == '\\')
                    fputc('\\', fout);
                fputc(*ptr, fout);
            }
            fprintf(fout, "\", %s\n", cur.dst);
        } else {
            fprintf(fout, "#%d: %s, %s, %s, %s\n", index, cur.op, cur.lft, cur.rht, cur.dst);
        }
    }
}
