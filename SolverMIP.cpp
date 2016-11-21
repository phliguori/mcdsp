#include "SolverMIP.h"

#include <cstdlib>
#include <iostream>

SolverMIP::SolverMIP()
	:env(NULL), lp(NULL), probName(std::string())
{
	env = CPXopenCPLEX(&status);
	if (env == NULL)
	{
		int errCode = 101;
		errorMsg(typeid(*this).name(), __func__, "Cannot open CPLEX enviroment", errCode);
		exit(errCode);
	}

	lp = CPXcreateprob(env, &status, "");
	if (lp == NULL)
	{
		int errCode = 102;
		errorMsg(typeid(*this).name(), __func__, "Cannot creat CPLEX problem", errCode);
		exit(errCode);
	}

	xSol = NULL;
	status = -1;
	executionTime = 0.;
	externalLpRelax = -1.0;
	foSense = FOSENSE::SENSE_UNKNOWN;
	method = METHOD::METHOD_UNKNOWN;
	pType = PROBTYPE::TYPE_UNKNOWN;
	problemCreated = false;

	return;
}

SolverMIP::~SolverMIP()
{
	if (xSol != NULL)
		delete[] xSol;

	status = CPXfreeprob(env, &lp);
	if (status)
	{
		int errCode = 103;
		errorMsg(typeid(*this).name(), __func__, "Cannot free CPLEX problem", errCode);
		exit(errCode);
	}

	status = CPXcloseCPLEX(&env);
	if (status)
	{
		int errCode = 104;
		errorMsg(typeid(*this).name(), __func__, "Cannot close CPLEX environment", errCode);
		exit(errCode);
	}

	lp = NULL;
	env = NULL;
	xSol = NULL;
	status = -1;
	foSense = FOSENSE::SENSE_UNKNOWN;
	method = METHOD::METHOD_UNKNOWN;
	pType = PROBTYPE::TYPE_UNKNOWN;
	problemCreated = false;

	return;
}

SolverMIP::SOLVERSTAT SolverMIP::getStat(void)
{
	SOLVERSTAT solverStat = SOLVERSTAT_ERROR;

	status = CPXgetstat(env, lp);

	if (status == 0)
	{
		int warnCode = 119;
		std::string msg = "CPXgetstat: no solution exists";
		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
	}
	else
	{
		switch (status)
		{
		case CPX_STAT_OPTIMAL:				solverStat = SOLVERSTAT_LPOPTIMAL;     break;
		case CPX_STAT_OPTIMAL_INFEAS:		solverStat = SOLVERSTAT_OPTIMALINFEAS; break;
		case CPX_STAT_INFEASIBLE:			solverStat = SOLVERSTAT_INFEASIBLE;    break;
		case CPX_STAT_UNBOUNDED:			solverStat = SOLVERSTAT_UNBOUNDED;     break;
		case CPXMIP_OPTIMAL:				solverStat = SOLVERSTAT_MIPOPTIMAL;    break;
		case CPXMIP_OPTIMAL_TOL:			solverStat = SOLVERSTAT_MIPOPTIMAL;    break;
		case CPXMIP_INFEASIBLE:				solverStat = SOLVERSTAT_INFEASIBLE;    break;
		case CPXMIP_NODE_LIM_FEAS:			solverStat = SOLVERSTAT_FEASIBLE;      break;
		case CPXMIP_TIME_LIM_FEAS:			solverStat = SOLVERSTAT_FEASIBLE;      break;
		case CPXMIP_FAIL_FEAS:				solverStat = SOLVERSTAT_FEASIBLE;      break;
		case CPXMIP_MEM_LIM_FEAS:			solverStat = SOLVERSTAT_FEASIBLE;      break;
		case CPXMIP_ABORT_FEAS:				solverStat = SOLVERSTAT_FEASIBLE;      break;
		case CPXMIP_FAIL_FEAS_NO_TREE:		solverStat = SOLVERSTAT_FEASIBLE;      break;
		case CPXMIP_SOL_LIM:				solverStat = SOLVERSTAT_FEASIBLE;      break;
		case CPXMIP_NODE_LIM_INFEAS:		solverStat = SOLVERSTAT_NOINTEGER;     break;
		case CPXMIP_TIME_LIM_INFEAS:		solverStat = SOLVERSTAT_NOINTEGER;     break;
		case CPXMIP_FAIL_INFEAS:			solverStat = SOLVERSTAT_NOINTEGER;     break;
		case CPXMIP_MEM_LIM_INFEAS:			solverStat = SOLVERSTAT_NOINTEGER;     break;
		case CPXMIP_ABORT_INFEAS:			solverStat = SOLVERSTAT_NOINTEGER;     break;
		case CPXMIP_FAIL_INFEAS_NO_TREE:	solverStat = SOLVERSTAT_NOINTEGER;     break;
		default:							solverStat = SOLVERSTAT_INFEASIBLE;
		}
	}

	return solverStat;
}

void SolverMIP::setNumThreads(int _nthreads)
{
	status = CPXsetintparam(env, CPX_PARAM_THREADS, _nthreads);
	if (status)
	{
		int warnCode = 128;
		std::string msg = "It was not possible to set limit on the nodes";
		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
	}

	return;
}

void SolverMIP::setNodesLim(int _nNodes)
{
	if (_nNodes >= 0 )
	{
		// * When this parameter is set to 0 (zero), CPLEX completes processing at the root;
		// * that is, it creates cuts and applies heuristics at the root.
		// *
		// * When this parameter is set to 1 (one), it allows branching from the root; 
		// * that is, nodes are created but not solved.
		status = CPXsetintparam(env, CPX_PARAM_NODELIM, _nNodes);
		if (status)
		{
			int warnCode = 128;
			std::string msg = "It was not possible to set limit on the nodes";
			warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
		}
	}
	
	return;
}

void SolverMIP::setTimeLim(double _timeLim)
{
	if (_timeLim > 0)
	{
		// * 0: automatic (cplex choses)
		// * 1: CPU time
		// * 2: wall clock time
		status = CPXsetintparam(env, CPX_PARAM_CLOCKTYPE, 1);
		if (status)
		{
			int warnCode = 128;
			std::string msg = "It was not possible to set CLOCK_TYPE to CPU time";
			warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
		}

		status = CPXsetdblparam(env, CPX_PARAM_TILIM, _timeLim);
		if (status)
		{
			int warnCode = 128;
			std::string msg = "It was not possible to set time limit";
			warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
		}
	}

	return;
}

Column::COLTYPE SolverMIP::choseColType(int _type, double lb, double ub)
{
	Column::COLTYPE ret = Column::COLTYPE::UNKNOWN;

	switch (_type)
	{
	case SolverMIP::PROBTYPE::INTEGER:
		if (fabs(lb - 0.0) < SOLVER_EPS && fabs(ub - 1.0) < SOLVER_EPS)
			ret = Column::COLTYPE::BINARY;
		else
			ret = Column::COLTYPE::INTEGRAL;
		break;
	case SolverMIP::PROBTYPE::CONTINUOUS:
	default:
		ret = Column::COLTYPE::CONTINUOUS;
		break;
	}

	return ret;
}

double SolverMIP::getTimeStamp() const
{
	double timeStamp = 0;
	int status = CPXgettime(env, &timeStamp);
	if (status)
	{
		int warnCode = 128;
		std::string msg = "It was not possible to get the time stamp";
		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);

		timeStamp = 0.;
	}

	return timeStamp;
}

bool SolverMIP::createLP(const char* _problemName, FOSENSE _sense, SolverMIP::PROBTYPE _type)
{
	probName = _problemName;

	status = CPXchgprobname(env, lp, _problemName);
	if (status)
	{
		int errCode = 105;
		warningMsg(typeid(*this).name(), __func__, "Cannot change problem name", errCode);
	}

	pType = _type;
	switch (pType)
	{
	case SolverMIP::PROBTYPE::CONTINUOUS:
		status = CPXchgprobtype(env, lp, CPXPROB_LP);
		break;
	case SolverMIP::PROBTYPE::INTEGER:
		status = CPXchgprobtype(env, lp, CPXPROB_MILP);
		break;
	default:
		status = -1;
		break;
	}

	if (status != 0)
	{
		int errCode = 106;
		errorMsg(typeid(*this).name(), __func__, "Cannot change problem type", errCode);
		exit(errCode);
	}

	foSense = _sense;
	switch (foSense)
	{
	case FOSENSE::MAXIMIZATION:
		status = CPXchgobjsen(env, lp, CPX_MAX);
		break;
	case FOSENSE::MINIMIZATION:
		status = CPXchgobjsen(env, lp, CPX_MIN);
		break;
	default:
		status = -1;
		break;
	}

	if (status)
	{
		int errCode = 108;
		errorMsg(typeid(*this).name(), __func__, "Cannot change problem sense", errCode);
		exit(errCode);
	}

	problemCreated = true;

	return problemCreated;
}

void SolverMIP::loadProblem(const char* _problemName, FOSENSE _sense, SolverMIP::PROBTYPE _type)
{
	if (!problemCreated)
	{
		if (_problemName != NULL)
			probName = _problemName;
		else
			probName = "DefaultProblem";

		createLP(probName.data(), _sense, _type);
	}

	int nVars, nCons;
	
	nVars = createVariables();

	nCons = createConstraints();

#ifndef DEBUG
	writeProbLP(&probName[0]);
#endif


#ifdef VERBOSE
	std::string msg  = probName + " loaded.";
	logMsg(__class__, __func__, msg);
#endif

//#ifdef VERBOSE
//	std::cout << "\n";
//	std::cout << "----- Problem loaded!";
//	std::cout << "\n";
//#endif
	return;
}

void SolverMIP::setSeparationRoutine(void * _cbdata, Separator * _separator)
{
	turnScreenOn();

	if (_separator != NULL)
	{
		status = CPXsetlazyconstraintcallbackfunc(env, _separator->integerSeparationFnc, _cbdata);
		if (status)
		{
			int errCode = 109;
			errorMsg(typeid(*this).name(), __func__, "Could not initialize CPXsetlazyconstraintcallbackfunc", errCode);
			exit(errCode);
		}

		status = CPXsetusercutcallbackfunc(env, _separator->fractionalSeparationFnc, _cbdata);
		if (status)
		{
			int errCode = 110;
			errorMsg(typeid(*this).name(), __func__, "Could not initialize CPXsetusercutcallbackfunc", errCode);
			exit(errCode);
		}
	}

	return;
}

SolverMIP::SOLVERSTAT SolverMIP::solveLRExtentedFormulations()
{
	SolverMIP::SOLVERSTAT ret = SolverMIP::SOLVERSTAT::SOLVERSTAT_UNKNOWN;

	return ret;
}

SolverMIP::SOLVERSTAT SolverMIP::solve(METHOD _method, bool _viz)
{
	if (_viz)
	{
		turnScreenOn();
	}

	double startTime = getTimeStamp();
	switch (_method)
	{
	case METHOD_PRIMAL:
		//status = CPXprimopt(env, lp);
		status = CPXlpopt(env, lp);
		if (status)
		{
			int warnCode = 114;
			std::string msg = "Problem with CPXprimopt";
			warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
		}
		break;
	case METHOD_DUAL:
		status = CPXdualopt(env, lp);
		if (status)
		{
			int warnCode = 115;
			std::string msg = "Problem with CPXdualopt";
			warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
		}
		break;
	case METHOD_MIP:
		status = CPXmipopt(env, lp);
		if (status)
		{
			int warnCode = 116;
			std::string msg = "Problem with CPXmipopt";
			warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
		}
		break;
	default:
		int warnCode = 117;
		std::string msg = "Invalid optimization method";
		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
	}

	double endTime = getTimeStamp();
	executionTime = (endTime - startTime);

	SOLVERSTAT solverStat = getStat();

	if (solverStat == SOLVERSTAT_ERROR)
	{
		int warnCode = 118;
		std::string msg = "No solution exists";
		errorMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
	}

	return solverStat;
}

int SolverMIP::getNCols() const
{
	return 	CPXgetnumcols(env, lp);
}

int SolverMIP::getNRows() const
{
	return 	CPXgetnumrows(env, lp);
}

int SolverMIP::getNUserCuts() const
{
	int nCuts = 0;
	int status = CPXgetnumcuts(env, lp, CPX_CUT_USER, &nCuts);

	if (status)
	{
		int warnCode = 129;
		std::string msg = "Cannot retrieve quantity of user cuts generated.";
		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
	}

	return nCuts;
}

int SolverMIP::getNodeCount() const
{
	return CPXgetnodecnt(env, lp);
}

int SolverMIP::getNodeLeftCount() const
{
	return CPXgetnodeleftcnt(env, lp);
}

double SolverMIP::getMipRelGap() const
{
	double gap = 0.;

	int status = CPXgetmiprelgap(env, lp, &gap);
	if (status)
	{
		int warnCode = 130;
		std::string msg = "Cannot retrieve mip relative gap.";
		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
	}

	return gap;
}

double SolverMIP::getIntegralityGap() const
{
	double gap = 0.;
	if (hasExternalLpRelax())
	{
		if (foSense == MINIMIZATION)
			gap = (getObjVal() - externalLpRelax) / (1e-10 + fabs(externalLpRelax));
		else
			gap = (externalLpRelax - getObjVal()) / (1e-10 + fabs(externalLpRelax));
	}

	return gap;
}

bool SolverMIP::shutDownHeuristics()
{
	bool ret = true;

	status = CPXsetintparam(env, CPXPARAM_MIP_Strategy_HeuristicFreq, -1);
	if (status)
	{
		int warnCode = 125;
		std::string msg = "Cannot shutdown mip heuristics";
		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
		ret = false;
	}

	return ret;
}

bool SolverMIP::shutDownAutomaticCuts()
{
	bool ret = true;

	double defaulDbl, minPreDbl, maxPreDbl;

	status = CPXinfodblparam(env, CPX_PARAM_CUTSFACTOR, &defaulDbl, &minPreDbl, &maxPreDbl);
	if (!status)
	{
		status = CPXsetdblparam(env, CPX_PARAM_CUTSFACTOR, minPreDbl);

		if (status)
		{
			int warnCode = 124;
			std::string msg = "Cannot shutdown automatic cuts";
			warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
			ret = false;
		}
	}

	return ret;
}

bool SolverMIP::shutDownAutomaticPreSolve()
{
	bool ret = true;

	status = CPXsetintparam(env, CPX_PARAM_PRELINEAR, CPX_OFF);
	if (status)
	{
		int warnCode = 125;
		std::string msg = "Cannot shutdown prelinear reduction";
		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
		ret = false;
	}

	status = CPXsetintparam(env, CPX_PARAM_PREIND, CPX_OFF);
	if (status)
	{
		int warnCode = 125;
		std::string msg = "Cannot shutdown automatic presolve";
		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
		ret = false;
	}
	
	return ret;
}

bool SolverMIP::turnScreenOn()
{
	bool ret = true;
	status = CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_ON);

	if (status != 0)
	{
		int warnCode = 126;
		std::string msg = "Cannot turn scree on";
		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
		ret = false;
	}

	return ret;
}

bool SolverMIP::turnScreenOff()
{
	bool ret = true;
	status = CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_OFF);

	if (status != 0)
	{
		int warnCode = 127;
		std::string msg = "Cannot turn scree off";
		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
		ret = false;
	}

	return ret;
}

bool SolverMIP::getX(int _start, int _end)
{
	bool ret = true;

	int nCols = getNCols() - 1;
	if (_start < 0 || _start > nCols)
	{
		int warnCode = 120;
		std::string msg = "_start index out of bound";
		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
	}

	if (_end > nCols)
	{
		int warnCode = 121;
		std::string msg = "_end index out of bound";
		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
	}

	if (_end < 0)
		_end = nCols;

	if (xSol == NULL)
		xSol = new double[getNCols()];

	status = CPXgetx(env, lp, xSol, _start, _end);

	if (status)
	{
		int warnCode = 122;
		std::string msg = "Cannot retrieve solution variable";
		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);
		ret = false;
	}

	return ret;
}

double SolverMIP::getObjVal() const
{
	double obj = 0.;

	int status = CPXgetobjval(env, lp, &obj);

	if (status)
	{
		int warnCode = 123;
		std::string msg = "Cannot retrieve obj value";
		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);

		obj = SOLVER_INF;
	}

	return obj;
}

double SolverMIP::getBestObjVal() const
{
	double bestObj = 0.;
	int status = CPXgetbestobjval(env, lp, &bestObj);

	if (status)
	{
		int warnCode = 123;
		std::string msg = "Cannot retrieve best obj value";
		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);

		bestObj = SOLVER_INF;
	}

	return bestObj;
}

bool SolverMIP::addCol(Column* _col)
{
	char* ctype = NULL;
	std::string aux = _col->toString();
	double objcoef = _col->getObjCoef();
	double lb = _col->getColLb();
	double ub = _col->getColUb();
	int nCols = 1;
	char* cname = NULL;
	bool ret = true;

	cname = &(aux[0]);

	if (pType != SolverMIP::PROBTYPE::CONTINUOUS)
	{
		ctype = new char;

		switch (_col->getColType())
		{
		case Column::COLTYPE::CONTINUOUS:
			*ctype = 'C';
			break;
		case Column::COLTYPE::INTEGRAL:
			*ctype = 'I';
			break;
		case Column::COLTYPE::BINARY:
			*ctype = 'B';
			break;
		default:
			break;
		}
	}

	if (strcmp(cname, "") != 0)
		status = CPXnewcols(env, lp, nCols, &objcoef, &lb, &ub, ctype, &cname);
	else
		status = CPXnewcols(env, lp, nCols, &objcoef, &lb, &ub, ctype, NULL);
	
	if (status)
	{
		int warnCode = 109;
		std::string msg = "Cannot add col ";

		if (cname != NULL) 
			msg += cname;

		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);

		ret = false;
	}

	if (pType != SolverMIP::PROBTYPE::CONTINUOUS)
		delete ctype;

	return ret;
}

bool SolverMIP::addCol(Column::COLTYPE _type, const double _objcoef, const double _lb, const double _ub, const char* _name)
{
	char ctype;
	char* cname = NULL;
	bool ret = true;

	if (_name != NULL)
	{
		if (strcmp(_name, "") != 0)
		{
			cname = (char*) malloc(sizeof _name + 1);
			strcpy_s(cname, sizeof _name, _name);
		}
	}

	switch (_type)
	{
	case Column::COLTYPE::CONTINUOUS:
		ctype = 'C';
		break;
	case Column::COLTYPE::INTEGRAL:
		ctype = 'I';
		break;
	case Column::COLTYPE::BINARY:
		ctype = 'B';
		break;
	default:
		break;
	}

	status = CPXnewcols(env, lp, 1, &_objcoef, &_lb, &_ub, &ctype, &cname);
	if (status)
	{
		int warnCode = 110;
		std::string msg = "Cannot add col ";
		
		if (cname != NULL) 
			msg += cname;
		
		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);

		ret = false;
	}

	free(cname);

	return ret;
}

bool SolverMIP::addRow(Row * _row)
{
	bool ret = true;
	int nRows = 1;
	char sense = 'N';
	char* rname = NULL;
	int rmatbeg = 0;
	int newColsAdded = 0;

	std::string aux = _row->toString();
	int nnz = _row->getRowNnz();
	double rhs = _row->getRowRhs();
	int* rmatind = _row->getRowMatInd();
	double* rmatval = _row->getRowMatVal();
	Row::ROWSENSE rsense = _row->getRowSense();

	rname = &(aux[0]);

	switch (rsense)
	{
	case Row::LESS:		sense = 'L'; break;
	case Row::EQUAL:	sense = 'E'; break;
	case Row::GREATER:	sense = 'G'; break;
	default:			sense = 'L';
	}

	if (strcmp(rname, "") != 0)
		status = CPXaddrows(env, lp, newColsAdded, nRows, nnz, &rhs, &sense, &rmatbeg, rmatind, rmatval, NULL, &rname);
	else
		status = CPXaddrows(env, lp, newColsAdded, nRows, nnz, &rhs, &sense, &rmatbeg, rmatind, rmatval, NULL, NULL);

	if (status)
	{
		int warnCode = 111;
		std::string msg = "Cannot add row ";

		if (rname != NULL)
			msg += rname;

		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);

		ret = false;
	}

	return ret;
}

bool SolverMIP::addRow(Row::ROWSENSE _sense, int _maxnnz, double _rhs, const char* _name)
{
	bool ret = true;
	int nRows = 1;
	char sense = 'N';
	char* rname = NULL;
	int rmatbeg = 0;
	int newColsAdded = 0;

	int nnz = _maxnnz;
	double rhs = _rhs;
	int* rmatind = NULL;
	double* rmatval = NULL;

	if (_name != NULL)
	{
		if (strcmp(_name, "") != 0)
		{
			rname = (char*) malloc(sizeof _name + 1);
			strcpy_s(rname, sizeof _name, _name);
		}
	}

	switch (_sense)
	{
	case Row::LESS:		sense = 'L'; break;
	case Row::EQUAL:	sense = 'E'; break;
	case Row::GREATER:	sense = 'G'; break;
	default:			sense = 'L';
	}

	if (strcmp(rname, "") != 0)
		status = CPXaddrows(env, lp, newColsAdded, nRows, nnz, &rhs, &sense, &rmatbeg, rmatind, rmatval, NULL, &rname);
	else
		status = CPXaddrows(env, lp, newColsAdded, nRows, nnz, &rhs, &sense, &rmatbeg, rmatind, rmatval, NULL, NULL);

	if (status)
	{
		int warnCode = 112;
		std::string msg = "Cannot add row ";

		if (rname != NULL) 
			msg += rname;

		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);

		ret = false;
	}

	free(rname);

	return ret;
}

bool SolverMIP::writeProbLP(const char * _lpName)
{
	bool  ret = true;
	std::string fileName;

	if (_lpName == NULL)
		fileName = "prob";
	else
		fileName = _lpName;

	fileName += ".lp";

	status = CPXwriteprob(env, lp, &fileName[0], NULL);

	if (status)
	{
		int warnCode = 113;
		std::string msg = "Cannot write lp";
		msg += fileName;		
		warningMsg(typeid(*this).name(), __func__, msg.data(), warnCode);

		ret = false;
	}

	return ret;
}

