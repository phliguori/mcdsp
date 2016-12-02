#include <cstdlib>
#include "Graph.h"
#include "Heuristic.h"
#include "GlobalVariables.h"
#include "CDSMip.h"
#include "Lagrange.h"
#include "BranchAndBound.h"

using namespace std;

void allCombinations(int _groundSet, int _nelements)
{

}

void completeColorFullGraph(C_graph* _g, int _nnodes, int _ncolors)
{

}

void bestRatioHeu(C_graph* _g, std::vector<C_assignment> & solHeu)
{
	std::set<C_assignment> bestRatioAssign;
	for (auto & v : _g->nodesVec)
	{
		C_assignment candidateAssign;
		candidateAssign.node = v->num;
		candidateAssign.ratio = 1e9;

		for (auto & cId : v->colors)
		{
			C_assignment tmp;
			tmp.node = v->num;
			C_color* color = _g->colorsVec[cId];
			auto & mIt = _g->nodeColorWeight.find(make_pair(v->num, cId));
			if (mIt != _g->nodeColorWeight.end())
			{
				tmp.color = cId;
				tmp.weight = mIt->second;
				tmp.ratio = (v->neighbors.size() > 0) ? tmp.weight / (double)v->neighbors.size() : tmp.weight;

				if (tmp < candidateAssign)
					candidateAssign = tmp;
			}
			else
			{
			}
		}
		bestRatioAssign.insert(candidateAssign);

	}

	printf("");
}

int main(int argc, char** argv)
{
	int seed = 0;
	C_graph g;
	srand(seed);
	g.random_graph(45, 0.3, 10, 100); // Generating a random graph

	//for (int i = 0; i < g.nb_nodes; ++i)
	//{
	//	cout << "v_" << i << ": [";
	//	for (int c = 0; c < g.nodesVec[i]->nb_colors; ++c)
	//	{
	//		cout << g.nodesVec[i]->colors[c];
	//		if (c < g.nodesVec[i]->nb_colors - 1)
	//			cout << ", ";
	//	}
	//	cout << "]\n";
	//}
	//printf("");

	for (int i = 0; i < g.nb_nodes; ++i)
	{
		sort(g.nodesVec[i]->colors.begin(), g.nodesVec[i]->colors.end());
	}

	//for (int i = 0; i < g.nb_nodes; ++i)
	//{
	//	cout << "v_" << i << ": [";
	//	for (int c = 0; c < g.nodesVec[i]->nb_colors; ++c)
	//	{
	//		cout << g.nodesVec[i]->colors[c];
	//		if (c < g.nodesVec[i]->nb_colors - 1)
	//			cout << ", ";
	//	}
	//	cout << "]\n";
	//}
	//printf("");

	for (int e = 0; e < g.nb_edges; ++e)
	{
		if (g.edgesVec[e]->end1->num > g.edgesVec[e]->end2->num)
		{
			C_node* tmp = g.edgesVec[e]->end1;
			g.edgesVec[e]->end1 = g.edgesVec[e]->end2;
			g.edgesVec[e]->end2 = tmp;
		}
	}
	sort(g.edgesVec.begin(), g.edgesVec.end(), C_edge());
	g.affiche(); // Printing the details of the graph

	// Data for the heuristic ( The heuristic works for any weight vector in the parameter)
	vector < C_assignment *> Solution_nodes;
	vector < C_assignment *> weights; // example of a weight vector

	for (int i = 0; i < g.nb_nodes; i++)
	{
		C_node* v = g.nodesVec[i];
		v->weights.resize(v->nb_colors);

		for (int c = 0; c < v->nb_colors; c++)
		{
			C_assignment * assignment = new(C_assignment);
			assignment->node = i;
			assignment->color = g.nodesVec[i]->colors[c];
			assignment->weight = (rand() % 100) + 1;
			weights.push_back(assignment);

			v->weights[c] = assignment->weight;
			g.nodeColorWeight[make_pair(assignment->node, assignment->color)] = assignment->weight;
		}
	}

	std::cout << std::endl << "------- ";
	std::cout << "Summary on the nodes colors";
	std::cout << std::endl;
	for (auto & v : g.nodesVec)
	{
		std::cout << "n" << v->num << ": " << v->colors.size() << " colors" << std::endl;
	}
	std::cout << std::endl;

	//Lagrangian l(&g);
	//list<pair<int, int>> varsFixedToOne;
	//list<pair<int, int>> varsFixedToZero;
	//varsFixedToOne.push_back(make_pair(6, 6));
	//l.fixVarToOne(6, 6);
	//varsFixedToOne.push_back(make_pair(11, 5));
	//l.fixVarToOne(11, 5);
	////varsFixedToOne.push_back(make_pair(13, 2));
	////l.fixVarToOne(13, 2);
	//varsFixedToOne.push_back(make_pair(23, 3));
	//l.fixVarToOne(23, 3);
	//varsFixedToOne.push_back(make_pair(26, 2));
	//l.fixVarToOne(26, 2);
	//varsFixedToOne.push_back(make_pair(33, 4));
	//l.fixVarToOne(33, 4);
	//varsFixedToOne.push_back(make_pair(39, 4));
	//l.fixVarToOne(39, 4);
	//varsFixedToOne.push_back(make_pair(42,6));
	//l.fixVarToOne(42, 6);

	//varsFixedToOne.push_back(make_pair(44, 4));
	//l.fixVarToOne(44, 4);
	//l.unfixVars(varsFixedToOne, varsFixedToZero);
	//varsFixedToOne.pop_back();

	//varsFixedToOne.push_back(make_pair(43, 5));
	//l.fixVarToOne(43, 5);
	//l.unfixVars(varsFixedToOne, varsFixedToZero);
	//varsFixedToOne.pop_back();

	//varsFixedToOne.push_back(make_pair(42, 8));
	//l.fixVarToOne(42, 8);
	//l.unfixVars(varsFixedToOne, varsFixedToZero);
	//varsFixedToOne.pop_back();

	//varsFixedToZero.push_back(make_pair(44, 4));
	//l.fixVarToZero(44, 4);
	//varsFixedToZero.push_back(make_pair(43, 5));
	//l.fixVarToZero(43, 5);
	//varsFixedToZero.push_back(make_pair(42, 8));
	//l.fixVarToZero(42, 8);

	//l.runSubGradientMethod(varsFixedToOne, varsFixedToZero, 32.0);
	//l.printXSol();

	BranchAndBound bb(&g);
	bb.run();

	////Start the heuristic
	//heuristic(g, Solution_nodes, weights);

	// Solving problem to optimality with CPLEX
	DominationForm mipForm(&g);
	mipForm.loadProblem("colorDom", SolverMIP::FOSENSE::MINIMIZATION, SolverMIP::PROBTYPE::INTEGER);

	mipForm.setNumThreads(1);

	mipForm.turnScreenOn();
	mipForm.shutDownHeuristics();
	mipForm.shutDownAutomaticCuts();
	mipForm.shutDownAutomaticPreSolve();

	int status = mipForm.solve(SolverMIP::METHOD::METHOD_MIP);

	double mip = 0.;
	if (status == SolverMIP::SOLVERSTAT::SOLVERSTAT_MIPOPTIMAL || status == SolverMIP::SOLVERSTAT::SOLVERSTAT_LPOPTIMAL ||
		status == SolverMIP::SOLVERSTAT::SOLVERSTAT_FEASIBLE)
	{
		mip = mipForm.getObjVal();
		mipForm.getX();

		std::cout << std::endl << "------";
		std::cout << "F.O: " << mip;
		std::cout << std::endl;
		std::cout << mipForm.printXSol();
	}

	return 0;
}

