#ifndef IRDevice_h
#define IRDevice_h

#include "../SerializeClass/SerializeClass.h"

class IRDevice : public SerializeClass
{
public:
	map<int,string> m_mapCodes;
	bool m_bImplementsDCE;
	bool m_bUsesIR,m_bTogglePower,m_iToggleDSP,m_iToggleInput,m_iToggleOutput;
	int m_iPowerDelay,m_iModeDelay,m_iDigitDelay;
	int m_iRepeatIR,m_iRepeatVolume;
	int m_PK_Device_ControlledVia;
	string m_sNumericEntry;
	vector<int> m_vectInputs;
	vector<int> m_vectCommands_WithoutCodes;

	IRDevice()
	{
		m_bUsesIR=m_bTogglePower=false;
		m_iToggleDSP=m_iToggleInput=m_iToggleOutput=0;
		m_iPowerDelay=m_iModeDelay=m_iDigitDelay=m_PK_Device_ControlledVia=0;
		m_iRepeatIR=m_iRepeatVolume=4;
		m_bImplementsDCE=false;
	}

	void SetupSerialization(int iSC_Version)
	{
		StartSerializeList() + m_mapCodes + 
			m_bUsesIR + m_bTogglePower + m_iToggleDSP + m_iToggleInput + m_iToggleOutput +
			m_iPowerDelay + m_iModeDelay + m_iDigitDelay + 
			m_sNumericEntry + m_vectInputs + m_iRepeatIR + m_iRepeatVolume;
	}
	virtual string SerializeClassClassName() { return "IRDevice"; }
};

#endif

