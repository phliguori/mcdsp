#include "Lagrange.h"

Lagrangian::Lagrangian(C_graph * _g, int _maxIterLag, int _maxIterNoLBImprov, double _alpha, double _eps)
{
	g = _g;
	maxIterLag = _maxIterLag;
	maxIterNoLBImprov = _maxIterNoLBImprov;
	alphaInit = _alpha;
	eps = _eps;

	totalVarsCounter = 0;
	fixedVarsCounter = 0;
	for (int i = 0; i < g->nb_nodes; ++i)
		totalVarsCounter += g->nodesVec[i]->nb_colors;

	heuristicSolution.clear();
	heuristicSolution.resize(g->nb_nodes, -1);

	bestXSol.clear();
	bestXSol.resize(g->nb_nodes, -1);

	// every position i will stock -1, in case that node i is not in the dominant or l, where l
	// is the index of the color chosen for vertex i in the dominant
	xSolDualLag.clear();
	xSolDualLag.resize(g->nb_nodes, -1);

	// innitializing all lagrangian multipliers to zero
	// one multiplier associated to each domination_contraints which is found dualized
	lMultipliers.clear();
	lMultipliers.resize(g->nb_nodes, 0.);

	// initializing all subgradients to zero
	// one subgradient associated to each domination_contraints which is found dualized
	subgradients.clear();
	subgradients.resize(g->nb_nodes, 0.);

	inactiveCons.clear();
	inactiveCons.resize(g->nb_nodes, 0);

	fixedToOneVars.clear();
	fixedToOneVars.resize(g->nb_nodes, -1);

	fixedToZeroVars.resize(g->nb_nodes, vector<bool>());
	for (int i = 0; i < g->nb_nodes; ++i)
		fixedToZeroVars[i].resize(g->nodesVec[i]->nb_colors, false);

	lCosts.resize(g->nb_nodes, vector<double>());	// bidimensional where element lcost[i][c] stocks the lagrangian dual cost
	for (int i = 0; i < g->nb_nodes; ++i)			// associated with vaiable x[g->nodesVec[i]][g->colorsVec[v->colors[c]]
	{
		lCosts[i].resize(g->nodesVec[i]->colors.size());
	}

	vUtilColorNeighborhoodSize.resize(g->nb_nodes, vector<int>());
	for (int i = 0; i < g->nb_nodes; ++i)
	{
		C_node* v = g->nodesVec[i];
		vUtilColorNeighborhoodSize[i].resize(v->colors.size(), 0);
	}

	vColorNeighborhood.resize(g->nb_nodes, vector<vector<int>>());	// structure to auxiliate the calculation of lagrangian costs
	for (int i = 0; i < g->nb_nodes; ++i)
	{
		C_node* v = g->nodesVec[i];
		vColorNeighborhood[i].resize(v->colors.size(), vector<int>());

		for (int c = 0; c < v->colors.size(); ++c)
		{
			C_color* color = g->colorsVec[v->colors[c]];

			for (int j = 0; j < v->neighbors.size(); ++j)
			{
				int u = v->neighbors[j];

				auto & vIt = find(color->nodes_with_this_color.begin(), color->nodes_with_this_color.end(), u);
				if (vIt != color->nodes_with_this_color.end())
				{
					vColorNeighborhood[i][c].push_back(u);
				}
			}

			vUtilColorNeighborhoodSize[i][c] = vColorNeighborhood[i][c].size() + 1;
		}
	}

	nodeColorDominance.resize(g->nb_nodes, vector<bool>());
	for (int i = 0; i < g->nb_nodes; ++i)
	{
		nodeColorDominance[i].resize(g->nb_colors, false);

		C_node* v = g->nodesVec[i];
		for (int c = 0; c < v->colors.size(); ++c)
		{
			C_color* color = g->colorsVec[v->colors[c]];
			nodeColorDominance[v->num][color->num] = true;
		}
	}

	initialize(INFINITY);
}

Lagrangian::~Lagrangian()
{
}

void Lagrangian::initialize(double _bestKnownUB)
{
	blb = -INFINITY;
	bub = _bestKnownUB;

	alpha = alphaInit;
	stepSize = 0.;
	iterCounter = 0;
	noLBImprovCounter = 0;

	fill(bestXSol.begin(), bestXSol.end(), -1);
	fill(heuristicSolution.begin(), heuristicSolution.end(), -1);
	copy(fixedToOneVars.begin(), fixedToOneVars.end(), xSolDualLag.begin());

	// innitializing all lagrangian multipliers to zero
	fill(lMultipliers.begin(), lMultipliers.end(), 0.);

	// initializing all subgradients to zero
	fill(subgradients.begin(), subgradients.end(), 0.);

	// initializing lagrangian costs with original costs
	for (int i = 0; i < g->nb_nodes; ++i)
	{
		C_node* v = g->nodesVec[i];

		for (int c = 0; c < v->colors.size(); ++c)
		{
			double initialCost = v->weights[c];
			lCosts[i][c] = initialCost;
		}
	}

	//int tmp, c;
	//// Fixing variables to one
	//tmp = 0; c = 0;  fixedToOneVars[tmp] = true; inactiveCons[tmp] = true; xSol[tmp] = c;
	//for (int j = 0; j < vColorNeighborhood[tmp][c].size(); ++j)
	//{
	//	int u = vColorNeighborhood[tmp][c][j];
	//	inactiveCons[u] = true;
	//}

	//tmp = 1; c = 0; fixedToOneVars[tmp] = true; inactiveCons[tmp] = true; xSol[tmp] = c;
	//for (int j = 0; j < vColorNeighborhood[tmp][c].size(); ++j)
	//{
	//	int u = vColorNeighborhood[tmp][c][j];
	//	inactiveCons[u] = true;
	//}

	//tmp = 2; c = 0; fixedToOneVars[tmp] = true; inactiveCons[tmp] = true; xSol[tmp] = c;
	//for (int j = 0; j < vColorNeighborhood[tmp][c].size(); ++j)
	//{
	//	int u = vColorNeighborhood[tmp][c][j];
	//	inactiveCons[u] = true;
	//}

	//tmp = 7; c = 0; fixedToOneVars[tmp] = true; inactiveCons[tmp] = true; xSol[tmp] = c;
	//for (int j = 0; j < vColorNeighborhood[tmp][c].size(); ++j)
	//{
	//	int u = vColorNeighborhood[tmp][c][j];
	//	inactiveCons[u] = true;
	//}

	//tmp = 8; c = 2; fixedToOneVars[tmp] = true; inactiveCons[tmp] = true; xSol[tmp] = c;
	//for (int j = 0; j < vColorNeighborhood[tmp][c].size(); ++j)
	//{
	//	int u = vColorNeighborhood[tmp][c][j];
	//	inactiveCons[u] = true;
	//}

	//// Fixing variables to zero
	//tmp = 3;
	//for (int c = 0; c < g->nodesVec[tmp]->nb_colors; ++c)
	//{
	//	fixedToZeroVars[tmp][c] = true;
	//}

	//tmp = 4;
	//for (int c = 0; c < g->nodesVec[tmp]->nb_colors; ++c)
	//{
	//	fixedToZeroVars[tmp][c] = true;
	//}

	//tmp = 5;
	//for (int c = 0; c < g->nodesVec[tmp]->nb_colors; ++c)
	//{
	//	fixedToZeroVars[tmp][c] = true;
	//}

	//tmp = 6;
	//for (int c = 0; c < g->nodesVec[tmp]->nb_colors; ++c)
	//{
	//	fixedToZeroVars[tmp][c] = true;
	//}

	//tmp = 9;
	//for (int c = 0; c < g->nodesVec[tmp]->nb_colors; ++c)
	//{
	//	fixedToZeroVars[tmp][c] = true;
	//}

	//for (int i = 0; i < g->nb_nodes; ++i)
	//{
	//	C_node* v = g->nodesVec[i];
	//	for (int c = 0; c < v->colors.size(); ++c)
	//	{
	//		C_color* color = g->colorsVec[v->colors[c]];
	//		cout << "n_" << v->num << ": (l_" << color->num << ", w_"<< v->weights[c] << "): [";
	//		for (int j = 0; j < vColorNeighborhood[i][c].size(); ++j)
	//		{
	//			cout << vColorNeighborhood[i][c][j];
	//			if (j < vColorNeighborhood[i][c].size() - 1)
	//				cout << ", ";
	//		}
	//		cout << "]\n";
	//	}
	//}
	//printf("");

	return;
}

Lagrangian::LAGRANGIAN_STATUS Lagrangian::runSubGradientMethod(list<pair<int, int>> & _lagFixationToOne, list<pair<int, int>> & _lagFixationToZero, double _bestKnownUB)
{	
	double prob = 0.95;
	LAGRANGIAN_STATUS status = Lagrangian::LAG_UNKNOWN;
	int nbConsecIterNoLbImprov = 0;
	int nbConsecIterNoUBImprov = 0;

	initialize(_bestKnownUB);

	start = clock();
	while ((iterCounter++ < maxIterLag) && (alpha > eps))
	{
		// 1. Solve Lagrangian dual problem
		solveLagragianDualSubproblem();

		// 2. Evaluate Lagrangian Lower Bound
		calculateLagrangianLB();

		if (lb > blb + eps)
		{
			blb = lb;
			nbConsecIterNoLbImprov = 0;
			
			tryToFixVariables(_lagFixationToOne, _lagFixationToZero);
		}
		else
			nbConsecIterNoLbImprov++;

		// 3. Run Lagrangian Heuristic
		if (nbConsecIterNoLbImprov == maxIterNoLBImprov)
			prob -= 0.01;

		ub = lagrangianHeuristic(0.95);

		if (ub < bub - eps)
		{
			bub = ub;
			prob = 0.95;
			nbConsecIterNoUBImprov = 0;

			copy(heuristicSolution.begin(), heuristicSolution.end(), bestXSol.begin());
			tryToFixVariables(_lagFixationToOne, _lagFixationToZero);
		}
		else
			nbConsecIterNoUBImprov++;

		// 4. Update subgradients
		calculateSubGradient();

		if (nbConsecIterNoLbImprov == 0)
		{
			double tmp = tryPrimalFeasibility();

			if (tmp < bub)
			{
				ub = tmp;
				bub = ub;
				prob = 0.95;
				nbConsecIterNoUBImprov = 0;
				copy(heuristicSolution.begin(), heuristicSolution.end(), bestXSol.begin());
			}
		}

		// 5. Update step-size
		double theta = calculateStepSize(nbConsecIterNoLbImprov);

		// 6. Update Lagrangian Multipliers
		updateLagragianMultipliers(theta);

		// 7. Update Lagrangian costs
		updateLagragianCosts();

		//if (iterCounter % 50 == 0 /*|| iterCounter < 30*/)
		//{
		//	end = clock();
		//	printStatus(status);
		//}

		if ((blb > bub + eps) || (bub - blb < 1.0 - eps))
			break;
	}
	end = clock();

	updateStatus(status);

	//cout << "\n";
	//cout << "----------------------------------\n";
	//cout << "---- Lagrangian Relaxation Output\n";
	//printStatus(status);

#ifdef DEBUG
	printLagrangianCosts();
	printXSol();
	printSubgradient();
	printLagMultiplier();
#endif

	return status;
}

void Lagrangian::tryToFixVarsToZero(list<pair<int, int>> & _varsFixedToOne, list<pair<int, int>> & _varsFixedToZero)
{
	for (int i = 0; i < g->nb_nodes; ++i)
	{
		C_node* v = g->nodesVec[i];
		for (int c = 0; c < v->nb_colors; ++c)
		{
			if (isVarFixedToZero(i, c) || isVarFixedToOne(i) || xSolDualLag[i] == c)
				continue;

			double newLb = lb - ((xSolDualLag[i] != -1) ? lCosts[i][xSolDualLag[i]] : 0.) + lCosts[i][c];

			if (newLb > bub)
			{
				interditedVars.insert(make_pair(i, c));
				fixVarToZero(i, c);
				_varsFixedToZero.push_back(make_pair(i, c));
			}
		}
	}
}

void Lagrangian::tryToFixVarsToOne(list<pair<int, int>> & _varsFixedToOne, list<pair<int, int>> & _varsFixedToZero)
{
	// For a given node v which belongs to the dominant with a certain color l_0,
	// We need to temporarily remove l_0 from the solution and build 
	// the better lagrangian dual solution which does not contain l_0.
	// 
	// After, we need to verify wheter the new lower bound obtained is greater
	// than the best upper bound known so far. If this is true, we can than fix
	// node v with color l_0 in the dominant.
	for (int i = 0; i < g->nb_nodes; ++i)
	{
		if (xSolDualLag[i] == -1 || isVarFixedToOne(i))
			continue;

		C_node* v = g->nodesVec[i];
		int chosenColor = xSolDualLag[i];

		double newLb = lb - lCosts[i][chosenColor];

		int bestColorCandidate = -1;
		double bestLCost = INFINITY;

		for (int c = 0; c < v->nb_colors; ++c)
		{
			if (c != chosenColor && !isVarFixedToZero(i, c))
			{
				double lCost = lCosts[i][c];

				if (lCost < bestLCost)
				{
					bestColorCandidate = c;
					bestLCost = lCost;
				}
			}
		}

		if (bestColorCandidate != -1 && bestLCost < -eps)
		{
			newLb += bestLCost;
		}

		if (newLb - eps > bub)
		{
			for (int c = 0; c < g->nodesVec[i]->nb_colors; ++c)
			{
				if (c != chosenColor && !isVarFixedToZero(i, c))
				{
					_varsFixedToZero.push_back(make_pair(i, c));
				}
			}

			_varsFixedToOne.push_back(make_pair(i, chosenColor));
			fixVarToOne(i, chosenColor);

			//for (int j = 0; j < v->neighbors.size(); ++j)
			//{
			//	C_node* u = g->nodesVec[v->neighbors[j]];

			//	for (int c = 0; c < u->nb_colors; ++c)
			//	{
			//		--vUtilColorNeighborhoodSize[u->num][c];
			//	}

			//	if (hasColor(u->num, g->colorsVec[chosenColor]->num))
			//	{
			//		for (int k = 0; k < u->neighbors.size(); ++k)
			//		{
			//			C_node* w = g->nodesVec[u->neighbors[k]];

			//			if (w->num != i)
			//			{
			//				for (int l = 0; l < vUtilColorNeighborhoodSize[w->num].size(); ++l)
			//				{
			//					--vUtilColorNeighborhoodSize[w->num][l];
			//				}
			//			}
			//		}
			//	}
			//}
		}
	}
}

void Lagrangian::unfixVars(list<pair<int, int>>& _nodeVarsFixedToOne, list<pair<int, int>>& _nodeVarsFixedToZero)
{
	for (auto & element : _nodeVarsFixedToZero)
	{
		int v = element.first;
		int c = element.second;

		unfixVarToZero(v, c);
	}

	for (auto & element : _nodeVarsFixedToOne)
	{
		int i = element.first;
		int c = element.second;

		unfixVarToOne(i, c);

		// Inactivate constraints
		--inactiveCons[i];
		for (int j = 0; j < vColorNeighborhood[i][c].size(); ++j)
		{
			int u = vColorNeighborhood[i][c][j];
			--inactiveCons[u];
		}


		//C_node* v = g->nodesVec[i];
		//int chosenColor = v->colors[c];

		//for (int j = 0; j < v->neighbors.size(); ++j)
		//{
		//	C_node* u = g->nodesVec[v->neighbors[j]];

		//	for (int c = 0; c < u->nb_colors; ++c)
		//	{
		//		++vUtilColorNeighborhoodSize[u->num][c];
		//	}

		//	if (hasColor(u->num, g->colorsVec[chosenColor]->num))
		//	{
		//		for (int k = 0; k < u->neighbors.size(); ++k)
		//		{
		//			C_node* w = g->nodesVec[u->neighbors[k]];

		//			if (w->num != i)
		//			{
		//				for (int l = 0; l < vUtilColorNeighborhoodSize[w->num].size(); ++l)
		//				{
		//					++vUtilColorNeighborhoodSize[w->num][l];
		//				}
		//			}
		//		}
		//	}
		//}
	}
}

void Lagrangian::fixVarToZero(int _v, int _c)
{
	++fixedVarsCounter;
	fixedToZeroVars[_v][_c] = true;

	//cout << "[" << fixedVars << "] Variable x_v" << _v << "_l" << g->nodesVec[_v]->colors[_c] << " is now fixed to zero (iteration " << iterCounter << ").\n";
}

void Lagrangian::fixVarToOne(int _v, int _c)
{
	//cout << "[" << fixedVars << "] Variable x_v" << _v << "_l" << g->nodesVec[_v]->colors[_c] << " is now fixed to one (iteration " << iterCounter << ").\n";
	fixedVarsCounter++;
	for (int c = 0; c < g->nodesVec[_v]->nb_colors; ++c)
	{
		if (c != _c && !isVarFixedToZero(_v, c))
		{
			fixedVarsCounter++;
			//fixVarToZero(_v, c);
		}
	}

	xSolDualLag[_v] = _c;
	++inactiveCons[_v];
	fixedToOneVars[_v] = _c;

	for (int j = 0; j < vColorNeighborhood[_v][_c].size(); ++j)
	{
		int u = vColorNeighborhood[_v][_c][j];
		++inactiveCons[u];
	}
}

inline void Lagrangian::unfixVarToZero(int _v, int _c)
{
	--fixedVarsCounter;
	fixedToZeroVars[_v][_c] = false;
}

inline void Lagrangian::unfixVarToOne(int _v, int _c)
{
	--fixedVarsCounter;
	xSolDualLag[_v] = -1;
	fixedToOneVars[_v] = -1;

	for (int c = 0; c < g->nodesVec[_v]->nb_colors; ++c)
	{
		if (c != _c && !isVarFixedToZero(_v, c))
		{
			--fixedVarsCounter;
		}
	}
}

inline double Lagrangian::getGap() const
{
	double fblb = floor(blb);
	fblb += (blb - fblb > eps) ? 1 : 0;

	return (bub - fblb) / bub;
}

double Lagrangian::getRatio(int _v, int _c)
{
	double ratio = INFINITY;

	double weight_v = g->nodesVec[_v]->weights[_c];
	double n_v = vUtilColorNeighborhoodSize[_v][_c];

	if (n_v > 0)
		ratio = weight_v / n_v;

	return ratio;
}

//inline double Lagrangian::getBestUb() const
//{
//	return bub;
//}
//
//inline double Lagrangian::getBestLb() const
//{
//	return blb;
//}
//
//void Lagrangian::getXSol(vector<int>& _destination)
//{
//	copy(bestXSol.begin(), bestXSol.end(), _destination.begin());
//}

void Lagrangian::updateLagragianCosts()
{
	// C_vl = c_vl + (lambda_v + \sum_{u in N(v) : l in N(u)}
	for (int i = 0; i < g->nb_nodes; ++i)
	{
		C_node* v = g->nodesVec[i];

		if (isVarFixedToOne(i))
		{
			int c = xSolDualLag[i];
			lCosts[i][c] = v->weights[c];
			continue;
		}

		double lambdaV = lMultipliers[i];

		for (int c = 0; c < v->colors.size(); ++c)
		{
			C_color* color = g->colorsVec[v->colors[c]];

			double initialCost = v->weights[c];
			double lCost = initialCost - lambdaV;

			for (int j = 0; j < vColorNeighborhood[i][c].size(); ++j)
			{
				int u = vColorNeighborhood[i][c][j];
				double lambdaU = lMultipliers[u];
				lCost -= lambdaU;
			}

			lCosts[i][c] = lCost;
		}
	}

	return;
}

void Lagrangian::updateLagragianMultipliers(double _theta)
{
	for (int j = 0; j < lMultipliers.size(); ++j)
	{
		if (isConsInactive(j))
			lMultipliers[j] = 0.;
		else
			lMultipliers[j] = max(0., lMultipliers[j] + _theta * subgradients[j]);
	}
}

void Lagrangian::solveLagragianDualSubproblem()
{
	for (int i = 0; i < g->nb_nodes; ++i)
	{
		if (isVarFixedToOne(i))
			continue;

		C_node* v = g->nodesVec[i];

		int bestColorIdx = -1;
		double bestLagrangianCost = INFINITY;
		for (int c = 0; c < v->colors.size(); ++c)
		{
			if (isVarFixedToZero(i, c))
				continue;

			C_color* color = g->colorsVec[v->colors[c]];
			if (lCosts[i][c] < -eps && lCosts[i][c] < bestLagrangianCost)
			{
				bestLagrangianCost = lCosts[i][c];
				bestColorIdx = c;
			}
		}
		xSolDualLag[i] = bestColorIdx;
	}
}

void Lagrangian::calculateSubGradient()
{
	for (int i = 0; i < g->nodesVec.size(); ++i)
	{
		if (isConsInactive(i))
			subgradients[i] = 0.;
		else
			subgradients[i] = 1.0;
	}

	for (int i = 0; i < g->nodesVec.size(); ++i)
	{
		if (!isConsInactive(i))
		{
			if (xSolDualLag[i] == -1)
				continue;

			--subgradients[i];

			int c = xSolDualLag[i];
			for (int j = 0; j < vColorNeighborhood[i][c].size(); ++j)
			{
				int u = vColorNeighborhood[i][c][j];

				if (!isConsInactive(u))
					--subgradients[u];
			}
		}
	}

	return;
}

double Lagrangian::calculateStepSize(int & _nbConsecIterNoLbImprov)
{
	double squareNorm = 0.;

	if (_nbConsecIterNoLbImprov == maxIterNoLBImprov)
	{
		alpha /= 2.0;
		_nbConsecIterNoLbImprov = 0;
	}

	for (int s = 0; s < subgradients.size(); ++s)
	{
		if (isConsInactive(s))
			continue;

		squareNorm += (subgradients[s] * subgradients[s]);
	}

	//return  (squareNorm > eps) ? alpha * (1.03 * ub - lb) / squareNorm : 0.;
	return  (squareNorm > eps) ? alpha * (1.03 * bub - lb) / squareNorm : 0.;
}

void Lagrangian::calculateLagrangianLB()
{
	lb = 0.;
	// lb = sum{v \in V} sum{l \in L(v)} (c_vl - (lambda_v + sum{u \in N(v) : l \in L(u)}lambda_u))*x_vl + sum{v in V} lambda_v
	for (int i = 0; i < xSolDualLag.size(); ++i)
	{
		if (isVarFixedToOne(i))
		{
			lb += g->nodesVec[i]->weights[xSolDualLag[i]];
		}
		else
		{
			lb += lMultipliers[i];

			if (xSolDualLag[i] != -1)
			{
				int c = xSolDualLag[i];
				lb += lCosts[i][c];
			}
		}
	}
}

void Lagrangian::tryToFixVariables(list<pair<int, int>> & _nodeVarsFixedToOne, list<pair<int, int>> & _nodeVarsFixedToZero)
{
	// try to fix variable to zero
	tryToFixVarsToZero(_nodeVarsFixedToOne, _nodeVarsFixedToZero);

	// try to fix variables to one
	tryToFixVarsToOne(_nodeVarsFixedToOne, _nodeVarsFixedToZero);
}

double Lagrangian::lagrangianHeuristic(double _rclParam)
{
	double solCost = 0.;
	int sentinel = 0;
	int dominationCounter = 0;

	copy(fixedToOneVars.begin(), fixedToOneVars.end(), heuristicSolution.begin());

	vector<bool> isDominated(g->nb_nodes, false);

	for (int i = 0; i < g->nb_nodes; ++i)
	{
		if (isVarFixedToOne(i))
		{
			int c = fixedToOneVars[i];
			solCost += g->nodesVec[i]->weights[c];
		}

		if (isConsInactive(i))
		{
			++dominationCounter;
			isDominated[i] = true;
		}
	}

	do
	{
		int vertexChoice = -1;
		int colorChoice = -1;
		double bestRatio = INFINITY;

		std::vector<std::pair<std::pair<int, int>, double>> colorRatios;
		for (int i = 0; i < g->nb_nodes; ++i)
		{
			if (isVarFixedToOne(i) || heuristicSolution[i] != -1)
				continue;

			C_node* v = g->nodesVec[i];

			for (int c = 0; c < v->colors.size(); ++c)
			{
				if (isVarFixedToZero(i, c))
					continue;

				int nonDominatedCounter = (isConsInactive(i) || isDominated[i]) ? 0 : 1;
				double weight = lCosts[v->num][c];

				for (int j = 0; j < vColorNeighborhood[v->num][c].size(); ++j)
				{
					if (!isDominated[v->neighbors[j]])
						++nonDominatedCounter;
				}

				// vertex i is already dominated by one of its neighbors and it cannot dominate any of its neighbors
				if (nonDominatedCounter == 0)
					continue;

				double ratio = (weight > eps) ? weight / (double)nonDominatedCounter : weight * (double)nonDominatedCounter;
				colorRatios.push_back(make_pair(make_pair(i, c), ratio));

				if (ratio < bestRatio)
				{
					bestRatio = ratio;
				}
			}
		}

		vector<pair<int, int>> rcl;
		for (auto & element : colorRatios)
		{
			pair<int, int> assign = element.first;
			double ratio = element.second;

			if ((ratio > 0 && ratio < bestRatio / _rclParam) || (ratio < 0 && ratio < bestRatio * _rclParam))
			{
				rcl.push_back(assign);
			}
		}

		if (rcl.size() > 0)
		{
			int randomChoice = rand() % rcl.size();
			vertexChoice = rcl[randomChoice].first;
			colorChoice = rcl[randomChoice].second;

			C_node* chosenCandidate = g->nodesVec[vertexChoice];

			if (!isDominated[chosenCandidate->num])
			{
				dominationCounter++;
				isDominated[chosenCandidate->num] = true;
			}

			for (int j = 0; j < vColorNeighborhood[chosenCandidate->num][colorChoice].size(); ++j)
			{
				if (!isDominated[vColorNeighborhood[chosenCandidate->num][colorChoice][j]])
				{
					dominationCounter++;
					isDominated[vColorNeighborhood[chosenCandidate->num][colorChoice][j]] = true;
				}
			}

			//heuristicSolution[chosenCandidate->num] = chosenCandidate->colors[colorChoice];
			heuristicSolution[chosenCandidate->num] = colorChoice;
			solCost += chosenCandidate->weights[colorChoice];
		}
	} while (dominationCounter < g->nb_nodes && ++sentinel <= g->nb_nodes + 1);

	if (sentinel > g->nb_nodes + 1) //no feasible solution found
		solCost = INFINITY;

	return solCost;
}

double Lagrangian::tryPrimalFeasibility()
{
	int totalDomination = 0;
	vector<bool> isDominated(g->nb_nodes, true);
	vector<int> infeasibleDomination;

	copy(xSolDualLag.begin(), xSolDualLag.end(), heuristicSolution.begin());

	for (int i = 0; i < g->nb_nodes; ++i)
	{
		if (subgradients[i] > eps)
		{
			infeasibleDomination.push_back(i);
			isDominated[i] = false;
		}
		else
			totalDomination++;
	}

	int sentinel = 0;
	while (totalDomination < g->nb_nodes && sentinel++ < infeasibleDomination.size())
	{
		double bestRatio = INFINITY;
		int vertexChoice, colorChoice;

		for (int k = 0; k < infeasibleDomination.size(); ++k)
		{
			if (isDominated[infeasibleDomination[k]])
				continue;

			C_node* v = g->nodesVec[infeasibleDomination[k]];
			std::vector<std::pair<std::pair<int, int>, double>> colorRatios;

			for (int c = 0; c < v->colors.size(); ++c)
			{
				int nonDominatedCounter = 1;
				double weight = v->weights[c];

				for (int j = 0; j < vColorNeighborhood[v->num][c].size(); ++j)
				{
					if (!isDominated[vColorNeighborhood[v->num][c][j]])
						++nonDominatedCounter;
				}

				double ratio = (weight > eps) ? weight / (double)nonDominatedCounter : weight * (double)nonDominatedCounter;
				colorRatios.push_back(make_pair(make_pair(v->num, c), ratio));

				if (ratio < bestRatio)
				{
					vertexChoice = v->num;
					colorChoice = c;
					bestRatio = ratio;
				}
			}
		}

		totalDomination++;
		isDominated[vertexChoice] = true;
		for (int j = 0; j < vColorNeighborhood[vertexChoice][colorChoice].size(); ++j)
		{
			int u = vColorNeighborhood[vertexChoice][colorChoice][j];
			if (!isDominated[u])
			{
				++totalDomination;
				isDominated[u] = true;
			}
		}

		heuristicSolution[vertexChoice] = colorChoice;
	}

	double solCost = 0.;
	for (int i = 0; i < heuristicSolution.size(); ++i)
	{
		if (heuristicSolution[i] != -1)
		{
			solCost += g->nodesVec[i]->weights[heuristicSolution[i]];
		}
	}

	return solCost;
}

void Lagrangian::updateStatus(LAGRANGIAN_STATUS & _status)
{
	if (blb > bub + eps)
	{
		_status = LAG_INFEAS;
	}

	if (blb < bub + eps && bub - blb < 1.0 - eps)
	{
		_status = LAG_OPTIMAL;
	}

	if (alpha < eps)
	{
		_status = LAG_FEAS_SMALL_STEPSIZE;
	}

	if (iterCounter >= maxIterLag)
	{
		_status = LAG_FEAS_MAX_ITER;
	}
}

void Lagrangian::printLagrangianCosts() const
{
	for (int v = 0; v < g->nb_nodes; ++v)
	{
		cout << "lCost_" << v << ": [";
		for (int c = 0; c < lCosts[v].size(); ++c)
		{
			cout << lCosts[v][c];
			if (c < lCosts[v].size() - 1)
				cout << ", ";
		}
		cout << "]\n";
	}
}

void Lagrangian::printXSol() const
{
	for (int i = 0; i < bestXSol.size(); ++i)
	{
		if (bestXSol[i] != -1)
		{
			cout << "x_v" << i << "_l" << g->nodesVec[i]->colors[bestXSol[i]] << "; 1; " << g->nodesVec[i]->weights[bestXSol[i]] << "\n";
		}
	}
	cout << "All other variables are equal to zero.\n";
}

void Lagrangian::printSubgradient() const
{
	for (int i = 0; i < subgradients.size(); ++i)
	{
		cout << "subgradient_" << i << ": [" << subgradients[i] << "]\n";
	}
}

void Lagrangian::printLagMultiplier() const
{
	for (int i = 0; i < lMultipliers.size(); ++i)
	{
		cout << "lambda_" << i << ": [" << lMultipliers[i] << "]\n";
	}
}

void Lagrangian::printStatus(LAGRANGIAN_STATUS _status) const
{
	cout << "--- Iter " << iterCounter << "\n";
	cout << "Status: " << statusToString(_status) << "\n";
	cout << "Alpha: " << alpha << "\n";
	cout << "Lb: " << lb << "\n";
	cout << "Ub: " << ub << "\n";
	cout << "Best Lb: " << blb << "\n";
	cout << "Best Ub: " << bub << "\n";
	cout << "Gap (%): " << getGap() << "\n";
	cout << "Execution time (s): " << (end - start) / (double)CLOCKS_PER_SEC << "\n";
	cout << "\n";
}

string Lagrangian::statusToString(LAGRANGIAN_STATUS _status) const
{
	string tmp;

	switch (_status)
	{
	case LAG_OPTIMAL:				tmp += "LAGRANGIAN_OPTIMAL"; break;
	case LAG_FEAS_MAX_ITER:			tmp += "LAGRANGIAN_FEASIBLE_MAX_ITER"; break;
	case LAG_FEAS_SMALL_STEPSIZE:	tmp += "LAGRANGIAN_FEASIBLE_STEP_SIZE"; break;
	case LAG_INFEAS:				tmp += "LAGRANGIAN_INFEASIBLE"; break;
	default:						tmp += "UNKNOWN";
	}

	return tmp;
}


// Test 1: the variables fixed to one imply a color dominating set
// Test 2: fixing some variables to zero
// Test 2: the variables fixed to one do not imply a color dominating set 