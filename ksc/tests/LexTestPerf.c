#include "../Lex.h"
#include "../Tokens.h"
#include <stdlib.h>
#include <string.h>

#define BUFLENGTH 1024000000

int main(void)
{
    char *ibuf = malloc(BUFLENGTH);
    memset(ibuf, '!', sizeof(ibuf));
    KscToken t;
    memset(&t, 0, sizeof(KscToken));
    while (KscLex(&t));
}