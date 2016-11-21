#ifndef VARIABLE_H
#define VARIABLE_H

#include "Column.h"
#include "Graph.h"

#include <map>

class Variable : public Column
{
public:
	typedef enum type
	{
		V_X = 10,
		V_UNKNOWN = 50
	} VARTYPE;

	class Comparator
	{
	public:
		bool operator()(const Variable & _left, const Variable & _right) const
		{
			return (_left < _right);
		}
	};

	class PtrComparator
	{
	public:
		bool operator()(const Variable* _left, const Variable* _right) const
		{
			Variable::Comparator compare;
			return compare(*_left, *_right);
		}
	};

	bool operator< (const Variable & _right) const
	{
		if (this->type != _right.type)
			return (this->type < _right.type);

		if (this->node != _right.node)
			return (this->node->num < _right.node->num);

		return (this->color->num < _right.color->num);
	}

	bool operator!= (const Variable & _right) const
	{
		return (*this < _right) || (_right < *this);
	}

	bool operator== (const Variable & _right) const
	{
		return !(*this != _right);
	}

	bool operator> (const Variable & _right) const
	{
		if (*this != _right)
			return _right < *this;

		return false;
	}

	// Constructors & destructors
	Variable(void);
	Variable(COLTYPE _type, double _objcoef, double _lb, double _ub);
	virtual ~Variable(void);

	// Printers
	std::string toString(void) const;

	// Getters
	VARTYPE getType(void) const { return type; }
	C_node* getNode(void) const { return node; }
	C_color* getColor(void) const { return color; }

	// Setters
	void setType(VARTYPE _type) { type = _type; }
	void setNode(C_node* _node) { node = _node; }
	void setColor(C_color* _color) { color = _color; }

protected:
	static int varCounter;

	int id;
	VARTYPE type;

	C_node* node;
	C_color* color;
};

typedef std::map<Variable, int, Variable::Comparator> VariableHash;
typedef std::map<Variable*, int, Variable::PtrComparator> PtrVariableHash;
typedef std::map<Variable::VARTYPE, VariableHash> TypeVariableHash;
typedef std::map<Variable::VARTYPE, VariableHash>* TypeVariableHashPtr;

#endif