#include "Lex.h"
#include "Tokens.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

#define ishexd(x) (isdigit(x) || (x >= 'a' && x <= 'f') || (x >= 'A' && x <= 'F'))
#define isoctald(x) (x >= '0' && x <= '7')
static int hextonum(int character)
{
	if (character >= '0' && character <= '9')
		return character - '0';
	if (character >= 'a' && character <= 'f')
		return character - 'a' + 10;
	if (character >= 'A' && character <= 'F')
		return character - 'A' + 10;
}

static char *inputStr = 0;
static union
{
	char *str;
	uint64_t i;
	double d;
} literal;
static size_t inputStrLength = 0;
static size_t inputStrPosition = 0;
static struct CLexDriver
{
	int32_t lCount;
	int32_t cCount;
} cLexDriver;
static KscToken lastGoodToken;

volatile KscToken *KscLexLastGoodTokenPtr(void) { return &lastGoodToken; }

static char curCharacter(void)
{
	if (inputStrPosition >= inputStrLength)
		return 0;
	return inputStr[inputStrPosition];
}

static char escapeSequence(char c)
{
	char result = c;
	if (c == '\\')
	{
		inputStrPosition++;
		c = curCharacter();
		switch (c)
		{
		case 'a':
		case 'A':
			result = 0x07;
			break;
		case 'b':
		case 'B':
			result = 0x08;
			break;
		case 'e':
		case 'E':
			result = 0x1B;
			break;
		case 'f':
		case 'F':
			result = 0x0C;
			break;
		case 'n':
		case 'N':
			result = 0x0A;
			break;
		case 'r':
		case 'R':
			result = 0x0D;
			break;
		case 't':
		case 'T':
			result = 0x09;
			break;
		case 'v':
		case 'V':
			result = 0x0B;
			break;
		case '\\':
			result = '\\';
			break;
		case '"':
			result = '"';
			break;
		case '\'':
			result = '\'';
			break;
		case '?':
			result = '?';
			break;
		default:
			fprintf(stderr, "(%d, %d): unknown character in escape sequence\n", cLexDriver.lCount, cLexDriver.cCount);
			return 0;
		}
	}
	return result;
}

static int32_t tokKeywordIdent(void)
{
	int n = 0;
	int m = 64;
	char *buf = malloc(m);
	while (1)
	{
		char cur = curCharacter();
		if (!isalnum(cur) && cur != '_')
			break;
		if (n + 1 >= m)
		{
			m += 64;
			buf = realloc(buf, m);
		}
		buf[n++] = cur;
		inputStrPosition++;
	}
	buf[n] = 0;
	literal.str = buf;

	for (int i = 0; i < KEYWORD_LIST_LENGTH; i++)
	{
		if (!strcmp(buf, keywordList[i].key))
			return keywordList[i].value;
	}
	if (buf[0] == 0)
		return 0;
	return KSC_IDENT;
}

static int32_t tokOperator(void)
{
	switch (curCharacter())
	{
	case -1:
		return KSC_TOKEN_EOF;
	case '$':
	case '@':
	case '~':
	case '?':
	case ',':
	case '(':
	case ')':
	case '[':
	case ']':
	case '{':
	case '}':
	case ';':
		return inputStr[inputStrPosition++];
	case ':':
	{
		char char1 = curCharacter();
		inputStrPosition++;
		if (curCharacter() == char1)
			return inputStrPosition++, CHAR2(char1, char1);
		return char1;
	}
	case '.':
	{
		char char1 = curCharacter();
		inputStrPosition++;
		if (curCharacter() == char1)
		{
			inputStrPosition++;
			if (curCharacter() == char1)
				return inputStrPosition++, CHAR3(char1, char1, char1);
			return CHAR2(char1, char1);
		}
		return char1;
	}
	case '|':
	case '&':
	case '+':
	{
		char char1 = curCharacter();
		inputStrPosition++;
		if (curCharacter() == char1)
			return inputStrPosition++, CHAR2(char1, char1);
		if (curCharacter() == '=')
			return inputStrPosition++, CHAR2(char1, '=');
		return char1;
	}
	case '*':
	case '/':
	case '%':
	case '^':
	case '!':
	{
		char char1 = curCharacter();
		inputStrPosition++;
		if (curCharacter() == '=')
			return inputStrPosition++, CHAR2(char1, '=');
		return char1;
	}
	case '-':
	case '=':
	{
		char char1 = curCharacter();
		inputStrPosition++;
		if (curCharacter() == char1)
			return inputStrPosition++, CHAR2(char1, char1);
		if (curCharacter() == '=')
			return inputStrPosition++, CHAR2(char1, '=');
		if (curCharacter() == '>')
			return inputStrPosition++, CHAR2(char1, '>');
		return char1;
	}
	case '<':
	case '>':
	{
		char char1 = curCharacter();
		inputStrPosition++;
		if (curCharacter() == char1)
		{
			inputStrPosition++;
			if (curCharacter() == '=')
				return inputStrPosition++, CHAR3(char1, char1, '=');
			return CHAR2(char1, char1);
		}
		return char1;
	}
	}
	return 0;
}

static int32_t tokLiteralNum(void)
{
	if (curCharacter() == '\'')
	{
		uint64_t accumulator = 0;
		int i = 0;
		inputStrPosition++;
		char c = curCharacter();
		while (c != '\'')
		{
			accumulator <<= 8;
			c = escapeSequence(c);
			if (!c)
				return 0;
			accumulator |= c;
			i++;
			inputStrPosition++;
			c = curCharacter();
		}
		inputStrPosition++;
		literal.i = accumulator;
		return KSC_TOKEN_LINT;
	}
	if (!isdigit(curCharacter()))
		return 0;
	double accumulator = 0;
	bool_t floating = FALSE;
	char c = curCharacter();
	if (c == '0')
	{
		inputStrPosition++;
		c = curCharacter();
		if (tolower(c) == 'x')
		{
			inputStrPosition++;
			c = curCharacter();
			while (ishexd(c))
			{
				accumulator = accumulator * 16 + hextonum(c);
				inputStrPosition++;
				c = curCharacter();
			}
			goto suffix;
		}
		else if (tolower(c) == 'b')
		{
			inputStrPosition++;
			c = curCharacter();
			while (c == '1' || c == '0')
			{
				accumulator = accumulator * 2 + (c == '1' ? 1 : 0);
				inputStrPosition++;
				c = curCharacter();
			}
			goto suffix;
		}
		else if (isoctald(c))
		{
			while (isoctald(c))
			{
				accumulator = accumulator * 8 + c - '0';
				inputStrPosition++;
				c = curCharacter();
			}
			goto suffix;
		}
	}

	while (isdigit(c))
	{
		accumulator = accumulator * 10 + c - '0';
		inputStrPosition++;
		c = curCharacter();
	}

	if (c == '.')
	{
		floating = TRUE;
		double scale = 1;
		inputStrPosition++;
		c = curCharacter();
		while (isdigit(c))
		{
			scale /= 10.0;
			accumulator = accumulator + (c - '0') * scale;
			inputStrPosition++;
			c = curCharacter();
		}
		if (tolower(c) == 'e')
		{
			int16_t exp = 0;
			int16_t scale = 1;
			inputStrPosition++;
			c = curCharacter();
			if (c == '-')
			{
				scale = -scale;
				inputStrPosition++;
				c = curCharacter();
			}
			while (isdigit(c))
			{
				exp = exp + (c - '0') * scale;
				inputStrPosition++;
				c = curCharacter();
				scale *= 10;
			}
			inputStrPosition--;
			accumulator *= (double)exp * 10;
		}
	}

suffix:
	if (floating)
	{
		literal.d = accumulator;
		return KSC_TOKEN_LFLOAT;
	}
	literal.i = accumulator;
	return KSC_TOKEN_LINT;
}

static int32_t tokLiteralString(void)
{
	char c = curCharacter();
	if (c == '"')
	{
		inputStrPosition++;
		int n = 0;
		int m = 64;
		char *buf = malloc(m);
		while (1)
		{
			char cur = curCharacter();
			if (cur == '"')
				break;
			if (!curCharacter())
			{
				fprintf(stderr, "(%d, %d): unfinished string\n", cLexDriver.lCount, cLexDriver.cCount);
				return 0;
			}
			if (n + 1 >= m)
			{
				m += 64;
				buf = realloc(buf, m);
			}
			buf[n++] = cur;
			inputStrPosition++;
		}
		inputStrPosition++;
		buf[n] = 0;
		literal.str = buf;
		return KSC_TOKEN_LSTRING;
	}
	return 0;
}

static void cLexDriverPrepareReadTok(void)
{
	while (isspace(curCharacter()))
	{
		if (curCharacter() == '\n')
			cLexDriver.lCount++;
		else
			cLexDriver.cCount++;
		inputStrPosition++;
	}
}

void KscLexFeed(const char *input)
{
	inputStr = malloc(strlen(input));
	strcpy(inputStr, input);
	strcat(inputStr, " ");
	inputStrPosition = 0;
	cLexDriver.lCount = 1;
	cLexDriver.cCount = 1;
	inputStrLength = strlen(inputStr);
}

bool_t KscLexPeek(KscToken *dest)
{
	int32_t pos = inputStrPosition;
	struct CLexDriver old2dpos = cLexDriver;
	bool_t result = KscLex(dest);
	cLexDriver = old2dpos;
	inputStrPosition = pos;
	return result;
}

bool_t KscLex(KscToken *dest)
{
	cLexDriverPrepareReadTok();
	dest->line = cLexDriver.lCount;

	int32_t pos = inputStrPosition;
	int32_t result = 0;

	result = tokOperator();
	if (result)
	{
		dest->kind = result;
		goto lReturn;
	}
	result = tokLiteralNum();
	if (result)
	{
		dest->kind = result;
		if (result == KSC_TOKEN_LFLOAT)
		{
			dest->data = malloc(sizeof(double)); // sizeof(double) might not == sizeof(uint64_t)
			*(double *)(dest->data) = literal.d;
			goto lReturn;
		}
		dest->data = malloc(sizeof(uint64_t));
		*(uint64_t *)(dest->data) = literal.i;
		goto lReturn;
	}
	result = tokLiteralString();
	if (result)
	{
		dest->kind = result;
		dest->data = literal.str;
		goto lReturn;
	}
	result = tokKeywordIdent();
	if (result)
	{
		dest->kind = result;
		dest->data = literal.str;
		goto lReturn;
	}

lReturn:
	dest->column = cLexDriver.cCount;
	cLexDriver.cCount += inputStrPosition - pos;
	if (result)
		lastGoodToken = *dest;
	return !!result; // clamping
}