#include "syntax.h"

// Definitions
void program();
void functionDefinitionModified(int, int); // <type> <identifier> are already read (in static)
void constantStatement();
void constantDefinition();
void variableStatement();
void variableDefinitionModified(int, int); // <type> <identifier> are already read (in static)
void parameterTable();
void complexStatement();
void statementList();
void statement();
void ifStatement();
void condition(int);
int expression();
int term();
int factor();
int callStatementModified(int, int); // <identifier> are already read (in static)
int valueParameterTable(int);
void forStatement();
void assignStatementModified(int, int); // <identifier> are already read (in static)
void readStatement();
void writeStatement();
void caseStatement();
void caseTable(int, int);
void caseSubstatement(int, int, int);
void defaultSubstatement();
void returnStatement();

// commonly used variables (should not be overwrite)
static enum SYMBOL typeSymbol;
static char identifier[TOKEN_MAX];
static bool hasReturnStatement;
static int ifCount, loopCount, switchCount;

void program() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    int globalEndLabel = generateLabel();
    int reviseCodeIndex = jumpLabel(globalEndLabel); // reviseCodeIndex = 0
    jumpLabel(globalEndLabel);
    if(symbol == CONSTSY) {
        constantStatement();
    }
    // memory variables for state machine
    bool hasVaribleDefinition = false, forbidVaribleDefinition = false, hasMainFunction = false;
    int startLineIndexOuter = currentFrontLineIndex, startColumnIndexOuter = currentFrontColumnIndex;
    int lastEndLineIndexInner, lastEndColumnIndexInner;
    // remains: [<variable statement>] {<function definition>} <main function definition>
    while(!hasMainFunction && (symbol == INTSY || symbol == CHARSY || symbol == VOIDSY)) {
        int startLineIndexInner = currentFrontLineIndex, startColumnIndexInner = currentFrontColumnIndex;
        typeSymbol = symbol;
        getSymbol();
        strcpy(identifier, token);
        if(symbol != IDSY && symbol != MAINSY) {
            addError(21);
            addError();
            break; // or skip to identifier
        } else {
            if(symbol == MAINSY) {
                hasMainFunction = true;
            }
            getSymbol();
        }
        // prefer to recognize as variable definition
        if(symbol != LPARSY && (typeSymbol == INTSY || typeSymbol == CHARSY)) {
            if(forbidVaribleDefinition) {
                addError(35);
                addError();
                // skip definition
            }
            hasVaribleDefinition = true;
            variableDefinitionModified(startLineIndexInner, startColumnIndexInner);
            if(symbol != SEMISY) {
                addError(15);
                if(symbol == COLONSY) // mismatched
                    getSymbol();
            } else {
                getSymbol();
            }
        } else {
            if(!forbidVaribleDefinition) {
                if(hasVaribleDefinition) { // pack all scanned variable definition as variable statement
#ifdef SYNTAX_DEBUG
                    fprintf(ferr, "There is a variable statement from (line %d, column %d) to (line %d, column %d)\n", startLineIndexOuter, startColumnIndexOuter, lastEndLineIndexInner, lastEndColumnIndexInner);
#endif
                }
                forbidVaribleDefinition = true;
            }
            functionDefinitionModified(startLineIndexInner, startColumnIndexInner);
        }
        lastEndLineIndexInner = lastEndLineIndex;
        lastEndColumnIndexInner = lastEndColumnIndex;
    }
    if(!hasMainFunction)
        addError(26);
    // code ending
    if(symbol != NOTSY) // invalid: extra code
        addError(34);
    setLabel(globalEndLabel);
    // feedback to reviseCodeIndex: j globalEndLabel -> call main
    strcpy(codeList[reviseCodeIndex].op, "call");
    strcpy(codeList[reviseCodeIndex].lft, "main");
    sprintf(codeList[reviseCodeIndex].rht, "%d", 0);
    strcpy(codeList[reviseCodeIndex].dst, "");
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a program from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

// <type> <identifier> are already read (in static)
void functionDefinitionModified(int startLineIndex, int startColumnIndex) {
//    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
//    typeSymbol = symbol;
//    getSymbol();
//    strcpy(identifier, token);
//    if(symbol != IDSY && symbol != MAINSY) {
//        addError(21);
//        addError();
//        break;
//    }
    headerIndex = findSymbol(identifier);
    if(headerIndex != -1) { // multi-definition
        // global multi-def
        addError(37);
        addError();
        // should skip to the end of definition
        return;
    }
    headerIndex = insertSymbol(identifier, FUNCTION, typeSymbol == VOIDSY ? VOID : (typeSymbol == INTSY ? INT : CHAR), true, 0);
    int startLabel = generateLabel(headerIndex, 1), endLabel = generateLabel(headerIndex, 2);
    setLabel(startLabel);
    int startCodeIndex = startOfFunction(headerIndex); // allocate local variables (in stack) and temporary variables (out of stack)
    if(symbol != LPARSY) { // should never be reached
        addError(9);
        addError();
        return;
    } else {
        getSymbol();
    }
    parameterTable();
    if(symbol != RPARSY) {
        addError(10);
        addError();
        // should skip to follow set
    } else {
        getSymbol();
    }
    if(symbol != BEGINSY) {
        addError(13);
        addError();
        return; // should skip to follow set
    } else {
        getSymbol();
    }
    hasReturnStatement = false;
    complexStatement(); // local variable, temporary variable managed in complex statement
    if(symbolTable[headerIndex].type != VOID && !hasReturnStatement)
        addError(25);
    if(symbol != ENDSY) {
        addError(14);
        addError();
        // should skip to follow set
    } else {
        getSymbol();
    }
    setLabel(endLabel);
    endOfFunction(headerIndex); // free local variables (in stack) and temporary variables (out of stack)
#ifdef SEMANTIC_DEBUG
    for(int i = 1; i <= temporaryLimit; ++i) {
        SYMBOLTABLE &cur = symbolTable[symbolCount - i];
        fprintf(fout, "remove symbol #%d: \"%s\", %s, %s, %s, %d, #%d, %d byte(s)\n", symbolCount - i, cur.name,
            cur.kind == CONST ? "constant" :
            cur.kind == VARIABLE ? "variable" :
            cur.kind == ARRAY ? "array" :
            cur.kind == FUNCTION ? "function" :
            cur.kind == PARAMETER ? "parameter variable" : "temporary variable",
            cur.type == VOID ? "void" :
            cur.type == INT ? "int" : "char",
            cur.isGlobal ? "global" : "local", cur.value, cur.preIndex, cur.storeSize);
    }
#endif
    symbolCount -= temporaryLimit;
    lastSymbolIndex = temporaryBaseIndex;
    // hide parameters and local variables
#ifdef SEMANTIC_DEBUG
    for(int i = lastSymbolIndex; i > headerIndex; --i) {
        SYMBOLTABLE &cur = symbolTable[i];
        fprintf(fout, "hidden symbol #%d: \"%s\", %s, %s, %s, %d, #%d, %d byte(s)\n", i, cur.name,
            cur.kind == CONST ? "constant" :
            cur.kind == VARIABLE ? "variable" :
            cur.kind == ARRAY ? "array" :
            cur.kind == FUNCTION ? "function" :
            cur.kind == PARAMETER ? "parameter variable" : "temporary variable",
            cur.type == VOID ? "void" :
            cur.type == INT ? "int" : "char",
            cur.isGlobal ? "global" : "local", cur.value, cur.preIndex, cur.storeSize);
    }
#endif
    lastSymbolIndex = headerIndex;
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a %s function \"%s\" from (line %d, column %d) to (line %d, column %d)\n", symbolTable[headerIndex].type != VOID ? (symbolTable[headerIndex].type == INT ? "int" : "char") : "void", symbolTable[headerIndex].name, startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

void constantStatement() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    while(symbol == CONSTSY) {
        getSymbol();
        constantDefinition();
        if(symbol != SEMISY) {
            addError(15);
            addError();
            return; // should skip to follow set
        } else {
            getSymbol();
        }
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a constant statement from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

void constantDefinition() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    if(symbol != INTSY && symbol != CHARSY) {
        // should try to match ?
        addError(22);
        addError();
        return; // should skip to the follow set
    }
    typeSymbol = symbol;
    do {
        getSymbol(); // read typeSymbol or COMMA
        if(symbol != IDSY) {
            addError(21);
            addError();
            return; // should skip to follow set or skip to identity
        }
        int index = findSymbol(token);
        if(index != -1 && (headerIndex == -1 || index == headerIndex || !symbolTable[index].isGlobal)) { // multi-definition
            // global multi-def, local def as same as function, local multi-def
            addError(37);
            addError();
            return; // skip to the end of definition
        } else {
            index = insertSymbol(token, CONST, typeSymbol == INTSY ? INT : CHAR, headerIndex == -1);
            getSymbol();
        }
        if(symbol != ASSIGNSY) {
            addError(17);
            addError();
            return; // skip to the follow set
        } else {
            getSymbol();
        }
        if(symbolTable[index].type == CHAR) {
            if(symbol != CHSY) {
                // should try to match ?
                addError(19);
                addError();
                return; // skip to the follow set
            } else {
                symbolTable[index].value = token[0];
            }
        } else { // symbolTable[index].type == INT
            int stage = 0;
            if(symbol == PLUSSY) {
                getSymbol();
                stage = 1;
            } else if(symbol == MINUSSY) {
                getSymbol();
                stage = -1;
            }
            if(symbol != NUMSY) {
                // should try to match ?
                addError(20);
                addError();
                return;
            } else if(stage && !number) {
                addWarning(4);
            }
            if(stage < 0) {
                if(number > (unsigned)INT_MAX + 1) {
                    symbolTable[index].value = INT_MIN;
                } else {
                    symbolTable[index].value = 0 - number;
                }
            } else {
                if(number > (unsigned)INT_MAX) {
                    if(number == (unsigned)INT_MAX + 1)
                        addWarning(5);
                    symbolTable[index].value = INT_MAX;
                } else {
                    symbolTable[index].value = number;
                }
            }
        }
        defineElement(index);
        getSymbol();
    } while(symbol == COMMASY);
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a constant definition of %s type from (line %d, column %d) to (line %d, column %d)\n", typeSymbol == INTSY ? "\"int\"" : "\"char\"", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

// only used in function (not used in global, so be careful when recognize function-like variable definitions)
void variableStatement() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    while(symbol == INTSY || symbol == CHARSY) {
        int startLineIndexInner = currentFrontLineIndex, startColumnIndexInner = currentFrontColumnIndex;
        typeSymbol = symbol;
        getSymbol();
        if(symbol != IDSY) {
            addError(21);
            addError();
            break;
        }
        strcpy(identifier, token);
        getSymbol();
        variableDefinitionModified(startLineIndexInner, startColumnIndexInner);
        if(symbol != SEMISY) {
            addError(15);
            if(symbol == COLONSY)
                getSymbol();
        }
        getSymbol();
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a variable statement from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

// <type> <identifier> are already read (in static)
void variableDefinitionModified(int startLineIndex, int startColumnIndex) {
//    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
//    if(symbol != INTSY && symbol != CHARSY) { // should never be reached
//        addError(0);
//        return;
//    }
//    typeSymbol = symbol;
//    getSymbol();
//    if(symbol != IDSY) {
//        addError(21);
//        addError();
//        return;
//    }
//    strcpy(identifier, token);
//    getSymbol();
    // parse first variable
    int index = findSymbol(identifier), index2;
    if(index != -1 && (headerIndex == -1 || index == headerIndex || !symbolTable[index].isGlobal)) { // multi-definition
        // global multi-def, local def as same as function, local multi-def
        addError(37);
        addError();
        return; // should skip to follow set
    }
    if(symbol == LPARSY) {
        addError(36);
        addError();
        return; // should skip to the follow set
    }
    if(symbol == LBRASY) { // array
        getSymbol();
        if(symbol != NUMSY || !number) {
            addWarning(4);
        } else if(number > ARRAY_MAX) {
            addWarning(6);
        }
        index = insertSymbol(identifier, ARRAY, typeSymbol == INTSY ? INT : CHAR, headerIndex == -1, number <= ARRAY_MAX ? number : ARRAY_MAX);
        getSymbol();
        if(symbol != RBRASY) {
            addError(12);
            addError();
            // should skip to the follow set
        } else {
            getSymbol();
        }
        // TODO optional: omit multi-dimension array definition
        if(symbol == LBRASY) {
            // match RBRASY and skip
        }
    } else { // variable
        index = insertSymbol(identifier, VARIABLE, typeSymbol == INTSY ? INT : CHAR, headerIndex == -1, 0);
    }
    defineElement(index);
    // parse others
    while(symbol == COMMASY) {
        getSymbol();
        if(symbol != IDSY) {
            addError(21);
            addError();
            return;
        }
        strcpy(identifier, token);
        index2 = findSymbol(identifier);
        if(index2 != -1 && (headerIndex == -1 || index2 == headerIndex || !symbolTable[index2].isGlobal)) { // multi-definition
            // global multi-def, local def as same as function, local multi-def
            addError(37);
            addError();
            return; // should skip to follow set
        } else {
            getSymbol();
        }
        if(symbol == LBRASY) { // array
            getSymbol();
            if(symbol != NUMSY || !number) {
                addWarning(4);
            } else if(number > ARRAY_MAX) {
                addWarning(6);
            }
            index2 = insertSymbol(identifier, ARRAY, symbolTable[index].type, headerIndex == -1, number <= ARRAY_MAX ? number : ARRAY_MAX);
            getSymbol();
            if(symbol != RBRASY) {
                addError(12);
                addError();
                // should skip to the follow set
            } else {
                getSymbol();
            }
            // TODO optional: omit multi-dimension array definition
            if(symbol == LBRASY) {
                // match RBRASY and skip
            }
        } else {
            index2 = insertSymbol(identifier, VARIABLE, symbolTable[index].type, headerIndex == -1, 0);
        }
        defineElement(index2);
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a variable definition from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

void parameterTable() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    if(symbol == INTSY || symbol == CHARSY) {
        if(strcmp(symbolTable[headerIndex].name, "main") == 0)
            addWarning(7);
        do {
            if(symbol == COMMASY) {
                getSymbol();
            } // else symbol == INTSY || symbol == CHARSY
            if(symbol == INTSY || symbol == CHARSY) {
                typeSymbol = symbol;
                getSymbol();
                if(symbol != IDSY) {
                    addError(21);
                    addError();
                    return; // should skip to the end of definition or skip to identifier
                }
                int index = findSymbol(token);
                if(index != -1 && (index == headerIndex || !symbolTable[index].isGlobal)) { // multi-definition in parameters
                    // local def as same as function, local multi-def
                    addError(37);
                    addError();
                    return; // should skip to the follow set
                } else {
                    index = insertSymbol(token, PARAMETER, typeSymbol == INTSY ? INT : CHAR, false, 0);
                }
                getSymbol();
                ++symbolTable[headerIndex].value;
                defineElement(index);
            } else {
                addError(22);
                addError();
                return; // should skip to the follow set
            }
        } while(symbol == COMMASY);
    }
#ifdef SYNTAX_DEBUG
    fprintf(fout, "There is a parameter table (%d parameters) from (line %d, column %d) to (line %d, column %d)\n", symbolTable[headerIndex].value, startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

void complexStatement() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    if(symbol == CONSTSY) {
        constantStatement();
    }
    if(symbol == INTSY || symbol == CHARSY) {
        variableStatement();
        for(int localIndex = lastSymbolIndex; localIndex >= 0 && (symbolTable[localIndex].kind == VARIABLE || symbolTable[localIndex].kind == ARRAY); localIndex = symbolTable[localIndex].preIndex)
            symbolTable[headerIndex].storeSize += symbolTable[localIndex].storeSize;
    }
    temporaryBaseIndex = lastSymbolIndex;
    temporaryCount = temporaryLimit = 0;
    ifCount = loopCount = switchCount = 0;
    statementList();
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a complex statement from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

void statementList() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    int stmtCount = 0;
    while(symbol == IFSY || symbol == FORSY || symbol == BEGINSY || symbol == IDSY
    || symbol == READSY || symbol == WRITESY || symbol == SEMISY || symbol == SWITCHSY || symbol == RETURNSY) {
        statement();
        ++stmtCount;
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a statement list (%d statements) from (line %d, column %d) to (line %d, column %d)\n", stmtCount, startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

void statement() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    switch(symbol) {
        case IFSY : {
            ifStatement();
            break;
        } case FORSY : {
            forStatement();
            break;
        } case BEGINSY : {
            getSymbol();
            statementList();
            if(symbol != ENDSY) {
                addError(14);
                addError();
                // should skip to follow set
            } else {
                getSymbol();
            }
            break;
        } case IDSY : { // call statement or assign statement
            strcpy(identifier, token);
            getSymbol();
            if(symbol != LPARSY && symbol != ASSIGNSY && symbol != LBRASY) {
                addError(11);
                addError();
                return; // should skip to follow set
            }
            if(symbol == LPARSY) {
                int returnIndex = callStatementModified(startLineIndex, startColumnIndex);
                revokeTemporarySymbol(returnIndex);
            } else { // symbol == ASSIGNSY || symbol == LBRASY
                assignStatementModified(startLineIndex, startColumnIndex);
            }
            if(symbol != SEMISY) {
                addError(15);
                addError();
                // should skip to follow set
            } else {
                getSymbol();
            }
            break;
        } case READSY : {
            readStatement();
            if(symbol != SEMISY) {
                addError(15);
                addError();
                // should skip to follow set
            } else {
                getSymbol();
            }
            break;
        } case WRITESY : {
            writeStatement();
            if(symbol != SEMISY) {
                addError(15);
                addError();
                // should skip to follow set
            } else {
                getSymbol();
            }
            break;
        } case SEMISY : {
            getSymbol();
            break;
        } case SWITCHSY : {
            caseStatement();
            break;
        } case RETURNSY : {
            returnStatement();
            if(symbol != SEMISY) {
                addError(15);
                addError();
                // should skip to follow set
            } else {
                getSymbol();
            }
            break;
        } default : {
            addError(15);
            addError();
            return; // should skip to follow set
        }
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a statement from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

void ifStatement() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    if(symbol != IFSY) { // should never be reached
        addError(0);
        return;
    } else {
        getSymbol();
    }
    if(symbol != LPARSY) {
        addError(9);
        addError();
        return; // skip to follow set
    } else {
        getSymbol();
    }
    ++ifCount;
    int elseLabel = generateLabel(IFSY, 1, ifCount), endIfLabel = generateLabel(IFSY, 2, ifCount);
    condition(elseLabel);
    if(symbol != RPARSY) {
        addError(10);
        addError();
        // skip to follow set
    } else {
        getSymbol();
    }
    statement();
    jumpLabel(endIfLabel);
    setLabel(elseLabel);
    if(symbol == ELSESY) {
        getSymbol();
        statement();
    }
    setLabel(endIfLabel);
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a if statement from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

void condition(int elseLabel) {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    int index = expression();
    if(symbol >= EQSY && symbol <= GEQSY) { // relational operators
        typeSymbol = symbol;
        getSymbol();
        int index2 = expression();
        branchLabel(typeSymbol, index, index2, elseLabel);
        revokeTemporarySymbol(index2);
    } else {
        branchLabel(NOTSY, index, -1, elseLabel);
    }
    revokeTemporarySymbol(index);
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a condition from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

int expression() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    int returnIndex = -1;
    if((symbol == PLUSSY || symbol == MINUSSY) && (!isDigit() || ch == '0')) { // get negative
        bool isPlus = symbol == PLUSSY;
        getSymbol();
        returnIndex = term();
        if(symbolTable[returnIndex].type != INT)
            formatterTemporarySymbol(returnIndex, INT);
        if(!isPlus) {
            formatterTemporarySymbol(returnIndex, INT);
            arithmeticOpeation(MINUSSY, -1, returnIndex, returnIndex);
        }
    } else { // normal
        returnIndex = term();
    }
    while(symbol == PLUSSY || symbol == MINUSSY) {
        bool isPlus = symbol == PLUSSY;
        getSymbol();
        formatterTemporarySymbol(returnIndex, INT);
        int tempIndex = term();
        if(symbolTable[tempIndex].type != INT)
            formatterTemporarySymbol(tempIndex, INT);
        if(isPlus) {
            arithmeticOpeation(PLUSSY, returnIndex, tempIndex, returnIndex);
        } else {
            arithmeticOpeation(MINUSSY, returnIndex, tempIndex, returnIndex);
        }
        revokeTemporarySymbol(tempIndex);
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is an expression from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
    return returnIndex;
}

int term() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    int returnIndex = factor();
    while(symbol == MULTISY || symbol == DIVISY) {
        bool isMultiply = symbol == MULTISY;
        getSymbol();
        formatterTemporarySymbol(returnIndex, INT);
        int tempIndex = factor();
        if(symbolTable[tempIndex].type != INT)
            formatterTemporarySymbol(tempIndex, INT);
        if(isMultiply) {
            arithmeticOpeation(MULTISY, returnIndex, tempIndex, returnIndex);
        } else {
            arithmeticOpeation(DIVISY, returnIndex, tempIndex, returnIndex);
        }
        revokeTemporarySymbol(tempIndex);
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a term from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
    return returnIndex;
}

int factor() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    int returnIndex = -1;
    if(symbol == IDSY) { // identifier or call statement
        strcpy(identifier, token);
        getSymbol();
        if(symbol == LPARSY) { // call statement
            returnIndex = callStatementModified(startLineIndex, startColumnIndex);
        } else {
            int index = findSymbol(identifier);
            if(index == -1) {
                addError(31);
                addError();
                return -1; // skip to the follow set
            }
            if(symbol == LBRASY) { // array
                getSymbol();
                returnIndex = generateTemporarySymbol(symbolTable[index].type); // may be CHAR
                int offsetIndex = expression();
                if(symbolTable[offsetIndex].type != INT)
                    formatterTemporarySymbol(offsetIndex, INT);
                // WARNING: if return value is less than zero or large than or equal to the size of array, undefined behavior would be caused
                loadArrayElement(index, offsetIndex, returnIndex);
                revokeTemporarySymbol(offsetIndex);
                if(symbol != RBRASY) {
                    addError(12);
                    addError();
                    // should skip to the follow set
                } else {
                    getSymbol();
                }
            } else {
                returnIndex = index;
            }
        }
    } else if(symbol == LPARSY) { // expression
        getSymbol();
        returnIndex = expression();
        if(symbol != RPARSY) {
            addError(12);
            addError();
            // should skip to the follow set
        } else {
            getSymbol();
        }
    } else if(symbol == CHSY) { // character
        // assign to the return symbol
        returnIndex = generateTemporarySymbol(CHAR, token[0]);
        getSymbol();
    } else { // number
        int stage = 0;
        if(symbol == PLUSSY) {
            getSymbol();
            stage = 1;
        } else if(symbol == MINUSSY) {
            getSymbol();
            stage = -1;
        }
        if(symbol != NUMSY) {
            // should try to match ?
            addError(20);
            addError();
            return -1;
        } else if(stage && !number) {
            addWarning(4);
        }
        returnIndex = generateTemporarySymbol(INT);
        if(stage < 0) {
            if(number > (unsigned)INT_MAX + 1) {
                symbolTable[returnIndex].value = INT_MIN;
            } else {
                symbolTable[returnIndex].value = 0 - number;
            }
        } else {
            if(number > (unsigned)INT_MAX) {
                if(number == (unsigned)INT_MAX + 1)
                    addWarning(5);
                symbolTable[returnIndex].value = INT_MAX;
            } else {
                symbolTable[returnIndex].value = number;
            }
        }
        getSymbol();
        storeImmediate(returnIndex, symbolTable[returnIndex].value);
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a factor from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
    return returnIndex;
}

// <identifier> are already read (in static)
int callStatementModified(int startLineIndex, int startColumnIndex) {
//    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
//    if(symbol != IDSY) {
//        // should try to match ?
//        puts("There should be an identifier.");
//        return; // should skip to the end of statement
//    }
//    strcpy(identifier, token);
//    getSymbol();
    int index = findSymbol(identifier), returnIndex = -1;
    if(index == -1 || symbolTable[index].kind != FUNCTION) {
        addError(32);
        addError();
        return -1; // should skip to the follow set
    } else if(strcmp(symbolTable[index].name, "main") == 0) {
        addWarning(8);
    }
    if(symbol != LPARSY) {
        addError(9);
        addError();
        return -1; // should skip to the follow set
    } else {
        getSymbol();
    }
    returnIndex = valueParameterTable(index);
    if(symbol != RPARSY) {
        addError(10);
        addError();
        // should skip to the follow set
    } else {
        getSymbol();
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a procedure or function calling from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
    return returnIndex;
}

int valueParameterTable(int calleeIndex) {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    int returnIndex = -1;
    if(symbolTable[calleeIndex].type != VOID) {
        returnIndex = generateTemporarySymbol(symbolTable[calleeIndex].type);
    }
    int parameterCount = 0, parameterBaseIndex = temporaryBaseIndex + temporaryCount;
    if(symbol != RPARSY) {
        while(true) {
            int tempIndex = expression();
            if(parameterCount == symbolTable[calleeIndex].value) {
                addError(38);
                addError();
                return -1; // should skip to the follow set
            }
            ++parameterCount;
            if(symbolTable[tempIndex].type != symbolTable[calleeIndex + parameterCount].type) {
                addWarning(symbolTable[tempIndex].type == INT ? 10 : 11);
            }
            formatterTemporarySymbol(tempIndex, symbolTable[calleeIndex + parameterCount].type);
            pushParameter(tempIndex);
            if(symbol != COMMASY)
                break;
            getSymbol();
        }
    }
    if(parameterCount != symbolTable[calleeIndex].value) {
        addError(38);
        addError();
        return -1;
    }
    userCall(calleeIndex, returnIndex);
    // after callee finished, free (not real, just revoke) parameters (caller's temporary variable)
    while(parameterCount) {
        revokeTemporarySymbol(parameterBaseIndex + parameterCount);
        --parameterCount;
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a value parameter table from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
    return returnIndex;
}

void forStatement() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    ++loopCount;
    int loopConditionLabel = generateLabel(FORSY, 1, loopCount), loopIterationLabel = generateLabel(FORSY, 2, loopCount);
    int loopBlockLabel = generateLabel(FORSY, 3, loopCount), loopEndLabel = generateLabel(FORSY, 4, loopCount);
    if(symbol != FORSY) { // should never be reached
        addError(0);
        return;
    } else {
        getSymbol();
    }
    if(symbol != LPARSY) {
        addError(9);
        addError(0);
        return; // should skip to the follow set
    } else {
        getSymbol();
    }
    if(symbol != IDSY) {
        addError(21);
        addError();
        return; // should skip to the follow set
    }
    int index = findSymbol(token);
    bool extraVariable = false;
    if(index == -1) {
        addError(31);
        extraVariable = true; // allocate a temporarily variable to replace
        index = generateTemporarySymbol(INT, 0);
    } else {
        getSymbol();
    }
    if(symbol != ASSIGNSY) {
        addError(17);
        addError();
        return; // should skip to the follow set
    } else {
        getSymbol();
    }
    int initIndex = expression();
    arithmeticOpeation(ASSIGNSY, initIndex, -1, index);
    revokeTemporarySymbol(initIndex);
    jumpLabel(loopBlockLabel);
    if(symbol != SEMISY) {
        addError(15);
        if(symbol == COLONSY)
            getSymbol();
    } else {
        getSymbol();
    }
    setLabel(loopConditionLabel);
    checkingIndex = index;
    checkingResult = false;
    condition(loopEndLabel);
    jumpLabel(loopBlockLabel);
    if(!checkingResult)
        addError(23);
    checkingIndex = -1;
    if(symbol != SEMISY) {
        addError(15);
        if(symbol == COLONSY)
            getSymbol();
    } else {
        getSymbol();
    }
    setLabel(loopIterationLabel);
    if(symbol != IDSY) {
        addError(21);
        addError();
        return; // should skip to the follow set
    } else if(findSymbol(token) != index) {
        addError(24);
    } else {
        getSymbol();
    }
    if(symbol != ASSIGNSY) {
        addError(17);
        addError();
        return; // should skip to the follow set
    } else {
        getSymbol();
    }
    if(symbol != IDSY) {
        addError(21);
        addError();
        return; // should skip to the follow set
    } else if(findSymbol(token) != index) {
        addError(24);
    } else {
        getSymbol();
    }
    if(symbol != PLUSSY && symbol != MINUSSY) {
        addError(18);
        addError();
        return; // should skip to the follow set
    } else {
        typeSymbol = symbol;
        getSymbol();
    }
    if(symbol != NUMSY || !number) {
        // should try to match ?
        addError(20);
        addError();
        return;
    } else if(number > (unsigned)INT_MAX) {
        if(number == (unsigned)INT_MAX + 1)
            addWarning(5);
    }
    int tempIndex = generateTemporarySymbol(INT, number < (unsigned)INT_MAX ? number : INT_MAX);
    if(typeSymbol == PLUSSY) {
        arithmeticOpeation(PLUSSY, index, tempIndex, index);
    } else { // MINUSSY
        arithmeticOpeation(MINUSSY, index, tempIndex, index);
    }
    revokeTemporarySymbol(tempIndex);
    jumpLabel(loopConditionLabel);
    getSymbol();
    if(symbol != RPARSY) {
        addError(10);
        addError();
        return; // should skip to the follow set
    } else {
        getSymbol();
    }
    setLabel(loopBlockLabel);
    statement();
    jumpLabel(loopIterationLabel);
    setLabel(loopEndLabel);
    if(extraVariable) { // free loop variable and further temporarily variables
        revokeTemporarySymbol(index);
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a for statement from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

// <identifier> are already read
void assignStatementModified(int startLineIndex, int startColumnIndex) {
//    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
//    if(symbol != IDSY) { // should never be reached
//        addError(0);
//        return;
//    }
//    strcpy(identifier, token);
//    getSymbol();
    int leftIndex = findSymbol(identifier), leftOffsetIndex = -1, rightIndex = -1;
    if(leftIndex == -1) {
        addError(31);
        addError();
        return; // should skip to follow set
    }
    if(symbol == LBRASY) { // array
        if(symbolTable[leftIndex].kind != ARRAY) {
            addError(31);
            addError();
            return; // should skip to follow set
        } else {
            getSymbol();
        }
        leftOffsetIndex = expression();
        if(symbolTable[leftOffsetIndex].type != INT)
            formatterTemporarySymbol(leftOffsetIndex, INT);
        // WARNING: if return value is less than zero or large than or equal to the size of array, undefined behavior would be caused
        if(symbol != RBRASY) {
            addError(12);
            addError();
            // should skip to follow set
        } else {
            getSymbol();
        }
    } else if(symbolTable[leftIndex].kind == CONST || symbolTable[leftIndex].kind == FUNCTION) {
        addError(33);
        addError();
        return; // should skip to follow set
    }
    if(symbol != ASSIGNSY) {
        addError(17);
        addError();
        return; // should skip to follow set
    } else {
        getSymbol();
        rightIndex = expression();
    }
    if(symbolTable[leftIndex].type == CHAR && symbolTable[rightIndex].type == INT) { // optional
        addWarning(10);
    }
    if(symbolTable[leftIndex].kind == ARRAY) {
        storeArrayElement(leftIndex, leftOffsetIndex, rightIndex);
    } else { // VARIABLE or PARAMETER
        arithmeticOpeation(ASSIGNSY, rightIndex, -1, leftIndex);
    }
    revokeTemporarySymbol(rightIndex);
    if(leftOffsetIndex != -1) {
        revokeTemporarySymbol(leftOffsetIndex);
    }
    revokeTemporarySymbol(leftIndex);
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is an assign statement from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

void readStatement() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    if(symbol != READSY) { // should never be reached
        addError(0);
        return;
    } else {
        getSymbol();
    }
    if(symbol != LPARSY) {
        addError(9);
        addError(0);
        return; // should skip to follow set
    }
    do {
        getSymbol(); // LPAR or COMMA
        int index;
        if(symbol != IDSY || (index = findSymbol(token)) == -1) {
            addError(symbol != IDSY ? 21 : 31);
            addError();
            return; // should skip to follow set
        } else if(symbolTable[index].kind != VARIABLE && symbolTable[index].kind != PARAMETER) {
            addError(33);
            addError();
            return; // should skip to follow set
        } else {
            getSymbol();
            if(symbolTable[index].type == INT) {
                sysCall(5, -1, index); // read INT
            } else { // CHAR
                sysCall(12, -1, index); // read CHAR
            }
        }
    } while(symbol == COMMASY);
    if(symbol != RPARSY) {
        addError(10);
        addError();
        // skip to follow set
    } else {
        getSymbol();
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a read statement from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

void writeStatement() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    if(symbol != WRITESY) { // should never be reached
        addError(0);
        return;
    } else {
        getSymbol();
    }
    if(symbol != LPARSY) {
        addError(9);
        addError();
        return; // should skip to follow set
    } else {
        getSymbol();
    }
    if(symbol == STRSY) {
        sysCall(4, -1, -1); // write STRING (in token)
        getSymbol();
        if(symbol == COMMASY) {
            getSymbol();
            int index = expression();
            // WARNING: if expression is character and not visible
            if(symbolTable[index].type == INT) {
                sysCall(1, index, -1); // write INT
            } else { // CHAR
                sysCall(11, index, -1); // write CHAR
            }
            revokeTemporarySymbol(index);
        }
    } else {
        int index = expression();
        // WARNING: if expression is character and not visible
        if(symbolTable[index].type == INT) {
            sysCall(1, index, -1); // write INT
        } else { // CHAR
            sysCall(11, index, -1); // write CHAR
        }
        revokeTemporarySymbol(index);
    }
    if(symbol != RPARSY) {
        addError(10);
        addError();
        // should skip to follow set
    } else {
        getSymbol();
    }
#ifdef OUTPUT_EOLN
    sysCall(11, -1, -1);
#endif
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a write statement from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

void caseStatement() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    if(symbol != SWITCHSY) { // should never be reached
        addError(0);
        return;
    } else {
        getSymbol();
    }
    if(symbol != LPARSY) {
        addError(9);
        addError();
        return; // should skip to follow set
    } else {
        getSymbol();
    }
    int index = expression();
    if(symbol != RPARSY) {
        addError(10);
        addError();
        // should skip to follow set
    } else {
        getSymbol();
    }
    if(symbol != BEGINSY) {
        addError(13);
        addError();
        return; // should skip to follow set
    } else {
        getSymbol();
    }
    int switchEndLabel = generateLabel(SWITCHSY, 1, ++switchCount);
    caseTable(index, switchEndLabel);
    revokeTemporarySymbol(index);
    defaultSubstatement();
    setLabel(switchEndLabel);
    if(symbol != ENDSY) {
        addError(14);
        addError();
        return; // should skip to follow set
    } else {
        getSymbol();
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a case statement from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

void caseTable(int expIndex, int switchEndLabel) {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    int caseCount = 0;
    while(symbol == CASESY) {
        caseSubstatement(expIndex, switchEndLabel, ++caseCount);
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a case table from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

void caseSubstatement(int expIndex, int switchEndLabel, int caseLabelIndex) {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    if(symbol != CASESY) { // should never be reached
        addError(0);
        return;
    } else {
        getSymbol();
    }
    int caseIndex = -1, caseEndLabel = generateLabel(CASESY, 1, caseLabelIndex, switchEndLabel);
    if(symbol == CHSY) {
        caseIndex = generateTemporarySymbol(CHAR, token[0]);
        getSymbol();
    } else { // try to match as number
        int stage = 0;
        if(symbol == PLUSSY) {
            getSymbol();
            stage = 1;
        } else if(symbol == MINUSSY) {
            getSymbol();
            stage = -1;
        }
        if(symbol != NUMSY) {
            // should try to match ?
            addError(20);
            addError();
            return;
        } else if(stage && !number) {
            addWarning(4);
        }
        caseIndex = generateTemporarySymbol(INT);
        if(stage < 0) {
            if(number > (unsigned)INT_MAX + 1) {
                symbolTable[caseIndex].value = INT_MIN;
            } else {
                symbolTable[caseIndex].value = 0 - number;
            }
        } else {
            if(number > (unsigned)INT_MAX) {
                if(number == (unsigned)INT_MAX + 1)
                    addWarning(5);
                symbolTable[caseIndex].value = INT_MAX;
            } else {
                symbolTable[caseIndex].value = number;
            }
        }
        getSymbol();
        storeImmediate(caseIndex, symbolTable[caseIndex].value);
    }
    if(symbol != COLONSY) {
        addError(16);
        if(symbol == SEMISY)
            getSymbol();
    } else {
        getSymbol();
    }
    if(symbolTable[expIndex].type != symbolTable[caseIndex].type) {
        addWarning(9);
    }
    branchLabel(EQSY, expIndex, caseIndex, caseEndLabel);
    revokeTemporarySymbol(caseIndex);
    statement();
    jumpLabel(switchEndLabel);
    setLabel(caseEndLabel);
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a case sub-statement from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

void defaultSubstatement() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    if(symbol != DEFAULTSY) {
        addError(0);
        return; // should skip to follow set
    } else {
        getSymbol();
    }
    if(symbol != COLONSY) {
        addError(16);
        if(symbol == SEMISY)
            getSymbol();
    } else {
        getSymbol();
    }
    statement();
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a default sub-statement from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

void returnStatement() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    if(symbol != RETURNSY) { // should never be reached
        addError(0);
        return;
    } else {
        getSymbol();
    }
    if(symbolTable[headerIndex].type == VOID) {
        if(symbol != SEMISY) {
            addError(39);
            addError();
        } else {
            userReturn(-1);
            hasReturnStatement = true;
        }
    } else {
        if(symbol != LPARSY) {
            addError(40);
            addError();
            // should skip to the follow set or skip to LPAR
        } else {
            getSymbol();
            int returnIndex = expression();
            if(symbol != RPARSY) {
                addError(10);
                addError();
                // should skip to the follow set
            } else {
                getSymbol();
            }
            if(symbolTable[returnIndex].type != symbolTable[headerIndex].type) {
                addWarning(symbolTable[returnIndex].type == INT ? 10 : 11);
                formatterTemporarySymbol(returnIndex, symbolTable[headerIndex].type);
            }
            userReturn(returnIndex);
            revokeTemporarySymbol(returnIndex);
            hasReturnStatement = true;
        }
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a return statement from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}
