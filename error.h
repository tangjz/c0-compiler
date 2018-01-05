#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include "common.h"
#include "scanner.h"

extern int errorCount, errorSequence[ERROR_MAX];
extern void error(int errNo = 0);

#endif // ERROR_H_INCLUDED
