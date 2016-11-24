#include <cstdlib>
#include "Graph.h"
#include "Heuristic.h"
#include "GlobalVariables.h"
#include "CDSMip.h"
#include "Knapsack.h"

using namespace std;

void allCombinations(int _groundSet, int _nelements) {

}

void completeColorFullGraph(C_graph* _g, int _nnodes, int _ncolors) {

}

int main(int argc, char** argv) {
    int seed = 0;
    C_graph g;
    srand(seed);
    g.random_graph(7000, 0.03, 10, 100); // Generating a random graph

    // g.affiche(); // Printing the details of the graph

    // Data for the heuristic ( The heuristic works for any weight vector in the parameter)
    vector < C_assignment *> Solution_nodes;
    vector < C_assignment *> weights; // example of a weight vector

    for (int i = 0; i < g.nb_nodes; i++)
        for (int j = 0; j < g.nodesVec[i]->nb_colors; j++) {
            C_assignment * assignment = new(C_assignment);
            assignment->node = i;
            assignment->color = g.nodesVec[i]->colors[j];
            assignment->weight = g.nodeColorWeight[make_pair(assignment->node, assignment->color)];
            weights.push_back(assignment);
        }

   /* std::cout << std::endl << "------- ";
    std::cout << "Summary on the nodes colors";
    std::cout << std::endl;
    for (auto& v : g.nodesVec)
    {
            std::cout << "n" << v->num << ": " << v->colors.size() << " colors" << std::endl;
    }
    std::cout << std::endl;
  */
   //Start the heuristic
   heuristic(g, Solution_nodes, weights);

    // Solving problem to optimality with CPLEX
 /*  DominationForm mipForm(&g);
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

*/

    /**********************Knapsack Section ******************/
   /*  vector < int> weights_;
    vector < int> profits_;
    vector <int> Knapsack_set;
    int Capacity = 0;

    for (int i = 0; i < g.nb_nodes; i++)
        for (int j = 0; j < g.nodesVec[i]->nb_colors; j++) {
            weights_.push_back((rand() % 100) + 1);
            profits_.push_back((rand() % 100) + 1);
            Capacity++;
        }
    //for (int i = 0 ; i < weights_.size() ; i++)
    //    cout<< weights_[i] << " ";
    //cout<<endl;

    cout << "The capacity of the Knapsack is " << Capacity << endl;
   cout << "The value of the Knapsack is " << Solve_Knapsack(Capacity, weights_, profits_, Knapsack_set) << endl;
    cout << "The Knapsack has the following items :";
    for (int i = 0; i < Knapsack_set.size(); i++)
        cout << Knapsack_set[i] << "   ";

    
    cout << "The value of the Knapsack is " << Solve_Lower_Knapsack(Capacity, weights_, profits_, Knapsack_set) << endl;
    cout << "The Knapsack does not contain the following items :";
    for (int i = 0; i < Knapsack_set.size(); i++)
        cout << Knapsack_set[i] << "   ";*/
    /******************************************************/

    return 0;
}

