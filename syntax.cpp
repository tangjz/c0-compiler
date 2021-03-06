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
int valueParameterTable(int calleeIndex);
void forStatement();
void assignStatementModified(int, int); // <identifier> are already read (in static)
void readStatement();
void writeStatement();
void caseStatement();
void caseTable(int expIndex, int);
void caseSubstatement(int expIndex, int, int, set<int> &);
void defaultSubstatement();
void returnStatement();

// commonly used variables (should not be overwrite)
static enum SYMBOL typeSymbol;
static char identifier[TOKEN_MAX];
static bool hasReturnStatement;
static int ifCount, loopCount, switchCount;

// skip tool: only used for syntax
enum SKIP {NOP, NTYP, NCTYP, NCS, NTYPST, NCP, NSP, NC, NS};
void skipSymbol(SKIP type) {
    switch(type) {
        case NOP: { // do nothing
            break;
        }
        case NTYP: { // before next type
            while(symbol != NOTSY && symbol != INTSY && symbol != CHARSY && symbol != VOIDSY)
                getSymbol();
            break;
        }
        case NCTYP: { // before next 'const' or type
            while(symbol != NOTSY && symbol != CONSTSY && symbol != INTSY && symbol != CHARSY && symbol != VOIDSY)
                getSymbol();
            break;
        }
        case NCS: { // before next ',' or ';'
            while(symbol != NOTSY && symbol != COMMASY && symbol != SEMISY)
                getSymbol();
            break;
        }
        case NTYPST: { // before next type or statement
            while(symbol != NOTSY && symbol != INTSY && symbol != CHARSY && symbol != VOIDSY
                && symbol != IFSY && symbol != FORSY && symbol != BEGINSY && symbol != IDSY && symbol != MAINSY
                && symbol != READSY && symbol != WRITESY && symbol != SEMISY && symbol != SWITCHSY && symbol != RETURNSY)
                getSymbol();
            break;
        }
        case NCP: { // before next ',' or ')'
            while(symbol != NOTSY && symbol != COMMASY && symbol != RPARSY)
                getSymbol();
            break;
        }
        case NSP: { // before next ';' or ')'
            while(symbol != NOTSY && symbol != SEMISY && symbol != RPARSY)
                getSymbol();
            break;
        }
        case NC: { // before next ':'
            while(symbol != NOTSY && symbol != COLONSY)
                getSymbol();
            break;
        }
        case NS: { // before next ';'
            while(symbol != NOTSY && symbol != SEMISY)
                getSymbol();
            break;
        }
        default: assert(false);
    }
}

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
varOrFunctDef:
        int startLineIndexInner = currentFrontLineIndex, startColumnIndexInner = currentFrontColumnIndex;
        typeSymbol = symbol;
        getSymbol();
        strcpy(identifier, token);
        if(symbol != IDSY && symbol != MAINSY) {
            addError(21);
            skipSymbol(NTYP);
            break;
        } else {
            if(symbol == MAINSY) {
                hasMainFunction = true;
            }
            getSymbol();
        }
        // prefer to recognize as variable definition
        if(symbol != LPARSY && (typeSymbol == INTSY || typeSymbol == CHARSY)) {
            if(symbol == RPARSY || symbol == BEGINSY)
                goto functDef;
            if(forbidVaribleDefinition) {
                addError(35);
                skipSymbol(NTYP);
                continue;
            }
            hasVaribleDefinition = true;
            variableDefinitionModified(startLineIndexInner, startColumnIndexInner);
            if(symbol != SEMISY) {
                addError(15);
                // do nothing
            } else {
                getSymbol();
            }
        } else {
functDef:
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
    if(symbol != NOTSY) {
        skipSymbol(NTYP);
        if(symbol == INTSY || symbol == CHARSY || symbol == VOIDSY) {
            goto varOrFunctDef;
        } else if(hasMainFunction) { // invalid: extra code after main function
            addError(34);
        }
    }
    if(!hasMainFunction)
        addError(26);
    // code ending
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
//        skipSymbol(NTYP);
//        return;
//    }
    headerIndex = findSymbol(identifier);
    if(headerIndex != -1) { // multi-definition
        // global multi-def
        addError(37);
        headerIndex = -1;
    } else {
        headerIndex = insertSymbol(identifier, FUNCTION, typeSymbol == VOIDSY ? VOID : (typeSymbol == INTSY ? INT : CHAR), true, 0);
    }
    int startLabel = headerIndex != -1 ? generateLabel(headerIndex, 1) : -1;
    int endLabel = headerIndex != -1 ? generateLabel(headerIndex, 2) : -1;
    if(headerIndex != -1) {
        setLabel(startLabel);
        startOfFunction(headerIndex); // allocate local variables (in stack) and temporary variables (out of stack)
    }
    if(symbol != LPARSY) {
        addError(9);
        // do nothing
    } else {
        getSymbol();
    }
    parameterTable();
    if(symbol != RPARSY) {
        addError(10);
        // do nothing
    } else {
        getSymbol();
    }
    if(symbol != BEGINSY) {
        addError(13);
        skipSymbol(NTYP);
        return;
    } else {
        getSymbol();
    }
    hasReturnStatement = false;
    complexStatement(); // local variable, temporary variable managed in complex statement
    if(headerIndex != -1 && symbolTable[headerIndex].type != VOID && !hasReturnStatement)
        addError(25);
    if(symbol != ENDSY) {
        addError(14);
        // do nothing
    } else {
        getSymbol();
    }
    if(headerIndex != -1) {
        setLabel(endLabel);
        endOfFunction(headerIndex); // free local variables (in stack) and temporary variables (out of stack)
    }
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
    if(headerIndex != -1)
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
    if(headerIndex != -1)
        lastSymbolIndex = headerIndex;
#ifdef SYNTAX_DEBUG
    if(headerIndex != -1)
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
            // do nothing
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
        addError(22);
        skipSymbol(NCTYP);
        return;
    }
    typeSymbol = symbol;
    do {
        getSymbol(); // read typeSymbol or COMMA
        if(symbol != IDSY && symbol != MAINSY) {
            addError(21);
            skipSymbol(NCS);
            continue;
        }
        int index = findSymbol(token);
        if(index != -1 && (headerIndex == -1 || index == headerIndex || !symbolTable[index].isGlobal)) { // multi-definition
            // global multi-def, local def as same as function, local multi-def
            addError(37);
            index = -1;
        } else {
            index = insertSymbol(token, CONST, typeSymbol == INTSY ? INT : CHAR, headerIndex == -1);
        }
        getSymbol();
        if(symbol != ASSIGNSY) {
            addError(17);
            skipSymbol(NCS);
            continue;
        } else {
            getSymbol();
        }
        if((index != -1 && symbolTable[index].type == CHAR) || (index == -1 && symbol == CHSY)) {
            if(symbol != CHSY) {
                addError(19);
                skipSymbol(NCS);
                continue;
            } else if(index != -1) {
                symbolTable[index].value = token[0];
            }
        } else { // index != -1 && symbolTable[index].type == INT || index == -1
            int stage = 0;
            if(symbol == PLUSSY) {
                getSymbol();
                stage = 1;
            } else if(symbol == MINUSSY) {
                getSymbol();
                stage = -1;
            }
            if(symbol != NUMSY) {
                addError(20);
                skipSymbol(NCS);
                continue;
            } else if(stage && !number) {
                addWarning(4);
            }
            int constValue;
            if(stage < 0) {
                if(number > (unsigned)INT_MAX + 1) {
                    constValue = INT_MIN;
                } else {
                    constValue = 0 - number;
                }
            } else {
                if(number > (unsigned)INT_MAX) {
                    if(number == (unsigned)INT_MAX + 1)
                        addWarning(5);
                    constValue = INT_MAX;
                } else {
                    constValue = number;
                }
            }
            if(index != -1)
                symbolTable[index].value = constValue;
        }
        if(index != -1)
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
        if(symbol != IDSY && symbol != MAINSY) {
            addError(21);
            skipSymbol(NTYP);
            continue;
        }
        strcpy(identifier, token);
        getSymbol();
        variableDefinitionModified(startLineIndexInner, startColumnIndexInner);
        if(symbol != SEMISY) {
            addError(15);
            // do nothing
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
//    if(symbol != IDSY && symbol != MAINSY) {
//        addError(21);
//        skipSymbol(NTYP);
//        return;
//    }
//    strcpy(identifier, token);
//    getSymbol();
    // parse first variable
    TYPE varType = typeSymbol == INTSY ? INT : CHAR;
    int index = findSymbol(identifier), index2;
    bool fake = false;
    if(index != -1 && (headerIndex == -1 || index == headerIndex || !symbolTable[index].isGlobal)) { // multi-definition
        // global multi-def, local def as same as function, local multi-def
        addError(37);
        fake = true;
    }
    if(symbol == LPARSY) {
        addError(36);
        skipSymbol(NTYPST);
        return;
    }
    if(symbol == LBRASY) { // array
        getSymbol();
        if(symbol != NUMSY) {
            addError(20);
        } else if(!number) {
            addWarning(4);
        } else if(number > ARRAY_MAX) {
            addWarning(6);
        }
        if(!fake) {
            index = insertSymbol(identifier, ARRAY, typeSymbol == INTSY ? INT : CHAR, headerIndex == -1, number <= ARRAY_MAX ? number : ARRAY_MAX);
        } else {
            index = -1;
        }
        if(symbol != RBRASY)
            getSymbol();
        if(symbol != RBRASY) {
            addError(12);
            // do nothing
        } else {
            getSymbol();
        }
    } else { // variable
        if(!fake) {
            index = insertSymbol(identifier, VARIABLE, varType, headerIndex == -1, 0);
        } else {
            index = -1;
        }
    }
    if(index != -1)
        defineElement(index);
    if(symbol == ASSIGNSY) {
        addError(33);
        skipSymbol(NCS);
    }
    // parse others
    while(symbol == COMMASY) {
        getSymbol();
        if(symbol != IDSY && symbol != MAINSY) {
            addError(21);
            skipSymbol(NCS);
            continue;
        }
        strcpy(identifier, token);
        index2 = findSymbol(identifier);
        bool fake = false;
        if(index2 != -1 && (headerIndex == -1 || index2 == headerIndex || !symbolTable[index2].isGlobal)) { // multi-definition
            // global multi-def, local def as same as function, local multi-def
            addError(37);
            fake = true;
        }
        getSymbol();
        if(symbol == LBRASY) { // array
            getSymbol();
            if(symbol != NUMSY) {
                addError(20);
            } else if(!number) {
                addWarning(4);
            } else if(number > ARRAY_MAX) {
                addWarning(6);
            }
            if(!fake) {
                index2 = insertSymbol(identifier, ARRAY, varType, headerIndex == -1, number <= ARRAY_MAX ? number : ARRAY_MAX);
            } else {
                index2 = -1;
            }
            getSymbol();
            if(symbol != RBRASY) {
                addError(12);
                // do nothing
            } else {
                getSymbol();
            }
        } else {
            if(!fake) {
                index2 = insertSymbol(identifier, VARIABLE, varType, headerIndex == -1, 0);
            } else {
                index2 = -1;
            }
        }
        if(index2 != -1)
            defineElement(index2);
        if(symbol == ASSIGNSY) {
            addError(33);
            skipSymbol(NCS);
        }
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a variable definition from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

void parameterTable() {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    if(symbol == INTSY || symbol == CHARSY || symbol == IDSY || symbol == COMMASY) {
        if(headerIndex != -1 && strcmp(symbolTable[headerIndex].name, "main") == 0)
            addWarning(7);
        if(symbol == COMMASY)
            addError(21);
        do {
            if(symbol == COMMASY) {
                getSymbol();
            } // else symbol == INTSY || symbol == CHARSY
            if(symbol == INTSY || symbol == CHARSY) {
                typeSymbol = symbol;
                getSymbol();
                if(symbol != IDSY && symbol != MAINSY) {
                    addError(21);
                    skipSymbol(NCP);
                    continue;
                }
                int index = findSymbol(token);
                if(index != -1 && (index == headerIndex || !symbolTable[index].isGlobal)) { // multi-definition in parameters
                    // local def as same as function, local multi-def
                    addError(37);
                    index = -1;
                } else {
                    index = insertSymbol(token, PARAMETER, typeSymbol == INTSY ? INT : CHAR, false, 0);
                }
                getSymbol();
                if(headerIndex != -1)
                    ++symbolTable[headerIndex].value;
                if(index != -1)
                    defineElement(index);
            } else {
                addError(22);
                skipSymbol(NCP);
                continue;
            }
        } while(symbol == COMMASY);
    }
#ifdef SYNTAX_DEBUG
    if(headerIndex != -1)
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
    while(symbol == IFSY || symbol == FORSY || symbol == BEGINSY || symbol == IDSY || symbol == MAINSY
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
                // do nothing
            } else {
                getSymbol();
            }
            break;
        } case IDSY : case MAINSY: { // call statement or assign statement
            strcpy(identifier, token);
            getSymbol();
            if(symbol != LPARSY && symbol != ASSIGNSY && symbol != LBRASY) {
                int index = findSymbol(identifier);
                if(index == -1) {
                    addError(31);
                } else if(symbolTable[index].kind == ARRAY) {
                    addError(11);
                } else if(symbolTable[index].kind == FUNCTION) {
                    addError(9);
                    if(symbol != RPARSY)
                        addError(10);
                } else {
                    addError(17);
                }
                // do nothing
                getSymbol();
                return;
            }
            if(symbol == LPARSY) {
                int returnIndex = callStatementModified(startLineIndex, startColumnIndex);
                revokeTemporarySymbol(returnIndex);
            } else { // symbol == ASSIGNSY || symbol == LBRASY
                assignStatementModified(startLineIndex, startColumnIndex);
            }
            if(symbol != SEMISY) {
                addError(15);
                // do nothing
            } else {
                getSymbol();
            }
            break;
        } case READSY : {
            readStatement();
            if(symbol != SEMISY) {
                addError(15);
                // do nothing
            } else {
                getSymbol();
            }
            break;
        } case WRITESY : {
            writeStatement();
            if(symbol != SEMISY) {
                addError(15);
                // do nothing
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
                // do nothing
            } else {
                getSymbol();
            }
            break;
        } default : {
            addError(15);
            // do nothing
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
        // do nothing
    } else {
        getSymbol();
    }
    ++ifCount;
    int elseLabel = generateLabel(IFSY, 1, ifCount), endIfLabel = generateLabel(IFSY, 2, ifCount);
    condition(elseLabel);
    if(symbol != RPARSY) {
        addError(10);
        // do nothing
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
    if(symbol == ASSIGNSY || (symbol >= EQSY && symbol <= GEQSY)) { // relational operators
        typeSymbol = symbol;
        getSymbol();
        if(symbol == ASSIGNSY || (symbol >= EQSY && symbol <= ASSIGNSY)) {
            getSymbol();
            typeSymbol = ASSIGNSY;
        }
        int index2 = expression();
        if(typeSymbol == ASSIGNSY || index2 == -1)
            addError(44);
        else if(index != -1 && index2 != -1)
            branchLabel(typeSymbol, index, index2, elseLabel);
        revokeTemporarySymbol(index2);
    } else {
        if(index != -1)
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
        if(returnIndex != -1 && symbolTable[returnIndex].type != INT)
            formatterTemporarySymbol(returnIndex, INT);
        if(returnIndex != -1 && !isPlus) {
            formatterTemporarySymbol(returnIndex, INT);
            arithmeticOpeation(MINUSSY, -1, returnIndex, returnIndex);
        }
    } else { // normal
        returnIndex = term();
    }
    while(symbol == PLUSSY || symbol == MINUSSY) {
        bool isPlus = symbol == PLUSSY;
        getSymbol();
        if(returnIndex != -1)
            formatterTemporarySymbol(returnIndex, INT);
        int tempIndex = term();
        if(tempIndex != -1 && symbolTable[tempIndex].type != INT)
            formatterTemporarySymbol(tempIndex, INT);
        if(returnIndex != -1 && tempIndex != -1) {
            if(isPlus) {
                arithmeticOpeation(PLUSSY, returnIndex, tempIndex, returnIndex);
            } else {
                arithmeticOpeation(MINUSSY, returnIndex, tempIndex, returnIndex);
            }
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
        if(returnIndex != -1)
            formatterTemporarySymbol(returnIndex, INT);
        int tempIndex = factor();
        if(tempIndex != -1 && symbolTable[tempIndex].type != INT)
            formatterTemporarySymbol(tempIndex, INT);
        if(returnIndex != -1 && tempIndex != -1) {
            if(isMultiply) {
                arithmeticOpeation(MULTISY, returnIndex, tempIndex, returnIndex);
            } else {
                arithmeticOpeation(DIVISY, returnIndex, tempIndex, returnIndex);
            }
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
    if(symbol == IDSY || symbol == MAINSY) { // identifier or call statement
        strcpy(identifier, token);
        getSymbol();
        if(symbol == LPARSY) { // call statement
            returnIndex = callStatementModified(startLineIndex, startColumnIndex);
        } else {
            int index = findSymbol(identifier);
            if(index == -1) {
                addError(31);
                return -1;
            }
            if(symbol == LBRASY) { // array
                getSymbol();
                if(symbolTable[index].kind != ARRAY) {
                    addError(41);
                    index = -1;
                }
                if(index != -1)
                    returnIndex = generateTemporarySymbol(symbolTable[index].type); // may be CHAR
                int offsetIndex = expression();
                if(offsetIndex != -1 && symbolTable[offsetIndex].type != INT)
                    formatterTemporarySymbol(offsetIndex, INT);
                // WARNING: if return value is less than zero or large than or equal to the size of array, undefined behavior would be caused
                if(index != -1 && offsetIndex != -1 && returnIndex != -1)
                    loadArrayElement(index, offsetIndex, returnIndex);
                revokeTemporarySymbol(offsetIndex);
                if(symbol != RBRASY) {
                    addError(12);
                    // do nothing
                } else {
                    getSymbol();
                }
            } else {
                if(!hasValue(index)) {
                    addError(41);
                } else {
                    returnIndex = index;
                }
            }
        }
    } else if(symbol == LPARSY) { // expression
        getSymbol();
        returnIndex = expression();
        if(symbol != RPARSY) {
            addError(10);
            // do nothing
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
            addError(stage ? 20 : 44);
            if(symbol == LPARSY) { // maybe expression
                int returnIndex = expression();
                if(returnIndex != -1 && stage == -1) {
                    formatterTemporarySymbol(returnIndex, INT);
                    arithmeticOpeation(MINUSSY, -1, returnIndex, returnIndex);
                }
                return returnIndex;
            } else {
                return -1;
            }
        } else if(stage && !number) {
            addWarning(4);
        }
        returnIndex = generateTemporarySymbol(INT);
        int returnValue;
        if(stage < 0) {
            if(number > (unsigned)INT_MAX + 1) {
                returnValue = INT_MIN;
            } else {
                returnValue = 0 - number;
            }
        } else {
            if(number > (unsigned)INT_MAX) {
                if(number == (unsigned)INT_MAX + 1)
                    addWarning(5);
                returnValue = INT_MAX;
            } else {
                returnValue = number;
            }
        }
        getSymbol();
        if(returnIndex != -1)
            storeImmediate(returnIndex, returnValue);
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a factor from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
    return returnIndex;
}

// <identifier> are already read (in static)
int callStatementModified(int startLineIndex, int startColumnIndex) {
//    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
//    if(symbol != IDSY && symbol != MAINSY) { // should never be reached
//        addError(0);
//        return;
//    }
//    strcpy(identifier, token);
//    getSymbol();
    int index = findSymbol(identifier), returnIndex = -1;
    if(index == -1 || symbolTable[index].kind != FUNCTION) {
        addError(32);
        index = -1;
    } else if(strcmp(symbolTable[index].name, "main") == 0) {
        addWarning(8);
    }
    if(symbol != LPARSY) { // should never be reached
        addError(0);
        return -1;
    } else {
        getSymbol();
    }
    returnIndex = valueParameterTable(index);
    if(symbol != RPARSY) {
        addError(10);
        // do nothing
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
    if(calleeIndex != -1 && symbolTable[calleeIndex].type != VOID) {
        returnIndex = generateTemporarySymbol(symbolTable[calleeIndex].type);
    }
    int parameterCount = 0, parameterBaseIndex = temporaryBaseIndex + temporaryCount;
    if(symbol != RPARSY) {
        while(true) {
            int tempIndex = expression();
            ++parameterCount;
            if(calleeIndex != -1 && parameterCount <= symbolTable[calleeIndex].value) {
                if(tempIndex != -1 && symbolTable[tempIndex].type != symbolTable[calleeIndex + parameterCount].type)
                    addWarning(symbolTable[tempIndex].type == INT ? 10 : 11);
                formatterTemporarySymbol(tempIndex, symbolTable[calleeIndex + parameterCount].type);
                pushParameter(tempIndex);
            }
            if(symbol != COMMASY)
                break;
            getSymbol();
        }
    }
    if(calleeIndex != -1) {
        if(parameterCount != symbolTable[calleeIndex].value) {
            addError(38);
            // do nothing
            return -1;
        }
        userCall(calleeIndex, returnIndex);
        // after callee finished, free (not real, just revoke) parameters (caller's temporary variable)
        while(parameterCount) {
            revokeTemporarySymbol(parameterBaseIndex + parameterCount);
            --parameterCount;
        }
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
        // do nothing
    } else {
        getSymbol();
    }
    int index = -1;
    if(symbol != IDSY && symbol != MAINSY) {
        addError(21);
        skipSymbol(NSP);
    } else {
        index = findSymbol(token);
        if(index == -1) {
            addError(31);
            // do nothing
        } else {
            if(!isVariable(index))
                addError(33);
            getSymbol();
        }
        if(symbol != ASSIGNSY) {
            addError(17);
            skipSymbol(NSP);
        } else {
            getSymbol();
            int initIndex = expression();
            if(index != -1 && initIndex != -1 && isVariable(index))
                arithmeticOpeation(ASSIGNSY, initIndex, -1, index);
            revokeTemporarySymbol(initIndex);
            jumpLabel(loopBlockLabel);
        }
    }
    if(symbol != SEMISY) {
        addError(15);
        // do nothing
    } else {
        getSymbol();
    }
    setLabel(loopConditionLabel);
    checkingIndex = index;
    checkingResult = false;
    condition(loopEndLabel);
    jumpLabel(loopBlockLabel);
    if(index != -1 && !checkingResult)
        addError(23);
    checkingIndex = -1;
    if(symbol != SEMISY) {
        addError(15);
        // do nothing
    } else {
        getSymbol();
    }
    setLabel(loopIterationLabel);
    if(symbol != IDSY && symbol != MAINSY) {
        addError(21);
        skipSymbol(NSP);
    } else {
        if(index != -1 && findSymbol(token) != index)
            addError(24);
        getSymbol();
        if(symbol != ASSIGNSY) {
            addError(17);
            skipSymbol(NSP);
        } else {
            getSymbol();
            if(symbol != IDSY && symbol != MAINSY) {
                addError(21);
                skipSymbol(NSP);
            } else {
                if(index != -1 && findSymbol(token) != index)
                    addError(24);
                getSymbol();
                if(symbol != PLUSSY && symbol != MINUSSY) {
                    addError(18);
                    skipSymbol(NSP);
                } else {
                    typeSymbol = symbol;
                    getSymbol();
                    if(symbol != NUMSY || !number) {
                        addError(20);
                        skipSymbol(NSP);
                    } else {
                        if(number > (unsigned)INT_MAX) {
                            if(number == (unsigned)INT_MAX + 1)
                                addWarning(5);
                        }
                        int tempIndex = generateTemporarySymbol(INT, number < (unsigned)INT_MAX ? number : INT_MAX);
                        if(index != -1 && isVariable(index)) {
                            if(typeSymbol == PLUSSY) {
                                arithmeticOpeation(PLUSSY, index, tempIndex, index);
                            } else { // MINUSSY
                                arithmeticOpeation(MINUSSY, index, tempIndex, index);
                            }
                        }
                        revokeTemporarySymbol(tempIndex);
                        getSymbol();
                        jumpLabel(loopConditionLabel);
                    }
                }
            }
        }
    }
    if(symbol != RPARSY) {
        addError(10);
        // do nothing
    } else {
        getSymbol();
    }
    setLabel(loopBlockLabel);
    statement();
    jumpLabel(loopIterationLabel);
    setLabel(loopEndLabel);
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a for statement from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

// <identifier> are already read
void assignStatementModified(int startLineIndex, int startColumnIndex) {
//    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
//    if(symbol != IDSY && symbol != MAINSY) { // should never be reached
//        addError(0);
//        return;
//    }
//    strcpy(identifier, token);
//    getSymbol();
    int leftIndex = findSymbol(identifier), leftOffsetIndex = -1, rightIndex = -1;
    if(leftIndex == -1) {
        addError(31);
        // do nothing
    }
    if(symbol == LBRASY) { // array
        if(leftIndex == -1 || symbolTable[leftIndex].kind != ARRAY) {
            if(leftIndex != -1) {
                addError(31);
                leftIndex = -1;
            }
        }
        getSymbol();
        leftOffsetIndex = expression();
        if(leftOffsetIndex != -1 && symbolTable[leftOffsetIndex].type != INT)
            formatterTemporarySymbol(leftOffsetIndex, INT);
        // WARNING: if return value is less than zero or large than or equal to the size of array, undefined behavior would be caused
        if(symbol != RBRASY) {
            addError(12);
            // do nothing
        } else {
            getSymbol();
        }
    } else if(leftIndex != -1 && !isVariable(leftIndex)) {
        addError(33);
        // do nothing
    }
    if(symbol != ASSIGNSY) {
        addError(17);
        skipSymbol(NTYPST);
        return;
    } else {
        getSymbol();
        rightIndex = expression();
    }
    if(leftIndex != -1 && rightIndex != -1) {
        if(symbolTable[leftIndex].type != symbolTable[rightIndex].type)
            addWarning(symbolTable[rightIndex].type == INT ? 10 : 11);
        if(symbolTable[leftIndex].kind == ARRAY) {
            if(leftOffsetIndex != -1)
                storeArrayElement(leftIndex, leftOffsetIndex, rightIndex);
        } else if(isVariable(leftIndex)) {
            arithmeticOpeation(ASSIGNSY, rightIndex, -1, leftIndex);
        }
    }
    revokeTemporarySymbol(rightIndex);
    revokeTemporarySymbol(leftOffsetIndex);
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
        // do nothing
    }
    do {
        getSymbol(); // LPAR or COMMA
        int index;
        if(symbol != IDSY && symbol != MAINSY) {
            addError(21);
            skipSymbol(NCP);
            continue;
        }
        index = findSymbol(token);
        if(index == -1) {
            addError(31);
            // do nothing
        } else if(!isVariable(index)) {
            addError(33);
            // do nothing
        }
        getSymbol();
        if(index != -1) {
            if(symbolTable[index].type == INT) {
                sysCall(5, -1, index); // read INT
            } else { // CHAR
                sysCall(12, -1, index); // read CHAR
            }
        }
    } while(symbol == COMMASY);
    if(symbol != RPARSY) {
        addError(10);
        // do nothing
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
        // do nothing
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
            if(index != -1) {
                if(symbolTable[index].type == INT) {
                    sysCall(1, index, -1); // write INT
                } else { // CHAR
                    sysCall(11, index, -1); // write CHAR
                }
            }
            revokeTemporarySymbol(index);
        }
    } else {
        int index = expression();
        // WARNING: if expression is character and not visible
        if(index != -1) {
            if(symbolTable[index].type == INT) {
                sysCall(1, index, -1); // write INT
            } else { // CHAR
                sysCall(11, index, -1); // write CHAR
            }
        }
        revokeTemporarySymbol(index);
    }
    if(symbol != RPARSY) {
        addError(10);
        // do nothing
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
        // do nothing
    } else {
        getSymbol();
    }
    int index = expression();
    if(symbol != RPARSY) {
        addError(10);
        // do nothing
    } else {
        getSymbol();
    }
    if(symbol != BEGINSY) {
        addError(13);
        // do nothing
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
        // do nothing
    } else {
        getSymbol();
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a case statement from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

void caseTable(int expIndex, int switchEndLabel) {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    set<int> caseSet;
    int caseCount = 0;
    while(symbol == CASESY) {
        caseSubstatement(expIndex, switchEndLabel, ++caseCount, caseSet);
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a case table from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}

void caseSubstatement(int expIndex, int switchEndLabel, int caseLabelIndex, set<int> &caseSet) {
    int startLineIndex = currentFrontLineIndex, startColumnIndex = currentFrontColumnIndex;
    if(symbol != CASESY) { // should never be reached
        addError(0);
        return;
    } else {
        getSymbol();
    }
    int caseIndex = -1, caseValue;
    int caseEndLabel = generateLabel(CASESY, 1, caseLabelIndex, switchEndLabel);
    if(symbol == CHSY) {
        caseValue = token[0];
        caseIndex = generateTemporarySymbol(CHAR, caseValue);
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
            addError(20);
            skipSymbol(NC);
        } else {
            if(stage && !number) {
                addWarning(4);
            }
            if(stage < 0) {
                if(number > (unsigned)INT_MAX + 1) {
                    caseValue = INT_MIN;
                } else {
                    caseValue = 0 - number;
                }
            } else {
                if(number > (unsigned)INT_MAX) {
                    if(number == (unsigned)INT_MAX + 1)
                        addWarning(5);
                    caseValue = INT_MAX;
                } else {
                    caseValue = number;
                }
            }
            caseIndex = generateTemporarySymbol(INT, caseValue);
            getSymbol();
        }
    }
    if(symbol != COLONSY) {
        addError(16);
        // do nothing
    } else {
        getSymbol();
    }
    if(caseIndex != -1 && expIndex != -1) {
        if(caseSet.count(caseValue)) {
            addError(42);
        } else {
            caseSet.insert(caseValue);
        }
        if(symbolTable[expIndex].type != symbolTable[caseIndex].type)
            addWarning(9);
        branchLabel(EQSY, expIndex, caseIndex, caseEndLabel);
        revokeTemporarySymbol(caseIndex);
    }
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
        addError(43);
        return;
    } else {
        getSymbol();
    }
    if(symbol != COLONSY) {
        addError(16);
        // do nothing
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
            skipSymbol(NS);
        } else {
            userReturn(-1);
            hasReturnStatement = true;
        }
    } else {
        if(symbol == SEMISY) {
            addError(40);
            skipSymbol(NS);
        } else {
            if(symbol != LPARSY) {
                addError(9);
                // do nothing
            } else {
                getSymbol();
            }
            int returnIndex = expression();
            if(symbol != RPARSY) {
                addError(10);
                // do nothing
            } else {
                getSymbol();
            }
            if(returnIndex != -1 && symbolTable[returnIndex].type != symbolTable[headerIndex].type) {
                addWarning(symbolTable[returnIndex].type == INT ? 10 : 11);
                formatterTemporarySymbol(returnIndex, symbolTable[headerIndex].type);
            }
            if(returnIndex != -1) {
                userReturn(returnIndex);
                hasReturnStatement = true;
            }
            revokeTemporarySymbol(returnIndex);
        }
    }
#ifdef SYNTAX_DEBUG
    fprintf(ferr, "There is a return statement from (line %d, column %d) to (line %d, column %d)\n", startLineIndex, startColumnIndex, lastEndLineIndex, lastEndColumnIndex);
#endif
}
