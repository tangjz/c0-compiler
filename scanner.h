#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED

#include "common.h"
#include "error.h"

extern char ch, token[TOKEN_MAX];
extern unsigned number;
extern enum SYMBOL {
	NOTSY, EQSY, NEQSY, LESSY, LEQSY, GRESY, GEQSY, PLUSSY, MINUSSY, MULTISY, DIVISY,
    ASSIGNSY, COMMASY, SEMISY, COLONSY, LPARSY, RPARSY, LBRASY, RBRASY, BEGINSY, ENDSY,
    IDSY, NUMSY, CHSY, STRSY, INTSY, CHARSY, CONSTSY, VOIDSY, MAINSY, READSY, WRITESY, RETURNSY,
    IFSY, ELSESY, FORSY, SWITCHSY, CASESY, DEFAULTSY
} symbol; // quotes (single/double) are resolved
extern int lineIndex, columnIndex, currentFrontLineIndex, currentFrontColumnIndex, lastEndLineIndex, lastEndColumnIndex;

extern bool isDigit();
extern void getSymbol();
//extern void printSymbol();

#endif // SCANNER_H_INCLUDED
