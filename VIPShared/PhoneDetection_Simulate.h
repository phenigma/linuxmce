#ifndef PhoneDetection_Simulate_h
#define PhoneDetection_Simulate_h

#include "PhoneDetectionEngine.h"

class PhoneDetection_Simulate : public PhoneDetectionEngine
{
public:

protected:
	// This function needs to be overridden 
	virtual bool ScanningLoop();
};
#endif

