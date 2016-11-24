#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "Graph.h"
#include "GlobalVariables.h"


int heuristic ( C_graph & G, vector < C_assignment *>  &Sol_nodes, vector < C_assignment *>  &weights);
bool is_dominated(C_graph &G, int node , vector < C_assignment *>  &Sol_nodes_tmp, vector < bool>  &assignments_tmp);

#endif 


