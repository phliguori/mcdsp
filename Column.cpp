#include "Column.h"

Column::Column()
{
	type = UNKNOWN;
	idx = -1;
	objCoef = 0.;
	lb = 0.;
	ub = 0.;
	value = 0;

	return;
}

Column::Column(COLTYPE _type, double _objcoef, double _lb, double _ub)
	: type(_type), objCoef(_objcoef), lb(_lb), ub(_ub)
{
	idx = -1;
	value = 0.;

	return;
}

Column::~Column()
{
	return;
}

std::string columnToString(Column::COLTYPE _type)
{
	std::string msg;
	switch (_type)
	{
	case Column::CONTINUOUS:
		msg += "Column::CONTINUOUS";
		break;
	case Column::INTEGRAL:
		msg += "Column::INTEGRAL";
		break;
	case Column::BINARY:
		msg += "Column::BINARY";
		break;
	default:
		msg += "Column::UNKNOWN";
	}

	return msg;
}
