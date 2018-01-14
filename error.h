#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

#include "common.h"
#include "scanner.h"

extern bool hasError;
extern void addError(int index = -1);
extern void addWarning(int index);

#endif // ERROR_H_INCLUDED
