/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   knapsack.h
 * Author: joe
 *
 * Created on 23 novembre 2016, 20:28
 */
#include <iostream>
#include <vector>
#include <stdio.h>

#ifndef KNAPSACK_H
#define KNAPSACK_H


using namespace std;

int Solve_Knapsack(int _Capacity, vector < int > & weights, vector < int > & profits, vector < int > & Set_items);
int Solve_Lower_Knapsack(int _Capacity, vector < int > & weights, vector < int > & profits, vector < int > & Set_items);


#endif /* KNAPSACK_H */

