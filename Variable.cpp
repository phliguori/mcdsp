#include "Variable.h"

int Variable::varCounter = 0;

Variable::Variable(void)
	:Column(), node(NULL), color(NULL)
{
	id = ++varCounter;
	type = Variable::V_UNKNOWN;

	return;
}

Variable::Variable(COLTYPE _type, double _objcoef, double _lb, double _ub)
	:Column(_type, _objcoef, _lb, _ub), node(NULL), color(NULL)
{
	id = ++varCounter;
	type = Variable::V_UNKNOWN;

	return;
}

Variable::~Variable(void)
{
}

std::string Variable::toString(void) const
{
	std::string varName;

	switch (type)
	{
		case Variable::V_X: varName = "X"; break;
		case Variable::V_UNKNOWN: varName = "UNKN"; break;
	}

	if (node != NULL)
	{
		varName += "_v";
		varName += to_string(node->num);
	}

	if (color != NULL)
	{
		varName += "_c";
		varName += to_string(color->num);
	}

	return varName;
}
