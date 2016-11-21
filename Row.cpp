#include "Row.h"
#include "Util.h"

Row::Row(int _maxnnz, ROWSENSE _sense, double _rhs, int _indicator, bool _complemented)
	:sense(_sense), rhs(_rhs), idx(_indicator), complemented(_complemented)
{
	init(_maxnnz);

	return;
}

Row::Row(const Row & _row)
{
	nnz = _row.nnz;
	maxnnz = _row.maxnnz;
	rhs = _row.rhs;
	sense = _row.sense;
	idx = _row.idx;
	complemented = _row.complemented;

	if (_row.matind != NULL)
	{
		matind = new int[maxnnz];
		memcpy(matind, _row.matind, nnz * sizeof(int));
	}
	else
		matind = NULL;

	if (_row.matval != NULL)
	{
		matval = new double[maxnnz];
		memcpy(matval, _row.matval, nnz * sizeof(double));
	}
	else
		matval = NULL;

	return;
}

Row::~Row()
{
	if (matind != NULL)
	{
		delete[] matind;
		matind = NULL;
	}

	if (matval != NULL)
	{
		delete[] matval;
		matval = NULL;
	}

	return;
}

Row Row::operator=(const Row & _row)
{
	if (this != &_row)
	{
		if (_row.maxnnz > maxnnz)
		{
			maxnnz = _row.maxnnz;
			matind = (int *)realloc(matind, maxnnz * sizeof(int));
			matval = (double *)realloc(matval, maxnnz * sizeof(double));
		}

		nnz = _row.nnz;
		rhs = _row.rhs;
		sense = _row.sense;
		idx = _row.idx;
		complemented = _row.complemented;

		memcpy(matind, _row.matind, nnz * sizeof(int));
		memcpy(matval, _row.matval, nnz * sizeof(double));
	}

	return *this;
}

std::string Row::toString(void) const
{
	return std::string();
}

void Row::rowAddVar(int idx, double val)
{
	// Verify if the coeff is already in the list (update in this case)
	for (int i = 0; i < nnz; i++)
	{
		if (matind[i] == idx)
		{
			matval[i] = val;
			return;
		}
	}

	if (nnz >= maxnnz)
	{
		if (maxnnz <= 0)
		{
			int errCode = 301;
			errorMsg(typeid(*this).name(), __func__, "Array out of bounds", errCode);
			exit(errCode);
		}
		else
		{
			int newnnz = 2 * maxnnz;
			matind = (int *) realloc(matind, newnnz * sizeof(int));
			matval = (double *) realloc(matval, newnnz * sizeof(double));
			maxnnz = newnnz;
		}
	}

	matind[nnz] = idx;
	matval[nnz] = val;
	nnz++;

	return;
}

void Row::init(int _max_nnz)
{
	nnz = 0;
	maxnnz = _max_nnz;

	if (_max_nnz > 0)
	{
		matind = new int[maxnnz];
		matval = new double[maxnnz];
	}
	else
	{
		matind = NULL;
		matval = NULL;
	}

	return;
}
