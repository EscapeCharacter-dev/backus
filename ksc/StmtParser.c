#include "Parser.h"
#include "IGenMod.h"
#include "Tokens.h"
#include "NodeKinds.h"
#include "Lex.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static bool_t statementsYet = FALSE;

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
	const bool_t oldDeclParseState = statementsYet;
	KscToken tok;
	KscLex(&tok); // '{'
	tok.kind = 0xFF;
	KscLexPeek(&tok);
	bool_t first = TRUE;
	statementsYet = FALSE;
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
	statementsYet = oldDeclParseState;
	if (tok.kind != '}')
	{
		fprintf(stdout, "(%d, %d): non-terminated block\n", tok.line, tok.column);
		return;
	}
	KscLex(&tok);
}

typedef struct VTOEntry
{
	uint64_t offset;
	char *identifier;
	KscType type;
} VTOEntry;

static VTOEntry **vto = 0;
static uint64_t vtoOffset = 0;
static uint64_t offsetBase = 0;

static void newStackVariable(const VTOEntry *toAdd)
{
	vto[vtoOffset++] = malloc(sizeof(VTOEntry));
	if (vtoOffset % 256 == 0) vto = realloc(vto, (vtoOffset + 256) * sizeof(VTOEntry *));
	memcpy(vto[vtoOffset - 1], toAdd, sizeof(VTOEntry));
}

static VTOEntry *getStackVariableFromIdentifier(const KscToken *tok)
{
	uint64_t i;
	for (i = 0; i < vtoOffset; i++)
	{
		if (!strcmp((char *)tok->data, vto[i]->identifier))
			return vto[i];
	}
	return 0;
}

static void composeStackVariable(VTOEntry *entry, char *identifier, const KscType *type)
{
	entry->identifier = identifier;
	memcpy(&entry->type, type, sizeof(KscType));
	entry->offset = offsetBase;
	memcpy(&entry->type, type, sizeof(KscType));
	offsetBase += KscGenGetTypeSize(type);
	offsetBase += offsetBase % KscGenGetAlignment();
}

void KscParseStmt(void)
{
	if (!vto) vto = malloc(sizeof(VTOEntry *) * 256);
	KscToken tok;
	if (!KscLexPeek(&tok))
	{
		volatile KscToken *v = KscLexLastGoodTokenPtr();
		fprintf(stdout, "(%d, %d): expected a statement\n", v->line, v->column);
		return;
	}

	if (!statementsYet)
	{
		KscType type;
		if (KscParseType(&type))
		{
			KscLex(&tok); // ident
			if (tok.kind != KSC_IDENT)
			{
				volatile KscToken *v = KscLexLastGoodTokenPtr();
				fprintf(stdout, "(%d, %d): expected an identifier after type\n", v->line, v->column);
				return;
			}
			KscLexPeek(&tok);
			if (tok.kind == ';')
			{
				KscLex(&tok);
				VTOEntry entry;
				composeStackVariable(&entry, (char *)tok.data, &type);
				newStackVariable(&entry);
				return;
			}
			else if (tok.kind == '=')
			{
				fprintf(stdout, "TODO: decl assign\n");
				abort();
				KscTree *tree = KscParseExpr(0);
				KscFreeTree(tree);
			}
			else
			{
				volatile KscToken *v = KscLexLastGoodTokenPtr();
				fprintf(stdout, "(%d, %d): expected ';' or '=' after ident\n", v->line, v->column);
				return;
			}
		}
		statementsYet = TRUE;
		KscGenStackFrame(offsetBase);
	}

	switch (tok.kind)
	{
	case KSC_KEYWORD_IF: parseIfStmt(); break;
	case KSC_KEYWORD_RETURN: parseRet(); break;
	case KSC_KEYWORD_WHILE: parseWhileStmt(); break;
	case ';': KscLex(&tok); break;
	case '{':
		{
			const uint64_t oldOffset = offsetBase;
			offsetBase = 0;
			parseBlkStmt();
			offsetBase = oldOffset;
			KscGenStackFrameRestore();
			break;
		}
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