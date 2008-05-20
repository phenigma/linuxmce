#ifndef __SCENE_DATA_H__
#define __SCENE_DATA_H__
//----------------------------------------------------------------------------------------------
class Scene_Data
{
private:

	int m_nSceneID;
	string m_sDescription;

public:

	Scene_Data(int nSceneID = 0)
	{
		m_nSceneID = nSceneID;
	}

	void Description(string sValue)
	{
		m_sDescription = sValue;
	}

	string Description()
	{
		return m_sDescription;
	}
};
//----------------------------------------------------------------------------------------------
#endif //__SCENE_DATA_H__
