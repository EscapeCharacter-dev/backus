#include "TypeKinds.h"
#include "Lex.h"
#include "Tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t parseAttributes(void)
{
    KscToken tok;
    uint8_t bitfield = 0;
    while (1)
    {
        if (!KscLexPeek(&tok))
            break;
        switch (tok.kind)
        {
        case KSC_KEYWORD_CONST:
            if (bitfield & KSC_TYPE_ATTRIBUTE_CONST)
            {
                fprintf(stderr, "(%d, %d): duplicate constant type attribute\n", tok.line, tok.column);
                return 0;
            }
            bitfield |= KSC_TYPE_ATTRIBUTE_CONST;
            goto searchForMore;
        case KSC_KEYWORD_VOLATILE:
            if (bitfield & KSC_TYPE_ATTRIBUTE_VOLATILE)
            {
                fprintf(stderr, "(%d, %d): duplicate volatile type attribute\n", tok.line, tok.column);
                return 0;
            }
            bitfield |= KSC_TYPE_ATTRIBUTE_VOLATILE;
            goto searchForMore;
        case KSC_KEYWORD_RESTRICT:
            if (bitfield & KSC_TYPE_ATTRIBUTE_RESTRICT)
            {
                fprintf(stderr, "(%d, %d): duplicate restrict type attribute\n", tok.line, tok.column);
                return 0;
            }
            bitfield |= KSC_TYPE_ATTRIBUTE_RESTRICT;
            goto searchForMore;
        case KSC_KEYWORD_ATOMIC:
            if (bitfield & KSC_TYPE_ATTRIBUTE_ATOMIC)
            {
                fprintf(stderr, "(%d, %d): duplicate atomic type attribute\n", tok.line, tok.column);
                return 0;
            }
            bitfield |= KSC_TYPE_ATTRIBUTE_ATOMIC;
            goto searchForMore;
        }
        return bitfield;
searchForMore:
        if (!KscLex(&tok))
            break;
        continue;
    }
    return bitfield;
}

bool_t KscParseType(KscType *type)
{
    KscToken tok;
    type->attributes = parseAttributes();
    KscLex(&tok);
    switch (tok.kind)
    {
    case KSC_KEYWORD_CHAR: type->kind = KSC_TYPE_SBYTE; break;
    case KSC_KEYWORD_UCHAR: type->kind = KSC_TYPE_BYTE; break;
    case KSC_KEYWORD_SHORT: type->kind = KSC_TYPE_SHORT; break;
    case KSC_KEYWORD_USHORT: type->kind = KSC_TYPE_USHORT; break;
    case KSC_KEYWORD_INT: type->kind = KSC_TYPE_INT; break;
    case KSC_KEYWORD_UINT: type->kind = KSC_TYPE_UINT; break;
    case KSC_KEYWORD_LONG: type->kind = KSC_TYPE_LONG; break;
    case KSC_KEYWORD_ULONG: type->kind = KSC_TYPE_ULONG; break;
    case KSC_KEYWORD_HALF: type->kind = KSC_TYPE_HALF; break;
    case KSC_KEYWORD_FLOAT: type->kind = KSC_TYPE_FLOAT; break;
    case KSC_KEYWORD_DOUBLE: type->kind = KSC_TYPE_DOUBLE; break;
    case KSC_KEYWORD_VOID: type->kind = KSC_TYPE_VOID; break;
    default: fprintf(stderr, "(%d, %d): invalid type\n", tok.line, tok.column); return 0;
    }
    KscLexPeek(&tok);
    while (tok.kind == '*' || tok.kind == '&' || tok.kind == '[')
    {
        if (!KscLex(&tok))
            break;
        KscType *childrenBlock = malloc(sizeof(KscType));
        memcpy(childrenBlock, type, sizeof(KscType));
        KscType *c = childrenBlock;
        memset(type, 0, sizeof(KscType));
        type->childType = c;
        int oldkind = tok.kind;
        KscLexPeek(&tok);
        if (tok.kind == '[')
        {
            KscLex(&tok);
            // TODO: parse expression
            type->kind = KSC_TYPE_ARRAY;
            if (tok.kind != ']')
            {
                fprintf(stderr, "(%d, %d): expected ]\n", tok.line, tok.column);
                return 0;
            }
        }
        else
        {
            type->kind = oldkind == '*' ? KSC_TYPE_POINTER : KSC_TYPE_REFERENCE;
        }
        type->attributes = parseAttributes();
        KscLexPeek(&tok);
    }
    return TRUE;
}