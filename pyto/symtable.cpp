#include "stdafx.h"
#include "symtable.h"



SymbolTable::SymbolTable()
{
	symlen		= 0;
	symcnt		= 0;
	freesymcnt	= 0;
}

void SymbolTable::InitKeywords()
{
	int curkw = 0;
	while ( keywords[curkw].lexptr != NULL )
	{
		Insert( keywords[curkw].token, keywords[curkw].lexptr );
		curkw++;
	}
	freesymcnt = symcnt;
}

void SymbolTable::Cleanup()
{
	Insert(INDENT,"");
	Insert(DEDENT,"");
	SetVariableValue(Lookup("True"),true);
	SetVariableValue(Lookup("False"),false);
}

void SymbolTable::InitialClear()
{
	symcnt = freesymcnt;
}

void SymbolTable::Print()
{
	printf("[%15s][%15s][%10s]\n", "STRING", "TOKEN", "ATTRIBUTE");
	for (int i=0; i<symcnt; i++)
	{
		symtable[i].PrintDebug();
	}
}

void SymbolTable::SetToken(int index, TOKEN token)
{
	symtable[index].token = token;
	//symtable[lu].token = NAME;
}

int SymbolTable::Lookup(TOKEN tok)
{
	for (int i=0; i<symcnt; i++)
		if ( symtable[i].token == tok )
			return i;
	return 0;
}

int SymbolTable::Lookup(char * s)
{
	for (int i=0; i<symcnt; i++)
		if ( strcmp(symtable[i].lexptr,s) == 0 )
			return i;
	return 0;
}

int SymbolTable::Insert(TOKEN token, char * s)
{
	char * addr = lexemes + symlen;
	size_t size = strlen(s);

	if ( symcnt+1 >= MAXSYMCNT )
		PrintError("Таблица символов переполнена");
	if ( symlen+size+1 >= MAXSYMLEN )
		PrintError("Массив лексем переполнен");

	symtable[symcnt].Set(token,addr);
	strcpy(addr,s);

	lexemes[symlen+size] = '\0';
	symlen += size+1;
	symcnt++;

	return symcnt-1;
}

void SymbolTable::SetVariableValue(int i, bool value)
{
	symtable[i].SetVariableValue(value);
}

void SymbolTable::SetVariableValue(int i, long value)
{
	symtable[i].SetVariableValue(value);
}

void SymbolTable::SetVariableValue(int i, double value)
{
	symtable[i].SetVariableValue(value);
}

void SymbolTable::SetList(int i, SymbolListInfo & psli)
{
	symtable[i].SetVariableValue(psli);
}

Symbol * SymbolTable::GetSymbolPointer(int index)
{
	return &symtable[index];
}
