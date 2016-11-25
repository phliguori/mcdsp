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
    return -1;
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
        cout << colorsVec[i]->num << endl;
    }
    cout << endl;


    cout << endl << "List of nodes :" << endl;
    for (int i = 0; i < nb_nodes; i++)
    {
        for (int c = 0; c < nodesVec[i]->nb_colors; c++)
        {
            cout << "n" << nodesVec[i]->num << ": c= " << nodesVec[i]->colors[c] << " W= " <<
                    nodeColorWeight[make_pair(nodesVec[i]->num, nodesVec[i]->colors[c])] << endl;
        }
    }

    cout << endl;
    cout << endl << "List of edges :" << endl;

    for (int i = 0; i < nb_edges; i++)
    {
        cout << edgesVec[i]->end1->num << " -- ";
        cout << edgesVec[i]->end2->num ;
        cout << endl;
    }

    cout << endl;

}

void random_graph_in_file(int nb, double density, int max_colors, int max_weights, char *inst)
{
    cout << "Generating a random graph in a file." << endl;

    //Initializing of tools
    vertices_size_typeM X(nb);

    // Adding nodes to the graph
    int nb_nodes = nb;

    // Adding edges to the graph with a specific density 
    int nb_edges = 0;

    typedef boost::erdos_renyi_iterator<boost::minstd_rand, graphM> ERGen;
    minstd_rand gen;

    graphM G(ERGen(gen, X, density), ERGen(), X);


    // After adding edges, we make the graph connected
    make_connected(G);


    // Get the edges in our graph structure
    vector <int> Colors_tmp;
    vector < vector <int> > colors_per_nodes(nb_nodes);

    deque < vertex_descriptorM > g_vertices;
    for (vertices_size_typeM s(0); s < num_vertices(G); s++)
        g_vertices.push_back(vertex(s, G));

    edge_iteratorM ui, ui_end;
    deque < vertex_descriptorM >::iterator pos, pos1;
    for (boost::tuples::tie(ui, ui_end) = edges(G); ui != ui_end; ++ui)
    {
        pos = find(g_vertices.begin(), g_vertices.end(), target(*ui, G));
        pos1 = find(g_vertices.begin(), g_vertices.end(), source(*ui, G));
        int node1 = pos - g_vertices.begin();
        int node2 = pos1 - g_vertices.begin();
        // Adding a same color the the nodes of the edge 

        // The following loop permits to select the  a color that was already chosen or the first next color not already chosen
        int l = rand() % max_colors;
        while (std::find(Colors_tmp.begin(), Colors_tmp.end(), l - 1) == Colors_tmp.end() && l > 0)
        {
            l--;
        }

        bool exist_a_comon_color = false;
        for (int i = 0; i < colors_per_nodes[node1].size(); i++)
            if (std::find(colors_per_nodes[node2].begin(), colors_per_nodes[node2].end(),
                    colors_per_nodes[node1][i]) != colors_per_nodes[node2].end())
            {
                exist_a_comon_color = true;
                break;
            }

        int probability_adding_colors = -1;
        if (exist_a_comon_color)
            probability_adding_colors = rand() % 7;

        // If the colors does not already exist in the colors of end1, we add the new color
        if (probability_adding_colors == 2 || !exist_a_comon_color)
        {
            if (std::find(colors_per_nodes[node1].begin(), colors_per_nodes[node1].end(), l) ==
                    colors_per_nodes[node1].end())
            {
                colors_per_nodes[node1].push_back(l);
                Colors_tmp.push_back(l);
            }

            // If the colors does not already exist in the colors of end2, we add the new color
            if (std::find(colors_per_nodes[node2].begin(), colors_per_nodes[node2].end(), l) ==
                    colors_per_nodes[node2].end())
            {
                colors_per_nodes[node2].push_back(l);
                Colors_tmp.push_back(l);
            }

        }

        nb_edges++;

    }
    char *nom_ext = new char[100];
    char *buffer = new char[1050];
    sprintf(nom_ext, inst);

    ofstream myfile;
    myfile.open(inst);

    myfile << "************************************************************\n";
    myfile << "******************Rainbow Coloring instance***************** \n";
    myfile << "************************************************************\n\n\n";
    myfile << "Dimensions\n";

    myfile << nb_nodes << " " << nb_edges << " " << Colors_tmp.size() << "\n";


    myfile << "\n\nNodes\n";
    for (int i = 0; i < nb_nodes; i++)
        for (int c = 0; c < colors_per_nodes[i].size(); c++)
            myfile << i << " " << rand() % max_weights << " " << colors_per_nodes[i][c] << "\n";

    myfile << "\n\nEdges\n";
    for (boost::tuples::tie(ui, ui_end) = edges(G); ui != ui_end; ++ui)
    {
        pos = find(g_vertices.begin(), g_vertices.end(), target(*ui, G));
        pos1 = find(g_vertices.begin(), g_vertices.end(), source(*ui, G));
        int node1 = pos - g_vertices.begin();
        int node2 = pos1 - g_vertices.begin();

        myfile << node1 << " " << node2 << "\n";
    }

    myfile.close();

    delete[] nom_ext;
    delete[] buffer;
}

void C_graph::random_graph(int nb, double density, int max_colors, int max_weights)
{
    cout << "Generating a random graph." << endl;

    //Initializing of tools
    vertices_size_typeM X(nb);

    C_node *ptr_node;
    C_edge *ptr_edge;

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

        bool exist_a_comon_color = false;
        for (int i = 0; i < nodesVec[ptr_edge->end1->num]->colors.size(); i++)
            if (std::find(nodesVec[ptr_edge->end2->num]->colors.begin(), nodesVec[ptr_edge->end2->num]->colors.end(),
                    nodesVec[ptr_edge->end1->num]->colors[i]) != nodesVec[ptr_edge->end2->num]->colors.end())
            {
                exist_a_comon_color = true;
                break;
            }

        int probability_adding_colors = -1;
        if (exist_a_comon_color)
            probability_adding_colors = rand() % 7;

        // If the colors does not already exist in the colors of end1, we add the new color
        if (probability_adding_colors == 2 || !exist_a_comon_color)
        {
            if (std::find(nodesVec[ptr_edge->end1->num]->colors.begin(), nodesVec[ptr_edge->end1->num]->colors.end(), l) == nodesVec[ptr_edge->end1->num]->colors.end())
            {
                nodesVec[ptr_edge->end1->num]->colors.push_back(l);
                nodeColorWeight[make_pair(ptr_edge->end1->num, l)] = rand() % max_weights;
                nodesVec[ptr_edge->end1->num]->nb_colors++;
            }

            // If the colors does not already exist in the colors of end2, we add the new color
            if (std::find(nodesVec[ptr_edge->end2->num]->colors.begin(), nodesVec[ptr_edge->end2->num]->colors.end(), l) == nodesVec[ptr_edge->end2->num]->colors.end())
            {
                nodesVec[ptr_edge->end2->num]->colors.push_back(l);
                nodeColorWeight[make_pair(ptr_edge->end2->num, l)] = rand() % max_weights;
                nodesVec[ptr_edge->end2->num]->nb_colors++;
            }
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
    C_color * ptr_color;
    for (int i = 0; i < nb_colors; i++)
    {
        ptr_color = new(C_color);
        ptr_color->num = i;
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

void C_graph::read_graph(char *nom_ext)
{
    cout << "Reading graph." << endl;

    char *buffer = new char[1050];

    C_node *ptr_node;
    C_edge *ptr_edge;

    int i, j, k;

    ifstream fic(nom_ext);

    fic>>buffer;
    while (strcmp(buffer, "Dimensions") != 0)
        fic >> buffer;

    fic>>nb_nodes;
    fic>>nb_edges;
    fic>>nb_colors;

    while (strcmp(buffer, "Nodes") != 0)
        fic >> buffer;

    for (int i = 0; i < nb_nodes; i++)
    {
        ptr_node = new(C_node);
        nodesVec.push_back(ptr_node);
    }

    for (int i = 0; i < nb_colors; i++)
    {
        C_color * ptr_color;
        ptr_color = new(C_color);
        ptr_color->num = i;
        colorsVec .push_back(ptr_color);
    }

    int tmp, tmp1, tmp2;
    while (strcmp(buffer, "Edges") != 0)
    {
        fic >> buffer;
        if (strcmp(buffer, "Edges") != 0)
        {
            tmp = atoi(buffer);
            fic >> tmp1;
            fic >> tmp2;
            nodesVec[tmp]->num = tmp;
            nodesVec[tmp]->colors.push_back(tmp2);
            colorsVec[tmp]->nodes_with_this_color.push_back(tmp);
            nodeColorWeight[make_pair(tmp, tmp2)] = tmp1;
        }
    }


    for (int i = 0; i < nb_nodes; i++)
    {
        nodesVec[i]->nb_colors = nodesVec[i]->colors.size();
    }

    for (int i = 0; i < nb_edges; i++)
    {
        ptr_edge = new(C_edge);

        ptr_edge->num = i;

        fic>>buffer;
        ptr_edge->end1 = nodesVec[atoi(buffer)];

        fic>>buffer;
        ptr_edge->end2 = nodesVec[atoi(buffer)];

        nodesVec[ptr_edge->end1->num]->neighbors.push_back(ptr_edge->end2->num);
        nodesVec[ptr_edge->end2->num]->neighbors.push_back(ptr_edge->end1->num);
        edgesVec.push_back(ptr_edge);
    }


}
