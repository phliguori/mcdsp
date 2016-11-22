#ifndef CONSTRAINT_H
#define CONSTRAINT_H

//#include "Util.h"
#include "Row.h"
#include "Graph.h"

#include <set>
#include <map>
#include <string>
#include <locale>

class Constraint : public Row
{
public:
	typedef enum
	{
		C_DOMINATION = 100,
		C_MAX_COLORS = 101,
		C_UNKNOWN = 999
	} CONTYPE;

	class Comparator
	{
	public:
		bool operator()(const Constraint & _left, const Constraint & _right) const
		{
			if (_left.type != _right.type)
				return _left.type < _right.type;
			
			if (_left.node != _right.node)
				return _left.node->num < _right.node->num;

			return _left.color->num < _right.color->num;
		}
	};

	// Constructors & destructors
	Constraint(void);
	Constraint(int _maxnnz, ROWSENSE _sense, double _rhs);
	Constraint(const Constraint & _constraint);
	virtual ~Constraint(void);

	// Getters
	int getType(void) const { return type; }
	C_node* getNode(void) const { return node; }
	C_color* getColor(void) const { return color; }

	// Setters
	void setType(CONTYPE _type) { type = _type; }
	void setNode(C_node* _node) { node = _node; }
	void setColor(C_color* _color) { color = _color; }

	// Util
	std::string toString(void) const;
	void addToSet(int vertex_id) { vertexSet.insert(vertex_id); }

private:
	static int consCounter;

	int id;
	CONTYPE type;
	C_node* node;
	C_color* color;

	std::set<int> vertexSet;
	long int hashValue;
};

long hashFunc(const Constraint & _constraint);

typedef std::map<Constraint, int, Constraint::Comparator> ConstraintHash;

#endif