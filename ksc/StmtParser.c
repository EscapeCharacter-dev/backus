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

void KscParseStmt(void)
{
	KscToken tok;
	if (!KscLexPeek(&tok))
	{
		volatile KscToken *v = KscLexLastGoodTokenPtr();
		fprintf(stdout, "(%d, %d): expected a statement\n", v->line, v->column);
		return;
	}

	if (tok.kind == KSC_KEYWORD_RETURN)
		parseRet();
	return;
}