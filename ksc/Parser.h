#ifndef KSC_PARSER_H
#define KSC_PARSER_H

#include "Types.h"

// Parses a type.
bool_t KscParseType(KscType *type);
// Parses an expression.
KscTree *KscParseExpr(int oprec);
// Parses a statement.
void KscParseStmt(void);
// Frees a binary tree.
void KscFreeTree(KscTree *tree);

#endif