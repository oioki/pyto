#include "stdafx.h"
#include "operations.h"
#include "symtable.h"

extern bool modeCalc;
extern SymbolTable symtable;

bool isOpAugAssign(TOKEN op)
{
	return ( op >= OP_PLUSEQ && op <= OP_POWEREQ );
}

bool isOpComp(TOKEN op)
{
	return ( op >= OP_EQ && op <=OP_IN );
}


void OpMinus(Symbol & a)
{
	if ( !modeCalc ) return;
	switch ( a.GetType() )
	{
	case TYPE_BOOLEAN:
	case TYPE_INTEGER:
		a.SetVariableValue(-a.GetIntegerValue());
		break;
	case TYPE_FLOAT:
		a.SetVariableValue(-a.GetFloatValue());
		break;
	default:
		PrintSynError("Неизвестный тип операнда");
	}
}

void OpBitNot(Symbol & a)
{
	if ( !modeCalc ) return;
	switch( a.GetType() )
	{
	case TYPE_BOOLEAN:
	case TYPE_INTEGER:
		a.SetVariableValue(-1-a.GetIntegerValue());
		break;
	case TYPE_FLOAT:
		PrintSynError("Оператор ~ неприменим к переменным типа 'float'");
		break;
	default:
		PrintSynError("Неизвестный тип операнда");
	}
}

long pw(long a, long b)
{
	long tmp = 1;
	for (long i=0; i<b; i++)
		tmp *= a;
	return tmp;
}

double pw(double a,long b)
{
	return pow(a,b);
}

double pw(long a, double b)
{
	return pow(a,b);
}

double pw(double a, double b)
{
	return pow(a,b);
}

#define DO_MULTGENERAL(op) \
	if ( op == OP_MULTIPLY ) \
	a.SetVariableValue(valuea*valueb); \
	else \
	if ( op == OP_DIVIDE ) \
	a.SetVariableValue(valuea/valueb); \
	else \
	if ( op == OP_POWER ) \
	a.SetVariableValue(pw(valuea,valueb)); \
	else


void OpMultiplyGeneral(Symbol & a, Symbol & b, TOKEN op )
{
	if ( !modeCalc ) return;
	TYPE typea = a.GetType();
	TYPE typeb = b.GetType();
	if ( typea == TYPE_BOOLEAN || typea == TYPE_INTEGER )
	{
		long valuea = a.GetIntegerValue();
		if ( typeb == TYPE_BOOLEAN || typeb == TYPE_INTEGER )
		{
			long valueb = b.GetIntegerValue();
			DO_MULTGENERAL(op);
			if ( op == OP_REMAINDER )
				a.SetVariableValue(valuea % valueb);
			return;
		}
		else
			if ( typeb == TYPE_FLOAT )
			{
				double valueb = b.GetFloatValue();
				DO_MULTGENERAL(op)
					if ( op == OP_REMAINDER )
						a.SetVariableValue(valuea - valueb*ceil(valuea/valueb));
				return;
			}
			else
			{
				PrintSynError("Неизвестный тип операнда");
			}
	}
	else
		if ( typea == TYPE_FLOAT )
		{
			double valuea = a.GetFloatValue();

			if ( typeb == TYPE_BOOLEAN || typeb == TYPE_INTEGER )
			{
				long valueb = b.GetIntegerValue();
				DO_MULTGENERAL(op)
					if ( op == OP_REMAINDER )
						a.SetVariableValue(valuea - valueb*ceil(valuea/valueb));
				return;
			}
			else
				if ( typeb == TYPE_FLOAT )
				{
					double valueb = b.GetFloatValue();
					DO_MULTGENERAL(op)
						if ( op == OP_REMAINDER )
							a.SetVariableValue(valuea - valueb*ceil(valuea/valueb));
					return;
				}
				else
				{
					PrintSynError("Неизвестный тип операнда");
				}
		}
		else
			PrintSynError("Неизвестный тип операнда");
}

#define DO_ADDGENERAL(op) \
	if ( op == OP_PLUS ) \
	a.SetVariableValue(valuea+valueb); \
	else \
	if ( op == OP_MINUS ) \
	a.SetVariableValue(valuea-valueb); \

void OpAddGeneral(Symbol & a, Symbol & b, TOKEN op)
{
	if ( !modeCalc ) return;
	TYPE typea = a.GetType();
	TYPE typeb = b.GetType();
	if ( typea == TYPE_BOOLEAN || typea == TYPE_INTEGER )
	{
		long valuea = a.GetIntegerValue();
		if ( typeb == TYPE_BOOLEAN || typeb == TYPE_INTEGER )
		{
			long valueb = b.GetIntegerValue();
			DO_ADDGENERAL(op);
			return;
		}
		else
			if ( typeb == TYPE_FLOAT )
			{
				double valueb = b.GetFloatValue();
				DO_ADDGENERAL(op)
					return;
			}
			else
			{
				PrintSynError("Неизвестный тип операнда");
			}
	}
	else
		if ( typea == TYPE_FLOAT )
		{
			double valuea = a.GetFloatValue();
			if ( typeb == TYPE_BOOLEAN || typeb == TYPE_INTEGER )
			{
				long valueb = b.GetIntegerValue();
				DO_ADDGENERAL(op)
					return;
			}
			else
				if ( typeb == TYPE_FLOAT )
				{
					double valueb = b.GetFloatValue();
					DO_ADDGENERAL(op)
						return;
				}
				else
				{
					PrintSynError("Неизвестный тип операнда");
				}
		}
		else
			PrintSynError("Неизвестный тип операнда");
}

bool isIntegral(TYPE type)
{
	return ( type == TYPE_BOOLEAN || type == TYPE_INTEGER );
}

/********************************************************************/
void OpBitGeneral(Symbol & a, Symbol & b, TOKEN op)
/*********************************************************************
Вычисляет логическую функцию (в зависимости от 'op') и записывает в 'a'
*********************************************************************/
{
	if ( !modeCalc ) return;
	TYPE typea = a.GetType();
	TYPE typeb = b.GetType();
	if ( isIntegral(typea) && isIntegral(typeb) )
	{
		if ( typea == TYPE_BOOLEAN && typeb == TYPE_BOOLEAN && ( op == OP_BITAND || op == OP_BITOR || op == OP_XOR ) )
		{
			bool valuea = a.GetBooleanValue();
			bool valueb = b.GetBooleanValue();
			switch( op )
			{
			case OP_BITAND:		a.SetVariableValue(valuea && valueb);	break;
			case OP_BITOR:		a.SetVariableValue(valuea || valueb);	break;
			case OP_XOR:		a.SetVariableValue(valuea != valueb);	break;
			}
		}
		else
		{
			long valuea = a.GetIntegerValue();
			long valueb = b.GetIntegerValue();
			switch( op )
			{
			case OP_RSHIFT:		a.SetVariableValue(valuea >> valueb);	break;
			case OP_LSHIFT:		a.SetVariableValue(valuea << valueb);	break;
			case OP_BITAND:		a.SetVariableValue(valuea & valueb);	break;
			case OP_BITOR:		a.SetVariableValue(valuea | valueb);	break;
			case OP_XOR:		a.SetVariableValue(valuea ^ valueb);	break;
			}
		}
		return;
	}
	else
	{
		PrintSynError("Несовместимые типы операндов: %s и %s\n",a.PrintType(),b.PrintType());
	}
}

Symbol OpCopy(Symbol & a)
{
	if ( !modeCalc ) return symNone;
	static Symbol ret;
	ret.lexptr = a.lexptr;
	ret.token = a.token;

	if ( a.addr == NULL )
	{
		ret.addr = NULL;
		//PrintSynError("Не найден идентификатор %s",a.lexptr);
		return ret;
	}

	if ( a.GetType() == TYPE_SYMLIST )
	{
		Symbol retList(a.lexptr,SYMLIST,NULL);
		//retList.addr = (TYPEUNION*) new SymbolListInfo;
		SymbolLink * pSym = ((SymbolListInfo*)a.addr)->begin;
		while ( pSym != NULL )
		{
			Symbol copy = OpCopy(pSym->sym);
			retList.Insert(copy);
			pSym = pSym->next;
		}
		return retList;
	}

	switch ( a.GetType() )
	{
	case TYPE_BOOLEAN:
		ret.addr = (TYPEUNION*) new IntegerVariable;
		PrintDebugNew("IntegerVariable", ret.addr);
		ret.SetVariableValue(a.GetBooleanValue());
		break;
	case TYPE_INTEGER:
		ret.addr = (TYPEUNION*) new IntegerVariable;
		PrintDebugNew("IntegerVariable", ret.addr);
		ret.SetVariableValue(a.GetIntegerValue());
		break;
	case TYPE_FLOAT:
		ret.addr = (TYPEUNION*) new FloatVariable;
		PrintDebugNew("FloatVariable", ret.addr);
		ret.SetVariableValue(a.GetFloatValue());
		break;
	default:
		ret.addr = NULL;
		PrintSynError("Неизвестный тип исходного символа %s",a.PrintType());
	}
	return ret;
}

void OpAssign(Symbol & a, Symbol & b)
{
	if ( a.token == NAME && a.lexptr != NULL )
	{
		int i = symtable.Lookup(a.lexptr);
		switch ( b.GetType() )
		{
		case TYPE_BOOLEAN:
			symtable.SetVariableValue(i,b.GetBooleanValue());
			break;
		case TYPE_INTEGER:
			symtable.SetVariableValue(i,b.GetIntegerValue());
			break;
		case TYPE_FLOAT:
			symtable.SetVariableValue(i,b.GetFloatValue());
			break;
		case TYPE_SYMLIST:
			symtable.SetList(i,b.GetList());
			break;
		}
	}
	else
	{
		PrintSynError("Слева не lvalue\n");
		a.Release();
		return;
	}
}

void OpAssign(SymbolListInfo listLeftInfo, SymbolListInfo listValueInfo)
{
	SymbolLink * pSymValue = listValueInfo.begin;
	SymbolLink * pSymLeft  = listLeftInfo.begin;
	while ( pSymValue != NULL )
	{
		SymbolLink * pNextValue = pSymValue->next;
		SymbolLink * pNextLeft  = pSymLeft->next;

		if ( pSymValue->sym.GetType() == TYPE_SYMLIST )
		{
			Symbol symCopy = OpCopy(pSymValue->sym);
			OpAssign(pSymLeft->sym,symCopy);
		}
		else
		{
			OpAssign(pSymLeft->sym,pSymValue->sym);
		}

		pSymValue = pNextValue;
		pSymLeft  = pNextLeft;
	}
}

void OpAssign(stack< Symbol > & stList)
{
	if ( !modeCalc ) return;
	Symbol listValue = stList.top();
	stList.pop();

	while ( stList.size() > 0 )
	{
		Symbol listLeft = stList.top();
		stList.pop();

		// идентификаторы справа не определены - ошибка
		if ( listValue.addr == NULL )
		{
			PrintSynError("Идентификаторы справа не определены\n");
			return;
		}

		// идентификатор слева еще не определен
		if ( listLeft.addr == NULL )
		{
			Symbol symCopy = OpCopy(listValue);
			OpAssign(listLeft,symCopy);
			continue;
		}

		if ( listLeft.GetType() == TYPE_SYMLIST )
		{
			// слева был идентификатор-список, а справа - не список
			// в этом случае переопределяем его тип
			if ( listValue.GetType() != TYPE_SYMLIST )
			{
				OpAssign(listLeft,listValue);
				continue;
			}

			if ( listLeft.lexptr != NULL )
			{
				OpAssign(listLeft,listValue);
				continue;
			}

			SymbolListInfo listLeftInfo  = *(SymbolListInfo*)(listLeft.addr);
			SymbolListInfo listValueInfo = *(SymbolListInfo*)(listValue.addr);

			if ( listLeftInfo.count != listValueInfo.count )
			{
				PrintSynError("Несоответствие количества в списках слева и справа");
				return;
			}

			// присваивание списка
			OpAssign(listLeftInfo,listValueInfo);
		}
		else
		{
			OpAssign(listLeft,listValue);
			continue;
		}
	}
	listValue.Release();
}

void OpNot(Symbol & a)
{
	switch ( a.GetType() )
	{
	case TYPE_BOOLEAN:
		a.SetVariableValue(!a.GetBooleanValue());
		break;
	case TYPE_INTEGER:
		if ( a.GetIntegerValue() == 0 )
			a.SetVariableValue(true);
		else
			a.SetVariableValue(false);
		break;
	case TYPE_FLOAT:
		if ( a.GetFloatValue() == 0.0 )
			a.SetVariableValue(true);
		else
			a.SetVariableValue(false);
		break;
	default:
		PrintSynError("Оператор 'not' несовместим с типом '%s'\n",a.PrintType());
	}
}

void OpLogicGeneral(Symbol & a, Symbol & b, TOKEN op)
{
	TYPE typea = a.GetType();
	TYPE typeb = b.GetType();
	if ( isIntegral(typea) )
	{
		if ( a.GetIntegerValue() == 0 )  // 'a' ложно
		{
			if ( op == OP_AND )
			{
				// возвращаем 'a', все остается как есть
				return;
			}
			else
			{
				switch( typeb )
				{
				case TYPE_BOOLEAN:
					a.SetVariableValue(b.GetBooleanValue());
					break;
				case TYPE_INTEGER:
					a.SetVariableValue(b.GetIntegerValue());
					break;
				case TYPE_FLOAT:
					a.SetVariableValue(b.GetFloatValue());
					break;
				}
			}
		}
		else  // 'a' истинно
		{
			if ( op == OP_AND )
			{
				switch( typeb )
				{
				case TYPE_BOOLEAN:
					a.SetVariableValue(b.GetBooleanValue());
					break;
				case TYPE_INTEGER:
					a.SetVariableValue(b.GetIntegerValue());
					break;
				case TYPE_FLOAT:
					a.SetVariableValue(b.GetFloatValue());
					break;
				}
			}
			else
			{
				// возвращаем 'a', все остается как есть
				return;
			}
		}
	}
	else
		if ( typea == TYPE_FLOAT )
		{

		}
}



#define DO_COMPARISONGENERAL() \
	bool result = false; \
	switch(op) \
{ \
	case OP_IS: \
	PrintWarning("Операция 'is' поддерживается не полностью."); \
	case OP_EQ: \
	result = ( valuea == valueb ); \
	break; \
	case OP_ISNOT: \
	PrintWarning("Операция 'is not' поддерживается не полностью."); \
	case OP_NOTEQ: \
	result = ( valuea != valueb ); \
	break; \
	case OP_LESSEQ: \
	result = ( valuea <= valueb ); \
	break; \
	case OP_LESS: \
	result = ( valuea < valueb ); \
	break; \
	case OP_GREATEREQ: \
	result = ( valuea >= valueb ); \
	break; \
	case OP_GREATER: \
	result = ( valuea > valueb ); \
	break; \
	case OP_NOTIN: \
	PrintWarning("Операция 'not in' не поддерживается."); \
	break; \
	case OP_IN: \
	PrintWarning("Операция 'in' не поддерживается."); \
	break; \
	default: \
	break; \
} \
	return result;

bool OpComparisonGeneral(Symbol & a, Symbol & b, TOKEN op)
{
	TYPE typea = a.GetType();
	TYPE typeb = b.GetType();
	if ( isIntegral(typea) )
	{
		long valuea = a.GetIntegerValue();
		if ( isIntegral(typeb) )
		{
			long valueb = b.GetIntegerValue();
			DO_COMPARISONGENERAL()
		}
		else
			if ( typeb == TYPE_FLOAT )
			{
				double valueb = b.GetFloatValue();
				DO_COMPARISONGENERAL()
			}
	}
	if ( typea == TYPE_FLOAT )
	{
		double valuea = a.GetFloatValue();
		if ( isIntegral(typeb) )
		{
			long valueb = b.GetIntegerValue();
			DO_COMPARISONGENERAL()
		}
		else
			if ( typeb == TYPE_FLOAT )
			{
				double valueb = b.GetFloatValue();
				DO_COMPARISONGENERAL()
			}
	}
	return false;
}
