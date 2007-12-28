#pragma once

#include <math.h>
#include "symbol.h"

bool	isOpAugAssign(TOKEN op);
bool	isOpComp(TOKEN op);

void	OpMinus(Symbol & a);
void	OpBitNot(Symbol & a);
void	OpMultiplyGeneral(Symbol & a, Symbol & b, TOKEN op );
void	OpAddGeneral(Symbol & a, Symbol & b, TOKEN op);
void	OpBitGeneral(Symbol & a, Symbol & b, TOKEN op);
Symbol	OpCopy(Symbol & a);
void	OpAssign(Symbol & a, Symbol & b);
void	OpAssign(SymbolListInfo listLeftInfo, SymbolListInfo listValueInfo);
void	OpAssign(stack< Symbol > & stList);
void	OpNot(Symbol & a);
void	OpLogicGeneral(Symbol & a, Symbol & b, TOKEN op);
bool	OpComparisonGeneral(Symbol & a, Symbol & b, TOKEN op);
