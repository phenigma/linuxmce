#pragma once

#include "Orbiter.h"
using namespace DCE;

class DesignObj_Orbiter;

class ObjectFactory
{
public:
	static DesignObj_Orbiter *CreateObject(unsigned long ulType, Orbiter *pOrbiter);
};
