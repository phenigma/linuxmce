#ifndef CGText_h
#define CGText_h

#include "pluto_main/Table_DesignObjVariation_Text_Skin_Language.h"
#include "Orbiter/DesignObj_Data.h"

class CGText : public DesignObjText
{
public:
	class Row_Text_LS *m_pdrText_LS;
	class Row_DesignObjVariation_Text_Skin_Language *m_pRow_DesignObjVariation_Text_Skin_Language;
	class Row_Orbiter *m_pdrOrbiter;
	class DesignObj_Generator *m_pParent;

	CGText(class DesignObj_Generator *Parent,class Row_DesignObjVariation_Text_Skin_Language *drOVTSL,class Row_Orbiter *drOrbiter);
	static class Row_Text_LS *GetText_LS(int PK_Text,class OrbiterGenerator *pOrbiterGen);
};

#endif
