#include "Separator.h"

Separator::Separator()
{
	integerSeparationFnc = NULL;
	fractionalSeparationFnc = NULL;

	return;
}

Separator::Separator(SeparationCallbackFnc _intSepFnc, SeparationCallbackFnc _fracSepFnc)
	: Separator()
{
	integerSeparationFnc = _intSepFnc;
	fractionalSeparationFnc = _fracSepFnc;

	return;
}

Separator::~Separator()
{
	integerSeparationFnc = NULL;
	fractionalSeparationFnc = NULL;

	return;
}