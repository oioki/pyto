#include "stdafx.h"
#include "syntax.h"
#include "stream.h"
#include "operations.h"


extern SymbolStream stream;
extern bool modeCalc;
extern bool isMultiline;
extern char lineinput[];

Symbol factor();
Symbol testlist();
Symbol stmt();



#define RETURN_UNMATCHED \
{ \
	stream.PopCursor(); \
	return symUnmatched; \
}

#define RETURN_IF_NONE(sym) \
	if ( sym.isNone() ) \
{ \
	stream.PopCursor(); \
	return symUnmatched; \
}

#define RETURN_IF_UNMATCHED(sym) \
	if ( sym.isUnmatched() ) \
{ \
	stream.PopCursor(); \
	return symUnmatched; \
}

#define BREAK_IF_UNMATCHED(sym) \
	if ( sym.isUnmatched() ) \
{ \
	stream.PopCursor(); \
	break; \
}



/**********************************************************************/
Symbol atom()
/***********************************************************************
atom ::=
'(' [testlist] ')' |
NAME | NUMBER
***********************************************************************/
{
	Symbol ret;

	stream.PushCursor();

	if ( stream.Look() == LBRACE )
	{
		stream.Match(LBRACE);
		ret = testlist();
		if ( stream.Look() == RBRACE )
		{
			stream.Match(RBRACE);
			stream.ApproveCursor();
			return ret;
		}
		ret.Release();
	}

	stream.PopCursor();
	if ( stream.Look() == NAME )
	{
		ret = OpCopy(stream.GetSymbol());
		stream.Match(NAME);
		return ret;
	}

	if ( stream.Look() == NUMBER )
	{
		ret = OpCopy(stream.GetSymbol());
		stream.Match(NUMBER);
		return ret;
	}

	{
		//PushSynError("atom(): Ожидалась константа или идентификатор");
		//PrintSynError("atom(): Ожидалась константа или идентификатор");
		return symUnmatched;
	}
}

/**********************************************************************/
Symbol power()
/***********************************************************************
power ::=
atom ('**' factor)*
***********************************************************************/
{
	Symbol ret = atom();

	TOKEN la = stream.Look();
	while ( la == OP_POWER )
	{
		stream.Match(la);
		Symbol extra = factor();
		OpMultiplyGeneral(ret,extra,la);
		extra.Release();
		la = stream.Look();
	}

	return ret;
}

/**********************************************************************/
Symbol factor()
/***********************************************************************
factor ::=
('+' | '-' | '~') factor | power
***********************************************************************/
{
	Symbol ret;
	TOKEN la = stream.Look();
	if ( la == OP_PLUS || la == OP_MINUS || la == OP_BITNOT )
	{
		stream.Match(la);
		ret = factor();
		if ( la == OP_MINUS )
		{
			OpMinus(ret);
		}
		else
			if ( la == OP_BITNOT )
			{
				OpBitNot(ret);
			}
			return ret;
	}

	return power();
}

/**********************************************************************/
Symbol term()
/***********************************************************************
term ::=
factor (('*' | '/' | '%') factor)*
***********************************************************************/
{
	Symbol ret = factor();

	TOKEN la = stream.Look();
	while ( la == OP_MULTIPLY || la == OP_DIVIDE || la == OP_REMAINDER )
	{
		stream.Match(la);
		Symbol extra = factor();
		OpMultiplyGeneral(ret,extra,la);
		extra.Release();
		la = stream.Look();
	}

	return ret;
}

/**********************************************************************/
Symbol arith_expr()
/***********************************************************************
arith_expr ::=
term (('+' | '-') term)*
***********************************************************************/
{
	Symbol ret = term();

	TOKEN la = stream.Look();
	while ( la == OP_PLUS || la == OP_MINUS )
	{
		stream.Match(la);
		Symbol extra = term();
		OpAddGeneral(ret,extra,la);
		extra.Release();
		la = stream.Look();
	}

	return ret;
}

/**********************************************************************/
Symbol shift_expr()
/***********************************************************************
shift_expr ::=
arith_expr (('<<' | '>>') arith_expr)*
***********************************************************************/
{
	Symbol ret = arith_expr();

	TOKEN la = stream.Look();
	while ( la == OP_RSHIFT || la == OP_LSHIFT )
	{
		stream.Match(la);
		Symbol extra = arith_expr();
		OpBitGeneral(ret,extra,la);
		extra.Release();
		la = stream.Look();
	}

	return ret;
}

/**********************************************************************/
Symbol and_expr()
/***********************************************************************
and_expr ::=
shift_expr ('&' shift_expr)*
***********************************************************************/
{
	Symbol ret = shift_expr();

	TOKEN la;
	while ( (la=stream.Look()) == OP_BITAND )
	{
		stream.Match(la);
		Symbol extra = shift_expr();
		OpBitGeneral(ret,extra,la);
		extra.Release();
	}

	return ret;
}

/**********************************************************************/
Symbol xor_expr()
/***********************************************************************
xor_expr ::=
and_expr ('^' and_expr)*
***********************************************************************/
{
	Symbol ret = and_expr();

	TOKEN la;
	while ( (la=stream.Look()) == OP_XOR )
	{
		stream.Match(la);
		Symbol extra = and_expr();
		OpBitGeneral(ret,extra,la);
		extra.Release();
	}

	return ret;
}

/**********************************************************************/
Symbol expr()
/***********************************************************************
expr ::=
xor_expr ('|' xor_expr)*
***********************************************************************/
{
	Symbol ret = xor_expr();

	TOKEN la;
	while ( (la = stream.Look()) == OP_BITOR )
	{
		stream.Match(la);
		Symbol extra = xor_expr();
		OpBitGeneral(ret,extra,la);
		extra.Release();
	}

	return ret;
}



/**********************************************************************/
Symbol comparison()
/***********************************************************************
comparison ::=
expr (comp_op expr)*
***********************************************************************/
{
	Symbol left = expr();
	Symbol right;
	Symbol ret;

	TOKEN op = stream.Look();
	if ( !isOpComp(op) ) return left;

	switch(left.GetType())
	{
	case TYPE_BOOLEAN:
		ret.SetVariableValue(left.GetBooleanValue());
		break;
	case TYPE_INTEGER:
		ret.SetVariableEqValue(left.GetIntegerValue());
		break;
	case TYPE_FLOAT:
		ret.SetVariableEqValue(left.GetFloatValue());
		break;
	}

	bool result = false;
	if ( modeCalc )
	{
		if ( ret.addr == NULL )
		{
			PrintSynError("Значение операнда не определено.");
			return symUnmatched;
		}

		result = ret.GetBooleanValue();	
	}

	while ( isOpComp(op) )
	{
		stream.Match(op);
		right = expr();

		// если result = истина, то стоит проверять дальнейшие условия
		if ( result == true )
		{
			result = OpComparisonGeneral(left,right,op);
		}
		left.Release();
		left = right;
		op = stream.Look();
	}

	right.Release();
	ret.SetVariableValue(result);
	return ret;
}



/**********************************************************************/
Symbol not_test()
/***********************************************************************
not_test ::=
'not' not_test | comparison
***********************************************************************/
{
	Symbol ret;
	TOKEN la = stream.Look();
	if ( la == OP_NOT )
	{
		stream.Match(la);
		ret = not_test();
		OpNot(ret);
		return ret;
	}
	else
	{
		return comparison();
	}
}


/**********************************************************************/
Symbol and_test()
/***********************************************************************
and_test ::=
not_test ('and' not_test)*
***********************************************************************/
{
	Symbol ret = not_test();
	TOKEN la = stream.Look();
	if ( la == OP_AND )
	{
		stream.Match(la);
		Symbol extra = not_test();
		OpLogicGeneral(ret,extra,la);
		extra.Release();
		la = stream.Look();
	}
	return ret;
}

/**********************************************************************/
Symbol test()
/***********************************************************************
test ::=
and_test ('or' and_test)*
***********************************************************************/
{
	Symbol ret = and_test();
	TOKEN la = stream.Look();
	if ( la == OP_OR )
	{
		stream.Match(la);
		Symbol extra = and_test();
		OpLogicGeneral(ret,extra,la);
		extra.Release();
		la = stream.Look();
	}
	return ret;
}

/**********************************************************************/
Symbol testlist()
/***********************************************************************
testlist ::=
test (',' test)* [',']
***********************************************************************/
{
	Symbol element = test();

	if ( element.isUnmatched() )
		return element;

	Symbol list(NULL,SYMLIST,NULL); // TODO:деструктор
	list.Insert(element);

	TOKEN la = stream.Look();
	while ( la == COMMA )
	{
		stream.Match(la);
		stream.PushCursor();
		Symbol extra = test();
		if ( extra.isNone() )
		{
			//la = lookback();
			stream.PopCursor();
			break;
		}
		stream.ApproveCursor();
		list.Insert(extra);
		la = stream.Look();
	}

	if ( la == COMMA )
	{
		stream.Match(la);
	}
	else
		if ( list.GetCount() == 1 )
		{
			Symbol symCopy = OpCopy(((SymbolListInfo*)(list.addr))->begin->sym);
			//		TOKEN tokenTemp = list.token;
			list.Release();
			list = symCopy;
			//list.token = tokenTemp;
		}

		return list;
}


/**********************************************************************/
Symbol expr_stmt()
/***********************************************************************
expr_stmt ::=
testlist (augassign testlist |
('=' testlist)*)
***********************************************************************/
{
	Symbol listLeft = testlist();
	stack< Symbol > stList;
	stList.push(listLeft);

	TOKEN la = stream.Look();
	if ( isOpAugAssign(la) )
	{
		stream.Match(la);
		stream.PushCursor();
		Symbol listRight = testlist();
		if ( listRight.isNone() )
		{
			stream.PopCursor();
			//la = lookback();
		}
		else
		{
			stream.ApproveCursor();
			stList.push(listRight);
			OpAssign(stList);
		}
	}

	while ( la == OP_ASSIGN )
	{
		stream.Match(la);
		Symbol listRight = testlist();
		stList.push(listRight);
		la = stream.Look();
	}

	if ( stList.size() == 1 )
	{
		stList.pop();
		return listLeft;
	}

	OpAssign(stList);
	return symNone;
}

/**********************************************************************/
Symbol small_stmt()
/***********************************************************************
small_stmt ::=
expr_stmt
***********************************************************************/
{
	Symbol ret = expr_stmt();
	return ret;
}

/**********************************************************************/
Symbol simple_stmt() 
/***********************************************************************
simple_stmt ::=
small_stmt NEWLINE
***********************************************************************/
{
	stream.PushCursor();
	Symbol ret = small_stmt();
	if ( stream.Look() == NEWLINE )
	{
		stream.Match(NEWLINE);
		stream.ApproveCursor();
		return ret;
	}
	RETURN_UNMATCHED;
}

/**********************************************************************/
Symbol suite()
/***********************************************************************
suite ::=
simple_stmt |
NEWLINE INDENT stmt+ DEDENT
***********************************************************************/
{
	stream.PushCursor();
	Symbol ret = simple_stmt();
	if ( !ret.isUnmatched() )
	{
		stream.ApproveCursor();
		return ret;
	}

	stream.PopCursor();
	if ( stream.Look() != NEWLINE )
		return symUnmatched;

	stream.Match(NEWLINE);

	if ( stream.Look() != INDENT )
		return symUnmatched;

	stream.Match(INDENT);

	ret = symUnmatched;
	Symbol newRet = stmt();

	while ( !newRet.isUnmatched() )
	{
		ret = symNone;
		newRet = stmt();
	}

	if ( stream.Look() != DEDENT )
		return symUnmatched;

	stream.Match(DEDENT);

	return ret;
}

/**********************************************************************/
Symbol if_stmt()
/***********************************************************************
if_stmt ::=
'if' test ':' suite
***********************************************************************/
{
	stream.PushCursor();

	TOKEN la = stream.Look();
	if ( la == IF )
	{
		stream.Match(IF);
		Symbol symTest = test();
		RETURN_IF_UNMATCHED(symTest);

		bool backupModeCalc = modeCalc;
		if ( !symTest.isNone() )
		{
			if ( symTest.GetBooleanValue() == false )
			{
				modeCalc = false;
			}
		}

		if ( stream.Look() != COLON )
		{
			modeCalc = backupModeCalc;
			RETURN_UNMATCHED;
		}

		stream.Match(COLON);
		Symbol ret = suite();
		modeCalc = backupModeCalc;
		if ( ret.isUnmatched() )
		{
			isMultiline = true;
			RETURN_UNMATCHED
		}
		stream.ApproveCursor();
		return symNone;
	}
	RETURN_UNMATCHED;
}

/**********************************************************************/
Symbol while_stmt()
/***********************************************************************
while_stmt ::=
'while' test ':' suite
***********************************************************************/
{
	stream.PushCursor();

	TOKEN la = stream.Look();
	if ( la == WHILE )
	{
		stream.Match(WHILE);
CheckAgain:
		stream.PushCursor();
		Symbol symTest = test();
		RETURN_IF_UNMATCHED(symTest);

		bool backupModeCalc = modeCalc;
		if ( !symTest.isNone() )
		{
			if ( symTest.GetBooleanValue() == false )
			{
				modeCalc = false;
			}
		}

		if ( stream.Look() != COLON )
		{
			modeCalc = backupModeCalc;
			RETURN_UNMATCHED;
		}

		stream.Match(COLON);
		Symbol ret = suite();

		if ( ret.isUnmatched() )
		{
			isMultiline = true;
			modeCalc = backupModeCalc;
			RETURN_UNMATCHED;
		}

		if ( modeCalc )
		{
			stream.PopCursor();
			goto CheckAgain;
		}

		modeCalc = backupModeCalc;
		stream.ApproveCursor();
		return symNone;
	}
	RETURN_UNMATCHED;
}

/**********************************************************************/
Symbol compound_stmt()
/***********************************************************************
compound_stmt ::=
if_stmt |
while_stmt
***********************************************************************/
{
	stream.PushCursor();
	Symbol ret = if_stmt();
	if ( !ret.isUnmatched() )
	{
		stream.ApproveCursor();
		return ret;
	}

	stream.PopCursor();
	ret = while_stmt();

	return ret;
}

/**********************************************************************/
Symbol stmt()
/***********************************************************************
stmt ::=
simple_stmt |
compount_stmt
***********************************************************************/
{
	stream.PushCursor();
	Symbol ret = simple_stmt();

	if ( !ret.isUnmatched() )
	{
		stream.ApproveCursor();
		return ret;
	}

	stream.PopCursor();
	ret = compound_stmt();

	return ret;
}


/**********************************************************************/
Symbol single_input()
/***********************************************************************
single_input ::=
NEWLINE |
simple_stmt |
compound_stmt NEWLINE
***********************************************************************/
{
	// проверка на 'bye' - выход из программы
	if ( strncmp(lineinput,"bye",3) == 0 )
	{
		printf(PrintString("Доброго здоровья и всего наилучшего."));
		exit(0);
	}

	if ( stream.Look() == NEWLINE )
	{
		stream.Match(NEWLINE);
		return symNone;
	}

	Symbol ret = simple_stmt();
	if ( ret.isUnmatched() )
	{
		Symbol ret = compound_stmt();
		if ( ret.isUnmatched() )
		{
			return symUnmatched;
		}
		stream.Match(NEWLINE);
		return symNone;
	}
	return ret;
}
