#ifndef INTERFACE_GENERATION_MODULE_H
#define INTERFACE_GENERATION_MODULE_H

#include "Types.h"

#define NOREG 0xFF

// An interface for generation modules
typedef struct KscIGenMod
{
	uint64_t returnAccumulator;
	uint64_t (*genExpression)(KscTree *tree, uint64_t accumulator, uint64_t condBranch, uint64_t elseCondBranch);
	void (*genWhileIteration)(uint64_t lIterator);
	void (*genReturn)(void);
	void (*genInit)(void);
	uint64_t (*genLabel)(void);
	void (*printLabel)(uint64_t l);
} KscIGenMod;

// Selects a generator.
void KscSelectGenerator(const KscIGenMod *const module);

// Generates a new expression. The passed accumulator will not be freed and will store
// the result of the expression.
uint64_t KscGenExpr(KscTree *tree, uint64_t accumulator, uint64_t condBranch, uint64_t elseCondBranch);

// Gets the return statement accumulator.
uint64_t KscGenGetReturnAcc(void);

// Performs initialization of the generator.
void KscGenInit(void);

// Generates a return statement.
void KscGenReturn(void);

// Generates an iterating jump statement.
void KscGenWhileIteration(uint64_t lIterate);

// Generates a new label.
uint64_t KscGenLabel(void);

// Generates the code for the label.
void KscPrintLabel(uint64_t l);

#endif