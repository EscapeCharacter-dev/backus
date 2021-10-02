#ifndef NODE_KINDS_H
#define NODE_KINDS_H

#include "Types.h"

static const char *const treeEnumString[] =
		{
				"MALFORMED",
				"<int>",
				"<float>",
				"<string>",
				"<ident>",
				"<module access>",
				"<postfix increment>",
				"<postfix decrement>",
				"<function call>",
				"<array subscript>",
				"<member access>",
				"<member access indirect by pointer>",
				"<prefix increment>",
				"<prefix decrement>",
				"<unary plus>",
				"<negation>",
				"<logical not>",
				"<bitwise not>",
				"<cast>",
				"<dereference>",
				"<address of>",
				"<sizeof>",
				"<multiply>",
				"<divide>",
				"<remainder of division>",
				"<add>",
				"<subtract>",
				"<left shift>",
				"<right shift>",
				"<lower>",
				"<lower or equal>",
				"<greater>",
				"<greater or equal>",
				"<equal>",
				"<not equal>",
				"<bitwise and>",
				"<bitwise exclusive or>",
				"<bitwise inclusive or>",
				"<logical and>",
				"<logical or>",
				"<ternary>",
				"<ternary+>",
				"<throw>",
				"<assign>",
				"<add assign>",
				"<sub assign>",
				"<mul assign>",
				"<div assign>",
				"<mod assign>",
				"<lsh assign>",
				"<rsh assign>",
				"<and assign>",
				"<xor assign>",
				"<or assign>",
				"<comma>",
};

enum
{
	KSC_TREE_MALFORMED = 0, // <bad tree/missing>

	KSC_TREE_LITERAL_INTEGER, // [0-9]+
	KSC_TREE_LITERAL_FLOAT,		// [0-9]+.[0-9]+
	KSC_TREE_LITERAL_STRING,	// "this is an example string"
	KSC_TREE_IDENTIFIER,			// [a-zA-Z0-9_]+

	KSC_TREE_MODULE_ACCESS,			// A::B
	KSC_TREE_POSTFIX_INC,				// A++
	KSC_TREE_POSTFIX_DEC,				// A--
	KSC_TREE_FUNCTION_CALL,			// A()
	KSC_TREE_ARRAY_SUBSCRIPT,		// A[]
	KSC_TREE_MEMBER_ACCESS,			// A.B
	KSC_TREE_MEMBER_ACCESS_IND, // A->B

	KSC_TREE_PREFIX_INC,	// ++A
	KSC_TREE_PREFIX_DEC,	// --A
	KSC_TREE_UNARY_PLUS,	// +A
	KSC_TREE_NEGATE,			// -A
	KSC_TREE_LOGICAL_NOT, // !A
	KSC_TREE_BITWISE_NOT, // ~A
	KSC_TREE_CAST,				// (B)A
	KSC_TREE_DEREFERENCE, // *A
	KSC_TREE_ADDRESSOF,		// &A
	KSC_TREE_SIZEOF,			// sizeof(a)

	KSC_TREE_MUL, // A*B
	KSC_TREE_DIV, // A/B
	KSC_TREE_MOD, // A%B

	KSC_TREE_ADD, // A+B
	KSC_TREE_SUB, // A-B

	KSC_TREE_LSHIFT, // A<<B
	KSC_TREE_RSHIFT, // A>>B

	KSC_TREE_LOWER,					// A<B
	KSC_TREE_LOWER_EQUAL,		// A<=B
	KSC_TREE_GREATER,				// A>B
	KSC_TREE_GREATER_EQUAL, // A>=B

	KSC_TREE_EQUAL,			// A==B
	KSC_TREE_NOT_EQUAL, // A!=B

	KSC_TREE_BITWISE_AND, // A&B

	KSC_TREE_BITWISE_XOR, // A^B

	KSC_TREE_BITWISE_OR, // A|B

	KSC_TREE_LOGICAL_AND, // A&&B

	KSC_TREE_LOGICAL_OR, // A||B

	KSC_TREE_TERNARY_ROOT,			// A?B:C (A?B)
	KSC_TREE_TERNARY_EVALUATES, // A?B:C (B:C)
	KSC_TREE_THROW,							// throw A

	KSC_TREE_ASSIGN,		 // A=B
	KSC_TREE_ADD_ASSIGN, // A+=B
	KSC_TREE_SUB_ASSIGN, // A-=B
	KSC_TREE_MUL_ASSIGN, // A*=B
	KSC_TREE_DIV_ASSIGN, // A/=B
	KSC_TREE_MOD_ASSIGN, // A%=B
	KSC_TREE_LSH_ASSIGN, // A<<=B
	KSC_TREE_RSH_ASSIGN, // A>>=B
	KSC_TREE_AND_ASSIGN, // A&=B
	KSC_TREE_XOR_ASSIGN, // A^=B
	KSC_TREE_OR_ASSIGN,	 // A|=B

	KSC_TREE_COMMA, // A,B
};

#endif