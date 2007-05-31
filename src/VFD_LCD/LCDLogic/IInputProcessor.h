#ifndef __IINPUT_PROCESSOR__
#define __IINPUT_PROCESSOR__
//--------------------------------------------------------------------------------------------------------
#include "IInput.h"
//--------------------------------------------------------------------------------------------------------
class IInputProcessor
{
public:

	IInputProcessor() 
	{
	}

	virtual ~IInputProcessor() 
	{
	}

	virtual bool ProcessInput(IInput *pInput) = 0;
};
//--------------------------------------------------------------------------------------------------------
#endif //__IINPUT_PROCESSOR__
