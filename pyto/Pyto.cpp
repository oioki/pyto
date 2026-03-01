#include "stdafx.h"

#include <stdio.h>

using namespace std;

#include "tokentypes.h"
#include "symbol.h"
#include "lexer.h"
#include "symtable.h"
#include "stream.h"
#include "input.h"

#include "syntax.h"

int cur;				// текущий символ строки 'lineinput'
int linenum = 0;		// текущая строка в визуальном смысле
int linecur = 0;		// текущий символ в текущей визуальной строке
char lineinput[100];	// входная строка для лексического разбора

SymbolTable symtable;	// таблица символов
SymbolStream stream;	// список токенов

bool modeCalc = true;		// включен ли режим вычисления
bool isMultiline = false;	// требуется ли еще ввод (для синтаксической корректности)


void Intro()
{
	char intro[] = "Pyto 2007. Самый позитивный в мире интерпретатор языка Python.\n";
	printf("%s",PrintString(intro));
}


/*********************************************************/
void Lexan()
/**********************************************************
Выполняет лексический анализ строки 'lineinput'
**********************************************************/
{
	cur = 0;
	int p = 0;
	do
	{
		LexAnalyzer();
	} while ( lineinput[cur] != '\0' );

	int lu = symtable.Lookup("pi");
	if ( lu > 0 )
	{
		symtable.SetToken(lu,NAME);
		symtable.SetVariableValue(lu, 3.14159);
	}
}


/*********************************************************/
void pyto_interactive()
/**********************************************************
Запуск Pyto в интерактивном режиме
**********************************************************/
{
	while (1)
	{
		symtable.Cleanup();
		stream.clear();

		isMultiline = false;
		modeCalc = false;

		Symbol result;
		do
		{
			string_entering();
			Lexan();
			stream.Init();
			result = single_input();
		} while ( isMultiline );

		modeCalc = true;
		stream.Init();
		result = single_input();

		result.Print(true);

		if ( result.addr != NULL )
			result.Release();
	}
}


/*********************************************************/
void pyto_batch(char * filename)
/**********************************************************
Запуск Pyto в режиме выполнения файла 'filename'
**********************************************************/
{
	FILE * fp = fopen(filename,"r");
	while ( !feof(fp) )
	{
		lineinput[cur] = fgetc(fp);
		cur++;
	}
	lineinput[cur-1] = '\0';
	fclose(fp);

	symtable.Cleanup();
	stream.clear();

	Lexan();
	Symbol symDedent(NULL,DEDENT,NULL);
	Symbol symNewline(NULL,NEWLINE,NULL);
	stream.Add(&symDedent);
	stream.Add(&symNewline);
	SetIndent(0);

	stream.Init();

	modeCalc = true;

	Symbol result;
	do
	{
		result = single_input();
	} while ( !stream.isEnded() );

	result.Print(true);

	if ( result.addr != NULL )
		result.Release();
}


int main(int argc, char * argv[])
{
	Intro();
	symtable.InitKeywords();

	if ( argc > 1 )
		pyto_batch(argv[1]);
	pyto_interactive();

#ifdef _WIN32
	system("PAUSE");
#endif
	return 0;
}
