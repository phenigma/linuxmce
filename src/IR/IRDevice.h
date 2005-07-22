#ifndef IRDevice_h
#define IRDevice_h

#include "../SerializeClass/SerializeClass.h"

class IRDevice : public SerializeClass
{
public:
	map<int,string> m_mapCodes;
	bool m_bUsesIR,m_bTogglePower,m_bToggleDSP,m_bToggleInput,m_bToggleOutput;
	int m_iPowerDelay,m_iModeDelay,m_iDigitDelay;
	string m_sNumericEntry;

	IRDevice()
	{
		m_bUsesIR=m_bTogglePower=m_bToggleDSP=m_bToggleInput=m_bToggleOutput=false;
		m_iPowerDelay=m_iModeDelay=m_iDigitDelay=0;
	}

	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + m_mapCodes + 
			m_bUsesIR + m_bTogglePower + m_bToggleDSP + m_bToggleInput + m_bToggleOutput +
			m_iPowerDelay + m_iModeDelay + m_iDigitDelay +
			m_sNumericEntry;
	}
	virtual string SerializeClassClassName() { return "IRDevice"; }
};

#endif

