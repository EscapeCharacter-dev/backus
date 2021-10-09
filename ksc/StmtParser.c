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
		KscFreeTree(tree); // prevent memory leaks
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
		KscFreeTree(tree); // prevent memory leaks
		return;
	}
	KscLex(&tok);
	uint64_t l0 = KscGenLabel();
	uint64_t c = KscGenLabel();
	uint64_t lead = KscGenLabel();
	KscJump(c);
	KscPrintLabel(l0);
	KscParseStmt();
	KscJump(lead);
	KscLexPeek(&tok);
	uint64_t l1 = 0;
	if (tok.kind == KSC_KEYWORD_ELSE)
	{
		KscLex(&tok);
		l1 = KscGenLabel();
		KscPrintLabel(l1);
		KscParseStmt();
		KscJump(lead);
	}
	KscPrintLabel(c);
	KscGenExpr(tree, NOREG, l0, l1);
	KscFreeTree(tree);
	KscPrintLabel(lead);
}

static void parseWhileStmt(void)
{
	KscToken tok;
	KscLex(&tok);
	tok.kind = 0xFF;
	KscLexPeek(&tok);
	if (tok.kind != '(')
	{
		volatile KscToken *v = KscLexLastGoodTokenPtr();
		fprintf(stdout, "(%d, %d): expected open bracket after while keyword\n", v->line, v->column);
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
		fprintf(stdout, "(%d, %d): expected closing bracket after expression in while statement, got %d\n", v->line, v->column, tok.kind);
		KscFreeTree(tree); // prevent memory leaks
		return;
	}
	KscLex(&tok);
	uint64_t lCondition = KscGenLabel();
	uint64_t lChildStmt = KscGenLabel();
	uint64_t lLead = KscGenLabel();
	KscPrintLabel(lCondition);
	KscGenExpr(tree, NOREG, lChildStmt, lLead);
	KscFreeTree(tree);
	KscPrintLabel(lChildStmt);
	KscParseStmt();
	KscJump(lCondition);
	KscPrintLabel(lLead);
}

static void parseBlkStmt(void)
{
	KscToken tok;
	KscLex(&tok); // '{'
	tok.kind = 0xFF;
	KscLexPeek(&tok);
	bool_t first = TRUE;
	while (tok.kind != '}')
	{
		if (!first)
		{
			KscLex(&tok);
			first = FALSE;
		}
		KscParseStmt();
		if (!KscLexPeek(&tok))
			break;
	}
	if (tok.kind != '}')
	{
		fprintf(stdout, "(%d, %d): non-terminated block\n", tok.line, tok.column);
		return;
	}
	KscLex(&tok);
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
	case KSC_KEYWORD_IF: parseIfStmt(); break;
	case KSC_KEYWORD_RETURN: parseRet(); break;
	case KSC_KEYWORD_WHILE: parseWhileStmt(); break;
	case ';': KscLex(&tok); break;
	case '{': parseBlkStmt(); break;
	default:
		{
			KscTree *tree = KscParseExpr(0);
			KscGenExpr(tree, NOREG, 0, 0);
			KscFreeTree(tree);
			tok.kind = 0xFF;
			KscLexPeek(&tok);
			if (tok.kind != ';')
			{
				fprintf(stdout, "(%d, %d): expected a semicolon\n", tok.line, tok.column);
			}
			KscLex(&tok);
		}
	}
	return;
}