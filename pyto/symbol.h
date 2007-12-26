#pragma once
#include "stdafx.h"
#include "pytotypes.h"

struct SymbolListInfo;

typedef struct TYPEUNION
{
	TYPE type;
	union    // Declare union type
	{
		long	i;
		double	f;
		bool	b;
//		SymbolListInfo* psli;
	} data;
} TYPEUNION;

class Symbol
{
public:
	char *	lexptr;		// указатель на символьное представление в массиве символов
	TOKEN	token;		// токен - род символа
	TYPEUNION*	addr;		// адрес на дополнительную информацию (атрибуты)
	Symbol();
	Symbol(char * lexptr, TOKEN token, ADDR addr);
	void		Set(TOKEN token, char * addr);
	void		Release();
	const TYPE	GetType();

	void		PrintDebug();
	char *		PrintType();
	void		Print(bool putNewline);
	void		PrintNumber();
	void		PrintList();
	
	bool		isNone();
	bool		isUnmatched();
	const bool		GetBooleanValue();
	const long		GetIntegerValue();
	const double	GetFloatValue();
	SymbolListInfo & GetList();
	int			GetCount();
	
	void		SetVariableValue(bool value);
	void		SetVariableEqValue(long value);
	void		SetVariableEqValue(double value);
	void		SetVariableValue(long value);
	void		SetVariableValue(double value);
	void		SetVariableValue(SymbolListInfo & psli);

	void		Insert(Symbol & elem);
};

typedef list< Symbol* > SymbolList;

struct SymbolLink
{
	Symbol sym;
	SymbolLink * next;
};

struct SymbolListInfo
{
	TYPE type;
	SymbolLink * begin;
	SymbolLink * end;
	int count;
};

static const Symbol symNone(NULL,NUL,NULL);
static const Symbol symUnmatched(NULL,UNMATCHED,NULL);
