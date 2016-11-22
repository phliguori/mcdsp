#include "Lagrange.h"

Lagrangian::Lagrangian(C_graph * _g, int _maxIterLag, int _maxIterNoLBImprov, double _alpha, double _eps)
{
	g = _g;
	maxIterLag = _maxIterLag;
	maxIterNoLBImprov = _maxIterNoLBImprov;
	alpha = _alpha;
	eps = _eps;

	noLBImprovCounter = 0;
	stepSize = 0.;
}

Lagrangian::~Lagrangian()
{
}

int Lagrangian::runSubGradientMethod()
{
	while (!isFinishedLagrange())
	{
		// 1. Solve Lagrangian dual problem
		solveLagragianDualSubproblem();

		// 2. Evaluate Lagrangian Lower Bound
		calculateLagrangianLB();

		// 3. Run Lagrangian Heuristic

		// 4. Update step-size
		calculateStepSize();

		// 5. Update subgradients
		calculateSubGradient();

		// 6. Update Lagrangian Multipliers
		updateLagragianMultipliers();

		// 7. Update Lagrangian costs
		updateLagragianCosts();

		// 8. Variable fixing
		variableFixing();
	}

	return 0;
}

void Lagrangian::updateLagragianCosts()
{
}

void Lagrangian::updateLagragianMultipliers()
{
}

void Lagrangian::solveLagragianDualSubproblem()
{
}

void Lagrangian::calculateSubGradient()
{
}

void Lagrangian::calculateStepSize()
{
}

void Lagrangian::calculateLagrangianLB()
{
}

void Lagrangian::variableFixing()
{
}

bool Lagrangian::isFinishedLagrange()
{
	return false;
}
