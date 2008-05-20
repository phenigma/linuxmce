#ifndef __DEVICE_TEMPLATE_DATA_H__
#define __DEVICE_TEMPLATE_DATA_H__
//----------------------------------------------------------------------------------------------
class DeviceTemplate_Data
{
private:

	int m_nPK_DeviceTemplate;
	string m_sDescription;
	int m_nFK_DeviceCategory;
	bool m_bImplementsDCE;
	bool m_bIsEmbedded;
	string m_sCommandLine;

public:

	DeviceTemplate_Data(int nPK_DeviceTemplate = 0)
	{
		m_nPK_DeviceTemplate = nPK_DeviceTemplate;
		m_nFK_DeviceCategory = 0;
		m_bImplementsDCE = true;
		m_bIsEmbedded = false;
	}

	void Description(string sValue)
	{
		m_sDescription = sValue;
	}

	string Description()
	{
		return m_sDescription;
	}

	void DeviceCategory(int nValue)
	{
		m_nFK_DeviceCategory = nValue;
	}

	int  DeviceCategory()
	{
		return m_nFK_DeviceCategory;
	}

	void ImplementsDCE(bool bValue)
	{
		m_bImplementsDCE = bValue;
	}

	bool ImplementsDCE()
	{
		return m_bImplementsDCE;
	}

	void IsEmbedded(bool bValue)
	{
		m_bIsEmbedded = bValue;
	}

	bool IsEmbedded()
	{
		return m_bIsEmbedded;
	}

	void CommandLine(string sValue)
	{
		m_sCommandLine = sValue;
	}

	string CommandLine()
	{
		return m_sCommandLine;
	}
};
//----------------------------------------------------------------------------------------------
#endif //__DEVICE_TEMPLATE_DATA_H__
