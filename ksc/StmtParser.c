#include "Parser.h"
#include "IGenMod.h"
#include "Tokens.h"
#include "NodeKinds.h"
#include "Lex.h"
#include <stdlib.h>
#include <stdio.h>

static void parseRet(void)
{
	KscToken tok;
	KscLex(&tok);
	KscLexPeek(&tok);
	if (tok.kind == ';')
	{
		KscLex(&tok);
		KscGenReturn();
	}
	KscTree *tree = KscParseExpr(0);
	if (!tree) return; // prevent segfaults
	tok.kind = 0xFF;
	KscLexPeek(&tok);
	if (tok.kind != ';')
	{
		volatile KscToken *v = KscLexLastGoodTokenPtr();
		fprintf(stdout, "(%d, %d): expected semicolon after return statement\n", v->line, v->column);
		free(tree); // prevent memory leaks
		return;
	}
	KscLex(&tok);
	KscGenExpr(tree, KscGenGetReturnAcc(), 0, 0);
	KscGenReturn();
}

static void parseIfStmt(void)
{
	KscToken tok;
	KscLex(&tok);
	tok.kind = 0xFF;
	KscLexPeek(&tok);
	if (tok.kind != '(')
	{
		volatile KscToken *v = KscLexLastGoodTokenPtr();
		fprintf(stdout, "(%d, %d): expected open bracket after if keyword\n", v->line, v->column);
		return;
	}
	KscLex(&tok);
	KscTree *tree = KscParseExpr(0);
	if (!tree) return; // prevent segfaults
	tok.kind = 0xFF;
	KscLexPeek(&tok);
	if (tok.kind != ')')
	{
		volatile KscToken *v = KscLexLastGoodTokenPtr();
		fprintf(stdout, "(%d, %d): expected closing bracket after expression in if statement, got %d\n", v->line, v->column, tok.kind);
		free(tree); // prevent memory leaks
		return;
	}
	KscLex(&tok);
	uint64_t l0 = KscGenLabel();
	uint64_t l1 = KscGenLabel();
	KscGenExpr(tree, NOREG, l0, l1);
	KscPrintLabel(l0);
	KscParseStmt();
	KscLexPeek(&tok);
	if (tok.kind == KSC_KEYWORD_ELSE)
	{
		KscLex(&tok);
		KscPrintLabel(l1);
		KscParseStmt();
	}
}

void KscParseStmt(void)
{
	KscToken tok;
	if (!KscLexPeek(&tok))
	{
		volatile KscToken *v = KscLexLastGoodTokenPtr();
		fprintf(stdout, "(%d, %d): expected a statement\n", v->line, v->column);
		return;
	}

	switch (tok.kind)
	{
	case KSC_KEYWORD_IF: return parseIfStmt();
	case KSC_KEYWORD_RETURN: return parseRet();
	case ';': KscLex(&tok); return;
	}
	return;
}