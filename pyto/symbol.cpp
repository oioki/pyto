#include "stdafx.h"
#include "symbol.h"

Symbol::Symbol()
{
	lexptr = NULL;
	token  = NUL;
	addr   = NULL;
}

Symbol::Symbol(char * lexptr, TOKEN token, ADDR addr)
{
	this->lexptr = lexptr;
	this->token  = token;
	this->addr   = (TYPEUNION*)addr;

	if ( token == SYMLIST )
	{
		SymbolListInfo * psli = new SymbolListInfo;
		PrintDebugNew("SymbolListInfo", psli);
		this->token = NAME;
		this->addr = (TYPEUNION *)psli;
		psli->type = TYPE_SYMLIST;
		psli->begin = NULL;
		psli->end = NULL;
		psli->count = 0;
	}
}

// TODO: возможно, обнуление lexptr и token не требуется
void Symbol::Release()
{
	if ( token == SYMLIST || ( token == NAME && addr != NULL && addr->type == TYPE_SYMLIST ) )
	{
		SymbolLink * pSym = ((SymbolListInfo*)(addr))->begin;
		while ( pSym != NULL )
		{
			SymbolLink * next = pSym->next;
			PrintDebugDelete("Symbol", pSym->sym.addr);
			delete pSym->sym.addr;
			PrintDebugDelete("SymbolLink", pSym);
			delete pSym;
			pSym = next;
		}
	}
	PrintDebugDelete("Symbol",addr);
	delete addr;
	this->addr = NULL;
	this->token = NUL;
	this->lexptr = NULL;
}

void Symbol::Set(TOKEN token, char * addr)
{
	this->token = token;
	this->lexptr = addr;
}

void Symbol::PrintDebug()
{
	printf("[%15s][%15s][0x%08X]\n", lexptr, PrintToken(token), (unsigned long)addr);
}

void Symbol::Print(bool putNewline)
{
	switch ( token )
	{
	case NAME:
	case NUMBER:
		PrintNumber();
		break;
	case SYMLIST:
		PrintList();
		break;
	default:
		putNewline = false;
		//PrintSynError("Неизвестный символ в качестве результата");
		break;
	}
	
	if ( putNewline )
		printf("\n");
}

void Symbol::PrintNumber()
{
	switch ( GetType() )
	{
	case TYPE_NULL:
		PrintSynError("Синтаксическая ошибка, результат неопределен");
		break;
	case TYPE_BOOLEAN:
		if ( GetBooleanValue() == true )
			printf("True");
		else
			printf("False");
		break;
	case TYPE_INTEGER:
		printf("%ld",GetIntegerValue());
		break;
	case TYPE_FLOAT:
		printf("%lf",GetFloatValue());
		break;
	case TYPE_SYMLIST:
		PrintList();
		break;
	}
}

void Symbol::PrintList()
{
	SymbolLink * pSym = ((SymbolListInfo*)addr)->begin;
	printf("(");
	while ( pSym != NULL )
	{
		SymbolLink * next = pSym->next;
		pSym->sym.Print(false);
		if ( next != NULL )
			printf(", ");
		pSym = next;
	}
	printf(")");
}

bool Symbol::isNone()
{
	return ( token == NUL );
}

bool Symbol::isUnmatched()
{
	return ( token == UNMATCHED );
}

int Symbol::GetCount()
{
	return ((SymbolListInfo*)(addr))->count;
}

const bool Symbol::GetBooleanValue()
{
	if ( GetIntegerValue() == 0.0 )
	{
		return false;
	}
	else
	{
		return true;
	}
}

const long Symbol::GetIntegerValue()
{
	return ((IntegerVariable*)(addr))->value;
}

const double Symbol::GetFloatValue()
{
	return ((FloatVariable*)(addr))->value;
}

SymbolListInfo & Symbol::GetList()
{
	return *(SymbolListInfo*)(addr);
}

const TYPE Symbol::GetType()
{
	if ( addr != NULL )
		return *(TYPE*)addr;
	else
		return TYPE_NULL;
}

char * Symbol::PrintType()
{
	static char buffer[128];
	switch( GetType() )
	{
	case TYPE_NULL:		return "null";		break;
	case TYPE_BOOLEAN:	return "bool";		break;
	case TYPE_INTEGER:	return "int";		break;
	case TYPE_FLOAT:	return "float";		break;
	default:			return "(unknown)";	break;
	}
}

void Symbol::SetVariableValue(SymbolListInfo & psli)
{
	char * strTemp = lexptr;
	TOKEN tokenTemp = token;

	SymbolListInfo * newaddr;
	if ( addr == NULL )
	{
		newaddr = new SymbolListInfo;
		PrintDebugNew("SymbolListInfo", newaddr);
		addr = (TYPEUNION*)newaddr;
	}
	else
	if ( GetType() != TYPE_SYMLIST )
	{
		Release();
		newaddr = new SymbolListInfo;
		PrintDebugNew("SymbolListInfo", newaddr);
		addr = (TYPEUNION*)newaddr;
	}

	this->lexptr = strTemp;
	this->token = tokenTemp;

	SymbolListInfo* addrInteger = (SymbolListInfo*)addr;
	addrInteger->type = TYPE_SYMLIST;
	addrInteger->begin = psli.begin;
	addrInteger->end = psli.end;
	addrInteger->count = psli.count;

	// TODO: скопировать?
}

void Symbol::SetVariableEqValue(double value)
{
	if ( value == 0.0 )
		SetVariableValue(false);
	else
		SetVariableValue(true);
}

void Symbol::SetVariableEqValue(long value)
{
	if ( value == 0 )
		SetVariableValue(false);
	else
		SetVariableValue(true);
}


void Symbol::SetVariableValue(bool value)
{
	char * strTemp = lexptr;
	TOKEN tokenTemp = token;

	IntegerVariable * newaddr;
	if ( addr == NULL )
	{
		newaddr = new IntegerVariable;
		PrintDebugNew("IntegerVariable", newaddr);
		addr = (TYPEUNION*)newaddr;
	}
	else
	if ( GetType() != TYPE_BOOLEAN )
	{
		Release();
		newaddr = new IntegerVariable;
		PrintDebugNew("IntegerVariable", newaddr);
		addr = (TYPEUNION*)newaddr;
	}

	this->lexptr = strTemp;
	if ( token == NUL )
	{
		this->token = NUMBER;
	}
	else
	{
		this->token = tokenTemp;
	}

	IntegerVariable* addrBoolean = (IntegerVariable*)addr;
	addrBoolean->type = TYPE_BOOLEAN;
	addrBoolean->value = value;
}

void Symbol::Insert(Symbol & elem)
{
	SymbolListInfo * psli = (SymbolListInfo*)addr;
	
	SymbolLink * newbie = new SymbolLink;
	PrintDebugNew("SymbolLink", newbie);

	newbie->sym = elem;
	newbie->next = NULL;

	if ( psli->begin == NULL )
	{
		psli->begin = newbie;
	}
	else
	{
		psli->end->next = newbie;
	}
	psli->end = newbie;

	psli->count++;
}

void Symbol::SetVariableValue(long value)
{
	char * strTemp = lexptr;
	TOKEN tokenTemp = token;

	IntegerVariable * newaddr;
	if ( addr == NULL )
	{
		newaddr = new IntegerVariable;
		PrintDebugNew("IntegerVariable", newaddr);
		addr = (TYPEUNION*)newaddr;
	}
	else
	if ( GetType() != TYPE_INTEGER )
	{
		Release();
		newaddr = new IntegerVariable;
		PrintDebugNew("IntegerVariable", newaddr);
		addr = (TYPEUNION*)newaddr;
	}

	this->lexptr = strTemp;
	this->token = tokenTemp;

	IntegerVariable* addrInteger = (IntegerVariable*)addr;
	addrInteger->type = TYPE_INTEGER;
	addrInteger->value = value;
}

void Symbol::SetVariableValue(double value)
{
	char * strTemp = lexptr;
	TOKEN tokenTemp = token;

	FloatVariable * newaddr;
	if ( addr == NULL )
	{
		newaddr = new FloatVariable;
		PrintDebugNew("FloatVariable", newaddr);
		addr = (TYPEUNION*)newaddr;
	}
	else
	if ( GetType() != TYPE_FLOAT )
	{
		Release();
		newaddr = new FloatVariable;
		PrintDebugNew("FloatVariable", newaddr);
		addr = (TYPEUNION*)newaddr;
	}

	this->lexptr = strTemp;
	this->token = tokenTemp;

	FloatVariable* addrFloat = (FloatVariable*)addr;
	addrFloat->type = TYPE_FLOAT;
	addrFloat->value = value;
}
