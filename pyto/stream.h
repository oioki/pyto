#pragma once
#include "symbol.h"

typedef stack< SymbolList::iterator > CursorStack;


class SymbolStream
{
	SymbolList::iterator cursor;
	CursorStack stCursor;
	SymbolList stream;
public:
	void	Init();
	void	Add(Symbol * sym);
	void	Print();
	void	clear();
	bool	isEnded();

	TOKEN	Look();
	void	Match(TOKEN token);

	void	PushCursor();
	void	ApproveCursor();
	void	PopCursor();

	Symbol &	GetSymbol();

	SymbolList::iterator	begin();
	SymbolList::iterator	end();
};
