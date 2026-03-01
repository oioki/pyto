#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "keywords.h"
#include "symbol.h"

#define MAXSYMLEN 4096
#define MAXSYMCNT 1024


class SymbolTable
{
	char lexemes[MAXSYMLEN];	// массив символов
	size_t symlen;				// указатель на свободное место в массиве символов
	Symbol symtable[MAXSYMCNT];	// таблица лексем
	int symcnt;					// количество лексем в таблице

	int freesymcnt;				// количество лексем в таблице до разбора введенной строки
public:
	SymbolTable();
	void	InitKeywords();
	void	Cleanup();
	void	InitialClear();
	void	Print();
	void	SetToken(int index, TOKEN token);
	int		Lookup(TOKEN tok);
	int		Lookup(char * s);
	int		Insert(TOKEN token, char * s);
	void	SetVariableValue(int i, bool value);
	void	SetVariableValue(int i, long value);
	void	SetVariableValue(int i, double value);
	void	SetList(int i, SymbolListInfo & value);
	Symbol*	GetSymbolPointer(int index);
};
