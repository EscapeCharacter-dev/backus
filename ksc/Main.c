#include "Lex.h"
#include "Tokens.h"
#include "Parser.h"
#include "NodeKinds.h"
#include "IGenMod.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char *const typeNames[] =
		{
				"void",
				"tuple",
				"struct",
				"record",
				"union",
				"enum",
				"pointer",
				"array",
				"reference",
				"function",
				"signed char",
				"unsigned char",
				"signed short",
				"unsigned short",
				"half",
				"signed int",
				"unsigned int",
				"float",
				"signed long long",
				"unsigned long long",
				"double",
};

static void printType(const KscType *const type)
{
	printf("%s : ", typeNames[type->kind]);
	if (type->attributes & 0b10000000)
		putc('1', stdout);
	else
		putc('0', stdout);
	if (type->attributes & 0b01000000)
		putc('1', stdout);
	else
		putc('0', stdout);
	if (type->attributes & 0b00100000)
		putc('1', stdout);
	else
		putc('0', stdout);
	if (type->attributes & 0b00010000)
		putc('1', stdout);
	else
		putc('0', stdout);
	if (type->attributes & 0b00001000)
		putc('1', stdout);
	else
		putc('0', stdout);
	if (type->attributes & 0b00000100)
		putc('1', stdout);
	else
		putc('0', stdout);
	if (type->attributes & 0b00000010)
		putc('1', stdout);
	else
		putc('0', stdout);
	if (type->attributes & 0b00000001)
		putc('1', stdout);
	else
		putc('0', stdout);
	if (type->childType)
	{
		printf(" of (");
		printType(type->childType);
		printf(")");
	}
}

static void printTree(int indent, KscTree *tree)
{
	for (int i = 0; i < indent; i++)
		printf("-");
	printf("%s", treeEnumString[tree->kind]);
	printf(" %d", tree->token.kind);
	if (!tree->left && !tree->right)
		printf(";\n");
	else
		printf(":\n");
	if (tree->left)
		printTree(indent + 1, tree->left);
	if (tree->right)
		printTree(indent + 1, tree->right);
}

extern KscIGenMod x86_64gm;

int main(int argc, char *argv[])
{
	char *ibuf = malloc(262144000);
	memset(ibuf, 0, sizeof(ibuf));
	KscSelectGenerator(&x86_64gm);
	while (1)
	{
		printf(">>> ");
		if (!fgets(ibuf, 262144000, stdin))
			abort();
		KscGenInit();
		KscLexFeed(ibuf);
		KscParseStmt();
	}
	free(ibuf);
	return 0;
}