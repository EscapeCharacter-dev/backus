#ifndef KSC_LEX_H
#define KSC_LEX_H

#include "Types.h"

// Feeds source code to the lexer.
void KscLexFeed(const char *input);

// Gets the next token.
bool_t KscLex(KscToken *token);

// Peeks at the next token.
bool_t KscLexPeek(KscToken *dest);

// Gets the last good token's pointer. Can be re-used at anytime; it's values might change from read to read.
volatile KscToken *KscLexLastGoodTokenPtr(void);

#endif