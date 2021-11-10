#include "IGenMod.h"

static const KscIGenMod *mod;

void KscSelectGenerator(const KscIGenMod *const module)
{
	mod = module;
}

uint64_t KscGenExpr(KscTree *tree, uint64_t accumulator, uint64_t condBranch, uint64_t elseCondBranch)
{
	mod->genExpression(tree, accumulator, condBranch, elseCondBranch);
}

uint64_t KscGenGetReturnAcc(void)
{
	return mod->returnAccumulator;
}

uint64_t KscGenGetAlignment(void)
{
	return mod->alignmentBytes;
}

uint64_t KscGenGetTypeSize(const KscType *type)
{
	return mod->getTypeSize(type);
}

void KscGenStackFrameRestore(void)
{
	return mod->genStackFrameRestore();
}

void KscGenStackFrame(uint64_t nBytes)
{
	return mod->genStackFrame(nBytes);
}

void KscGenInit(void)
{
	mod->genInit();
}

void KscGenReturn(void)
{
	mod->genReturn();
}

uint64_t KscGenLabel(void)
{
	return mod->genLabel();
}

void KscPrintLabel(uint64_t l)
{
	mod->printLabel(l);
}

void KscJump(uint64_t lIteration)
{
	mod->genJump(lIteration);
}