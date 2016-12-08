#ifndef LAGRANGE_H
#define LAGRANGE_H

#include "Graph.h"

#include <set>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include "GlobalVariables.h"

class Lagrangian {
public:

    typedef enum LAG_STATUS {
        LAG_OPTIMAL = 100,
        LAG_FEAS_MAX_ITER = 200,
        LAG_FEAS_SMALL_STEPSIZE = 300,
        LAG_INFEAS = 400,
        LAG_UNKNOWN = 999
    } LAGRANGIAN_STATUS;

    Lagrangian(C_graph * _g, int _maxIterLag = 2000, int _maxIterNoLBImprov = 125, double _alpha = 2, double _eps = 1e-4);
    ~Lagrangian();

    //int runSubGradientMethod();
    LAGRANGIAN_STATUS runSubGradientMethod(list<pair<int, int>> &_lagFixationToOne, list<pair<int, int>> &_lagFixationToZero, double _bestKnownUB = INFINITY);

    bool isVarFixedToOne(int _v) const {
        return fixedToOneVars[_v] != -1;
    }

    bool isVarFixedToZero(int _v, int _c) const {
        return isVarFixedToOne(_v) || fixedToZeroVars[_v][_c];
    }

    bool isConsInactive(int _v) const {
        return inactiveCons[_v] > 0;
    }

    void tryToFixVariables(list<pair<int, int>> &_varsFixedToOne, list<pair<int, int>> &_varsFixedToZero);
    void tryToFixVarsToZero(list<pair<int, int>> &_varsFixedToOne, list<pair<int, int>> &_varsFixedToZero);
    void tryToFixVarsToOne(list<pair<int, int>> &_varsFixedToOne, list<pair<int, int>> &_varsFixedToZero);
    void fixVarToZero(int _v, int _c);
    void fixVarToOne(int _v, int _c);

    void unfixVars(list<pair<int, int>> &nodeVarsFixedToOne, list<pair<int, int>> &nodeVarsFixedToZero);
    inline void unfixVarToZero(int _v, int _c);
    inline void unfixVarToOne(int _v, int _c);

    inline double getGap() const;

    double getBestUb() const {
        return bub;
    }

    double getBestLb() const {
        return blb;
    }

    void getXSol(vector<int> & _destination) {
        copy(bestXSol.begin(), bestXSol.end(), _destination.begin());
    }

    double getRatio(int _v, int _c);

    bool hasColor(int _v, int _c) {
        return nodeColorDominance[_v][_c];
    }

    //LAGRANGIAN_STATUS getStatus() const { return status; }
    void printLagrangianCosts() const;
    void printXSol() const;
    void printSubgradient() const;
    void printLagMultiplier() const;
    void printStatus(LAGRANGIAN_STATUS _status) const;
    string statusToString(LAGRANGIAN_STATUS _status) const;

protected:
    void updateLagragianCosts();
    void updateLagragianMultipliers(double _theta);
    void solveLagragianDualSubproblem();
    void calculateSubGradient();
    double calculateStepSize(int & _nbConsecIterNoLbImprov);
    void calculateLagrangianLB();
    double lagrangianHeuristic(double _rclParam,double _sizeList) ;
    double tryPrimalFeasibility();
    void updateStatus(LAGRANGIAN_STATUS & _status);
    void initialize(double _bestKnownUB);
    double Heuristic();
    bool is_dominated(int node, vector < C_assignment *> &Sol_nodes_tmp, vector < bool> &assignments_tmp);
    void localSearch(int  *_Solution);
    int solveSacaDos();
    int change_color_node(int * _Solution);
    int fixedVarsCounter;
    int totalVarsCounter;
    int iterCounter;
    int maxIterLag;
    int noLBImprovCounter;
    int maxIterNoLBImprov;
    double eps;
    double alpha, alphaInit;
    double stepSize;
    double lb, ub; // Lower and upper bounds
    double blb, bub; // Best lower and upper bounds
    

    clock_t start, end;

    set<pair<int, int>> interditedVars;

    vector<int> xSolDualLag;
    vector<int> heuristicSolution, bestXSol;

    vector<int> fixedToOneVars;
    vector<vector<bool>> fixedToZeroVars;

    vector<int> inactiveCons;

    vector<double> lMultipliers;
    vector<double> subgradients;
    vector<vector<double>> lCosts;
    vector<vector<vector<int>>> vColorNeighborhood;
    vector<vector<int>> vUtilColorNeighborhoodSize;
    vector<vector<bool>> nodeColorDominance;

    C_graph* g;
};

#endif