#ifndef __COMMAND_DATA_H__
#define __COMMAND_DATA_H__
//----------------------------------------------------------------------------------------------
#include <map>
//----------------------------------------------------------------------------------------------
class Command_Data
{
private:

	int m_nDevice_From;
	int m_nDevice_To;
	int m_nPK_Command;
	int m_nDelay;
	bool m_bCancelIfOther;
	bool m_bIsTemporary;
	std::map<int, string> m_mapParams;

public:

	Command_Data()
	{
		m_nDevice_From = 0;
		m_nDevice_To = 0;
		m_nPK_Command = 0;
	}

	void Device_From(int nDeviceID)
	{
		m_nDevice_From = nDeviceID;
	}

	int Device_From()
	{
		return m_nDevice_From;
	}

	void Device_To(int nDeviceID)
	{
		m_nDevice_To = nDeviceID;
	}

	int Device_To()
	{
		return m_nDevice_To;
	}

	void PK_Command(int nPK_Command)
	{
		m_nPK_Command = nPK_Command;
	}

	int PK_Command()
	{
		return m_nPK_Command;
	}

	void Delay(int nDelay)
	{
		m_nDelay = nDelay;
	}

	int Delay()
	{
		return m_nDelay;
	}

	void CancelIfOther(int nCancelIfOther)
	{
		m_nCancelIfOther = nCancelIfOther;
	}

	int CancelIfOther()
	{
		return m_nCancelIfOther;
	}

	void IsTemporary(int nIsTemporary)
	{
		m_nIsTemporary = nIsTemporary;
	}

	int IsTemporary()
	{
		return m_nIsTemporary;
	}

	std::map<int, string>& Params()
	{
		return m_mapParams;
	}
};
//----------------------------------------------------------------------------------------------
#endif //__COMMAND_DATA_H__
