#include "../IGenMod.h"
#include "../NodeKinds.h"
#include "../TypeKinds.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static bool_t isIntegerType(int typeKind)
{
	return typeKind >= KSC_TYPE_SBYTE && typeKind <= KSC_TYPE_ULONG || typeKind == KSC_TYPE_POINTER;
}

static const char *registers[] =
{
	"rax", "eax",  "ax",   "al",
	"rbx", "ebx",  "bx",   "bl",
	"rdx", "edx",  "dx",   "dl",
	"r8",  "r8d",  "r8w",  "r8b",
	"r9",  "r9d",  "r9w",  "r9b",
	"r10", "r10d", "r10w", "r10b",
	"r11", "r11d", "r11w", "r11b",
	"r12", "r12d", "r12w", "r12b",
	"r13", "r13d", "r13w", "r13b",
	"r14", "r14d", "r14w", "r14b",
	"r15", "r15d", "r15w", "r15b",
	"rcx", "ecx",  "cx",   "cl",
};

static bool_t usemap[] =
{
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
};

static uint64_t genExpr(KscTree *node, uint64_t acc, uint64_t condBranchSymbol, uint64_t condBranchSymbol2);

// allocates a 64-bit register
static uint64_t regAlloc_64(void)
{
	for (int i = 0; i < 44; i++)
	{
		if (!usemap[i]) return usemap[i] = TRUE, i << 2;
	}
	fprintf(stderr, "x86_64gm-ksc: out of registers\n");
	return 0;
}

// allocates a 32-bit register
static uint64_t regAlloc_32(void)
{
	for (int i = 0; i < sizeof(usemap)/sizeof(bool_t); i++)
	{
		if (!usemap[i]) return usemap[i] = TRUE, (i << 2) + 1;
	}
	fprintf(stderr, "x86_64gm-ksc: out of registers\n");
	return 0;
}

// allocates a 16-bit register
static uint64_t regAlloc_16(void)
{
	for (int i = 0; i < sizeof(usemap)/sizeof(bool_t); i++)
	{
		if (!usemap[i]) return usemap[i] = TRUE, (i << 2) + 2;
	}
	fprintf(stderr, "x86_64gm-ksc: out of registers\n");
	return 0;
}

// allocates a 8-bit register
static uint64_t regAlloc_8(void)
{
	for (int i = 0; i < sizeof(usemap)/sizeof(bool_t); i++)
	{
		if (!usemap[i]) return usemap[i] = TRUE, (i << 2) + 3;
	}
	fprintf(stderr, "x86_64gm-ksc: out of registers\n");
	return 0;
}

static uint64_t reg64(uint64_t reg) { return reg >> 2 << 2; /* trimming */ }
static uint64_t reg32(uint64_t reg) { return reg64(reg) + 1; }
static uint64_t reg16(uint64_t reg) { return reg64(reg) + 2; }
static uint64_t reg8(uint64_t reg)  { return reg64(reg) + 3; }

// frees an allocated register
static void freeReg(uint64_t reg)
{
	int index = reg >> 2;
	usemap[index] = FALSE;
}

// frees all used registers
static void freeAllRegs(void)
{
	memset(usemap, 0, sizeof(usemap));
}

static uint64_t genILiteral(KscTree *node, uint64_t acc)
{
	uint64_t r = acc;
	fprintf(stdout, "\tmov %s, %ld\n", registers[r], *(int64_t *)node->token.data);
	return r;
}

static uint64_t genIAdd(KscTree *node, uint64_t acc)
{
	uint64_t l = genExpr(node->left, acc, 0, 0);
	uint64_t r = genExpr(node->right, NOREG, 0, 0);
	fprintf(stdout, "\tadd %s, %s\n", registers[l], registers[r]);
	freeReg(r);
	return l;
}

static uint64_t genISub(KscTree *node, uint64_t acc)
{
	uint64_t l = genExpr(node->left, acc, 0, 0);
	uint64_t r = genExpr(node->right, NOREG, 0, 0);
	fprintf(stdout, "\tsub %s, %s\n", registers[l], registers[r]);
	freeReg(r);
	return l;
}

static uint64_t genIMul(KscTree *node, uint64_t acc)
{
	uint64_t l = genExpr(node->left, acc, 0, 0);
	uint64_t r = genExpr(node->right, NOREG, 0, 0);
	fprintf(stdout, "\timul %s, %s\n", registers[l], registers[r]);
	freeReg(r);
	return l;
}

static uint64_t genIDiv(KscTree *node, uint64_t acc)
{
	uint64_t l = genExpr(node->left, acc, 0, 0);
	uint64_t r = genExpr(node->right, NOREG, 0, 0);
	fprintf(stdout, "\tmov rax, %s\n", registers[l]);
	fprintf(stdout, "\tcqo\n");
	fprintf(stdout, "\tidiv %s\n", registers[r]);
	fprintf(stdout, "\tmov %s, rax\n", registers[l]);
	freeReg(r);
	return l;
}

static uint64_t genMod(KscTree *node, uint64_t acc)
{
	uint64_t l = genExpr(node->left, acc, 0, 0);
	uint64_t r = genExpr(node->right, NOREG, 0, 0);
	fprintf(stdout, "\tmov rax, %s\n", registers[l]);
	fprintf(stdout, "\tcqo\n");
	fprintf(stdout, "\tidiv %s\n", registers[r]);
	if (l != 4)
		fprintf(stdout, "\tmov %s, rdx\n", registers[l]);
	freeReg(r);
	return l;
}

static uint64_t genLsh(KscTree *node, uint64_t acc)
{
	uint64_t l = genExpr(node->left, acc, 0, 0);
	uint64_t r = genExpr(node->right, 44, 0, 0);
	fprintf(stdout, "\tsal %s, %s\n", registers[l], registers[reg8(r)]);
	freeReg(r);
	return l;
}

static uint64_t genRsh(KscTree *node, uint64_t acc)
{
	uint64_t l = genExpr(node->left, acc, 0, 0);
	uint64_t r = genExpr(node->right, 44, 0, 0);
	fprintf(stdout, "\tsar %s, %s\n", registers[l], registers[reg8(r)]);
	freeReg(r);
	return l;
}

static uint64_t genBitwiseAnd(KscTree *node, uint64_t acc)
{
	uint64_t l = genExpr(node->left, acc, 0, 0);
	uint64_t r = genExpr(node->right, NOREG, 0, 0);
	fprintf(stdout, "\tand %s, %s\n", registers[l], registers[r]);
	freeReg(r);
	return l;
}

static uint64_t genBitwiseXor(KscTree *node, uint64_t acc)
{
	uint64_t l = genExpr(node->left, acc, 0, 0);
	uint64_t r = genExpr(node->right, NOREG, 0, 0);
	fprintf(stdout, "\txor %s, %s\n", registers[l], registers[r]);
	freeReg(r);
	return l;
}

static uint64_t genBitwiseOr(KscTree *node, uint64_t acc)
{
	uint64_t l = genExpr(node->left, acc, 0, 0);
	uint64_t r = genExpr(node->right, NOREG, 0, 0);
	fprintf(stdout, "\tor %s, %s\n", registers[l], registers[r]);
	freeReg(r);
	return l;
}

static uint64_t genBitwiseNot(KscTree *node, uint64_t acc)
{
	uint64_t u = genExpr(node->left, acc, 0, 0);
	fprintf(stdout, "\tnot %s\n", registers[u]);
	return u;
}

static uint64_t genNegate(KscTree *node, uint64_t acc)
{
	uint64_t u = genExpr(node->left, acc, 0, 0);
	fprintf(stdout, "\tneg %s\n", registers[u]);
	return u;
}

static uint64_t genEqual(KscTree *node, uint64_t acc, uint64_t condBranchSymbol, uint64_t condBranchSymbol2)
{
	uint64_t l = genExpr(node->left, acc, 0, 0);
	uint64_t r = genExpr(node->right, NOREG, 0, 0);
	fprintf(stdout, "\tcmp %s, %s\n", registers[l], registers[r]);
	if (condBranchSymbol)
	{
		fprintf(stdout, "\tje .L%ld\n", condBranchSymbol);
		if (condBranchSymbol2)
		{
			fprintf(stdout, "\tjmp .L%ld\n", condBranchSymbol2);
		}
	}
	else
	{
		fprintf(stdout, "\tsete %s\n", registers[reg8(l)]);
		fprintf(stdout, "\tmovzx %s, %s\n", registers[l], registers[reg8(l)]);
	}
	freeReg(r);
	return l;
}

static uint64_t genNotEqual(KscTree *node, uint64_t acc, uint64_t condBranchSymbol, uint64_t condBranchSymbol2)
{
	uint64_t l = genExpr(node->left, acc, 0, 0);
	uint64_t r = genExpr(node->right, NOREG, 0, 0);
	fprintf(stdout, "\tcmp %s, %s\n", registers[l], registers[r]);
	if (condBranchSymbol)
	{
		fprintf(stdout, "\tjne .L%ld\n", condBranchSymbol);
		if (condBranchSymbol2)
		{
			fprintf(stdout, "\tjmp .L%ld\n", condBranchSymbol2);
		}
	}
	else
	{
		fprintf(stdout, "\tsetne %s\n", registers[reg8(l)]);
		fprintf(stdout, "\tmovzx %s, %s\n", registers[l], registers[reg8(l)]);
	}
	freeReg(r);
	return l;
}

static uint64_t genLower(KscTree *node, uint64_t acc, uint64_t condBranchSymbol, uint64_t condBranchSymbol2)
{
	uint64_t l = genExpr(node->left, acc, 0, 0);
	uint64_t r = genExpr(node->right, NOREG, 0, 0);
	fprintf(stdout, "\tcmp %s, %s\n", registers[l], registers[r]);
	if (condBranchSymbol)
	{
		fprintf(stdout, "\tjl .L%ld\n", condBranchSymbol);
		if (condBranchSymbol2)
		{
			fprintf(stdout, "\tjmp .L%ld\n", condBranchSymbol2);
		}
	}
	else
	{
		fprintf(stdout, "\tsetl %s\n", registers[reg8(l)]);
		fprintf(stdout, "\tmovzx %s, %s\n", registers[l], registers[reg8(l)]);
	}
	freeReg(r);
	return l;
}

static uint64_t genGreater(KscTree *node, uint64_t acc, uint64_t condBranchSymbol, uint64_t condBranchSymbol2)
{
	uint64_t l = genExpr(node->left, acc, 0, 0);
	uint64_t r = genExpr(node->right, NOREG, 0, 0);
	fprintf(stdout, "\tcmp %s, %s\n", registers[l], registers[r]);
	if (condBranchSymbol)
	{
		fprintf(stdout, "\tjg .L%ld\n", condBranchSymbol);
		if (condBranchSymbol2)
		{
			fprintf(stdout, "\tjmp .L%ld\n", condBranchSymbol2);
		}
	}
	else
	{
		fprintf(stdout, "\tsetg %s\n", registers[reg8(l)]);
		fprintf(stdout, "\tmovzx %s, %s\n", registers[l], registers[reg8(l)]);
	}
	freeReg(r);
	return l;
}

static uint64_t genLowerEqual(KscTree *node, uint64_t acc, uint64_t condBranchSymbol, uint64_t condBranchSymbol2)
{
	uint64_t l = genExpr(node->left, acc, 0, 0);
	uint64_t r = genExpr(node->right, NOREG, 0, 0);
	fprintf(stdout, "\tcmp %s, %s\n", registers[l], registers[r]);
	if (condBranchSymbol)
	{
		fprintf(stdout, "\tjle .L%ld\n", condBranchSymbol);
		if (condBranchSymbol2)
		{
			fprintf(stdout, "\tjmp .L%ld\n", condBranchSymbol2);
		}
	}
	else
	{
		fprintf(stdout, "\tsetle %s\n", registers[reg8(l)]);
		fprintf(stdout, "\tmovzx %s, %s\n", registers[l], registers[reg8(l)]);
	}
	freeReg(r);
	return l;
}

static uint64_t genGreaterEqual(KscTree *node, uint64_t acc, uint64_t condBranchSymbol, uint64_t condBranchSymbol2)
{
	uint64_t l = genExpr(node->left, acc, 0, 0);
	uint64_t r = genExpr(node->right, NOREG, 0, 0);
	fprintf(stdout, "\tcmp %s, %s\n", registers[l], registers[r]);
	if (condBranchSymbol)
	{
		fprintf(stdout, "\tjge .L%ld\n", condBranchSymbol);
		if (condBranchSymbol2)
		{
			fprintf(stdout, "\tjmp .L%ld\n", condBranchSymbol2);
		}
	}
	else
	{
		fprintf(stdout, "\tsetge %s\n", registers[reg8(l)]);
		fprintf(stdout, "\tmovzx %s, %s\n", registers[l], registers[reg8(l)]);
	}
	freeReg(r);
	return l;
}

static uint64_t genLogicalNot(KscTree *node, uint64_t acc, uint64_t condBranchSymbol, uint64_t condBranchSymbol2)
{
	uint64_t u = genExpr(node->left, acc, 0, 0);
	fprintf(stdout, "\ttest %s, %s\n", registers[u], registers[u]);
	if (condBranchSymbol)
	{
		fprintf(stdout, "\tje .L%ld\n", condBranchSymbol);
		if (condBranchSymbol2)
		{
			fprintf(stdout, "\tjmp .L%ld\n", condBranchSymbol2);
		}
	}
	else
	{
		fprintf(stdout, "\tsete %s\n", registers[reg8(u)]);
		fprintf(stdout, "\tmovzx %s, %s\n", registers[u], registers[reg8(u)]);
	}
	return u;
}

static uint64_t genLogicalAnd(KscTree *node, uint64_t acc, uint64_t condBranchSymbol, uint64_t condBranchSymbol2)
{
	uint64_t l = genExpr(node->left, acc, 0, 0);
	uint64_t r = genExpr(node->right, NOREG, 0, 0);
	if (condBranchSymbol)
	{
		if (condBranchSymbol2)
		{
			fprintf(stdout, "\ttest %s, %s\n", registers[l], registers[l]);
			fprintf(stdout, "\tje .L%ld\n", condBranchSymbol2);
			fprintf(stdout, "\ttest %s, %s\n", registers[r], registers[r]);
			fprintf(stdout, "\tje .L%ld\n", condBranchSymbol2);
			fprintf(stdout, "\tjmp .L%ld\n", condBranchSymbol);
		}
		else
		{
			uint64_t v = KscGenLabel();
			fprintf(stdout, "\ttest %s, %s\n", registers[l], registers[l]);
			fprintf(stdout, "\tje .L%ld\n", v);
			fprintf(stdout, "\ttest %s, %s\n", registers[r], registers[r]);
			fprintf(stdout, "\tje .L%ld\n", v);
			fprintf(stdout, "\tjmp .L%ld\n", condBranchSymbol);
			KscPrintLabel(v);
		}
	}
	else
	{
		fprintf(stdout, "\ttest %s, %s\n", registers[l], registers[l]);
		fprintf(stdout, "\tsetne %s\n", registers[reg8(l)]);
		fprintf(stdout, "\ttest %s, %s\n", registers[r], registers[r]);
		fprintf(stdout, "\tsetne %s\n", registers[reg8(r)]);
		fprintf(stdout, "\tand %s, %s\n", registers[reg8(l)], registers[reg8(r)]);
		fprintf(stdout, "\tmovzx %s, %s\n", registers[l], registers[reg8(l)]);
	}
	freeReg(r);
	return l;
}

static uint64_t genLogicalOr(KscTree *node, uint64_t acc, uint64_t condBranchSymbol, uint64_t condBranchSymbol2)
{
	uint64_t l = genExpr(node->left, acc, 0, 0);
	uint64_t r = genExpr(node->right, NOREG, 0, 0);
	if (condBranchSymbol)
	{
		fprintf(stdout, "\ttest %s, %s\n", registers[l], registers[l]);
		fprintf(stdout, "\tjne .L%ld\n", condBranchSymbol);
		fprintf(stdout, "\ttest %s, %s\n", registers[r], registers[r]);
		fprintf(stdout, "\tjne .L%ld\n", condBranchSymbol);
		if (condBranchSymbol2)
		{
			fprintf(stdout, "\tjmp .L%ld\n", condBranchSymbol2);
		}
	}
	else
	{
		fprintf(stdout, "\ttest %s, %s\n", registers[l], registers[l]);
		fprintf(stdout, "\tsetne %s\n", registers[reg8(l)]);
		fprintf(stdout, "\ttest %s, %s\n", registers[r], registers[r]);
		fprintf(stdout, "\tsetne %s\n", registers[reg8(r)]);
		fprintf(stdout, "\tor %s, %s\n", registers[reg8(l)], registers[reg8(r)]);
		fprintf(stdout, "\tmovzx %s, %s\n", registers[l], registers[reg8(l)]);
	}
	freeReg(r);
	return l;
}

static uint64_t genExpr(KscTree *node, uint64_t acc, uint64_t condBranchSymbol, uint64_t condBranchSymbol2)
{
	if (acc == NOREG)
	{
		switch (node->type->kind)
		{
		case KSC_TYPE_BYTE:
		case KSC_TYPE_SBYTE:
			acc = regAlloc_8();
			break;
		case KSC_TYPE_SHORT:
		case KSC_TYPE_USHORT:
			acc = regAlloc_16();
			break;
		case KSC_TYPE_INT:
		case KSC_TYPE_UINT:
			acc = regAlloc_32();
			break;
		case KSC_TYPE_LONG:
		case KSC_TYPE_ULONG:
		case KSC_TYPE_POINTER:
			acc = regAlloc_64();
			break;
		default:
			fprintf(stdout, "(%d, %d): type %d cannot be used in expression\n", node->token.line, node->token.column, node->type->kind);
			return 0;
		}
	}
	uint64_t ret = 0;
	switch (node->kind)
	{
	case KSC_TREE_CAST:
		if (isIntegerType(node->type->kind))
		{
			ret = genExpr(node->left, NOREG, 0, 0);
			if (node->type->kind == node->left->type->kind)
				goto condBranchSymbolResolve;
			if (node->type->kind == KSC_TYPE_POINTER) acc = reg64(ret);
			else if (node->type->kind < node->left->type->kind)
			{
				switch (node->type->kind)
				{
				case KSC_TYPE_SBYTE: case KSC_TYPE_BYTE: acc = reg8(ret); break;
				case KSC_TYPE_SHORT: case KSC_TYPE_USHORT: acc = reg16(ret); break;
				case KSC_TYPE_INT: case KSC_TYPE_UINT: acc = reg32(ret); break;
				default: abort();
				}
				ret = acc;
				goto condBranchSymbolResolve;
			}
			else
			{
				switch (node->type->kind)
				{
				case KSC_TYPE_SBYTE: case KSC_TYPE_BYTE: acc = reg8(ret); break;
				case KSC_TYPE_SHORT: case KSC_TYPE_USHORT: acc = reg16(ret); break;
				case KSC_TYPE_INT: case KSC_TYPE_UINT: acc = reg32(ret); break;
				case KSC_TYPE_LONG: case KSC_TYPE_ULONG: acc = reg64(ret); break;
				default: fprintf(stdout, "invalid type %d\n", node->type->kind); abort();
				}
				fprintf(stdout, "\tmovsx %s, %s\n", registers[acc], registers[ret]);
				ret = acc;
				goto condBranchSymbolResolve;
			}
		}
	case KSC_TREE_LITERAL_INTEGER: ret = genILiteral(node, acc); goto condBranchSymbolResolve;
	case KSC_TREE_ADD: ret = genIAdd(node, acc); goto condBranchSymbolResolve;
	case KSC_TREE_SUB: ret = genISub(node, acc); goto condBranchSymbolResolve;
	case KSC_TREE_MUL: ret = genIMul(node, acc); goto condBranchSymbolResolve;
	case KSC_TREE_DIV: ret = genIDiv(node, acc); goto condBranchSymbolResolve;
	case KSC_TREE_MOD: ret = genMod(node, acc); goto condBranchSymbolResolve;
	case KSC_TREE_LSHIFT: ret = genLsh(node, acc); goto condBranchSymbolResolve;
	case KSC_TREE_RSHIFT: ret = genRsh(node, acc); goto condBranchSymbolResolve;
	case KSC_TREE_BITWISE_AND: ret = genBitwiseAnd(node, acc); goto condBranchSymbolResolve;
	case KSC_TREE_BITWISE_XOR: ret = genBitwiseXor(node, acc); goto condBranchSymbolResolve;
	case KSC_TREE_BITWISE_OR: ret = genBitwiseOr(node, acc); goto condBranchSymbolResolve;
	case KSC_TREE_BITWISE_NOT: ret = genBitwiseNot(node, acc); goto condBranchSymbolResolve;
	case KSC_TREE_NEGATE: ret = genNegate(node, acc); goto condBranchSymbolResolve;
condBranchSymbolResolve:
		if (condBranchSymbol)
		{
			fprintf(stdout, "\ttest %s, %s\n", registers[acc], registers[acc]);
			fprintf(stdout, "\tjne .L%ld\n", condBranchSymbol);
			if (condBranchSymbol2)
			{
				fprintf(stdout, "\tjmp .L%ld\n", condBranchSymbol2);
			}
		}
		return ret;
	case KSC_TREE_LOWER: return genLower(node, acc, condBranchSymbol, condBranchSymbol2);
	case KSC_TREE_LOWER_EQUAL: return genLowerEqual(node, acc, condBranchSymbol, condBranchSymbol2);
	case KSC_TREE_GREATER: return genGreater(node, acc, condBranchSymbol, condBranchSymbol2);
	case KSC_TREE_GREATER_EQUAL: return genGreaterEqual(node, acc, condBranchSymbol, condBranchSymbol2);
	case KSC_TREE_EQUAL: return genEqual(node, acc, condBranchSymbol, condBranchSymbol2);
	case KSC_TREE_NOT_EQUAL: return genNotEqual(node, acc, condBranchSymbol, condBranchSymbol2);
	case KSC_TREE_LOGICAL_NOT: return genLogicalNot(node, acc, condBranchSymbol, condBranchSymbol2);
	case KSC_TREE_LOGICAL_AND: return genLogicalAnd(node, acc, condBranchSymbol, condBranchSymbol2);
	case KSC_TREE_LOGICAL_OR: return genLogicalOr(node, acc, condBranchSymbol, condBranchSymbol2);
	case KSC_TREE_COMMA:
		genExpr(node->left, acc, condBranchSymbol, condBranchSymbol2);
	  ret = genExpr(node->right, acc, condBranchSymbol, condBranchSymbol2);
		goto condBranchSymbolResolve;
	}
}

static uint64_t getTypeSize(const KscType *pType)
{
	switch (pType->kind)
	{
	case KSC_TYPE_BYTE:
	case KSC_TYPE_SBYTE:
		return 1;
	case KSC_TYPE_SHORT:
	case KSC_TYPE_USHORT:
	case KSC_TYPE_HALF:
		return 2;
	case KSC_TYPE_INT:
	case KSC_TYPE_UINT:
	case KSC_TYPE_FLOAT:
		return 4;
	case KSC_TYPE_LONG:
	case KSC_TYPE_ULONG:
	case KSC_TYPE_DOUBLE:
	case KSC_TYPE_POINTER:
		return 8;
	default:
		fprintf(stdout, "TODO: other type sizes\n");
		abort();
	}
}

static uint64_t labels = 1;

static void genInit(void)
{
	freeAllRegs();
	labels = 1;
}

static void genStackFrame(uint64_t nBytes)
{
	fprintf(stdout, "\tsub rsp, %ld\n", nBytes);
	fprintf(stdout, "\tpush rbp\n");
	fprintf(stdout, "\tmov rbp, rsp\n");
}

static void genStackFrameRestore(void) { fprintf(stdout, "\tpop rbp\n"); }
static void genJump(uint64_t l) { fprintf(stdout, "\tjmp .L%ld\n", l); }
static void genRet(void){ fprintf(stdout, "\tret\n"); }
static uint64_t genLabel(void) { return labels++; }
static void printLabel(uint64_t l) { fprintf(stdout, ".L%ld:\n", l); }

KscIGenMod x86_64gm =
{
	.genExpression = genExpr,
	.genReturn = genRet,
	.genInit = genInit,
	.genLabel = genLabel,
	.printLabel = printLabel,
	.genJump = genJump,
	.returnAccumulator = NOREG,
	.alignmentBytes = 8,
	.getTypeSize = getTypeSize,
	.genStackFrame = genStackFrame,
	.genStackFrameRestore = genStackFrameRestore,
};