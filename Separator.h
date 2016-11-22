#ifndef SEPARATOR_H
#define SEPARATOR_H

#include <cplex.h>

typedef int (CPXPUBLIC *SeparationCallbackFnc)(CPXCENVptr _env, void* _cbdata, int _wherefrom, void* _cbhandle, int* _useraction_p);

class Separator
{
public:
	Separator();
	Separator(SeparationCallbackFnc _intSepFnc, SeparationCallbackFnc _fracSepFnc);
	// Separator(Graph* _g, TypeVariableHashPtr _vHashPtr);
	virtual ~Separator();

	SeparationCallbackFnc integerSeparationFnc;

	SeparationCallbackFnc fractionalSeparationFnc;

	//int __stdcall separateIntegerCutSet(Graph* g, TypeVariableHashPtr vHashPtr, std::vector<int> & x);

private:
	//CBData cbdata;
};

#endif