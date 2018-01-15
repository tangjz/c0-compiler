#include "error.h"

bool hasError = false;

const char *errMsg[] = {
    "", // should never be reached
    "characters in one line are out of range",
    "size of symbol table is out of range",
    "size of label table is out of range",
    "size of quad code list is out of range",
    "size of string table is out of range",
    "missing '\''",
    "missing '\"'",
    "missing '=' after '!'",
    "missing '('",
    "missing ')'",
    "missing '=' or '(' or '['",
    "missing ']'",
    "missing '{'",
    "missing '}'",
    "missing ';'",
    "missing ':'",
    "missing '='",
    "missing '+' or '-'",
    "missing constant char",
    "missing constant int",
    "missing identifier",
    "missing type identifier",
    "missing loop variable in condition",
    "missing loop variable in iteration",
    "missing return statement of non-void function",
    "missing main function",
    "unknown identifier start with digits",
    "unknown char with unknown characters",
    "unknown char with no character",
    "unknown string with unknown characters",
    "unknown identifier",
    "unknown function",
    "unknown type to be assigned",
    "unknown characters after main function",
    "invalid variable definition",
    "invalid function definition",
    "duplicated definition",
    "mismatched parameters",
    "return expression of void function",
    "return no expression of non-void function",
    "unknown type to get its value",
    "duplicated case",
    NULL
};
void addError(int index) {
    if(index >= 1 && index <= 42) {
        printf("Error (Line %d, Column %d): %s\n", currentFrontLineIndex, currentFrontColumnIndex, errMsg[index]);
#ifdef ERROR_DEBUG
        fprintf(ferr, "Error (Line %d, Column %d): %s\n", currentFrontLineIndex, currentFrontColumnIndex, errMsg[index]);
#endif
        hasError = true;
    } else {
        printf("Error (Line %d, Column %d): %s\n", currentFrontLineIndex, currentFrontColumnIndex, "unknown error");
#ifdef ERROR_DEBUG
        fprintf(ferr, "Error (Line %d, Column %d): %s\n", currentFrontLineIndex, currentFrontColumnIndex, "unknown error");
#endif
    }
    if(index <= 5) {
        printf("Fatal error occurred during compilation.\n");
#ifdef ERROR_DEBUG
        fprintf(ferr, "Fatal error occurred during compilation.\n");
#endif
        fclose(fin);
        fclose(fout);
        fclose(ferr);
        exit(0);
    }
}

const char *warnMsg[] = {
    "", // should never be reached
    "constant string length out of range",
    "constant char with multiple characters",
    "constant int with leading zeros",
    "invalid constant zero",
    "constant int out of range",
    "array size out of range",
    "ignored parameter of main function",
    "dangerous call for main function",
    "mismatched types of switch expression and case constant",
    "dangerous type conversion from 'int' to 'char'",
    "dangerous type conversion from 'char' to 'int'",
    "operation between two constants",
    "integer divided by zero",
    NULL
};
void addWarning(int index) {
    if(index >= 1 && index <= 11) {
        printf("Warning (Line %d, Column %d): %s\n", currentFrontLineIndex, currentFrontColumnIndex, warnMsg[index]);
#ifdef ERROR_DEBUG
        fprintf(ferr, "Warning (Line %d, Column %d): %s\n", currentFrontLineIndex, currentFrontColumnIndex, warnMsg[index]);
#endif
    } else {
        printf("Warning (Line %d, Column %d): %s\n", currentFrontLineIndex, currentFrontColumnIndex, "unknown warning");
#ifdef ERROR_DEBUG
        fprintf(ferr, "Warning (Line %d, Column %d): %s\n", currentFrontLineIndex, currentFrontColumnIndex, "unknown warning");
#endif
    }
}
