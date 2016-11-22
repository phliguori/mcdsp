#ifndef CDSMIP_H
#define CDSMIP_H

#include "Variable.h"
#include "SolverMIP.h"
#include "Graph.h"
#include "Constraint.h"

#include <fstream>

class DominationForm : public SolverMIP
{
public:
	DominationForm(C_graph* _g);

	virtual ~DominationForm();

	std::string printXSol();

	int createVariables();

	int createConstraints();

	// Variable hash
	TypeVariableHash vHash;

	// Constraint hash
	ConstraintHash cHash;

protected:
	int createVarX(Column::COLTYPE _colType);

	int createConsDomination();

	int createConsMaxColors();

	C_graph* g;	// Base graph
};

#endif