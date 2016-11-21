#include "CDSMip.h"

#include <set>
#include <list>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>

DominationForm::DominationForm(C_graph * _g)
	:SolverMIP()
{
	g = _g;
	return;
}

DominationForm::~DominationForm()
{
}

int DominationForm::createVariables()
{
	std::string msg;
	Column::COLTYPE colType;
	int nVars = 0, tmp = 0;
	clock_t start, end;

#ifdef VERBOSE
	std::cout << "\n";
	std::cout << "=======================================\n";
	std::cout << "-------------  Variables  -------------\n";
#endif

	start = clock();
	colType = Column::BINARY;
	tmp = createVarX(colType);
	end = clock();
	nVars += tmp;
#ifdef VERBOSE
	std::cout << "Variables X: " << tmp << " (" << (end - start) / (double)CLOCKS_PER_SEC << ")\n";
#endif

	return nVars;
}

int DominationForm::createConstraints()
{
	int nCons = 0, tmp = 0;
	clock_t start, end;

#ifdef VERBOSE
	std::cout << "\n";
	std::cout << "========================================\n";
	std::cout << "-------------  Constraints  -------------\n";
	std::cout << "----------  Base Constraints  -----------\n";
#endif

	start = clock();
	tmp = createConsDomination();
	end = clock();
	nCons += tmp;
#ifdef VERBOSE
	std::cout << "Domination Constraints: " << tmp << " (" << (end - start) / (double)CLOCKS_PER_SEC << ")\n";
#endif

	start = clock();
	tmp = createConsMaxColors();
	end = clock();
	nCons += tmp;
#ifdef VERBOSE
	std::cout << "Max colors constraints: " << tmp << " (" << (end - start) / (double)CLOCKS_PER_SEC << ")\n";
#endif


	return nCons;
}

int DominationForm::createVarX(Column::COLTYPE _colType)
{
	int nVars = 0;
	double coeff = 0.0;
	double lb = 0.;
	double ub = 1.;
	VariableHash::iterator vit;
	Variable::VARTYPE varType = Variable::V_X;

	for (auto & v : g->nodesVec)
	{
		int vCod = v->num;

		std::sort(v->neighbors.begin(), v->neighbors.end());
		std::sort(v->colors.begin(), v->colors.end());

		for (auto & cCod : v->colors)
		{
			Variable var(_colType, coeff, lb, ub);
			var.setType(varType);
			var.setNode(v);
			var.setColor(g->colorsVec[cCod]);

			coeff = g->nodeColorWeight[make_pair(vCod, cCod)];

			vit = vHash[varType].find(var);
			if (vit != vHash[varType].end())
				continue;

			bool isInserted = addCol(&var);
			if (isInserted)
			{
				var.setColIdx(getNCols() - 1);
				vHash[varType][var] = var.getColIdx();
				++nVars;
			}
		}
	}

	return nVars;
}

int DominationForm::createConsDomination()
{
	int numCons = 0;
	VariableHash::iterator vit;
	Row::ROWSENSE sense = Row::GREATER;
	int maxnnz = g->nb_nodes*g->nb_colors;
	double rhs = 1.0;

	for (auto & v : g->nodesVec)
	{
		int vCod = v->num;

		Constraint c(maxnnz, sense, rhs);
		c.setType(Constraint::C_DOMINATION);
		c.setNode(v);

		Variable x;
		x.setType(Variable::V_X);
		x.setNode(v);

		set<int> colorsV;

		// This node with all its colors
		for (auto & cCod : v->colors)
		{
			colorsV.insert(cCod);
			x.setColor(g->colorsVec[cCod]);

			vit = vHash[Variable::V_X].find(x);
			if (vit != vHash[Variable::V_X].end())
			{
				int colVarX = vit->second;
				c.rowAddVar(colVarX, 1.0);
			}
		}

		// Look for all the neighbors and add those that share a color with v
		for (auto & neighborCod : v->neighbors)
		{
			C_node* neighbor = g->nodesVec[neighborCod];

			x.setNode(neighbor);
			for (auto & neighborColorCod : neighbor->colors)
			{
				if (colorsV.find(neighborColorCod) == colorsV.end())
					continue;

				x.setColor(g->colorsVec[neighborColorCod]);

				vit = vHash[Variable::V_X].find(x);
				if (vit != vHash[Variable::V_X].end())
				{
					int colVarX = vit->second;
					c.rowAddVar(colVarX, 1.0);
				}
			}
		}

		if (c.getRowNnz() > 0)
		{
			bool isInserted = addRow(&c);

			if (isInserted)
			{
				c.setRowIdx(getNRows() - 1);
				cHash[c] = c.getRowIdx();
				numCons++;
			}
		}
	}

	return numCons;
}

int DominationForm::createConsMaxColors()
{
	int numCons = 0;
	VariableHash::iterator vit;
	Row::ROWSENSE sense = Row::LESS;
	int maxnnz = 0;
	double rhs = 1.0;

	for (auto & v : g->nodesVec)
	{
		int vCod = v->num;
		
		Constraint c(v->colors.size(), sense, rhs);
		c.setType(Constraint::C_MAX_COLORS);
		c.setNode(v);

		Variable x;
		x.setType(Variable::V_X);
		x.setNode(v);

		for (auto & cCod : v->colors)
		{
			x.setColor(g->colorsVec[cCod]);

			vit = vHash[Variable::V_X].find(x);
			if (vit != vHash[Variable::V_X].end())
			{
				int colVarX = vit->second;
				c.rowAddVar(colVarX, 1.0);
			}
		}

		if (c.getRowNnz() > 0)
		{
			bool isInserted = addRow(&c);

			if (isInserted)
			{
				c.setRowIdx(getNRows() - 1);
				cHash[c] = c.getRowIdx();
				numCons++;
			}
		}
	}

	return numCons;
}

std::string DominationForm::printXSol()
{
	std::ostringstream tmp;

	VariableHash::iterator vit;
	for (int varType = Variable::V_X; varType != Variable::V_UNKNOWN; varType += 10)
	{
		if (vHash.find((Variable::VARTYPE)varType) != vHash.end())
		{
			for (vit = vHash.at((Variable::VARTYPE)varType).begin(); vit != vHash.at((Variable::VARTYPE)varType).end(); ++vit)
			{
				int idx = vit->second;

				if (xSol[idx] > SOLVER_EPS)
					tmp << (vit->first).toString() << "; " << xSol[idx] << std::endl;
			}
		}
	}

	return tmp.str();
}

