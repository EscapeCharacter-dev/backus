#include "../IGenMod.h"
#include "../NodeKinds.h"
#include <stdio.h>

static int64_t genExpression(KscTree *tree)
{
	if (!tree) return 0;

	if (tree->right) genExpression(tree->right);
	if (tree->left) genExpression(tree->left);

	switch (tree->kind)
	{
	case KSC_TREE_LITERAL_INTEGER:
		fprintf(stdout, "\tpush dword %ld\n", *(int64_t *)tree->token.data);
		return 0;
	case KSC_TREE_LITERAL_FLOAT:
		fprintf(stdout, "\tpush dword %ld\n", *(int64_t *)tree->token.data); // floating point evil trick
		return 0;
	case KSC_TREE_LITERAL_STRING:
		// TODO
		return 0;
	case KSC_TREE_UNARY_PLUS:
		// parser-level
		return 0;
	case KSC_TREE_NEGATE:
		fprintf(stdout,
		"\tpop rax\n"
		"\tneg rax\n"
		"\tpush rax\n");
		return 0;
	case KSC_TREE_BITWISE_NOT:
		fprintf(stdout, 
		"\tpop rax\n"
		"\tnot rax\n"
		"\tpush rax\n");
		return 0;
	case KSC_TREE_LOGICAL_NOT:
		fprintf(stdout, 
		"\tpop rax\n"
		"\tcmp rax, 0\n"
		"\tsete al\n"
		"\tmovzx rax, al\n"
		"\tpush rax\n");
		return 0;
	case KSC_TREE_DEREFERENCE:
		fprintf(stdout, 
		"\tpop rax\n"
		"\tmov rax, [rax]\n"
		"\tpush rax\n");
		return 0;
	case KSC_TREE_ADDRESSOF:
		fprintf(stdout, 
		"\tlea rax, [rbp-8]\n"
		"\tpush rax\n");
		return 0;
	case KSC_TREE_MUL:
		fprintf(stdout, 
		"\tpop rbx\n"
		"\tpop rax\n"
		"\timul rax, rbx\n"
		"\tpush rax\n");
		return 0;
	case KSC_TREE_DIV:
		fprintf(stdout,
		"\tpop rax\n"
		"\tpop rbx\n"
		"\tcqo\n"
		"\tidiv rbx\n"
		"\tpush rax\n");
		return 0;
	case KSC_TREE_MOD:
		fprintf(stdout,
		"\tpop rax\n"
		"\tpop rbx\n"
		"\tcqo\n"
		"\tidiv rbx\n"
		"\tpush rdx\n");
		return 0;
	case KSC_TREE_ADD:
		fprintf(stdout,
		"\tpop rbx\n"
		"\tpop rax\n"
		"\tadd rax, rbx\n"
		"\tpush rax\n");
		return 0;
	case KSC_TREE_SUB:
		fprintf(stdout,
		"\tpop rbx\n"
		"\tpop rax\n"
		"\tsub rax, rbx\n"
		"\tpush rax\n");
		return 0;
	}
}

// x86-64 generation module
const KscIGenMod x86_64genm =
{
	.genExpression = genExpression,
};