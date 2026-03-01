// stdafx.cpp : source file that includes just the standard includes
// Pyto.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

static char buffer[2048];


/*********************************************************/
void PrintDebugNew(char * str, void * addr)
/**********************************************************
Вывод отладочной информации о выделяемой памяти
**********************************************************/
{
	static int count = 0;
#ifdef DEBUG
	printf("NEW %-5d%-20s0x%08X\n",count++,str,addr);
#endif
}


/*********************************************************/
void PrintDebugDelete(char * str, void * addr)
/**********************************************************
Вывод отладочной информации об освобождаемой памяти
**********************************************************/
{
	static int count = 0;
#ifdef DEBUG
	printf("DEL %-5d%-20s0x%08X\n",count++,str,addr);
#endif
}


/*********************************************************/
char * PrintToken(TOKEN token)
/**********************************************************
Возвращает указатель на строку, содержащую токен символа
**********************************************************/
{
#define caseequal(S) case S: return #S;
	switch(token)
	{
		caseequal(NEWLINE);
		caseequal(BRACE);
		caseequal(NAME);
		caseequal(NUL);
		caseequal(NUMBER);
		caseequal(OP);
		caseequal(OP_PLUS);
		caseequal(OP_MINUS);
		caseequal(OP_DIVIDE);
		caseequal(OP_MULTIPLY);
		caseequal(OP_REMAINDER);
		caseequal(OP_BITAND);
		caseequal(OP_BITOR);
		caseequal(OP_XOR);
		caseequal(OP_BITNOT);
		caseequal(OP_RSHIFT);
		caseequal(OP_LSHIFT);
		caseequal(OP_EQ);
		caseequal(OP_NOTEQ);
		caseequal(OP_LESSEQ);
		caseequal(OP_LESS);
		caseequal(OP_GREATEREQ);
		caseequal(OP_GREATER);
		caseequal(OP_ISNOT);
		caseequal(OP_NOTIN);
		caseequal(OP_IS);
		caseequal(OP_IN);
		caseequal(COMMA);
		caseequal(DOT);
		caseequal(OP_NOT);
		caseequal(OP_AND);
		caseequal(OP_OR);
	}
	return "(NOT FOUND)";
}


/*********************************************************/
unsigned char AnsiToOem(unsigned char c)
/**********************************************************
Интерпретирует символ 'c' как символ в кодировке Win1251 и
возвращает этот символ в кодировке OEM866
**********************************************************/
{
	if ( c >= 0xC0 && c <= 0xEF )
		return (c + (0x80-0xC0));

	if ( c >= 0xF0 && c <= 0xFF )
		return (c + (0xE0-0xF0));

	if ( c == 0xB8 )  // ё
		return (0xF1);

	if ( c == 0xA8 )  // Ё
		return (0xF0);

	return (c);
}


/*********************************************************/
char * PrintString(const char * str)
/**********************************************************
Печатает строку 'str'
**********************************************************/
{
	unsigned int i;
	for (i=0; i<strlen(str); i++)
		buffer[i] = AnsiToOem(str[i]);
	buffer[i]='\0';
	return buffer;
}


/*********************************************************/
void PrintWarning(char * str)
/**********************************************************
Печатает предупреждающее сообщение 'str'
**********************************************************/
{
	printf("Warning: %s\n", PrintString(str));
}


/*********************************************************/
void PrintSynError(char * str, ... )
/**********************************************************
Печатает синтаксическую ошибку 'str'
**********************************************************/
{
	va_list list;

	va_start(list, str);
	vsprintf(buffer, str, list);
	va_end(list);

	printf("SYNTAN error: %s", PrintString(buffer));
}


/*********************************************************/
void PrintError(const char * str)
/**********************************************************
Печатает ошибку 'str'
**********************************************************/
{
	printf("%s\n",PrintString(str));
#ifdef _WIN32
	system("PAUSE");
#endif
	exit(0);
}
