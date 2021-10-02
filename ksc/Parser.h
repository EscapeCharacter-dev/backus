#ifndef KSC_PARSER_H
#define KSC_PARSER_H

#include "Types.h"

// Parses a type.
bool_t KscParseType(KscType *type);
// Parses an expression.
KscTree *KscParseExpr(int oprec);

#endif