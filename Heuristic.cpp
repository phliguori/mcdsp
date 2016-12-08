#include "Heuristic.h"
#include <algorithm>

int compare(const C_assignment * pi1, C_assignment  * pi2)
{
	return (pi1->weight >pi2->weight);
}

int heuristic(C_graph & G, vector < C_assignment *>& Sol_nodes, vector < C_assignment *>& weights) {

	/************************************ Heuristic for the Rainbow dominating set***********************/
	//   The heursitic consists in  putting every vertex in the dominating set we the color having a minimum weight over all its colors. 
	//   Then, the second set consists in deleting one by one each vertex from the dominating set and check if the new solution remains feasible. Otherwise, we put it back in the set.
	/******************************************************************************************************/

	vector < C_assignment *> Sol_nodes_tmp;
	// Initializing Sol_nodes
	for (int i = 0; i < G.nb_nodes; i++) {
		C_assignment * assignment = new(C_assignment);
		assignment->node = i;
		assignment->color = 0;
		assignment->weight = HUGE_VAL;
		Sol_nodes_tmp.push_back(assignment);
	}


	// Put all vertices in the dominants, such that each vertex has the color with the minimum weight
	for (int i = 0; i < weights.size(); i++)
		if (weights[i]->weight < Sol_nodes_tmp[weights[i]->node]->weight) {
			Sol_nodes_tmp[weights[i]->node]->color = weights[i]->color;
			Sol_nodes_tmp[weights[i]->node]->weight = weights[i]->weight;
		}


	// Sort couples (node, color) with respect to weights (Larger to smaller)
	vector<int> sorted_node;
	/* for (int i = 0; i < G.nb_nodes; i++) {
	int maximum = -1 * HUGE_VAL;
	for (int j = 0; j < G.nb_nodes; j++)
	if (Sol_nodes_tmp[j]->weight > maximum && std::find(sorted_node.begin(), sorted_node.end(), j) == sorted_node.end())
	maximum = j;
	sorted_node.push_back(maximum);
	}*/

	sort(Sol_nodes_tmp.begin(), Sol_nodes_tmp.end(), compare);

	// Deleting one by one each vertex and check if the newsolution remains feasible.

	vector <bool> assignments_tmp(G.nb_nodes, 1);
	for (int i = 0; i < G.nb_nodes; i++) {
		assignments_tmp[Sol_nodes_tmp[i]->node] = 0;

		if (!is_dominated(G, Sol_nodes_tmp[i]->node, Sol_nodes_tmp, assignments_tmp))
			assignments_tmp[Sol_nodes_tmp[i]->node] = 1;

		for (int j = 0; j < G.nodesVec[Sol_nodes_tmp[i]->node]->neighbors.size(); j++)
			if (!is_dominated(G, G.nodesVec[Sol_nodes_tmp[i]->node]->neighbors[j], Sol_nodes_tmp, assignments_tmp)) {
				assignments_tmp[Sol_nodes_tmp[i]->node] = 1;
				break;
			}
	}


	// Computing the objectif value and recuperation of the solution
	int val = 0;
	for (int i = 0; i < G.nb_nodes; i++)
		if (assignments_tmp[i] == 1) {
			Sol_nodes.push_back(Sol_nodes_tmp[i]);

			val += Sol_nodes_tmp[i]->weight;
		}


	// Print The results
	cout << " Objectif Value = " << val << endl;
	cout << " Number of vertices in the dominant = " << Sol_nodes.size() << "/" << G.nb_nodes << endl;

	for (int i = 0; i < Sol_nodes.size(); i++) {
		cout << " Node " << Sol_nodes[i]->node << " with color " << Sol_nodes[i]->color << endl;
	}


	// Deleting the used vectors
	Sol_nodes_tmp.clear();
	assignments_tmp.clear();
	return val;
}

bool is_dominated(C_graph &G, int node, vector < C_assignment *> &Sol_nodes_tmp, vector < bool> &assignments_tmp) {
	if (assignments_tmp[node] == 1)
		return true;

	for (int i = 0; i < G.nodesVec[node]->neighbors.size(); i++) {
		if (assignments_tmp[G.nodesVec[node]->neighbors[i]] == 1 && std::find(G.nodesVec[node]->colors.begin(), G.nodesVec[node]->colors.end(),
			Sol_nodes_tmp[G.nodesVec[node]->neighbors[i]]->color) != G.nodesVec[node]->colors.end())
			return true;
	}
	return false;
}