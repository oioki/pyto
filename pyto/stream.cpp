#include "stdafx.h"
#include "stream.h"

void SymbolStream::Init()
{
	cursor = stream.begin();
}

TOKEN SymbolStream::Look()
{
	if ( cursor != stream.end() )
		return (*cursor)->token;
	else
		return NUL;
}

void SymbolStream::Match(TOKEN token)
{
	if ( Look() == token )
	{
		cursor++;
	}
	else
	{
		//PrintSynError("Йоптваюмать!");
	}
}

Symbol & SymbolStream::GetSymbol()
{
	return **cursor;
}

void SymbolStream::PushCursor()
{
	stCursor.push(cursor);
}

void SymbolStream::ApproveCursor()
{
	stCursor.pop();
}

void SymbolStream::PopCursor()
{
	cursor = stCursor.top();
	stCursor.pop();
}

void SymbolStream::Add(Symbol * sym)
{
	stream.push_back(sym);
}

void SymbolStream::Print()
{
	printf("[%15s][%15s][%10s]\n", "STRING", "TOKEN", "ATTRIBUTE");

	SymbolList::iterator p = stream.begin();
	while ( p != stream.end() )
	{
		Symbol * pSymbol = *p;
		pSymbol->PrintDebug();
		p++;
	}
}

bool SymbolStream::isEnded()
{
	return ( cursor == stream.end() );
}

SymbolList::iterator SymbolStream::begin()
{
	return stream.begin();
}

SymbolList::iterator SymbolStream::end()
{
	return stream.end();
}

void SymbolStream::clear()
{
	stream.clear();
}
