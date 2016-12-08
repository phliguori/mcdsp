/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "Knapsack.h"


// Function for solving the Knapsack problem with Upper capacity constraint
int Solve_Knapsack(int _Capacity, vector < int > & weights, vector < int > & profits, vector < int > & Set_items) {
    // Initializing the Matrix
    vector <vector <int > > matrix;

    for (int i = 0; i <= weights.size(); i++) {
        vector <int> tmpVec(_Capacity + 1, 0);
        matrix.push_back(tmpVec);
        tmpVec.clear();
    }

    for (int i = 1; i <= weights.size(); i++) {
        for (int w = 0; w <= _Capacity; w++) {
            if (weights[i - 1] <= w) {
                if ((profits[i - 1] + matrix[i - 1][w - weights[i - 1]]) > matrix[i - 1][w]) {
                    matrix[i][w] = profits[i - 1] + matrix[i - 1][w - weights[i - 1]];
                } else {
                    matrix[i][w] = matrix[i - 1][w];
                }

            } else {
                matrix[i][w] = matrix[i - 1][w];
            }
        }

    }

    int optimal_value = matrix[weights.size() ][_Capacity ];

    int i = weights.size();
    int k = _Capacity;

    while (i > 0 && k > 0) {
        if (matrix[i][k] != matrix[i - 1][k]) {
            Set_items.push_back(i);
            i = i - 1;
            k = k - weights[i - 1];
        } else {
            i = i - 1;
        }
    }

    for (int i = 0; i <= weights.size(); i++)
        matrix[i].clear();

    return optimal_value;
}


// Function for solving the Knapsack problem with Lower capacity constraint
int Solve_Lower_Knapsack(int __Capacity, vector < int > & weights, vector < int > & profits, vector < int > & Set_items) {
    // Initializing the Matrix
    vector <vector <int > > matrix;
    int _Capacity = 0.00;
    int Value_max = 0.00;

    for (int i = 0; i < weights.size(); i++) {
        _Capacity += weights[i];
        Value_max += profits[i];
    }

    _Capacity = _Capacity - __Capacity;

    for (int i = 0; i <= weights.size(); i++) {
        vector <int> tmpVec(_Capacity + 1, 0);
        matrix.push_back(tmpVec);
        tmpVec.clear();
    }

    for (int i = 1; i <= weights.size(); i++) {
        for (int w = 0; w <= _Capacity; w++) {
            if (weights[i - 1] <= w) {
                if ((profits[i - 1] + matrix[i - 1][w - weights[i - 1]]) > matrix[i - 1][w]) {
                    matrix[i][w] = profits[i - 1] + matrix[i - 1][w - weights[i - 1]];
                } else {
                    matrix[i][w] = matrix[i - 1][w];
                }

            } else {
                matrix[i][w] = matrix[i - 1][w];
            }
        }

    }

    int optimal_value = matrix[weights.size() ][_Capacity ];

    int i = weights.size();
    int k = _Capacity;
    vector <int > not_selected(weights.size(), 0);
    
    while (i > 0 && k > 0) {
        if (matrix[i][k] != matrix[i - 1][k]) {
            not_selected[i - 1] = 1;
            i = i - 1;
            k = k - weights[i - 1];
        } else {
            i = i - 1;
        }
    }
     
    for (int j = 0; j < weights.size(); j++)
        if (not_selected[j] == 0)
            Set_items.push_back(j);

    for (int j = 0; j <= weights.size(); j++)
        matrix[j].clear();

    return Value_max -optimal_value;
}