#ifndef DesignObj_Generator_h
#define DesignObj_Generator_h

#include "OGText.h"
#include "Orbiter/DesignObj_Data.h"

typedef vector<class DesignObj_Generator *> listDesignObj_Generator;

class DesignObj_Generator : public DesignObj_Data
{
public:
	class Database_pluto_main * m_mds;
	class Row_DesignObj * m_drDesignObj;
	DesignObj_Generator * m_ocoParent;
	class OrbiterGenerator *m_pOrbiterGenerator;
	map<int,string> m_VariableMap;

	class Row_DesignObjVariation * m_drDesignObjVariation,*m_drStandardVariation;
	vector<class Row_DesignObjVariation *> m_alDesignObjVariations;
//	vector<class CGZone *> m_alZones;
//	vector<class CGCommand *> m_alLoadCommands,m_alUnloadCommands,m_alTimeoutCommands,m_alStartupCommands;

	vector<class DesignObj_Generator *> m_alChildDesignObjs;
	vector<class CGArray *> m_alNonMPArrays,m_alMPArray;
	vector<class CGVarAss *> m_alVariableAssignments;
	DesignObj_Generator *m_DesignObj_GeneratorGoto;
	string m_sDesignObjGoto;
	bool m_bContainsArrays;
	bool m_bDontShare,m_bUsingCache,m_bRendered;
	bool m_bCanBeHidden,m_bChildrenBehind,m_bDontMergeBackground;
	bool m_bValuesScaled;
	PlutoPoint m_pFloorplanFillPoint;
	int m_iVersion;
	int m_iFloorplanPage,m_iFloorplanDevice; // Only used for floorplan objects
	int m_iPK_CommandGroup_Touch_Extra;  // An extra action group to execute, used when this object is a button in an action group array.  
	class Row_Device * m_drDevice_Goto;

	DesignObj_Generator() { m_bRendered=false; } // when we're serializing from disk
	DesignObj_Generator(class OrbiterGenerator *pOrbiterGenerator,class Row_DesignObj * drDesignObj,class PlutoRectangle rPosition,class DesignObj_Generator *ocoParent,bool bAddToGenerated,bool bDontShare);
	~DesignObj_Generator();

	int LookForGoto(DesignObjCommandList *alCommands);
	void HandleGoto(int PK_DesignObj_Goto);
	static void PickVariation(OrbiterGenerator *pGenerator,class Row_DesignObj *drDesignObj,class Row_DesignObjVariation **drDesignObjVariation,class Row_DesignObjVariation **drStandardVariation, vector<class Row_DesignObjVariation *> *alDesignObjVariations);
	static void PickStyleVariation(class Row_Style * drStyle,OrbiterGenerator *pGenerator,MapTextStyle &mapTextStyle);
	static TextStyle *PickStyleVariation(vector<Row_StyleVariation *> &vectrsv,OrbiterGenerator *pGenerator,int Version);

	vector<class ArrayValue *> *GetArrayValues(class Row_DesignObjVariation_DesignObj * drOVO);
	void ScaleAllValues(int FactorX,int FactorY,class DesignObj_Generator *pTopmostObject);
	class PlutoPoint *ScaleValue(class PlutoPoint *pt,int FactorX,int FactorY);
	string GetParm(int PK_Parameter);
	string GetParm(int PK_Parameter,bool bReplaceNulls);
	string GetParm(int PK_Parameter,class Row_DesignObjVariation * drDesignObjVariation);
	string GetParm(int PK_Parameter,class Row_DesignObjVariation * drDesignObjVariation,bool bReplaceNulls);

	string SubstituteVariables(string Text,bool *bContainsRunTimeVariables);
};

#endif
