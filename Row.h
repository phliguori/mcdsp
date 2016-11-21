#ifndef ROW_H
#define ROW_H

#include<string>

class Row {
public:
	typedef enum {
		LESS = 2001,
		EQUAL,
		GREATER,
		UNKNOWN
	} ROWSENSE;

	Row(int _maxnnz = 0, ROWSENSE _sense = Row::UNKNOWN, double _rhs = 0., int _indicator = -1, bool _complemented=false);

	Row(const Row & _row);

	virtual ~Row();

	Row operator=(const Row & _row);

	virtual std::string toString(void) const;

	ROWSENSE getRowSense() const{ return sense; }
	void setRowSense(ROWSENSE _sense) { sense = _sense; }

	int getRowIdx() const { return idx; }
	void setRowIdx(int _idx) { idx = _idx; }

	int getRowNnz() const { return nnz; }
	void setRowNnz(int _nnz) { nnz = _nnz; }

	int getRowMaxNnz() const { return maxnnz; }
	void setRowMaxNnz(int _maxnnz) { maxnnz = _maxnnz; }

	double getRowRhs() const { return rhs; }
	void setRowRhs(double _rhs) { rhs = _rhs; }

	bool isRowComplemented() const { return complemented; }

	int* getRowMatInd() const { return matind; }

	double* getRowMatVal() const { return matval; }

	void rowAddVar(int idx, double val);
protected:

	// Row sense
	ROWSENSE sense;

	// Number of nonzero values.
	int nnz;
	
	// Maximum number of nonzero values.
	int maxnnz;

	// Right hand side value.
	double rhs;

	// Array of matrix coefficient indices.
	int* matind;

	// Array of matrix coefficient values.
	double* matval;

	//Row index
	int idx;

	// The complemented flag.
	bool complemented;
	
	// This routine is used to initialize the row allocating the necessary arrays.
	// @param _max_nnz Maximum number of nonzero elements.
	void init(int _max_nnz);
};

#endif