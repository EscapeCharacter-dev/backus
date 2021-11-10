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
// Gets a variable's offset from stack.
uint64_t KscStmtGetStackVariableOffset(const KscToken *tok);
// Gets a variable's type from stack.
KscType *KscStmtGetStackVariableType(const KscToken *tok);

#endif