#ifndef _GRAPH_H
#define _GRAPH_H

#include <vector>
#include <ctime>
#include <stdio.h>
#include <cstdlib>
#include <iostream>

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

using namespace::std;
using namespace boost;

class C_color;
class C_node;
class C_edge;
class C_graph;


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

class C_color {
public:
	int num; // color number
	int weight; // color color
	vector<int> nodes_with_this_color; // set of nodes with the current color

	C_color();
	~C_color();
};

class C_edge {
public:
	int num; // edge number
	C_node *end1; //first end of the edge
	C_node *end2; //second end of the edge

	C_edge();
	C_edge(C_node *v1, C_node *v2);
	~C_edge();

	C_node* get_end1();
	C_node* get_end2();
};

class C_node {
public:
	int num; // node number
	int nb_colors;
	vector<int> neighbors;
	vector<int> colors; // Set of colors associated with the node
	C_node();
	~C_node();
};

class C_graph {
public:
	int nb_nodes; // number of nodes
	int nb_edges; // number of edges
	int nb_colors;
	vector<C_color*> colorsVec; // Set of all colors  in the graph
	vector<C_node*> nodesVec; // pointers of nodes
	vector<C_edge*> edgesVec; // pointers of edges
	C_graph();
	~C_graph();

	int get_edge(int n1, int n2); // Get edge from the Id of its nodes
	bool is_edge(int n1, int n2); // Check if there is an edge between two nodes

	void affiche(); // Print the graph
	void random_graph(int nb, double density, int max_colors, int max_weights, int seed = 0); // Generate a random graph
};

#endif



