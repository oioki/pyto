#include "stdafx.h"
#include "lexer.h"

extern int cur;
extern int linenum;
extern int linecur;
extern char lineinput[];
extern SymbolTable symtable;

#define BSIZE 128			// размер буфера анализатора
char lexbuf[BSIZE];			// буфер
extern SymbolStream stream;	// список токенов

int valIndent;					// текущий уровень отступа
static int valPrevIndent = 0;	// предыдущий уровень отступа


/*********************************************************/
void PrintLexError(const char * str)
/**********************************************************
Печатет сообщение о лексической ошибке 'str'
**********************************************************/
{
	printf("LEXAN error (line %d, num %d): %s\n", linenum, linecur, PrintString(str));
#ifdef _WIN32
	system("PAUSE");
#endif
	exit(0);
}

bool ise(char c)
{
	return ( c == 'e' || c == 'E' );
}

bool ispm(char c)
{
	return ( c == '+' || c == '-' );
}

void SetIndent(int n)
{
	valPrevIndent = n;
}

char nextchar()
{
	return lineinput[++cur];
}


/*********************************************************/
int LexNumber()
/**********************************************************
Если распознан символ с токеном NUMBER, то он добавляется
в таблицу символов и возвращается номер этой новой строки
**********************************************************/
{
	TYPE type = TYPE_INTEGER;

	int state = 0;
	int b = 0;
	int curbackup = cur;
	unsigned char c = lineinput[cur];

	while(1)
	{
		switch(state)
		{
		case 0:
			if ( isdigit(c) )		state = 1;
			else if ( c == '.' )	state = 7;
			else					{ cur = curbackup; return 0; }
			break;
		case 1:
			if ( isdigit(c) )		;
			else if ( c == '.' )	state = 2;
			else if ( ise(c) )		state = 4;
			else					{ state = 9; continue; }
			break;
		case 2:
			type = TYPE_FLOAT;
			if ( isdigit(c) )		state = 3;
			else if ( ise(c) )		state = 4;
			else					{ state = 9; continue; }
			break;
		case 3:
			if ( isdigit(c) )		;
			else if ( ise(c) )		state = 4;
			else					{ state = 9; continue; }
			break;
		case 4:
			type = TYPE_FLOAT;
			if ( ispm(c) )			state = 5;
			else if ( isdigit(c) )	state = 6;
			else					{ PrintLexError("Похоже забыли показатель степени"); cur = curbackup; return 0; }
			break;
		case 5:
			if ( isdigit(c) )		state = 6;
			else					{ PrintLexError("Похоже забыли показатель степени"); cur = curbackup; return 0; }
			break;
		case 6:
			if ( isdigit(c) )		;
			else					{ state = 9; continue; }
			break;
		case 7:
			type = TYPE_FLOAT;
			if ( isdigit(c) )		state = 8;
			else					{ state = 9; continue; }
			break;
		case 8:
			if ( isdigit(c) )		;
			else					{ state = 9; continue; }
			break;
		case 9:
			lexbuf[b] = '\0';

			int p = symtable.Lookup(lexbuf);
			if ( p == 0 )
			{
				p = symtable.Insert(NUMBER, lexbuf);
			}
			if ( type == TYPE_INTEGER )
			{
				symtable.SetVariableValue(p,atol(lexbuf));
			}
			else
			if ( type == TYPE_FLOAT )
			{
				symtable.SetVariableValue(p,atof(lexbuf));
			}
			return p;
		}
		lexbuf[b++] = c;
		c = nextchar();
	}
}


/*********************************************************/
int LexID()
/**********************************************************
Если распознан символ с токеном ID, то он добавляется
в таблицу символов и возвращается номер этой новой строки
**********************************************************/
{
	int state = 0;
	int b = 0;
	int curbackup = cur;
	unsigned char c = lineinput[cur];

	while ( true )
	{
		switch(state)
		{
		case 0:
			if ( isalpha(c) || c == '_' )	state = 1;
			else							{ cur = curbackup; return 0; }
			break;
		case 1:
			if ( isalnum(c) || c == '_' )	;
			else							{ state = 2; continue; }
			break;
		case 2:
			lexbuf[b] = '\0';

			int p = symtable.Lookup(lexbuf);
			if ( p == 0 ) p = symtable.Insert(NAME, lexbuf);
			return p;
		}
		lexbuf[b++] = c;
		if ( b >= BSIZE) PrintLexError("Слишком длинный идентификатор");
		c = nextchar();
	}	
}


/*********************************************************/
int LexString()
/**********************************************************
Если распознан символ с токеном STRING, то он добавляется
в таблицу символов и возвращается номер этой новой строки
**********************************************************/
{
	int state = 0;
	int b = 0;
	int curbackup = cur;
	char c = lineinput[cur];

	while ( true )
	{
		switch(state)
		{
		case 0:
			if ( c == '\"' )	state = 1;
			else				{ cur = curbackup; return 0; }
			break;
		case 1:
			if ( c == '\"' )	{ state = 2; cur++; }
			else				;
			break;
		case 2:
			lexbuf[b] = '\0';

			int p = symtable.Lookup(lexbuf);
			if ( p == 0 ) p = symtable.Insert(STRING, lexbuf);
			return p;			
		}
		lexbuf[b++] = c;
		if ( b >= BSIZE ) PrintLexError("Слишком длинная строка");
		c = nextchar();
	}
}

/*********************************************************/
int LexOperator()
/**********************************************************
Если распознан символ с токеном OP, то он добавляется
в таблицу символов и возвращается номер этой новой строки
**********************************************************/
{
	for (int i=symtable.Lookup("+"); i<=symtable.Lookup("for"); i++)
	{
		char * st = symtable.GetSymbolPointer(i)->lexptr;
		size_t size = strlen(st);
		if ( strncmp(lineinput+cur,st,size) == 0 )
		{
			cur += size;
			return i;
		}
	}
	return 0;
}


/*********************************************************/
int LexBrace()
/**********************************************************
Если распознан символ с токеном BRACE, то он добавляется
в таблицу символов и возвращается номер этой новой строки
**********************************************************/
{
	for (int i=symtable.Lookup("("); i<symtable.Lookup("}"); i++)
	{
		char * st = symtable.GetSymbolPointer(i)->lexptr;
		size_t size = strlen(st);
		if ( strncmp(lineinput+cur,st,size) == 0 )
		{
			cur += size;
			return i;
		}
	}
	return 0;
}


/*********************************************************/
void LexAnalyzer()
/**********************************************************
Выделяет из входной строки 'lineinput' очередной токен и
добавляет его в список токенов
**********************************************************/
{
	int ret;
	char c;

	int valIndent = 0;
	static bool isBeginning = true;

	while ( true )
	{
		c = lineinput[cur];
		while ( isBeginning )
		{
			if ( c == '\t' )
			{
				valIndent++;
				cur++;
				c = lineinput[cur];
			}
			else
			{
				if ( valIndent > valPrevIndent )
				{
					for (int i=0; i < valIndent - valPrevIndent; i++)
					{
						stream.Add(symtable.GetSymbolPointer(symtable.Lookup(INDENT)));
					}
				}
				else
				if ( valIndent < valPrevIndent )
				{
					for (int i=0; i < valPrevIndent - valIndent; i++)
					{
						stream.Add(symtable.GetSymbolPointer(symtable.Lookup(DEDENT)));
					}
				}
				valPrevIndent = valIndent;
				isBeginning = false;
				return;
			}
		}

		c = lineinput[cur];

		if ( c == ' ' || c == '\t' )
			cur++;
		else
		if ( c == '\n' )
		{
			cur++;
			isBeginning = true;
			//if ( lineinput[cur] == '\0' )
			stream.Add(symtable.GetSymbolPointer(0));
			return;
			linecur = 0;
			linenum++;
		}
		else if ( ( ret = LexOperator()	) != 0 )
		{
			stream.Add(symtable.GetSymbolPointer(ret));
			return;
		}
		else if ( ( ret = LexBrace()	) != 0 )
		{
			stream.Add(symtable.GetSymbolPointer(ret));
			return;
		}
		else if ( ( ret = LexNumber()	) != 0 )
		{
			stream.Add(symtable.GetSymbolPointer(ret));
			return;
		}
		else if ( ( ret = LexString()	) != 0 )
		{
			stream.Add(symtable.GetSymbolPointer(ret));
			return;
		}
		else if ( ( ret = LexID()		) != 0 )
		{
			stream.Add(symtable.GetSymbolPointer(ret));
			return;
		}
		else
		{
			PrintLexError("Не найдено лексических соответствий!");
			exit(0);
		}
	}
}
