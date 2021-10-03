#ifndef INTERFACE_GENERATION_MODULE_H
#define INTERFACE_GENERATION_MODULE_H

#include "Types.h"

// An interface for generation modules
typedef struct KscIGenMod
{
	uint64_t returnAccumulator;
	int64_t (*genExpression)(KscTree *tree, uint64_t accumulator, int64_t condBranch, int64_t elseCondBranch);
	void (*genReturn)(void);
	void (*genInit)(void);
} KscIGenMod;

// Selects a generator.
void KscSelectGenerator(const KscIGenMod *const module);

// Generates a new expression. The passed accumulator will not be freed and will store
// the result of the expression.
uint64_t KscGenExpr(KscTree *tree, uint64_t accumulator, int64_t condBranch, int64_t elseCondBranch);

// Gets the return statement accumulator.
uint64_t KscGenGetReturnAcc(void);

// Performs initialization of the generator.
void KscGenInit(void);

// Generates a return statement.
void KscGenReturn(void);

#endif