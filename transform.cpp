#include "transfrom.h"

//int codeCount;
//struct QUADCODE {
//    char op[OPER_MAX], lft[TOKEN_MAX], rht[TOKEN_MAX], dst[TOKEN_MAX];
//    // lft, rht may be identifier or number, dst must be identifier
//} codeList[CODE_MAX];

const char *regName[REG_MAX] = {
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};
const int $zero = 0, $v0 = 2, $a0 = 4, $gp = 28, $sp = 29, $ra = 31;
const int localRegLow = 8, localRegUpp = 25;
int regRef[TABLE_SIZE];
bool regAvailable[REG_MAX];

//TODO: global register allocation should be implemented in here
void newReg(int &id) {
    if(id != -1)
        return;
    for(id = localRegLow; id <= localRegUpp; ++id)
        if(regAvailable[id]) {
            regAvailable[id] = false;
            return;
        }
    assert(false);
}
void freeReg(int &id) {
    if(id >= localRegLow && id <= localRegUpp) {
        assert(!regAvailable[id]);
        regAvailable[id] = true;
        id = -1;
    }
}

#ifdef PEEPHOLE_OPTIMIZATION
char lastOp[TOKEN_MAX];
int lastRegMemRt, lastRegMemRs, lastRegMemOffset;
#endif
void printReg(const char *op, int rd) { // mflo
    assert(strcmp(op, "mflo") == 0);
    fprintf(fout, "\t" "%s %s" "\n", op, regName[rd]);
#ifdef PEEPHOLE_OPTIMIZATION
    strcpy(lastOp, op);
#endif
}
void printRegReg(const char *op, int rs, int rt) { // mult, div
    fprintf(fout, "\t" "%s %s, %s" "\n", op, regName[rs], regName[rt]);
#ifdef PEEPHOLE_OPTIMIZATION
    strcpy(lastOp, op);
#endif
}
void printRegReg(const char *op, int rd, int rs, int rt) { // add, sub
#ifdef PEEPHOLE_OPTIMIZATION
    if((rt == $zero && rd == rs) || (rs == $zero && rd == rt && strcmp(op, "add") == 0))
        return;
#endif
    fprintf(fout, "\t" "%s %s, %s, %s" "\n", op, regName[rd], regName[rs], regName[rt]);
#ifdef PEEPHOLE_OPTIMIZATION
    strcpy(lastOp, op);
#endif
}
void printRegImm(const char *op, int rt, int offset) { // li
    assert(strcmp(op, "li") == 0);
    fprintf(fout, "\t" "%s %s, %d" "\n", op, regName[rt], offset);
#ifdef PEEPHOLE_OPTIMIZATION
    strcpy(lastOp, op);
#endif
}
void printRegImm(const char *op, int rt, const char *label) { // la
    assert(strcmp(op, "la") == 0);
    fprintf(fout, "\t" "%s %s, %s" "\n", op, regName[rt], label);
#ifdef PEEPHOLE_OPTIMIZATION
    strcpy(lastOp, op);
#endif
}
void printRegImm(const char *op, int rt, int rs, int offset) { // addi, subi
    if(offset > OFFSET_MAX) {
        int tp = -1;
        newReg(tp);
        printRegImm("li", tp, offset);
        printRegReg(op, rt, rs, tp);
        freeReg(tp);
    } else if(offset) { // add -> addi
        fprintf(fout, "\t" "%si %s, %s, %d" "\n", op, regName[rt], regName[rs], offset);
#ifdef PEEPHOLE_OPTIMIZATION
        strcpy(lastOp, op);
        strcat(lastOp, "i");
#endif
    } else {
        printRegReg(op, rt, rs, $zero);
    }
}
void printRegMem(const char *op, int rt, int rs, int offset) { // lb, lw, sb, sw
#ifdef PEEPHOLE_OPTIMIZATION
    if(lastOp[1] == op[1] && (lastOp[0] == 'l' || lastOp[0] == 's') && lastOp[0] != op[0]
       && lastRegMemRt == rt && lastRegMemRs == rs && lastRegMemOffset == offset)
        return;
#endif
    if(offset > OFFSET_MAX) {
        int tp = -1;
        newReg(tp);
        printRegImm("li", tp, offset);
        printRegReg("add", tp, rs, tp);
        fprintf(fout, "\t" "%s %s, 0(%s)" "\n", op, regName[rt], regName[tp]);
        freeReg(tp);
    } else {
        fprintf(fout, "\t" "%s %s, %d(%s)" "\n", op, regName[rt], offset, regName[rs]);
    }
#ifdef PEEPHOLE_OPTIMIZATION
        strcpy(lastOp, op);
        lastRegMemRt = rt;
        lastRegMemRs = rs;
        lastRegMemOffset = offset;
#endif
}
void printLabel(const char *label, char endc) { // label:
    static char token[TOKEN_MAX];
    strcpy(token, label);
    for(char *ptr = token; *ptr; ++ptr)
        if(*ptr == '@')
            *ptr = '_';
    fprintf(fout, "%s:" "%c", token, endc);
}
void printJump(const char *op, int rs) { // jr
    fprintf(fout, "\t" "%s %s" "\n", op, regName[rs]);
#ifdef PEEPHOLE_OPTIMIZATION
        strcpy(lastOp, op);
#endif
}
void printJump(const char *op, const char *label) { // j, jal
    static char token[TOKEN_MAX];
    strcpy(token, label);
    for(char *ptr = token; *ptr; ++ptr)
        if(*ptr == '@')
            *ptr = '_';
    fprintf(fout, "\t" "%s %s" "\n", op, token);
#ifdef PEEPHOLE_OPTIMIZATION
        strcpy(lastOp, op);
#endif
}
void printBranch(const char *op, int rs, int rt, const char *label) { // bne, beq, bge, bgt, ble, blt
    static char token[TOKEN_MAX];
    strcpy(token, label);
    for(char *ptr = token; *ptr; ++ptr)
        if(*ptr == '@')
            *ptr = '_';
    fprintf(fout, "\t" "%s %s, %s, %s" "\n", op, regName[rs], regName[rt], token);
#ifdef PEEPHOLE_OPTIMIZATION
        strcpy(lastOp, op);
#endif
}
void printSyscall() {
    fprintf(fout, "\t" "syscall" "\n");
#ifdef PEEPHOLE_OPTIMIZATION
        strcpy(lastOp, "syscall");
#endif
}

int symbolOffset[TABLE_SIZE]; // offset from $gp or $sp
int stringCount, stringIndex[CODE_MAX];

static bool isLetter(char ch) {
	return ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

void convertMIPS() {
    fprintf(fout, "#Copyright: Jingzhe Tang, Jan. 5th, 2018\n");
    // step 1. convert string and save them in data segment
    fprintf(fout, ".data" "\n");
    for(int index = 0; index < codeCount; ++index) {
        QUADCODE &cur = codeList[index];
        if(strcmp(cur.op, "syscall") == 0 && strcmp(cur.lft, "4") == 0) {
            int pos;
            for(pos = stringCount; pos; --pos)
                if(strcmp(cur.rht, codeList[stringIndex[pos]].rht) == 0)
                    break;
            if(pos)
                continue;
            if(stringCount == TABLE_SIZE - 1)
                addError(5);
            stringIndex[++stringCount] = index;
            fprintf(fout, "\t" "string_%d:" "\t" ".asciiz \"", stringCount);
            for(char *ptr = cur.rht; *ptr; ++ptr) {
                if(*ptr == '\\')
                    fputc('\\', fout);
                fputc(*ptr, fout);
            }
            fprintf(fout, "\"" "\n");
        }
    }
    // step 2. calculate function variables offset
    int globalSize = 0;
    {
        int index = 0;
        for( ; index < symbolCount && symbolTable[index].kind == CONST; ++index)
            assert(symbolTable[index].isGlobal);
        int tempIndex = index;
        for( ; index < symbolCount && (symbolTable[index].kind == VARIABLE || symbolTable[index].kind == ARRAY); ++index) {
            assert(symbolTable[index].isGlobal);
            globalSize += symbolTable[index].storeSize;
        }
        for(int tempSize = globalSize; tempIndex < index; ++tempIndex) {
            tempSize -= symbolTable[tempIndex].storeSize;
            symbolOffset[tempIndex] = tempSize;
        }
        while(index < symbolCount) {
            assert(symbolTable[index].isGlobal && symbolTable[index].kind == FUNCTION);
            // data stack (higher to lower): parameter variables, return address, local variables ($sp), temporary variables
            int localVariableOffset = symbolTable[index].storeSize;
            int parameterOffset = localVariableOffset + (symbolTable[index].value + 1) * BYTE_PER_INT;
            ++index;
            for( ; index < symbolCount && symbolTable[index].kind == PARAMETER; ++index) {
                assert(!symbolTable[index].isGlobal);
                parameterOffset -= BYTE_PER_INT;
                symbolOffset[index] = parameterOffset;
            }
            for( ; index < symbolCount && symbolTable[index].kind == CONST; ++index)
                assert(!symbolTable[index].isGlobal);
            for( ; index < symbolCount && (symbolTable[index].kind == VARIABLE || symbolTable[index].kind == ARRAY); ++index) {
                assert(!symbolTable[index].isGlobal);
                localVariableOffset -= symbolTable[index].storeSize;
                symbolOffset[index] = localVariableOffset;
            }
        }
    }
    // step 3. construct codes
    for(int id = localRegLow; id <= localRegUpp; ++id)
        regAvailable[id] = true;
    fprintf(fout, ".text" "\n");
    { // allocate global variables
        printRegMem("sw", $gp, $sp, -BYTE_PER_INT);
        printRegImm("sub", $sp, $sp, globalSize + BYTE_PER_INT);
        printRegReg("add", $gp, $sp, $zero);
    }
    int realLastSymbolIndex = lastSymbolIndex;
#ifdef DEAD_CODE_ELIMINATION
    int currentBlockIndex = 0;
#endif
    for(int codeIndex = 0, functionIndex = -1; codeIndex < codeCount; ++codeIndex) {
        QUADCODE &cur = codeList[codeIndex];
#ifdef TRANSFORM_DEBUG
        if(strcmp(cur.op, "syscall") == 0 && strcmp(cur.lft, "4") == 0) {
            fprintf(fout, "#%d: %s, %s, \"", codeIndex, cur.op, cur.lft);
            for(char *ptr = cur.rht; *ptr; ++ptr) {
                if(*ptr == '\\') {
                    fputc('\\', fout);
                }
                fputc(*ptr, fout);
            }
            fprintf(fout, "\", %s\n", cur.dst);
        } else {
            fprintf(fout, "#%d: %s, %s, %s, %s\n", codeIndex, cur.op, cur.lft, cur.rht, cur.dst);
        }
#endif
        if(strncmp(cur.op, "def", 3) == 0 || strcmp(cur.op, "pushPara") == 0)
            continue;
#ifdef DEAD_CODE_ELIMINATION
        for( ; currentBlockIndex < blockCount && codeIndex >= blockStartIndex[currentBlockIndex + 1]; ++currentBlockIndex);
        if(!blockReachable[currentBlockIndex])
            continue;
#endif
        int index = -1, offset = 0, $rs = $zero, $rt = $zero, $rd = $zero;
        bool hasImmediate = false;
        if(strcmp(cur.op, "add") == 0 || strcmp(cur.op, "sub") == 0) { // add/sub, left, right, destination
            newReg($rd);
            if(isLetter(cur.lft[0]) && ((index = findSymbol(cur.lft)) == -1 || symbolTable[index].kind != CONST)) {
                newReg($rs);
                if(index == -1) { // temporary variable
                    printRegMem("lw", $rs, $sp, -getTemporaryIndex(cur.lft) * BYTE_PER_INT);
                } else {
                    printRegMem(symbolTable[index].type == INT ? "lw" : "lb", $rs, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
                }
            } else {
                hasImmediate = true;
                if(!isLetter(cur.lft[0])) {
                    sscanf(cur.lft, "%d", &offset);
                } else {
                    offset = symbolTable[index].value;
                }
                if(!offset) {
                    $rs = $zero;
                } else {
                    newReg($rs);
                    printRegImm("li", $rs, offset);
                }
            }
            if(isLetter(cur.rht[0]) && ((index = findSymbol(cur.rht)) == -1 || symbolTable[index].kind != CONST)) {
                newReg($rt);
                if(index == -1) { // temporary variable
                    printRegMem("lw", $rt, $sp, -getTemporaryIndex(cur.rht) * BYTE_PER_INT);
                } else {
                    printRegMem(symbolTable[index].type == INT ? "lw" : "lb", $rt, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
                }
                printRegReg(cur.op, $rd, $rs, $rt);
            } else {
                if(hasImmediate)
                    puts("There exists operation between two constants (could be optimized).");
                if(!isLetter(cur.rht[0])) {
                    sscanf(cur.rht, "%d", &offset);
                } else {
                    offset = symbolTable[index].value;
                }
                if(!offset) {
                    printRegReg(cur.op, $rd, $rs, $zero);
                } else {
                    printRegImm(cur.op, $rd, $rs, offset);
                }
            }
            index = findSymbol(cur.dst);
            if(index == -1) { // temporary variable
                printRegMem("sw", $rd, $sp, -getTemporaryIndex(cur.dst) * BYTE_PER_INT);
            } else {
                printRegMem(symbolTable[index].type == INT ? "sw" : "sb", $rd, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
            }
        } else if(strcmp(cur.op, "mult") == 0 || strcmp(cur.op, "div") == 0) { // mult/div, left, right, destination
            if(isLetter(cur.lft[0]) && ((index = findSymbol(cur.lft)) == -1 || symbolTable[index].kind != CONST)) {
                newReg($rs);
                if(index == -1) { // temporary variable
                    printRegMem("lw", $rs, $sp, -getTemporaryIndex(cur.lft) * BYTE_PER_INT);
                } else {
                    printRegMem(symbolTable[index].type == INT ? "lw" : "lb", $rs, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
                }
            } else {
                hasImmediate = true;
                if(!isLetter(cur.lft[0])) {
                    sscanf(cur.lft, "%d", &offset);
                } else {
                    offset = symbolTable[index].value;
                }
                if(!offset) {
                    $rs = $zero;
                } else {
                    newReg($rs);
                    printRegImm("li", $rs, offset);
                }
            }
            if(isLetter(cur.rht[0]) && ((index = findSymbol(cur.rht)) == -1 || symbolTable[index].kind != CONST)) {
                newReg($rt);
                if(index == -1) { // temporary variable
                    printRegMem("lw", $rt, $sp, -getTemporaryIndex(cur.rht) * BYTE_PER_INT);
                } else {
                    printRegMem(symbolTable[index].type == INT ? "lw" : "lb", $rt, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
                }
            } else {
                if(hasImmediate)
                    puts("There exists operation between two constants (could be optimized).");
                if(!isLetter(cur.rht[0])) {
                    sscanf(cur.rht, "%d", &offset);
                } else {
                    offset = symbolTable[index].value;
                }
                if(!offset) {
                    $rt = $zero;
                } else {
                    newReg($rt);
                    printRegImm("li", $rt, offset);
                }
            }
            index = findSymbol(cur.dst);
            if($rs == $zero || $rt == $zero) {
                if($rt == $zero && strcmp(cur.op, "div") == 0)
                    puts("There exists operation divided by zero");
                if(index == -1) { // temporary variable
                    printRegMem("sw", $zero, $sp, -getTemporaryIndex(cur.dst) * BYTE_PER_INT);
                } else {
                    printRegMem(symbolTable[index].type == INT ? "sw" : "sb", $zero, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
                }
            } else {
                newReg($rd);
                printRegReg(cur.op, $rs, $rt);
                printReg("mflo", $rd);
                if(index == -1) { // temporary variable
                    printRegMem("sw", $rd, $sp, -getTemporaryIndex(cur.dst) * BYTE_PER_INT);
                } else {
                    printRegMem(symbolTable[index].type == INT ? "sw" : "sb", $rd, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
                }
            }
        } else if(strcmp(cur.op, "=") == 0) { // =, source, , destination
            if(isLetter(cur.lft[0]) && ((index = findSymbol(cur.lft)) == -1 || symbolTable[index].kind != CONST)) {
                newReg($rs);
                if(index == -1) { // temporary variable
                    printRegMem("lw", $rs, $sp, -getTemporaryIndex(cur.lft) * BYTE_PER_INT);
                } else {
                    printRegMem(symbolTable[index].type == INT ? "lw" : "lb", $rs, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
                }
            } else {
                hasImmediate = true;
                if(!isLetter(cur.lft[0])) {
                    sscanf(cur.lft, "%d", &offset);
                } else {
                    offset = symbolTable[index].value;
                }
                if(!offset) {
                    $rs = $zero;
                } else {
                    newReg($rs);
                    printRegImm("li", $rs, offset);
                }
            }
            index = findSymbol(cur.dst);
            if(index == -1) { // temporary variable
                printRegMem("sw", $rs, $sp, -getTemporaryIndex(cur.dst) * BYTE_PER_INT);
            } else {
                printRegMem(symbolTable[index].type == INT ? "sw" : "sb", $rs, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
            }
        } else if(strcmp(cur.op, "=[]") == 0) { // =[], array, offset, destination
            int arrayIndex = findSymbol(cur.lft);
            assert(arrayIndex >= 0);
            newReg($rs);
            if(isLetter(cur.rht[0]) && ((index = findSymbol(cur.rht)) == -1 || symbolTable[index].kind != CONST)) {
                printRegImm("add", $rs, symbolTable[arrayIndex].isGlobal ? $gp : $sp, symbolOffset[arrayIndex]);
                newReg($rt);
                if(index == -1) { // temporary variable
                    printRegMem("lw", $rt, $sp, -getTemporaryIndex(cur.rht) * BYTE_PER_INT);
                } else {
                    printRegMem(symbolTable[index].type == INT ? "lw" : "lb", $rt, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
                }
                if(symbolTable[arrayIndex].type == INT) {
                    int $tp = -1;
                    newReg($tp);
                    printRegImm("li", $tp, BYTE_PER_INT);
                    printRegReg("mult", $rt, $tp);
                    freeReg($tp);
                    printReg("mflo", $rt);
                }
                printRegReg("add", $rt, $rs, $rt);
                printRegMem(symbolTable[arrayIndex].type == INT ? "lw" : "lb", $rs, $rt, 0);
            } else {
                hasImmediate = true;
                if(!isLetter(cur.rht[0])) {
                    sscanf(cur.rht, "%d", &offset);
                } else {
                    offset = symbolTable[index].value;
                }
                offset = symbolOffset[arrayIndex] + offset * (symbolTable[arrayIndex].type == INT ? BYTE_PER_INT : BYTE_PER_CHAR);
                printRegMem(symbolTable[arrayIndex].type == INT ? "lw" : "lb", $rs, symbolTable[arrayIndex].isGlobal ? $gp : $sp, offset);
            }
            index = findSymbol(cur.dst);
            if(index == -1) { // temporary variable
                printRegMem("sw", $rs, $sp, -getTemporaryIndex(cur.dst) * BYTE_PER_INT);
            } else {
                printRegMem(symbolTable[index].type == INT ? "sw" : "sb", $rs, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
            }
        } else if(strcmp(cur.op, "[]=") == 0) { // []=, source, offset, array
            if(isLetter(cur.lft[0]) && ((index = findSymbol(cur.lft)) == -1 || symbolTable[index].kind != CONST)) {
                newReg($rs);
                if(index == -1) { // temporary variable
                    printRegMem("lw", $rs, $sp, -getTemporaryIndex(cur.lft) * BYTE_PER_INT);
                } else {
                    printRegMem(symbolTable[index].type == INT ? "lw" : "lb", $rs, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
                }
            } else {
                hasImmediate = true;
                if(!isLetter(cur.lft[0])) {
                    sscanf(cur.lft, "%d", &offset);
                } else {
                    offset = symbolTable[index].value;
                }
                if(!offset) {
                    $rs = $zero;
                } else {
                    newReg($rs);
                    printRegImm("li", $rs, offset);
                }
            }
            int arrayIndex = findSymbol(cur.dst);
            assert(arrayIndex >= 0);
            if(isLetter(cur.rht[0]) && ((index = findSymbol(cur.rht)) == -1 || symbolTable[index].kind != CONST)) {
                newReg($rd);
                printRegImm("add", $rd, symbolTable[arrayIndex].isGlobal ? $gp : $sp, symbolOffset[arrayIndex]);
                newReg($rt);
                if(index == -1) { // temporary variable
                    printRegMem("lw", $rt, $sp, -getTemporaryIndex(cur.rht) * BYTE_PER_INT);
                } else {
                    printRegMem(symbolTable[index].type == INT ? "lw" : "lb", $rt, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
                }
                if(symbolTable[arrayIndex].type == INT) {
                    int $tp = -1;
                    newReg($tp);
                    printRegImm("li", $tp, BYTE_PER_INT);
                    printRegReg("mult", $rt, $tp);
                    freeReg($tp);
                    printReg("mflo", $rt);
                }
                printRegReg("add", $rt, $rd, $rt);
                printRegMem(symbolTable[arrayIndex].type == INT ? "sw" : "sb", $rs, $rt, 0);
            } else {
                hasImmediate = true;
                if(!isLetter(cur.rht[0])) {
                    sscanf(cur.rht, "%d", &offset);
                } else {
                    offset = symbolTable[index].value;
                }
                offset = symbolOffset[arrayIndex] + offset * (symbolTable[arrayIndex].type == INT ? BYTE_PER_INT : BYTE_PER_CHAR);
                printRegMem(symbolTable[arrayIndex].type == INT ? "sw" : "sb", $rs, symbolTable[arrayIndex].isGlobal ? $gp : $sp, offset);
            }
        } else if(strcmp(cur.op, "label") == 0) { // label, labelString, ,
            bool nxt = codeIndex + 1 < codeCount && strcmp(codeList[codeIndex + 1].op, "label") == 0;
            printLabel(cur.lft, nxt ? '\t' : '\n');
            while(nxt) {
                ++codeIndex;
                nxt = codeIndex + 1 < codeCount && strcmp(codeList[codeIndex + 1].op, "label") == 0;
                printLabel(codeList[codeIndex].lft, nxt);
            }
        } else if(strncmp(cur.op, "j", 1) == 0) { // j, , , labelString
            printJump(cur.op, cur.dst);
        } else if(strncmp(cur.op, "b", 1) == 0) { // bne/beq/bge/bgt/ble/blt, left, right, labelString
            if(isLetter(cur.lft[0]) && ((index = findSymbol(cur.lft)) == -1 || symbolTable[index].kind != CONST)) {
                newReg($rs);
                if(index == -1) { // temporary variable
                    printRegMem("lw", $rs, $sp, -getTemporaryIndex(cur.lft) * BYTE_PER_INT);
                } else {
                    printRegMem(symbolTable[index].type == INT ? "lw" : "lb", $rs, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
                }
            } else {
                hasImmediate = true;
                if(!isLetter(cur.lft[0])) {
                    sscanf(cur.lft, "%d", &offset);
                } else {
                    offset = symbolTable[index].value;
                }
                if(!offset) {
                    $rs = $zero;
                } else {
                    newReg($rs);
                    printRegImm("li", $rs, offset);
                }
            }
            if(isLetter(cur.rht[0]) && ((index = findSymbol(cur.rht)) == -1 || symbolTable[index].kind != CONST)) {
                newReg($rt);
                if(index == -1) { // temporary variable
                    printRegMem("lw", $rt, $sp, -getTemporaryIndex(cur.rht) * BYTE_PER_INT);
                } else {
                    printRegMem(symbolTable[index].type == INT ? "lw" : "lb", $rt, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
                }
            } else {
                if(hasImmediate)
                    puts("There exists operation between two constants (could be optimized).");
                if(!isLetter(cur.rht[0])) {
                    sscanf(cur.rht, "%d", &offset);
                } else {
                    offset = symbolTable[index].value;
                }
                if(!offset) {
                    $rt = $zero;
                } else {
                    newReg($rt);
                    printRegImm("li", $rt, offset);
                }
            }
            printBranch(cur.op, $rs, $rt, cur.dst);
        } else if(strcmp(cur.op, "syscall") == 0) { // syscall, serviceNumber, source, destination
            int serviceNumber = 0;
            sscanf(cur.lft, "%d", &serviceNumber);
            printRegImm("li", $v0, serviceNumber);
            if(serviceNumber == 4) {
                int pos;
                for(pos = stringCount; pos; --pos)
                    if(strcmp(cur.rht, codeList[stringIndex[pos]].rht) == 0)
                        break;
                assert(pos);
                static char token[TOKEN_MAX];
                sprintf(token, "string_%d", pos);
                printRegImm("la", $a0, token);
            } else if(serviceNumber == 1 || serviceNumber == 11) {
                if(isLetter(cur.rht[0]) && ((index = findSymbol(cur.rht)) == -1 || symbolTable[index].kind != CONST)) {
                    if(index == -1) { // temporary variable
                        printRegMem("lw", $a0, $sp, -getTemporaryIndex(cur.rht) * BYTE_PER_INT);
                    } else {
                        printRegMem(symbolTable[index].type == INT ? "lw" : "lb", $a0, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
                    }
                } else {
                    hasImmediate = true;
                    if(!isLetter(cur.rht[0])) {
                        sscanf(cur.rht, "%d", &offset);
                    } else {
                        offset = symbolTable[index].value;
                    }
                    if(!offset) {
                        printRegReg("add", $a0, $zero, $zero);
                    } else {
                        printRegImm("li", $a0, offset);
                    }
                }
            }
            printSyscall();
            if(serviceNumber == 5 || serviceNumber == 12) {
                index = findSymbol(cur.dst);
                if(index == -1) { // temporary variable
                    printRegMem("sw", $v0, $sp, -getTemporaryIndex(cur.dst) * BYTE_PER_INT);
                } else {
                    printRegMem(symbolTable[index].type == INT ? "sw" : "sb", $v0, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
                }
            }
#ifdef OUTPUT_EOLN
            if(serviceNumber == 12) { // read CHAR and then write an end of line (for safety)
                printRegImm("li", $v0, 11);
                printRegImm("li", $a0, '\n');
                printSyscall();
            }
#endif
        } else if(strcmp(cur.op, "function") == 0) { // function, name, type,
            functionIndex = findSymbol(cur.lft);
            assert(functionIndex >= 0 && strcmp(cur.rht, symbolTable[functionIndex].type == VOID ? "void" :
                                                (symbolTable[functionIndex].type == INT ? "int" : "char")) == 0);
            printRegMem("sw", $ra, $sp, -BYTE_PER_INT);
            printRegImm("sub", $sp, $sp, symbolTable[functionIndex].storeSize + BYTE_PER_INT);
            // reset symbol table to the end of function
            for(lastSymbolIndex = functionIndex; lastSymbolIndex + 1 < symbolCount && !symbolTable[lastSymbolIndex + 1].isGlobal; ++lastSymbolIndex);
        } else if(strcmp(cur.op, "endFunction") == 0) { // endFunction, name, type,
            assert(functionIndex >= 0 && functionIndex == findSymbol(cur.lft)
                && strcmp(cur.rht, symbolTable[functionIndex].type == VOID ? "void" :
                    (symbolTable[functionIndex].type == INT ? "int" : "char")) == 0);
            printRegImm("add", $sp, $sp, symbolTable[functionIndex].storeSize + BYTE_PER_INT);
            printRegMem("lw", $ra, $sp, -BYTE_PER_INT);
            printJump("jr", $ra);
            functionIndex = -1;
            // reset symbol table to global
            lastSymbolIndex = realLastSymbolIndex;
        } else if(strcmp(cur.op, "call") == 0) { // call, function, temporaryCount, result
            int calleeIndex = findSymbol(cur.lft);
            assert(calleeIndex >= 0);
            int calleeStartLabel = findFunctionLabel(calleeIndex, 1);
            int temporaryCount;
            sscanf(cur.rht, "%d", &temporaryCount);
            if(temporaryCount)
                printRegImm("sub", $sp, $sp, temporaryCount * BYTE_PER_INT);
            printJump("jal", labelTable[calleeStartLabel]);
            if(temporaryCount)
                printRegImm("add", $sp, $sp, temporaryCount * BYTE_PER_INT);
            if(strcmp(cur.dst, "") != 0) {
                index = findSymbol(cur.dst);
                if(index == -1) { // temporary variable
                    printRegMem("sw", $v0, $sp, -getTemporaryIndex(cur.dst) * BYTE_PER_INT);
                } else {
                    printRegMem(symbolTable[index].type == INT ? "sw" : "sb", $v0, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
                }
            }
        } else if(strcmp(cur.op, "return") == 0) { // return, result, ,
            if(strcmp(cur.lft, "") != 0) {
                if(isLetter(cur.lft[0]) && ((index = findSymbol(cur.lft)) == -1 || symbolTable[index].kind != CONST)) {
                    if(index == -1) { // temporary variable
                        printRegMem(symbolTable[functionIndex].type == INT ? "lw" : "lb", $v0, $sp, -getTemporaryIndex(cur.lft) * BYTE_PER_INT);
                    } else {
                        printRegMem(symbolTable[functionIndex].type == INT ? "lw" : "lb", $v0, symbolTable[index].isGlobal ? $gp : $sp, symbolOffset[index]);
                    }
                } else {
                    hasImmediate = true;
                    if(!isLetter(cur.lft[0])) {
                        sscanf(cur.lft, "%d", &offset);
                    } else {
                        offset = symbolTable[index].value;
                    }
                    if(!offset) {
                        printRegReg("add", $v0, $zero, $zero);
                    } else {
                        printRegImm("li", $v0, offset);
                    }
                }
            }
            int functionEndLabel = findFunctionLabel(functionIndex, 2);
            printJump("j", labelTable[functionEndLabel]);
        } else {
            assert(false);
        }
        freeReg($rs);
        freeReg($rt);
        freeReg($rd);
    }
    { // free global variables
        printRegImm("add", $sp, $sp, globalSize + BYTE_PER_INT);
        printRegMem("lw", $gp, $sp, -BYTE_PER_INT);
    }
    lastSymbolIndex = realLastSymbolIndex;
}
