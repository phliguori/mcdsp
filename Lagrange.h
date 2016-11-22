#ifndef LAGRANGE_H
#define LAGRANGE_H

#include "Graph.h"

#include <vector>

class Lagrangian
{
public:
	 Lagrangian(C_graph * _g, int _maxIterLag=2000, int _maxIterNoLBImprov=125, double _alpha=2, double _eps=1e4);
	~Lagrangian();

	int runSubGradientMethod();

protected:
	void updateLagragianCosts();
	void updateLagragianMultipliers();
	void solveLagragianDualSubproblem();
	void calculateSubGradient();
	void calculateStepSize();
	void calculateLagrangianLB();
	void variableFixing();
	inline bool isFinishedLagrange();

	int noLBImprovCounter;
	int maxIterNoLBImprov;
	int maxIterLag;
	double eps;
	double alpha;
	double stepSize;
	vector<double> xSol;
	vector<double> subgradients;
	vector<double> lMultipliers;
	vector<double> lCoeffs;
	vector<bool> fixedVaraibles;
	
	C_graph* g;
};

#endif