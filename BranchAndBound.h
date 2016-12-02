#ifndef BRANCH_AND_BOUND
#define BRANCH_AND_BOUND

#include "Graph.h"
#include "Lagrange.h"

#include <vector>

class BranchAndBound
{
public:
	BranchAndBound(C_graph* _g);
	~BranchAndBound();

	void run();
	void printSol();
	double getBestUb() const { return bestUb; }
	double getBestLb() const { return bestLb; }

protected:

	void branch(int _v, int _l, int _val);

	C_graph* g;

	Lagrangian lag;

	int nNodes;
	double bestUb, bestLb;
	vector<int> xBest;
};

#endif
