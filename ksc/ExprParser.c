#include "Lex.h"
#include "Parser.h"
#include "Tokens.h"
#include "NodeKinds.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// unary node constructor
static KscTree *newUnary(KscTree *child, int kind, KscToken *restrict tok)
{
	KscTree *t = malloc(sizeof(KscTree));
	memset(t, 0, sizeof(KscTree));
	t->left = child;
	t->right = NULL;
	t->kind = kind;
	memcpy(&t->token, tok, sizeof(KscToken));
	return t;
}

// binary node constructor
static KscTree *newBinary(KscTree *left, KscTree *right, int kind, KscToken *restrict tok)
{
	KscTree *t = malloc(sizeof(KscTree));
	memset(t, 0, sizeof(KscTree));
	t->left = left;
	t->right = right;
	t->kind = kind;
	memcpy(&t->token, tok, sizeof(KscToken));
	return t;
}

// childless node constructor
static KscTree *newAtom(int kind, KscToken *restrict tok)
{
	KscTree *t = malloc(sizeof(KscTree));
	memset(t, 0, sizeof(KscTree));
	t->left = NULL;
	t->right = NULL;
	t->kind = kind;
	memcpy(&t->token, tok, sizeof(KscToken));
	return t;
}

static int uopprec(int tok)
{
	switch (tok)
	{
	case KSC_TOKEN_PLUSPLUS:
	case KSC_TOKEN_MINUSMINUS:
	case '+':
	case '-':
	case '*':
	case '&':
	case '!':
	case '~':
	case KSC_KEYWORD_SIZEOF:
		return 11;
	default:
		return 0;
	}
}

static int bopprec(int tok)
{
	switch (tok)
	{
	case KSC_TOKEN_PIPES:
		return 1;
	case KSC_TOKEN_AMPERSANDS:
		return 2;
	case '|':
		return 3;
	case '^':
		return 4;
	case '&':
		return 5;
	case KSC_TOKEN_EQUALEQUAL:
	case KSC_TOKEN_EXCLEQUAL:
		return 6;
	case '<':
	case '>':
	case KSC_TOKEN_LARROWEQUAL:
	case KSC_TOKEN_RARROWEQUAL:
		return 7;
	case KSC_TOKEN_LARROWS:
	case KSC_TOKEN_RARROWS:
		return 8;
	case '+':
	case '-':
		return 9;
	case '*':
	case '/':
	case '%':
		return 10;
	default:
		return 0;
	}
}

static int uopk(int op)
{
	switch (op)
	{
	case KSC_TOKEN_PLUSPLUS:
		return KSC_TREE_PREFIX_INC;
	case KSC_TOKEN_MINUSMINUS:
		return KSC_TREE_PREFIX_DEC;
	case '+':
		return KSC_TREE_UNARY_PLUS;
	case '-':
		return KSC_TREE_NEGATE;
	case '!':
		return KSC_TREE_LOGICAL_NOT;
	case '~':
		return KSC_TREE_BITWISE_NOT;
	case '*':
		return KSC_TREE_DEREFERENCE;
	case '&':
		return KSC_TREE_ADDRESSOF;
	case KSC_KEYWORD_SIZEOF:
		return KSC_TREE_SIZEOF;
	default:
		return 0;
	}
}

static int bopk(int op)
{
	switch (op)
	{
	case KSC_TOKEN_PIPES:
		return KSC_TREE_LOGICAL_OR;
	case KSC_TOKEN_AMPERSANDS:
		return KSC_TREE_LOGICAL_AND;
	case '|':
		return KSC_TREE_BITWISE_OR;
	case '^':
		return KSC_TREE_BITWISE_XOR;
	case '&':
		return KSC_TREE_BITWISE_AND;
	case KSC_TOKEN_EQUALEQUAL:
		return KSC_TREE_EQUAL;
	case KSC_TOKEN_EXCLEQUAL:
		return KSC_TREE_NOT_EQUAL;
	case '<':
		return KSC_TREE_LOWER;
	case '>':
		return KSC_TREE_GREATER;
	case KSC_TOKEN_LARROWEQUAL:
		return KSC_TREE_LOWER_EQUAL;
	case KSC_TOKEN_RARROWEQUAL:
		return KSC_TREE_GREATER_EQUAL;
	case KSC_TOKEN_LARROWS:
		return KSC_TREE_LSHIFT;
	case KSC_TOKEN_RARROWS:
		return KSC_TREE_RSHIFT;
	case '+':
		return KSC_TREE_ADD;
	case '-':
		return KSC_TREE_SUB;
	case '*':
		return KSC_TREE_MUL;
	case '/':
		return KSC_TREE_DIV;
	case '%':
		return KSC_TREE_MOD;
	default:
		return 0;
	}
}

static KscTree *pPrimary(void)
{
	KscToken tok;
	memset(&tok, 0, sizeof(KscToken));
	if (!KscLex(&tok))
	{
		const volatile KscToken *t = KscLexLastGoodTokenPtr();
		fprintf(stderr, "(%d, %d): unknown token/unexpected EOF\n", t->line, t->column);
		return NULL;
	}

	switch (tok.kind)
	{
	case KSC_TOKEN_LINT:
		return newAtom(KSC_TREE_LITERAL_INTEGER, &tok);
	case KSC_TOKEN_LFLOAT:
		return newAtom(KSC_TREE_LITERAL_FLOAT, &tok);
	case KSC_TOKEN_LSTRING:
		return newAtom(KSC_TREE_LITERAL_STRING, &tok);
	case KSC_IDENT:
		return newAtom(KSC_TREE_IDENTIFIER, &tok);
	case '(':
	{
		KscTree *tree = KscParseExpr(0);
		if (!KscLex(&tok))
			return tree;
		if (tok.kind != ')')
		{
			fprintf(stderr, "(%d, %d): expected closing parenthesis\n", tok.line, tok.column);
			return NULL;
		}
		return tree;
	}
	}
}

KscTree *KscParseExpr(int oprec)
{
	KscTree *left;
	KscTree *right;
	KscToken tok;

	if (!KscLexPeek(&tok))
		return NULL;

	int up = uopprec(tok.kind);
	if (up && up >= oprec)
	{
		KscLex(&tok);
		int op = uopk(tok.kind);
		left = newUnary(KscParseExpr(up), op, &tok);
	}
	else
	{
		left = pPrimary();
	}

	if (!KscLexPeek(&tok))
		return left;

	while (1)
	{
		if (!KscLexPeek(&tok))
			break;
		int bprec = bopprec(tok.kind);
		if (!bprec || bprec <= oprec)
			break;
		KscLex(&tok);
		int op = bopk(tok.kind);
		right = KscParseExpr(bprec);
		left = newBinary(left, right, op, &tok);
	}

	return left;
}