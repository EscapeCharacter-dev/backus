#ifndef KSC_LEX_TOKENS_H
#define KSC_LEX_TOKENS_H

#define CHAR2(x, y)             (x << 8 | y)
#define CHAR3(x, y, z)          (x << 16 | y << 8 | z)

#define KSC_TOKEN_EOF           -1
#define KSC_TOKEN_LINT          0x80
#define KSC_TOKEN_LFLOAT        0x81
#define KSC_TOKEN_LSTRING       0x82
#define KSC_TOKEN_DOTDOT        CHAR2('.', '.')
#define KSC_TOKEN_DOTDOTDOT     CHAR3('.', '.', '.')
#define KSC_TOKEN_CUBE          CHAR2(':', ':')
#define KSC_TOKEN_PLUSPLUS      CHAR2('+', '+')
#define KSC_TOKEN_MINUSMINUS    CHAR2('-', '-')
#define KSC_TOKEN_ARROW         CHAR2('-', '>')
#define KSC_TOKEN_LARROWS       CHAR2('<', '<')
#define KSC_TOKEN_RARROWS       CHAR2('>', '>')
#define KSC_TOKEN_LARROWSEQU    CHAR3('<', '<', '=')
#define KSC_TOKEN_RARROWSEQU    CHAR3('>', '>', '=')
#define KSC_TOKEN_EQUALEQUAL    CHAR2('=', '=')
#define KSC_TOKEN_EXCLEQUAL     CHAR2('!', '=')
#define KSC_TOKEN_AMPERSANDS    CHAR2('&', '&')
#define KSC_TOKEN_PIPES         CHAR2('|', '|')
#define KSC_TOKEN_PLUSEQUAL     CHAR2('+', '=')
#define KSC_TOKEN_MINUSEQUAL    CHAR2('-', '=')
#define KSC_TOKEN_STAREQUAL     CHAR2('*', '=')
#define KSC_TOKEN_SLASHEQUAL    CHAR2('/', '=')
#define KSC_TOKEN_PERCENTEQUAL  CHAR2('%', '=')
#define KSC_TOKEN_LARROWEQUAL   CHAR2('<', '=')
#define KSC_TOKEN_RARROWEQUAL   CHAR2('>', '=')
#define KSC_TOKEN_AMPERSEQUAL   CHAR2('&', '=')
#define KSC_TOKEN_CARETEQUAL    CHAR2('^', '=')
#define KSC_TOKEN_PIPEEQUAL     CHAR2('|', '=')

enum
{
	KSC_KEYWORD_DISCARD = 140,
	KSC_KEYWORD_ATOMIC,
	KSC_KEYWORD_AUTO,
	KSC_KEYWORD_BOOL,
	KSC_KEYWORD_BREAK,
	KSC_KEYWORD_CASE,
	KSC_KEYWORD_CATCH,
	KSC_KEYWORD_CHAR,
	KSC_KEYWORD_CONST,
	KSC_KEYWORD_CONTINUE,
	KSC_KEYWORD_DEFAULT,
	KSC_KEYWORD_DO,
	KSC_KEYWORD_DOUBLE,
	KSC_KEYWORD_DYNAMIC,
	KSC_KEYWORD_ELSE,
	KSC_KEYWORD_ENUM,
	KSC_KEYWORD_EXTERN,
	KSC_KEYWORD_FLOAT,
	KSC_KEYWORD_FOR,
	KSC_KEYWORD_FUNCTION,
	KSC_KEYWORD_GOTO,
	KSC_KEYWORD_HALF,
	KSC_KEYWORD_IF,
	KSC_KEYWORD_INT,
	KSC_KEYWORD_IMPLEMENTS,
	KSC_KEYWORD_INTERFACE,
	KSC_KEYWORD_LONG,
	KSC_KEYWORD_MODULE,
	KSC_KEYWORD_PUBLIC,
	KSC_KEYWORD_PURE,
	KSC_KEYWORD_RECORD,
	KSC_KEYWORD_REGISTER,
	KSC_KEYWORD_RESTRICT,
	KSC_KEYWORD_RETURN,
	KSC_KEYWORD_SHORT,
	KSC_KEYWORD_SIZEOF,
	KSC_KEYWORD_STATIC,
	KSC_KEYWORD_STATIC_ASSERT,
	KSC_KEYWORD_STRUCT,
	KSC_KEYWORD_SWITCH,
	KSC_KEYWORD_THROW,
	KSC_KEYWORD_TRY,
	KSC_KEYWORD_TYPEDEF,
	KSC_KEYWORD_UCHAR,
	KSC_KEYWORD_UINT,
	KSC_KEYWORD_ULONG,
	KSC_KEYWORD_UNION,
	KSC_KEYWORD_USHORT,
	KSC_KEYWORD_USING,
	KSC_KEYWORD_VOID,
	KSC_KEYWORD_VOLATILE,
	KSC_KEYWORD_WHILE,
	KSC_IDENT,
};

#define KSC_PLATFORM_KEYWORDS // for now, there's no platform keywords.

// keyword dictionary
static struct { const char *key; int value; } keywordList[] =
{
	{ "_", KSC_KEYWORD_DISCARD },                   // discard
	{ "atomic", KSC_KEYWORD_ATOMIC },               // atomic qualifier
	{ "auto", KSC_KEYWORD_AUTO },                   // automatic storage duration
	{ "bool", KSC_KEYWORD_BOOL },                   // boolean type
	{ "byte", KSC_KEYWORD_UCHAR },                  // 8-bit unsigned integer
	{ "break", KSC_KEYWORD_BREAK },                 // break statement
	{ "case", KSC_KEYWORD_CASE },                   // switch case label
	{ "catch", KSC_KEYWORD_CATCH },                 // catch part of the try statement
	{ "char", KSC_KEYWORD_CHAR },                   // 8-bit signed integer
	{ "const", KSC_KEYWORD_CONST },                 // const qualifier
	{ "continue", KSC_KEYWORD_CONTINUE },           // continue statement
	{ "default", KSC_KEYWORD_DEFAULT },             // default switch label
	{ "do", KSC_KEYWORD_DO },                       // do statement
	{ "double", KSC_KEYWORD_DOUBLE },               // 64-bit float
	{ "dynamic", KSC_KEYWORD_DYNAMIC },             // adds dynamic linking meaning to 'extern' or 'public'
	{ "else", KSC_KEYWORD_ELSE },                   // else branch of if statement
	{ "enum", KSC_KEYWORD_ENUM },                   // enum declaration
	{ "extern", KSC_KEYWORD_EXTERN },               // extern function specifier
	{ "float", KSC_KEYWORD_FLOAT },                 // 32-bit float
	{ "for", KSC_KEYWORD_FOR },                     // for statement
	{ "function", KSC_KEYWORD_FUNCTION },           // function pointer type
	{ "goto", KSC_KEYWORD_GOTO },                   // goto statement
	{ "half", KSC_KEYWORD_HALF },                   // 16-bit float
	{ "if", KSC_KEYWORD_IF },                       // if statement
	{ "int", KSC_KEYWORD_INT },                     // 32-bit signed integer
	{ "implements", KSC_KEYWORD_IMPLEMENTS },       // module interface function implementation
	{ "interface", KSC_KEYWORD_INTERFACE },         // module interface
	{ "long", KSC_KEYWORD_LONG },                   // 64-bit signed integer
	{ "module", KSC_KEYWORD_MODULE },               // module declaration
	{ "public", KSC_KEYWORD_PUBLIC },               // public symbol qualifier
	{ "pure", KSC_KEYWORD_PURE },                   // pure function qualifier
	{ "record", KSC_KEYWORD_RECORD },               // record declaration
	{ "register", KSC_KEYWORD_REGISTER },           // register storage location
	{ "restrict", KSC_KEYWORD_RESTRICT },           // restrict qualifier
	{ "return", KSC_KEYWORD_RETURN },               // return statement
	{ "sbyte", KSC_KEYWORD_CHAR },                  // 8-bit signed integer
	{ "short", KSC_KEYWORD_SHORT },                 // 16-bit signed integer
	{ "sizeof", KSC_KEYWORD_SIZEOF },               // sizeof operator
	{ "static", KSC_KEYWORD_STATIC },               // static storage duration
	{ "static_assert", KSC_KEYWORD_STATIC_ASSERT }, // compile-time assertion
	{ "struct", KSC_KEYWORD_STRUCT },               // struct declaration
	{ "switch", KSC_KEYWORD_SWITCH },               // switch statement
	{ "throw", KSC_KEYWORD_THROW },                 // throw expression
	{ "try", KSC_KEYWORD_TRY },                     // try statement
	{ "typedef", KSC_KEYWORD_TYPEDEF },             // user type declaration
	{ "uchar", KSC_KEYWORD_UCHAR },                 // 8-bit unsigned integer
	{ "uint", KSC_KEYWORD_UINT },                   // 32-bit unsigned integer
	{ "ulong", KSC_KEYWORD_ULONG },                 // 64-bit unsigned integer
	{ "union", KSC_KEYWORD_UNION },                 // union declaration
	{ "ushort", KSC_KEYWORD_USHORT },               // 16-bit unsigned integer
	{ "using", KSC_KEYWORD_USING },                 // module import
	{ "void", KSC_KEYWORD_VOID },                   // incomplete type
	{ "volatile", KSC_KEYWORD_VOLATILE },           // volatile memory qualifier
	{ "while", KSC_KEYWORD_WHILE },                 // while statement
	KSC_PLATFORM_KEYWORDS                           // other platform keywords
};

#define KEYWORD_LIST_LENGTH     sizeof(keywordList)/sizeof(struct { const char *key; int value; })

#endif