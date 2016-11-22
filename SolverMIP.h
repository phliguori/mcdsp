#ifndef SOLVER_MIP_H
#define SOLVER_MIP_H

#include "Util.h"
#include "Row.h"
#include "Column.h"
#include "Separator.h"

#include <string>
#include <cplex.h>

#include <cstring>  // Added by Youcef
#include <math.h>  // Added by Youcef

#define SOLVER_INF    CPX_INFBOUND
#define SOLVER_EPS    1e-8

class SolverMIP {
public:
	typedef enum {
		MAXIMIZATION = 2001,
		MINIMIZATION,
		SENSE_UNKNOWN
	} FOSENSE;

	typedef enum {
		METHOD_PRIMAL = 3001,
		METHOD_DUAL,
		METHOD_MIP,
		METHOD_UNKNOWN
	} METHOD;

	typedef enum {
		CONTINUOUS = 4001,
		INTEGER,
		TYPE_UNKNOWN
	}PROBTYPE;

	typedef enum {
		SOLVERSTAT_MIPOPTIMAL = 5001,
		SOLVERSTAT_LPOPTIMAL,
		SOLVERSTAT_OPTIMALINFEAS,
		SOLVERSTAT_UNOPTIMIZED,
		SOLVERSTAT_ERROR,
		SOLVERSTAT_FEASIBLE,
		SOLVERSTAT_INFEASIBLE,
		SOLVERSTAT_NOINTEGER,
		SOLVERSTAT_NOFEASIBLE,
		SOLVERSTAT_UNBOUNDED,
		SOLVERSTAT_UNKNOWN
	} SOLVERSTAT;

	// Constructors & destructors
	SolverMIP();
	virtual ~SolverMIP();

	// Loading & solving routines
	void loadProblem(const char* _problemName = NULL, FOSENSE _sense = FOSENSE::MAXIMIZATION, PROBTYPE _type = PROBTYPE::INTEGER);
	virtual int createVariables() = 0;
	virtual int createConstraints() = 0;
	virtual SOLVERSTAT solveLRExtentedFormulations();
	virtual SOLVERSTAT solve(METHOD _method, bool _viz=false);

	// Util
	virtual bool writeProbLP(const char * lpName);
	Column::COLTYPE choseColType(int _type, double lb = 0., double ub = SOLVER_INF);
	virtual bool hasExternalLpRelax() const { return fabs(externalLpRelax + 1.0) > SOLVER_EPS; }

	virtual bool turnScreenOn();
	virtual bool turnScreenOff();
	virtual bool shutDownHeuristics();
	virtual bool shutDownAutomaticCuts();
	virtual bool shutDownAutomaticPreSolve();

	// Getters
	bool getX(int _start = 0, int _end = -1);
	virtual double getObjVal() const;
	virtual double getBestObjVal() const;
	virtual int getNCols() const;
	virtual int getNRows() const;
	virtual int getNUserCuts() const;
	virtual int getNodeCount() const;
	virtual int getNodeLeftCount() const;
	virtual double getMipRelGap() const;
	virtual double getIntegralityGap() const;
	virtual double getExternalLpRelax() const { return externalLpRelax; }
	virtual PROBTYPE getProblemType() const { return pType; }
	virtual SOLVERSTAT getStat(void);
	virtual double getExecutionTime() const { return executionTime; }

	// Setters
	virtual void setSeparationRoutine(void* _cbdata, Separator * _separator);
	virtual void setExternalLpRelax(double _lpRelax) { externalLpRelax = _lpRelax; }
	virtual void setNumThreads(int _nthreads);
	virtual void setNodesLim(int _nNodes);
	virtual void setTimeLim(double _timeLim);

protected:
	virtual double getTimeStamp() const;

	bool createLP(const char* _problemName, FOSENSE _sense, PROBTYPE _type = PROBTYPE::INTEGER);

	bool addCol(Column* _col);
	bool addCol(Column::COLTYPE _type, double _objcoef, double _lb, double _ub, const char* _name);

	bool addRow(Row* _row);
	bool addRow(Row::ROWSENSE _sense, int _maxnnz, double _rhs, const char* _name);

	double executionTime;

	double* xSol;

	// Stocks lp relaxation bound given by external (i.e user or other methods rather than Cplex) source
	double externalLpRelax;

	// Used to retrieve cplex status
	int status;

	// Indicates whether the problem is already created
	bool problemCreated;

	// Store the objective function sense
	FOSENSE foSense;

	// Store the solution method
	METHOD method;

	// Store the problem type
	PROBTYPE pType;

	// Store the problem name
	std::string probName;

	// Pointer to cplex environment
	CPXENVptr env;

	//Pointer to cplex model
	CPXLPptr lp;
};

#endif
