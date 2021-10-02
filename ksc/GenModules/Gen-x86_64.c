#include "../IGenMod.h"
#include "../NodeKinds.h"
#include <stdio.h>

static int64_t symbols = 0;

static int64_t genExpression(KscTree *tree)
{
	if (!tree) return 0;

	if (tree->left) genExpression(tree->left);
	if (tree->right) genExpression(tree->right);

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
	case KSC_TREE_LSHIFT:
		fprintf(stdout,
		"\tpop rcx\n"
		"\tpop rax\n"
		"\tsal rax, cl\n"
		"\tpush rax\n");
		return 0;
	case KSC_TREE_RSHIFT:
		fprintf(stdout,
		"\tpop rcx\n"
		"\tpop rax\n"
		"\tsar rax, cl\n"
		"\tpush rax\n");
		return 0;
	case KSC_TREE_LOWER:
		fprintf(stdout,
		"\tpop rbx\n"
		"\tpop rax\n"
		"\tcmp rax, rbx\n"
		"\tsetl al\n"
		"\tmovzx rax, al\n");
		return 0;
	case KSC_TREE_LOWER_EQUAL:
		fprintf(stdout,
		"\tpop rbx\n"
		"\tpop rax\n"
		"\tcmp rax, rbx\n"
		"\tsetle al\n"
		"\tmovzx rax, al\n");
		return 0;
	case KSC_TREE_GREATER:
		fprintf(stdout,
		"\tpop rbx\n"
		"\tpop rax\n"
		"\tcmp rax, rbx\n"
		"\tsetg al\n"
		"\tmovzx rax, al\n");
		return 0;
	case KSC_TREE_GREATER_EQUAL:
		fprintf(stdout,
		"\tpop rbx\n"
		"\tpop rax\n"
		"\tcmp rax, rbx\n"
		"\tsetge al\n"
		"\tmovzx rax, al\n");
		return 0;
	case KSC_TREE_EQUAL:
		fprintf(stdout,
		"\tpop rbx\n"
		"\tpop rax\n"
		"\tcmp rax, rbx\n"
		"\tsete al\n"
		"\tmovzx rax, al\n");
		return 0;
	case KSC_TREE_NOT_EQUAL:
		fprintf(stdout,
		"\tpop rbx\n"
		"\tpop rax\n"
		"\tcmp rax, rbx\n"
		"\tsetne al\n"
		"\tmovzx rax, al\n");
		return 0;
	case KSC_TREE_BITWISE_AND:
		fprintf(stdout,
		"\tpop rbx\n"
		"\tpop rax\n"
		"\tand rax, rbx\n"
		"\tpush rax\n");
		return 0;
	case KSC_TREE_BITWISE_XOR:
		fprintf(stdout,
		"\tpop rbx\n"
		"\tpop rax\n"
		"\txor rax, rbx\n"
		"\tpush rax\n");
		return 0;
	case KSC_TREE_BITWISE_OR:
		fprintf(stdout,
		"\tpop rbx\n"
		"\tpop rax\n"
		"\tor rax, rbx\n"
		"\tpush rax\n");
		return 0;
	case KSC_TREE_LOGICAL_AND:
		{
			const int64_t s0 = symbols++;
			const int64_t s1 = symbols++;
			fprintf(stdout,
			"\tpop rbx\n"
			"\tpop rax\n"
			"\ttest rax, rax\n"
			"\tjmp .L%ld\n"
			"\ttest rbx, rbx\n"
			"\tjmp .L%ld\n"
			"\tmov rax, 1\n"
			"\tjmp .L%ld\n"
			".L%ld: mov rax, 0\n"
			".L%ld: push rax\n", s0, s0, s1, s0, s1);
		}
		return 0;
	case KSC_TREE_LOGICAL_OR:
		{
			const int64_t s0 = symbols++;
			const int64_t s1 = symbols++;
			const int64_t s2 = symbols++;
			fprintf(stdout,
			"\tpop rbx\n"
			"\tpop rax\n"
			"\ttest rax, rax\n"
			"\tjmp .L%ld\n"
			"\ttest rbx, rbx\n"
			"\tjmp .L%ld\n"
			".L%ld:\n"
			"\tmov rax, 1\n"
			"\tjmp .L%ld\n"
			".L%ld: mov rax, 0\n"
			".L%ld: push rax\n", s0, s1, s1, s2, s1, s2);
		}
		return 0;
	}
}

// x86-64 generation module
const KscIGenMod x86_64genm =
{
	.genExpression = genExpression,
};