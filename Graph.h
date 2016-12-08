#ifndef _GRAPH_H
#define _GRAPH_H

#include <iostream>
#include <vector>
#include <map>
#include <stdio.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/make_connected.hpp>
#include <boost/graph/make_biconnected_planar.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/graph/random.hpp>
#include <boost/graph/erdos_renyi_generator.hpp>
#include <boost/graph/small_world_generator.hpp>
#include <boost/graph/plod_generator.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/random/linear_congruential.hpp>
using namespace boost;

typedef adjacency_list
< setS,
	vecS,
	undirectedS,
	property <vertex_index_t, int, property <vertex_degree_t, int > >,
	property <edge_index_t, int, property <edge_color_t, int > >
>
graphM;

typedef boost::graph_traits<graphM> graph_traitsM;

typedef graph_traitsM::vertex_descriptor vertex_descriptorM;
typedef graph_traitsM::vertices_size_type vertices_size_typeM;
typedef graph_traitsM::vertex_iterator vertex_iteratorM;
typedef graph_traitsM::edge_descriptor edge_descriptorM;
typedef graph_traitsM::edges_size_type edges_size_typeM;
typedef graph_traitsM::edge_iterator edge_iteratorM;

using namespace std;

class C_color;
class C_node;
class C_edge;
class C_graph;


class C_color {
public:
	int num; // color number
	vector <int> nodes_with_this_color; // set of nodes with the current color

	C_color();
	~C_color();
};


class C_node {
public:
	int num; // node number
	int nb_colors;
	vector<int> neighbors;
	vector<int> colors;		// Set of colors associated with the node
	vector<double> weights; // Set of weights associated with the each color -> colors.size() == weights.size()
	C_node();
	~C_node();
};


class C_edge {
public:
	bool operator() (const C_edge * _lPtr, const C_edge * _rPtr) const
	{
		if (_lPtr != NULL && _rPtr != NULL)
		{
			if (_lPtr->end1->num != _rPtr->end1->num)
				return _lPtr->end1->num < _rPtr->end1->num;

			return _lPtr->end2->num < _rPtr->end2->num;
		}

		return false;
	}

	int num; // edge number
	C_node *end1; //first end of the edge
	C_node *end2; //second end of the edge

	C_edge();
	C_edge(C_node *v1, C_node *v2);
	~C_edge();

	C_node* get_end1();
	C_node* get_end2();
};


class C_graph {
public:
	int nb_nodes; // number of nodes
	int nb_edges; // number of edges
	int nb_colors;
	vector<C_color*> colorsVec; // Set of all colors  in the graph
	vector<C_node*> nodesVec; // pointers of nodes
	vector<C_edge*> edgesVec; // pointers of edges
	
	map<pair<int, int>, double> nodeColorWeight;

	C_graph();
	~C_graph();

	int get_edge(int n1, int n2); // Get edge from the Id of its nodes
	bool is_edge(int n1, int n2); // Check if there is an edge between two nodes

	void affiche(); // Print the graph
	void read_graph(char *nom_ext);
	void random_graph(int nb, double density, int max_colors, int max_weights); // Generate a random graph
	void random_graph_in_file(int nb, double density, int max_colors, int max_weights, char *inst);
};

#endif