#ifndef CGArray_h
#define CGArray_h

#include "OGText.h"

class ArrayValue
{
public:
	string m_ID;
	string m_sDescription;
	class Row_Icon * m_drIcon;
	int m_iPK_CommandGroup;
	int m_iPK_Variable;
	bool m_bCanHide;
	bool m_bHideByDefault;
	bool m_bSamePosition;
	int m_PK_Criteria_Orbiter;
	int m_PK_DesignObjID_Substitute;

	ArrayValue(string ID,string Description,class Row_Icon * drIcon,int PK_Criteria_Orbiter,int PK_DesignObjID_Substitute,int PK_CommandGroup,int PK_Variable,bool bCanHide,bool bHideByDefault,bool bSamePosition)
	{ 
		m_ID=ID; m_sDescription=Description; m_drIcon=drIcon; m_PK_Criteria_Orbiter=PK_Criteria_Orbiter;
		m_iPK_CommandGroup=PK_CommandGroup; m_iPK_Variable=PK_Variable; m_bCanHide=bCanHide; m_bSamePosition=bSamePosition; m_bHideByDefault=bHideByDefault;
		m_PK_DesignObjID_Substitute=PK_DesignObjID_Substitute;
	}
};

class CGArray
{
public:
	bool m_bContainsMore;
	int m_iLastVisibleArrayEntry,iScreenNum;
	vector<class DesignObj_Generator *> m_alChildDesignObjs;
	vector<class ArrayValue *> *m_alValues;
	int m_iStartingOffset,m_iPage;
	class DesignObj_Generator *m_DesignObj_Generator_Parent,*m_ocBack,*m_ocFwd;
	class PlutoRectangle m_rBounds;
	class Database_pluto_main *m_mds;
	map<int,string> *m_VariableMap;
	class Row_DesignObjVariation *m_drDesignObjVariation,*m_drStandardVariation;
	
	vector<class Row_DesignObjVariation *> m_alDesignObjVariations;
	class Row_DesignObjVariation_DesignObjParameter * m_drOVCP_MoreFwd, *m_drOVCP_MoreBack;
	class Row_DesignObjVariation_DesignObj * m_drDesignObjVariation_DesignObj;
	class PlutoPoint m_ptNextPosition;

	CGArray(class DesignObj_Generator *DesignObj_Generator_Parent,class Row_DesignObjVariation_DesignObj * drDesignObjVariation_DesignObj,vector<class ArrayValue *> *alValues,class PlutoRectangle rBounds,int StartingOffset,int Page);
	void CheckLastEntry();
};

#endif
