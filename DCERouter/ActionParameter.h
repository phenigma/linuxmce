#ifndef ACTIONPARAMETER_H
#define ACTIONPARAMETER_H

class CommandParameter
{
public:
	string m_sValue;
	int m_C_PK_CommandParameter;

	CommandParameter(int PK_CommandParameter,string sValue) : m_sValue(sValue), m_C_PK_CommandParameter(PK_CommandParameter) {}
};
#endif

