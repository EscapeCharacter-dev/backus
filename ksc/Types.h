#ifndef KSC_TYPES_H
#define KSC_TYPES_H

#include <stdint.h>

typedef _Bool bool_t;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

// A token
typedef struct KscToken
{
	uint32_t kind;	 // The kind of the token
	uint32_t line;	 // Line
	uint32_t column; // Column
	void *data;			 // Data pointer
} KscToken;

// Type structure
typedef struct KscType
{
	int kind;									 // Kind
	uint8_t attributes;				 // Attribute bit field
	void *data;								 // Additional data
	struct KscType *childType; // Sub type WARNING: IF USING VOODOO MAGIC IN TYPEPARSER.C MAKE THIS LAST FIELD OF THE STRUCT
} KscType;

// Constant attribute value
#define KSC_TYPE_ATTRIBUTE_CONST 0b00000001
// Volatile attribute value
#define KSC_TYPE_ATTRIBUTE_VOLATILE 0b00000010
// Restrict attribute value
#define KSC_TYPE_ATTRIBUTE_RESTRICT 0b00000100
// Atomic attribute value
#define KSC_TYPE_ATTRIBUTE_ATOMIC 0b00001000

// Represents a binary tree.
typedef struct KscTree
{
	struct KscTree *left;	 // Left tree node
	struct KscTree *right; // Right tree node
	uint32_t kind;				 // Binary tree kind
	KscToken token;				 // Tree node
} KscTree;

#endif