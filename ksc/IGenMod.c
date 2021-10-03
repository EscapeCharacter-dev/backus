#include "IGenMod.h"

static const KscIGenMod *mod;

void KscSelectGenerator(const KscIGenMod *const module)
{
	mod = module;
}

uint64_t KscGenExpr(KscTree *tree, uint64_t accumulator, int64_t condBranch, int64_t elseCondBranch)
{
	mod->genExpression(tree, accumulator, condBranch, elseCondBranch);
}

uint64_t KscGenGetReturnAcc(void)
{
	return mod->returnAccumulator;
}

void KscGenInit(void)
{
	mod->genInit();
}

void KscGenReturn(void)
{
	mod->genReturn();
}