#include "Constraint.h"

#include <sstream>

int Constraint::consCounter = 0;

Constraint::Constraint(void)
	:Row(), node(NULL), color(NULL)
{
	id = ++consCounter;
	type = Constraint::C_UNKNOWN;

	return;
}

Constraint::Constraint(int _maxnnz, ROWSENSE _sense, double _rhs)
	:Row(_maxnnz, _sense, _rhs), node(NULL), color(NULL)
{
	id = ++consCounter;
	type = Constraint::C_UNKNOWN;

	return;
}

Constraint::Constraint(const Constraint & _constraint)
{
	++consCounter;
	
	type = _constraint.type;
	node  = _constraint.node;
	color = _constraint.color;
	
	return;
}

Constraint::~Constraint(void)
{
	return;
}

std::string Constraint::toString(void) const
{
	std::string consName;

	switch (type)
	{
	case Constraint::C_DOMINATION:			consName = "C_DOM";			break;
	case Constraint::C_MAX_COLORS:			consName = "C_MCOLORS";		break;
	default:								consName = "C_UNKWN";		break;
	}

	if (node != NULL)
	{
		consName += "_";
		consName += to_string(node->num);
	}

	if (color != NULL)
	{
		consName += "_";
		consName += to_string(color->num);
	}

	return consName;
}
