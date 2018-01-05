#include "error.h"

int errorCount, errorSequence[ERROR_MAX];

void error(int errNo) {
    if(errNo) {
        // TODO
    } else {
        printf("Line %d, Column %d: Fatal error occurred during compilation.\n", currentFrontLineIndex, currentFrontColumnIndex);
        fclose(fin);
        fclose(fout);
        fclose(ferr);
        exit(0);
    }
}
