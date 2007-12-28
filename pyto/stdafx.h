// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdarg.h>

#include <iostream>
#include <list>
#include <stack>

using namespace std;

#include "pytotypes.h"
#include "tokentypes.h"

char *	PrintString(const char * str);
void	PrintDebugNew(char * str, void * addr);
void	PrintDebugDelete(char * str, void * addr);
void	PrintWarning(char * str);
char *	PrintToken(TOKEN token);
void	PrintSynError(char * str, ... );
void	PrintError(const char * s);

struct IntegerVariable
{
	TYPE	type;
	long	fictive;  // поле только для отладочных целей
	long	value;
};

struct FloatVariable
{
	TYPE	type;
	double	value;
};

//#define DEBUG
