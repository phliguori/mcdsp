#include "Graph.h"

/****************************************************************/
/********************* C_Color class **************************/
/****************************************************************/
C_color::C_color()
{
}

C_color::~C_color()
{
}



/****************************************************************/
/********************* C_edge class **************************/
/****************************************************************/
C_edge::C_edge()
{
}

C_edge::C_edge(C_node *v1, C_node *v2)
{
	end1 = v1;
	end2 = v2;
}

C_edge::~C_edge()
{
}

C_node*
C_edge::get_end1()
{
	return end1;
}

C_node*
C_edge::get_end2()
{
	return end2;
}

/****************************************************************/
/********************* C_node class **************************/

/****************************************************************/

C_node::C_node()
{
	num = 0;
	nb_colors = 0;
}

C_node::~C_node()
{
}


/****************************************************************/
/********************** C_graph class **************************/

/**************************************************************/

C_graph::C_graph()
{
	nb_nodes = 0;
	nb_edges = 0;
	nb_colors = 0;
}

C_graph::~C_graph()
{

}

bool C_graph::is_edge(int n1, int n2)
{
	if (std::find(nodesVec[n1]->neighbors.begin(), nodesVec[n1]->neighbors.end(), n2) != nodesVec[n1]->neighbors.end())
		return true;
	else return false;
}

int C_graph::get_edge(int n1, int n2)
{
	int i = 0;
	if (is_edge(n1, n2) == false || n1 > nb_nodes || n2 > nb_nodes) return -1;

	for (i = 0; i < nb_edges; i++)
		if ((edgesVec[i]->end1->num == n1 && edgesVec[i]->end2->num == n2) || (edgesVec[i]->end1->num == n2 && edgesVec[i]->end2->num == n1))
			return i;
}

void C_graph::affiche()
{
	cout << "Graph G :" << endl << endl;

	cout << nb_nodes << " nodes" << endl;
	cout << nb_edges << " edges" << endl;
	cout << nb_colors << " colors" << endl << endl;

	cout << endl << "List of colors :" << endl;
	for (int i = 0; i < nb_colors; i++)
	{
		cout << colorsVec[i]->num << " :  with weight " << colorsVec[i]->weight << endl;
	}
	cout << endl;


	cout << endl << "List of nodes :" << endl;
	for (int i = 0; i < nb_nodes; i++)
	{
		cout << nodesVec[i]->num << ":";

		for (int j = 0; j < nodesVec[i]->nb_colors; j++)
		{
			cout << "  " << nodesVec[i]->colors[j];
		}

		cout << endl;
	}
	cout << endl;
	cout << endl << "List of edges :" << endl;

	for (int i = 0; i < nb_edges; i++)
	{
		cout << edgesVec[i]->num;
		cout << " : " << edgesVec[i]->end1->num << " -- ";
		cout << edgesVec[i]->end2->num << "    ";
		cout << endl;
	}

	cout << endl;

}

void C_graph::random_graph(int nb, double density, int max_colors, int max_weights, int seed)
{
	cout << "Generating a random graph." << endl;

	//Initializing of tools
	vertices_size_typeM X(nb);

	C_node *ptr_node;
	C_edge *ptr_edge;
	C_color *ptr_color;
	srand(seed);

	// Adding nodes to the graph
	nb_nodes = nb;

	nodesVec.resize(nb_nodes);

	for (int i = 0; i < nb_nodes; i++)
	{
		ptr_node = new(C_node);
		ptr_node->num = i;
		nodesVec[i] = ptr_node;
	}

	// Adding edges to the graph with a specific density 
	nb_edges = 0;

	typedef boost::erdos_renyi_iterator<boost::minstd_rand, graphM> ERGen;
	minstd_rand gen;

	graphM G(ERGen(gen, X, density), ERGen(), X);


	// After adding edges, we make the graph connected
	make_connected(G);


	// Get the edges in our graph structure
	vector <int> Colors_tmp;
	deque < vertex_descriptorM > g_vertices;
	for (vertices_size_typeM s(0); s < num_vertices(G); s++)
		g_vertices.push_back(vertex(s, G));

	edge_iteratorM ui, ui_end;
	deque < vertex_descriptorM >::iterator pos, pos1;
	for (boost::tuples::tie(ui, ui_end) = edges(G); ui != ui_end; ++ui)
	{
		pos = find(g_vertices.begin(), g_vertices.end(), target(*ui, G));
		pos1 = find(g_vertices.begin(), g_vertices.end(), source(*ui, G));
		ptr_edge = new(C_edge);
		ptr_edge->num = nb_edges;
		ptr_edge->end1 = nodesVec[pos - g_vertices.begin()];
		ptr_edge->end2 = nodesVec[pos1 - g_vertices.begin()];
		nodesVec[ptr_edge->end1->num]->neighbors.push_back(ptr_edge->end2->num);
		nodesVec[ptr_edge->end2->num]->neighbors.push_back(ptr_edge->end1->num);


		// Adding a same color the the nodes of the edge 
		int l = rand() % max_colors; // Select one color
		while (std::find(Colors_tmp.begin(), Colors_tmp.end(), l - 1) == Colors_tmp.end() && l > 0)
		{
			l--;
		}

		// If the colors does not already exist in the colors of end1, we add the new color
		if (std::find(nodesVec[ptr_edge->end1->num]->colors.begin(), nodesVec[ptr_edge->end1->num]->colors.end(), l) == nodesVec[ptr_edge->end1->num]->colors.end())
		{
			nodesVec[ptr_edge->end1->num]->colors.push_back(l);
			nodesVec[ptr_edge->end1->num]->nb_colors++;
		}

		// If the colors does not already exist in the colors of end2, we add the new color
		if (std::find(nodesVec[ptr_edge->end2->num]->colors.begin(), nodesVec[ptr_edge->end2->num]->colors.end(), l) == nodesVec[ptr_edge->end2->num]->colors.end())
		{
			nodesVec[ptr_edge->end2->num]->colors.push_back(l);
			nodesVec[ptr_edge->end2->num]->nb_colors++;
		}

		// If the colors does not already exist in the set Colors, we add the new color
		if (std::find(Colors_tmp.begin(), Colors_tmp.end(), l) == Colors_tmp.end())
		{
			Colors_tmp.push_back(l);
			nb_colors++;
		}

		edgesVec.push_back(ptr_edge);
		nb_edges++;
	}

	colorsVec.resize(nb_colors);
	for (int i = 0; i < nb_colors; i++)
	{
		ptr_color = new(C_color);
		ptr_color->num = i;
		ptr_color->weight = rand() % max_weights;
		colorsVec[i] = ptr_color;
	}

	for (int i = 0; i < nb_nodes; i++)
	{
		for (int j = 0; j < nodesVec[i]->colors.size(); j++)
		{
			colorsVec[nodesVec[i]->colors[j]]->nodes_with_this_color.push_back(i);
		}
	}

	Colors_tmp.clear();

}
