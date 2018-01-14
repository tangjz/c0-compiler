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
        addError(3);
        return false;
    }
    return true;
}
bool checkCodeLimit() {
    if(codeCount == CODE_MAX) {
        addError(4);
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
#ifdef CONSTANT_EXPRESS
        if(symbolTable[leftIndex].kind == CONST) {
            sprintf(cur.lft, "%d", symbolTable[leftIndex].value);
        } else {
#endif
            strcpy(cur.lft, symbolTable[leftIndex].name);
#ifdef CONSTANT_EXPRESS
        }
#endif
    }
    if(oper != ASSIGNSY) {
        assert(rightIndex >= 0 && rightIndex < symbolCount && hasValue(rightIndex));
#ifdef CONSTANT_EXPRESS
        if(symbolTable[rightIndex].kind == CONST) {
            sprintf(cur.rht, "%d", symbolTable[rightIndex].value);
        } else {
#endif
            strcpy(cur.rht, symbolTable[rightIndex].name);
#ifdef CONSTANT_EXPRESS
        }
#endif
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
#ifdef CONSTANT_EXPRESS
    if(symbolTable[leftIndex].kind == CONST) {
        sprintf(cur.lft, "%d", symbolTable[leftIndex].value);
    } else {
#endif
        strcpy(cur.lft, symbolTable[leftIndex].name);
#ifdef CONSTANT_EXPRESS
    }
#endif
    if(oper != NOTSY) {
        assert(rightIndex >= 0 && rightIndex < symbolCount && hasValue(rightIndex));
#ifdef CONSTANT_EXPRESS
        if(symbolTable[rightIndex].kind == CONST) {
            sprintf(cur.rht, "%d", symbolTable[rightIndex].value);
        } else {
#endif
            strcpy(cur.rht, symbolTable[rightIndex].name);
#ifdef CONSTANT_EXPRESS
        }
#endif
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
#ifdef CONSTANT_EXPRESS
    if(symbolTable[sourceIndex].kind == CONST) {
        sprintf(cur.lft, "%d", symbolTable[sourceIndex].value);
    } else {
#endif
        strcpy(cur.lft, symbolTable[sourceIndex].name);
#ifdef CONSTANT_EXPRESS
    }
#endif
    assert(offsetIndex >= 0 && offsetIndex < symbolCount && hasValue(offsetIndex));
#ifdef CONSTANT_EXPRESS
    if(symbolTable[offsetIndex].kind == CONST) {
        sprintf(cur.rht, "%d", symbolTable[offsetIndex].value);
    } else {
#endif
        strcpy(cur.rht, symbolTable[offsetIndex].name);
#ifdef CONSTANT_EXPRESS
    }
#endif
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
#ifdef CONSTANT_EXPRESS
    if(symbolTable[offsetIndex].kind == CONST) {
        sprintf(cur.rht, "%d", symbolTable[offsetIndex].value);
    } else {
#endif
        strcpy(cur.rht, symbolTable[offsetIndex].name);
#ifdef CONSTANT_EXPRESS
    }
#endif
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
#ifdef CONSTANT_EXPRESS
    if(symbolTable[index].kind == CONST) {
        sprintf(cur.rht, "%d", symbolTable[index].value);
    } else {
#endif
        strcpy(cur.rht, symbolTable[index].name);
#ifdef CONSTANT_EXPRESS
    }
#endif
    strcpy(cur.dst, ""); // optional
#ifdef FOURCODE_DEBUG
    fprintf(ferr, "#%d: %s, %s, %s, %s\n", codeCount, cur.op, cur.lft, cur.rht, cur.dst);
#endif
    return codeCount++;
}

static bool isLetter(char ch) {
	return ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}
void printCodeList() {
    for(int index = 0; index < codeCount; ++index) {
        QUADCODE &cur = codeList[index];
        fprintf(ferr, "#%d: ", index);
        if(strncmp(cur.op, "def", 3) == 0) { // def kind, type, value, name
            switch(cur.op[3]) {
                case 'C': {
#ifndef CONSTANT_EXPRESS
                    switch(cur.lft[0]) {
                        case 'i': {
                            fprintf(ferr, "const %s %s = %s", cur.lft, cur.dst, cur.rht);
                            break;
                        }
                        case 'c': {
                            int value;
                            sscanf(cur.rht, "%d", &value);
                            fprintf(ferr, "const %s %s = '%c'", cur.lft, cur.dst, value);
                            break;
                        }
                        default: assert(false);
                    }
#endif
                    break;
                }
                case 'V': {
                    fprintf(ferr, "var %s %s", cur.lft, cur.dst);
                    break;
                }
                case 'P': {
                    fprintf(ferr, "para %s %s", cur.lft, cur.dst);
                    break;
                }
                case 'T': {
                    fprintf(ferr, "temp %s %s", cur.lft, cur.dst);
                    break;
                }
                case 'A': {
                    fprintf(ferr, "arr %s %s[%s]", cur.lft, cur.dst, cur.rht);
                    break;
                }
                default: assert(false);
            }
        } else if(strcmp(cur.op, "pushPara") == 0) { // pushPara, (type), name,
            if(isLetter(cur.rht[0])) {
                fprintf(ferr, "push %s", cur.rht);
            } else { // constant
                int value;
                sscanf(cur.rht, "%d", &value);
                switch(cur.lft[0]) {
                    case 'i': {
                        fprintf(ferr, "push %d", value);
                        break;
                    }
                    case 'c': {
                        fprintf(ferr, "push '%c'", value);
                        break;
                    }
                    default: assert(false);
                }
            }
        } else if(strcmp(cur.op, "add") == 0 || strcmp(cur.op, "sub") == 0
            || strcmp(cur.op, "mult") == 0 || strcmp(cur.op, "div") == 0) { // add/sub/mult/div, left, right, destination
            switch(cur.op[0]) {
                case 'a': {
                    fprintf(ferr, "%s = %s + %s", cur.dst, cur.lft, cur.rht);
                    break;
                }
                case 's': {
                    fprintf(ferr, "%s = %s - %s", cur.dst, cur.lft, cur.rht);
                    break;
                }
                case 'm': {
                    fprintf(ferr, "%s = %s * %s", cur.dst, cur.lft, cur.rht);
                    break;
                }
                case 'd': {
                    fprintf(ferr, "%s = %s / %s", cur.dst, cur.lft, cur.rht);
                    break;
                }
                default: assert(false);
            }
        } else if(strcmp(cur.op, "=") == 0) { // =, source, , destination
            fprintf(ferr, "%s = %s", cur.dst, cur.lft);
        } else if(strcmp(cur.op, "=[]") == 0) { // =[], array, offset, destination
            fprintf(ferr, "%s = %s[%s]", cur.dst, cur.lft, cur.rht);
        } else if(strcmp(cur.op, "[]=") == 0) { // []=, source, offset, array
            fprintf(ferr, "%s[%s] = %s", cur.dst, cur.rht, cur.lft);
        } else if(strcmp(cur.op, "label") == 0) { // label, labelString, ,
            fprintf(ferr, "%s:", cur.lft);
        } else if(strncmp(cur.op, "j", 1) == 0) { // j, , , labelString
            fprintf(ferr, "GOTO %s", cur.dst);
        } else if(strncmp(cur.op, "b", 1) == 0) { // bne/beq/bge/bgt/ble/blt, left, right, labelString
            int len = 0;
            static char op[3];
            switch(cur.op[1]) {
                case 'n': {
                    op[len++] = '!';
                    break;
                }
                case 'e': {
                    op[len++] = '=';
                    break;
                }
                case 'l': {
                    op[len++] = '<';
                    break;
                }
                case 'g': {
                    op[len++] = '>';
                    break;
                }
                default: assert(false);
            }
            switch(cur.op[2]) {
                case 'e': case 'q': {
                    op[len++] = '=';
                    break;
                }
                case 't': {
                    break;
                }
                default: assert(false);
            }
            op[len] = '\0';
            fprintf(ferr, "IF %s %s %s GOTO %s", cur.lft, op, cur.rht, cur.dst);
        } else if(strcmp(cur.op, "syscall") == 0) { // syscall, serviceNumber, source, destination
            // TODO
            int serviceNumber = 0;
            sscanf(cur.lft, "%d", &serviceNumber);
            switch(serviceNumber) {
                case 1: {
                    fprintf(ferr, "write %s", cur.rht);
                    break;
                }
                case 4: {
                    fprintf(ferr, "write \"%s\"", cur.rht);
                    break;
                }
                case 5: case 12: {
                    fprintf(ferr, "read %s", cur.dst);
                    break;
                }
                case 11: {
                    if(isLetter(cur.rht[0])) {
                        fprintf(ferr, "write %s", cur.rht);
                    } else {
                        int value;
                        sscanf(cur.rht, "%d", &value);
                        fprintf(ferr, "write '%c'", value);
                    }
                    break;
                }
                default: assert(false);
            }
        } else if(strcmp(cur.op, "function") == 0) { // function, name, type,
            fprintf(ferr, "%s %s()", cur.rht, cur.lft);
        } else if(strcmp(cur.op, "endFunction") == 0) { // endFunction, name, type,
            // fprintf(ferr, "END OF %s %s()", cur.rht, cur.lft);
        } else if(strcmp(cur.op, "call") == 0) { // call, function, temporaryCount, result
            fprintf(ferr, "call %s", cur.lft);
            if(cur.dst[0])
                fprintf(ferr, "%s = RET", cur.dst);
        } else if(strcmp(cur.op, "return") == 0) { // return, result, ,
            fprintf(ferr, "ret %s", cur.lft);
        } else {
            assert(false);
        }
        fprintf(ferr, "\n");
    }
}
