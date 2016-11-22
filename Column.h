#ifndef COLUMN_H
#define COLUMN_H

#include<string>

class Column
{
public:
	typedef enum {
		INTEGRAL = 1001,
		BINARY,
		CONTINUOUS,
		UNKNOWN
	} COLTYPE;

	// Constructors & destructors
	Column();
	Column(COLTYPE _type, double _objcoef, double _lb, double _ub);
	virtual ~Column();

	// Printers
	virtual std::string toString(void) const = 0;

	// Getters
	int getColIdx() const { return idx; }
	double getValue() const { return value; }
	double getObjCoef() const { return objCoef; }
	double getColLb() const { return lb; }
	double getColUb() const { return ub; }
	COLTYPE getColType() const { return type; }

	// Setters
	void setColIdx(int _idx) { idx = _idx; }
	void setValue(double _value) { value = _value; }
	void setObjCoeff(double _objcoef) { objCoef = _objcoef; }
	void setColLb(double _lb) { lb = _lb; }
	void setColUb(double _ub) { ub = _ub; }
	void setColType(COLTYPE _type) { lb = _type; }

private:

	COLTYPE type;
	int idx;
	double objCoef;
	double lb;
	double ub;
	double value;
};

std::string columnToString(Column::COLTYPE _type);

#endif