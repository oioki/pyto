/*****************************************************
	LEXER.H
	Модуль лексического анализа
*****************************************************/

#pragma once

//#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symtable.h"
#include "stream.h"

void	SetIndent(int n);

void	PrintLexError(const char * s);
bool	ise(char c);
bool	ispm(char c);
int		LexNumber();
int		LexID();
int		LexString();
void	LexAnalyzer();
int		LexOperator();
int		LexBrace();
char	nextchar();
