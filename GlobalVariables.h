#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include <cmath>

class C_assignment{
public:
    int node; // Node
    int color; // Color
    double weight;
	double ratio; //weight divided by the vertex degree

	bool operator< (const C_assignment & _assign) const
	{
		if (fabs (ratio - _assign.ratio) > 1e-6)
			return ratio < _assign.ratio;

		if (weight != _assign.weight)
			return weight < _assign.weight;

		return node < _assign.node;
	}

    C_assignment();
    ~C_assignment();
};

#endif 