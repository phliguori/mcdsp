#include "BranchAndBound.h"
#include <list>
#include <string>

BranchAndBound::BranchAndBound(C_graph* _g)
	: lag(_g)
{
	g = _g;
	nNodes = 0;
	xBest.resize(g->nb_nodes, -1);

	bestUb = INFINITY;
	bestLb = -INFINITY;
}

BranchAndBound::~BranchAndBound()
{
}

void BranchAndBound::run()
{
	clock_t start = clock();

	cout << right << setw(6) << "Node"
		<< right << setw(2) << ""
		<<  left << setw(6) << "Var"
		<< right << setw(2) << ""
		<< right << setw(3) << "Val"
		<< right << setw(2) << ""
		<< right << setw(8) << "BestUb"
		<< right << setw(2) << ""
		<< right << setw(8) << "NodeLb"
		<< right << setw(2) << ""
		<< right << setw(8) << "NodeUb"
		<< right << setw(2) << ""
		<< right << setw(8) << "NodeGap"
		<< right << setw(2) << ""
		<<  left << setw(18) << "Lagran Status"
		<< "\n";

	branch(0, 0, -1);
	clock_t end = clock();
	printSol();
	cout << "\n---- ";
	cout << "Total elapsed time: " << (end - start) / (double) CLOCKS_PER_SEC;
}

void BranchAndBound::printSol()
{
	std::cout << "------";
	std::cout << "F.O: " << bestUb << "\n";
	for (int i = 0; i < xBest.size(); ++i)
	{
		if (xBest[i] != -1)
		{
			cout << "x_v" << i << "_l" << g->nodesVec[i]->colors[xBest[i]] << "; 1; " << g->nodesVec[i]->weights[xBest[i]] << "\n";
		}
	}
	cout << "All other variables are equal to zero.\n";
}

void BranchAndBound::branch(int _v, int _l, int _val)
{
	double nodeLb, nodeUb;
	Lagrangian::LAGRANGIAN_STATUS status;
	list<pair<int, int>> nodeVarsFixedToOne;
	list<pair<int, int>> nodeVarsFixedToZero;

	if (_val == 0)
	{
		lag.fixVarToZero(_v, _l);
		nodeVarsFixedToZero.push_back(make_pair(_v, _l));
	}
	else if (_val == 1)
	{
		lag.fixVarToOne(_v, _l);
		nodeVarsFixedToOne.push_back(make_pair(_v, _l));
	}

	nNodes++;
	status = lag.runSubGradientMethod(nodeVarsFixedToOne, nodeVarsFixedToZero, bestUb);
	nodeLb = lag.getBestLb();
	nodeUb = lag.getBestUb();

	double fblb, cblb;
	double gap = 0.;
	if (status != Lagrangian::LAG_INFEAS)
	{
		fblb = floor(nodeLb);
		cblb = fblb + ((nodeLb - fblb > 1e-4) ? 1 : 0);
		gap = lag.getGap();
	}
	else
	{
		gap = INFINITY;
		cblb = INFINITY;
	}
	
	cout << right << setw(6) << nNodes
		 << right << setw(2) << ""
		 <<  left << setw(6) << ((nNodes == 1) ? "" : string("x_") + to_string(_v) + string("_") + to_string(g->nodesVec[_v]->colors[_l]))
		 << right << setw(2) << ""
		 << right << setw(3) << ((nNodes == 1) ? "" : to_string(_val))
		 << right << setw(2) << ""
		 << right << setw(8) << bestUb
		 << right << setw(2) << ""
		 << right << setw(8) << cblb
		 << right << setw(2) << ""
		 << right << setw(8) << nodeUb
		 << right << setw(2) << ""
		 << right << setw(8) << setprecision(4) << gap
		 << right << setw(2) << ""
		 <<  left << setw(12) << lag.statusToString(status)
		 << "\n";

	//cout << "Node " << nNodes;
	//if (nNodes > 0)
	//{
	//	cout << ": Variable x_" << left << setw(2) << _v << "_" << left << setw(2) << g->nodesVec[_v]->colors[_l] << " is fixed to " << _val;
	//	cout << "\tBest UB: " << right << setw(4) << bestUb << "\tNode Ub: " << right << setw(4) << nodeUb << "\n";

	//}
	//else
	//	cout << "\n";

	if (nodeUb < bestUb)
	{
		bestUb = nodeUb;
		lag.getXSol(xBest);
		//printSol();
	}

	if (status == Lagrangian::LAG_FEAS_MAX_ITER || status == Lagrangian::LAG_FEAS_SMALL_STEPSIZE)
	{
		int chosenVertex, chosenColor;
		double bestRatio = INFINITY;
		chosenVertex = -1;
		chosenColor = -1;
		
		// chose the variable to branch
		if (nodeLb < bestUb)
		{
			for (int i = 0; i < g->nb_nodes; ++i)
			{
				if (lag.isVarFixedToOne(i))
					continue;

				for (int c = 0; c < g->nodesVec[i]->nb_colors; ++c)
				{
					if (lag.isVarFixedToZero(i, c))
						continue;

					double ratio = lag.getRatio(i, c);
					if (ratio < bestRatio)
					{
						chosenVertex = i;
						chosenColor = c;
					}
				}
			}

			if (chosenVertex == -1)
			{
				cout << "[ERROR] - 100 - Cannot choose a variable to branch.\n";
				lag.unfixVars(nodeVarsFixedToOne, nodeVarsFixedToZero);
				return;
			}
		}

		int val = 1;
		double randomBranching = rand() / (double)RAND_MAX;

		if (randomBranching > 0.95)
			val = 0;

		if (nodeLb < bestUb)
		{
			branch(chosenVertex, chosenColor, val);
		}

		if (nodeLb < bestUb)
		{
			if (val == 1)
				val = 0;
			else
				val = 1;
			branch(chosenVertex, chosenColor, val);
		}
	}

	lag.unfixVars(nodeVarsFixedToOne, nodeVarsFixedToZero);
}
