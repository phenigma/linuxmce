#ifndef __DEVICE_CATEGORY_DATA_H__
#define __DEVICE_CATEGORY_DATA_H__
//----------------------------------------------------------------------------------------------
class DeviceCategory_Data
{
private:

	int m_nPK_DeviceCategory;
	int m_nFK_DeviceCategory_Parent;
	string m_sDescription;

public:

	DeviceCategory_Data(int nPK_DeviceCategory = 0)
	{
		m_nPK_DeviceCategory = nPK_DeviceCategory;
		m_nFK_DeviceCategory_Parent = 0;
	}

	void Description(string sValue)
	{
		m_sDescription = sValue;
	}

	string Description()
	{
		return m_sDescription;
	}

	void DeviceCategoryParent(int nValue)
	{
		m_nFK_DeviceCategory_Parent = nValue;
	}

	int  DeviceCategoryParent()
	{
		return m_nFK_DeviceCategory_Parent;
	}
};
//----------------------------------------------------------------------------------------------
#endif //__DEVICE_CATEGORY_DATA_H__
