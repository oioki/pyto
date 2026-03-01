#pragma once

struct keyword
{
	char * lexptr;
	TOKEN token;
};

static keyword keywords[] = 
{
	{	"{NEWLINE}"	,	NEWLINE		} ,
	{	"{INDENT}"	,	INDENT		} ,
	{	"{DEDENT}"	,	DEDENT		} ,
	{	"+"			,	OP_PLUS		} ,
	{	"-"			,	OP_MINUS	} ,
	{	"**"		,	OP_POWER	} ,
	{	"*"			,	OP_MULTIPLY	} ,
	{	"/"			,	OP_DIVIDE	} ,
	{	"%"			,	OP_REMAINDER} ,
	{	"~"			,	OP_BITNOT	} ,
	{	"^"			,	OP_XOR		} ,
	{	"&"			,	OP_BITAND	} ,
	{	"|"			,	OP_BITOR	} ,
	{	">>"		,	OP_RSHIFT	} ,
	{	"<<"		,	OP_LSHIFT	} ,

	// Условные операторы
	// должны идти именно в таком порядке
	{	"=="		,	OP_EQ		} ,
	{	"!="		,	OP_NOTEQ	} ,
	{	"<>"		,	OP_NOTEQ	} ,
	{	"<="		,	OP_LESSEQ	} ,
	{	"<"			,	OP_LESS		} ,
	{	">="		,	OP_GREATEREQ} ,
	{	">"			,	OP_GREATER	} ,
	{	"is not"	,	OP_ISNOT	} ,
	{	"not in"	,	OP_NOTIN	} ,
	{	"is"		,	OP_IS		} ,
	{	"in"		,	OP_IN		} ,

	{	"not"		,	OP_NOT		} ,
	{	"and"		,	OP_AND		} ,
	{	"or"		,	OP_OR		} ,

	{	","			,	COMMA		} ,
	{	"."			,	DOT			} ,
	
	{	"+="		,	OP_PLUSEQ	} ,
	{	"-="		,	OP_MINUSEQ	} ,
	{	"*="		,	OP_MULTIPLYEQ} ,
	{	"/="		,	OP_DIVIDEEQ	} ,
	{	"%="		,	OP_REMAINDEREQ} ,
	{	"&="		,	OP_BITANDEQ	} ,
	{	"|="		,	OP_BITOREQ	} ,
	{	"^="		,	OP_XOREQ	} ,
	{	"<<="		,	OP_LSHIFTEQ	} ,
	{	">>="		,	OP_RSHIFTEQ	} ,
	{	"**="		,	OP_POWEREQ	} ,
	{	"="			,	OP_ASSIGN	} ,

	{	"if"		,	IF			} ,
	{	":"			,	COLON		} ,
	{	"elif"		,	ELIF		} ,
	{	"else"		,	ELSE		} ,
	{	"while"		,	WHILE		} ,
	{	"for"		,	FOR			} ,

	{	"("			,	LBRACE		} ,
	{	")"			,	RBRACE		} ,
	{	"["			,	LQBRACE		} ,
	{	"]"			,	RQBRACE		} ,
	{	"{"			,	BRACE		} ,
	{	"}"			,	BRACE		} ,

	{	"True"		,	NUMBER		} ,
	{	"False"		,	NUMBER		} ,
	{	NULL		,	NUL			} ,
	{	NULL		,	UNMATCHED	}
};
