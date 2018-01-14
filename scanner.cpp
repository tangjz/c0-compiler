#include "scanner.h"

const char *reserver[] = {"int", "char", "const", "void", "main", "scanf", "printf", "return", "if", "else", "for", "switch", "case", "default", NULL};
const char *pattern = "+-*/=,;:()[]{}";

char buffer[BUF_SIZE];

char ch = ' ', token[TOKEN_MAX] = {}, *tokenTail = token; // TODO: be careful when output '\\' in MARS
unsigned number = 0;
enum SYMBOL symbol = NOTSY;
int lineIndex = 0, columnIndex = 0, columnLimit = 0;
int currentFrontLineIndex = 0, currentFrontColumnIndex = 0;
int lastEndLineIndex = 0, lastEndColumnIndex = 0;
bool tokenOverlong = false;

void getChar() {
    if(columnIndex == columnLimit) { // get a new line
        if(feof(fin)) {
            ch = EOF;
            return;
        }
        ++lineIndex;
        columnIndex = columnLimit = 0;
        for(char tp; !feof(fin) && (tp = fgetc(fin)) != '\n'; buffer[columnLimit++] = tp);
        buffer[columnLimit++] = '\n';
        if(columnLimit > BUF_SIZE)
            addError(1);
#if (defined SCANNER_DEBUG) || (defined SYNTAX_DEBUG) || (defined SEMANTIC_DEBUG)
        fprintf(ferr, "Scanner: read a new line %d with %d characters.\n", lineIndex, columnLimit);
#endif
    }
    // ch = fgetc(fin);
    ch = buffer[columnIndex++];
}
void revokeChar(char preChar) {
    ungetc(ch, fin);
    ch = preChar;
}
void clearToken() {
    tokenOverlong = false;
    tokenTail = token;
    *tokenTail = '\0';
}
void catToken() {
    if(tokenTail - token == TOKEN_MAX) {
        if(!tokenOverlong) {
            tokenOverlong = true;
            addWarning(1);
        }
    } else {
        *(tokenTail++) = ch;
        *tokenTail = '\0';
    }
}
bool isEof() {
    return ch == EOF;
}
bool isBlank() {
	return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}
bool isLetter() {
	return ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}
bool isDigit() {
	return ch >= '0' && ch <= '9';
}
bool isSingleChar() {
    return ch == '+' || ch == '-' || ch == '*' || ch == '/' || isLetter() || isDigit();
}
bool isVisible() {
    return ch != 34 && ch >= 32 && ch <= 126;
}
int getReserver() {
	for(int i = 0; reserver[i] != NULL; ++i)
		if(strcmp(token, reserver[i]) == 0)
			return INTSY + i;
	return 0;
}
unsigned getNumber() {
	unsigned val = 0, pre = 0;
	for(char *ptr = token; ptr != tokenTail; ++ptr) {
        pre = val;
		val = (val << 3) + (val << 1) + *ptr - '0';
		if(val < pre) { // overflow
            addWarning(5);
            return UINT_MAX;
		}
	}
	if(val > (unsigned)INT_MAX + 1)
        addWarning(5);
	return val;
}
void getSymbol() {
    lastEndLineIndex = lineIndex;
    lastEndColumnIndex = columnIndex;
	for(clearToken(); isBlank(); getChar());
	currentFrontLineIndex = lineIndex;
	currentFrontColumnIndex = columnIndex;
	if(isEof()) {
        symbol = NOTSY;
	} else if(isLetter()) {
		while(isLetter() || isDigit()) {
			catToken();
			getChar();
		} // else token is collected
		int retVal = getReserver();
		symbol = retVal != 0 ? (SYMBOL)retVal : IDSY;
	} else if(isDigit()) {
	    bool hasLetter = false; // support to recognize wrong identifier
		while(isDigit() || isLetter()) {
            hasLetter |= isLetter();
			catToken();
			getChar();
		} // else number (or wrong identifier) is collected
		if(hasLetter) {
            addError(27);
            symbol = IDSY;
		} else {
            number = getNumber();
            if(token[0] == '0' && token[1] != '\0') {
                addWarning(3);
            }
            symbol = NUMSY;
		}
	} else if(ch == '\'') {
        getChar();
        if(isSingleChar()) {
            catToken();
            getChar();
            if(ch != '\'') {
                for( ; !isEof() && ch != '\'' && ch != '\n'; getChar());
                if(ch == '\'') {
                    addWarning(2);
                } else {
                    addError(6);
                }
            }
        } else if(ch != '\'') {
            catToken();
            for( ; !isEof() && ch != '\'' && ch != '\n'; getChar());
            if(ch == '\'') {
                addError(28);
            } else {
                addError(6);
            }
        } else { // ch == '\'' // no character
            addError(29);
        }
        if(ch == '\'' || ch == '\n')
            getChar();
        symbol = CHSY;
	} else if(ch == '\"') {
	    getChar();
        while(isVisible()) {
            catToken();
            getChar();
        }
        if(ch != '\"') {
            for( ; !isEof() && ch != '\"' && ch != '\n'; getChar())
                catToken();
            if(ch == '\"') {
                addError(30);
            } else {
                addError(7);
            }
        }
        if(ch == '\"' || ch == '\n')
            getChar();
        symbol = STRSY;
	} else if(ch == '=') {
		getChar();
		if(ch == '=') {
            symbol = EQSY;
            getChar();
		} else {
            symbol = ASSIGNSY;
		}
	} else if(ch == '!') {
		getChar();
		if(ch == '=') {
            getChar();
		} else {
		    addError(8);
		}
		symbol = NEQSY;
	} else if(ch == '<') {
		getChar();
		if(ch == '=') {
            symbol = LEQSY;
            getChar();
		} else {
            symbol = LESSY;
		}
	} else if(ch == '>') {
		getChar();
		if(ch == '=') {
            symbol = GEQSY;
            getChar();
		} else {
            symbol = GRESY;
		}
	} else {
		symbol = (SYMBOL)0;
		for(int i = 0; pattern[i]; ++i)
			if(ch == pattern[i]) {
				symbol = (SYMBOL)(PLUSSY + i);
				break;
			}
		if(!symbol && !isEof()) {
            symbol = NOTSY;
		} else {
            getChar();
		}
	}
}

#ifdef SCANNER_DEBUG
void printSymbol() {
    #define OUTPUT_SYMBOL(x) case x: fputs(#x " ", fout); break;
    switch(symbol) {
        OUTPUT_SYMBOL(EQSY);
        OUTPUT_SYMBOL(NEQSY);
        OUTPUT_SYMBOL(LESSY);
        OUTPUT_SYMBOL(LEQSY);
        OUTPUT_SYMBOL(GRESY);
        OUTPUT_SYMBOL(GEQSY);
        OUTPUT_SYMBOL(PLUSSY);
        OUTPUT_SYMBOL(MINUSSY);
        OUTPUT_SYMBOL(MULTISY);
        OUTPUT_SYMBOL(DIVISY);
        OUTPUT_SYMBOL(ASSIGNSY);
        OUTPUT_SYMBOL(COMMASY);
        OUTPUT_SYMBOL(SEMISY);
        OUTPUT_SYMBOL(COLONSY);
        OUTPUT_SYMBOL(LPARSY);
        OUTPUT_SYMBOL(RPARSY);
        OUTPUT_SYMBOL(LBRASY);
        OUTPUT_SYMBOL(RBRASY);
        OUTPUT_SYMBOL(BEGINSY);
        OUTPUT_SYMBOL(ENDSY);
        OUTPUT_SYMBOL(IDSY);
        OUTPUT_SYMBOL(NUMSY);
        OUTPUT_SYMBOL(CHSY);
        OUTPUT_SYMBOL(STRSY);
        OUTPUT_SYMBOL(INTSY);
        OUTPUT_SYMBOL(CHARSY);
        OUTPUT_SYMBOL(CONSTSY);
        OUTPUT_SYMBOL(VOIDSY);
        OUTPUT_SYMBOL(MAINSY);
        OUTPUT_SYMBOL(IFSY);
        OUTPUT_SYMBOL(ELSESY);
        OUTPUT_SYMBOL(SWITCHSY);
        OUTPUT_SYMBOL(CASESY);
        OUTPUT_SYMBOL(FORSY);
        OUTPUT_SYMBOL(DEFAULTSY);
        OUTPUT_SYMBOL(READSY);
        OUTPUT_SYMBOL(WRITESY);
        OUTPUT_SYMBOL(RETURNSY);
        default: puts("Unknown symbol detected."); error();
    }
    if(symbol >= EQSY && symbol <= GEQSY) {
        switch(symbol) {
            case EQSY: fputc('=', fout); break;
            case NEQSY: fputc('!', fout); break;
            case LESSY: case LEQSY: fputc('<', fout); break;
            case GRESY: case GEQSY: fputc('>', fout); break;
            default: ;
        }
        switch(symbol) {
            case EQSY: case NEQSY: case LEQSY: case GEQSY: fputs("=\n", fout); break;
            default: fputs("\n", fout);
        }
    } else if(symbol >= PLUSSY && symbol <= ENDSY) {
        fprintf(fout, "%c\n", pattern[symbol - PLUSSY]);
    } else if(symbol == IDSY) {
        fprintf(fout, "%s\n", token);
    } else if(symbol == NUMSY) {
        fprintf(fout, "%u\n", number);
    } else if(symbol == CHSY) {
        fprintf(fout, "\'%c\'\n", token[0]);
    } else if(symbol == STRSY) {
        fprintf(fout, "\"%s\"\n", token);
    } else if(symbol >= INTSY && symbol <= DEFAULTSY) {
        fprintf(fout, "%s\n", reserver[symbol - INTSY]);
    } else { // not a symbol
        fputs("\n", fout);
    }
}
#endif
