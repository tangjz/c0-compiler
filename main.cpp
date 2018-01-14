#include "common.h"
#include "error.h"
#include "scanner.h"
#include "syntax.h"
#include "pseudo.h"
#include "optimize.h"
#include "transfrom.h"
#include <io.h>

char filePath[BUF_SIZE];
FILE *fin, *fout, *ferr;

int main() {
    while(true) {
        printf("Please type the path of input source file (e.g. a.c): ");
        if(gets(filePath) != NULL && access(filePath, 4) != -1) {
            fin = fopen(filePath, "r");
            break;
        } else {
            puts("Invalid.");
        }
    }
    while(true) {
        printf("Please type the path of output source file (e.g. a.asm): ");
        if(gets(filePath) != NULL && (access(filePath, 2) != -1 || access(filePath, 0) == -1)) {
            fout = fopen(filePath, "w");
            break;
        } else {
            puts("Invalid.");
        }
    }
    while(true) {
        printf("Please type the path of auxiliary source file (e.g. a.txt): ");
        if(gets(filePath) != NULL && (access(filePath, 2) != -1 || access(filePath, 0) == -1)) {
            ferr = fopen(filePath, "w");
            break;
        } else {
            puts("Invalid.");
        }
    }
    getSymbol();
#ifdef SCANNER_DEBUG
    for(int counter = 1; symbol != NOTSY; ++counter) {
        fprintf(fout, "%d ", counter);
        printSymbol();
        getSymbol();
    }
#else
    program();
#endif
    if(hasError) {
        puts("Complication Error.");
        return -1;
    }
    puts("Complication Finished.");
#ifdef OPTIMIZATION
    divideBlocks();
    #ifdef COMMON_SUBEXPRESSION_ELIMINATION
        commonSubexpressionElimination();
    #endif
#endif
#ifdef FOURCODE_OUTPUT
    printCodeList(); // print to fout
#endif
    convertMIPS();
    return 0;
}
