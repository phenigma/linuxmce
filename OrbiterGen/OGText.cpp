#include "PlutoUtils/CommonIncludes.h"	
#include "OGText.h"
#include "DesignObj_Generator.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Text_LS.h"
#include "pluto_main/Table_DesignObjVariation_Text_Skin_Language.h"
#include "pluto_main/Table_DesignObjVariation_Text.h"
#include "pluto_main/Table_DesignObjVariation.h"
#include "pluto_main/Table_Orbiter.h"
#include "pluto_main/Table_Text_LS_AltVersions.h"

CGText::CGText(class DesignObj_Generator *Parent,class Row_DesignObjVariation_Text_Skin_Language *drOVTSL,class Row_Orbiter *drOrbiter) 
{
if( drOVTSL->FK_DesignObjVariation_Text_getrow()->FK_Text_get()==924 )
{
int k=2;
}
	m_bPreRender=true;
	m_pParent=Parent;
	Database_pluto_main *mds = drOVTSL->Table_DesignObjVariation_Text_Skin_Language_get()->Database_pluto_main_get();
	m_pdrDesignObjVariation_Text_Skin_Language=drOVTSL;
	m_pdrOrbiter=drOrbiter;

	Row_DesignObjVariation_Text *pp = drOVTSL->FK_DesignObjVariation_Text_getrow();
	m_PK_Text=pp->FK_Text_get();
	m_pdrText_LS=mds->Text_LS_get()->GetRow(drOVTSL->FK_DesignObjVariation_Text_getrow()->FK_Text_get(),drOrbiter->FK_Language_get());
	if( m_pdrText_LS==NULL )
		m_pdrText_LS=mds->Text_LS_get()->GetRow(drOVTSL->FK_DesignObjVariation_Text_getrow()->FK_Text_get(),0);
	if( m_pdrText_LS==NULL )
		m_pdrText_LS=mds->Text_LS_get()->GetRow(drOVTSL->FK_DesignObjVariation_Text_getrow()->FK_Text_get(),1);
	if( m_pdrText_LS==NULL )
	{
		cerr << "Warning!  Text Object: " << drOVTSL->FK_DesignObjVariation_Text_getrow()->FK_Text_get() << " in object: " << drOVTSL->FK_DesignObjVariation_Text_getrow()->FK_DesignObjVariation_getrow()->FK_DesignObj_get() << " has no text!" << endl;
		return;
	}

	m_sText = m_pdrText_LS->Description_get();
	m_rPosition.X = m_pdrDesignObjVariation_Text_Skin_Language->X_get();
	m_rPosition.Y = m_pdrDesignObjVariation_Text_Skin_Language->Y_get();
	if( m_pdrDesignObjVariation_Text_Skin_Language->Width_get()==0 )
		m_rPosition.Width = m_pParent->m_rPosition.Width - m_rPosition.X;
	else
		m_rPosition.Width = m_pdrDesignObjVariation_Text_Skin_Language->Width_get();

	if(m_pdrDesignObjVariation_Text_Skin_Language->Height_get()==0)
		m_rPosition.Height = m_pParent->m_rPosition.Height - m_rPosition.Y;
	else
		m_rPosition.Height = m_pdrDesignObjVariation_Text_Skin_Language->Height_get();

	bool bContainsRunTimeVariables;
	m_sText = m_pParent->SubstituteVariables(m_sText,&bContainsRunTimeVariables);
/*

todo c++
	vector<Row_Text_LS_AltVersions *> vectAVs;
    m_pdrText_LS->Text_LS_AltVersions_FK_Text_getrows(&vectAVs);
	for(size_t i=0;i<vectAVs.size();++i)
	{
		Row_Text_LS_AltVersions *drTLAV = vectAVs[i];
		if( drTLAV->FK_Language_get()==1 || drTLAV->FK_Language_get()==drOrbiter->FK_Language_get() )
		{
			bool b;
			// if this is the matching language, then force the use of this variation, otherwise only use it if we don't already have a matching language
			if( drTLAV->FK_Language_get()==drOrbiter->FK_Language_get() || 
				m_mapAltTextVersions.find(drTLAV->Version_get())==m_mapAltTextVersions.end() )
				m_mapAltTextVersions[drTLAV->Version_get()]=Parent->SubstituteVariables(drTLAV->Description_get(),&b);
		}
	}
*/
	m_bPreRender = (!bContainsRunTimeVariables || m_mapAltVersions.size()>0);
}
