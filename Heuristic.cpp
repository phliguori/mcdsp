#include "Heuristic.h"

int heuristic(C_graph & G, vector < C_assignment *>& Sol_nodes, vector < C_assignment *>& weights)
{

    /************************************ Heuristic for the Rainbow dominating set***********************/
    //   The heursitic consists in  putting every vertex in the dominating set we the color having a minimum weight over all its colors. 
    //   Then, the second set consists in deleting one by one each vertex from the dominating set and check if the new solution remains feasible. Otherwise, we put it back in the set.
    /******************************************************************************************************/


    // Sort couples (node, color) with respect to weights (Smaller to Larger)
    for (int i = 0; i < weights.size() - 1; i++)
    {
        for (int j = i + 1; j < weights.size(); j++)
        {
            if (weights[i]->weight > weights[j]->weight)
            {
                int tmp1 = weights[i]->node;
                int tmp2 = weights[i]->color;
                int tmp3 = weights[i]->weight;

                weights[i]->node = weights[j]->node;
                weights[i]->color = weights[j]->color;
                weights[i]->weight = weights[j]->weight;

                weights[j]->node = tmp1;
                weights[j]->color = tmp2;
                weights[j]->weight = tmp3;
            }
        }
    }


    // Put all vertices in the dominants, such that each vertex has the color with the minimum weight
    vector <bool> assignments_tmp(G.nb_nodes, 0);
    vector < C_assignment *> Sol_nodes_tmp;

    for (int i = 0; i < weights.size(); i++)
    {
        if (assignments_tmp[weights[i]->node] == 0)
        {
            assignments_tmp[weights[i]->node] = 1;
            C_assignment * assignment = new(C_assignment);
            assignment->node = weights[i]->node;
            assignment->color = weights[i]->color;
            assignment->weight = weights[i]->weight;

            Sol_nodes_tmp.push_back(assignment);
        }
    }


    // Sort couples (node, color) with respect to weights (Larger to smaller)
    vector<int> sorted_node;
    for (int i = 0; i < G.nb_nodes; i++)
    {
        int maximum = -1 * HUGE_VAL;
        for (int j = 0; j < G.nb_nodes; j++)
            if (Sol_nodes_tmp[j]->weight > maximum && std::find(sorted_node.begin(), sorted_node.end(), j) == sorted_node.end())
                maximum = j;

        sorted_node.push_back(maximum);
    }


    // Deleting one by one each vertex and check if the newsolution remains feasible.
    for (int i = 0; i < G.nb_nodes; i++)
    {
        assignments_tmp[sorted_node[i]] = 0;

        if (!is_dominated(G, sorted_node[i], Sol_nodes_tmp, assignments_tmp))
            assignments_tmp[sorted_node[i]] = 1;

        for (int j = 0; j < G.nodesVec[sorted_node[i]]->neighbors.size(); j++)
            if (!is_dominated(G, G.nodesVec[sorted_node[i]]->neighbors[j], Sol_nodes_tmp, assignments_tmp))
            {
                assignments_tmp[sorted_node[i]] = 1;
                break;
            }
    }


    // Computing the objectif value and recuperation of the solution
    int val = 0;
    for (int i = 0; i < G.nb_nodes; i++)
        if (assignments_tmp[i] == 1)
        {
            Sol_nodes.push_back(Sol_nodes_tmp[i]);

            val += Sol_nodes_tmp[i]->weight;
        }


    // Print The results
    cout << " Objectif Value = " << val << endl;
    cout << " Number of vertices in the dominant = " << Sol_nodes.size() << "/" << G.nb_nodes << endl;

    for (int i = 0; i < Sol_nodes.size(); i++)
    {
        cout << " Node " << Sol_nodes[i]->node << " with color " << Sol_nodes[i]->color << endl;
    }


    // Deleting the used vectors
    Sol_nodes_tmp.clear();
    assignments_tmp.clear();
    return val;
}

bool is_dominated(C_graph &G, int node, vector < C_assignment *> &Sol_nodes_tmp, vector < bool> &assignments_tmp)
{
    if (assignments_tmp[node] == 1)
        return true;

    for (int i = 0; i < G.nodesVec[node]->neighbors.size(); i++)
    {
        if (assignments_tmp[G.nodesVec[node]->neighbors[i]] == 1 && std::find(G.nodesVec[node]->colors.begin(), G.nodesVec[node]->colors.end(),
                Sol_nodes_tmp[G.nodesVec[node]->neighbors[i]]->color) != G.nodesVec[node]->colors.end())
            return true;
    }
    return false;
}