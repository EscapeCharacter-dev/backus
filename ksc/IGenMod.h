#ifndef INTERFACE_GENERATION_MODULE_H
#define INTERFACE_GENERATION_MODULE_H

#include "Types.h"

// An interface for generation modules
typedef struct KscIGenMod
{
	int64_t (*genExpression)(KscTree *tree);
} KscIGenMod;

// x86-64 generation module
extern const KscIGenMod x86_64genm;

#endif