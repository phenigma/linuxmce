#include "PlutoUtils/CommonIncludes.h"
#include <iostream>
#include <string>
#include "OGArray.h"
#include "OGCommand.h"
#include "DesignObj_Generator.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "OrbiterGen.h"
#include "SerializeClass/ShapesColors.h"

#include "pluto_main/Database_pluto_main.h"
#include "pluto_main/Table_Text_LS.h"
#include "pluto_main/Table_Floorplan.h"
#include "pluto_main/Table_FloorplanType.h"
#include "pluto_main/Table_FloorplanObjectType.h"
#include "pluto_main/Table_CachedScreens.h"
#include "pluto_main/Table_Image.h"
#include "pluto_main/Table_Installation.h"
#include "pluto_main/Table_Installation_Users.h"
#include "pluto_main/Table_DesignObj.h"
#include "pluto_main/Table_DesignObjParameter.h"
#include "pluto_main/Table_DesignObjVariation.h"
#include "pluto_main/Table_DesignObjVariation_Text_Skin_Language.h"
#include "pluto_main/Table_DesignObjVariation_Text.h"
#include "pluto_main/Table_DesignObjVariation_Zone.h"
#include "pluto_main/Table_DesignObjVariation_DesignObj.h"
#include "pluto_main/Table_DesignObjVariation_DesignObjParameter.h"
#include "pluto_main/Table_EntertainArea.h"
#include "pluto_main/Table_Room.h"
#include "pluto_main/Table_Size.h"
#include "pluto_main/Table_Language.h"
#include "pluto_main/Table_Skin.h"
#include "pluto_main/Table_Style.h"
#include "pluto_main/Table_StyleVariation.h"
#include "pluto_main/Table_Device.h"
#include "pluto_main/Table_DeviceData.h"
#include "pluto_main/Table_Device_DeviceData.h"
#include "pluto_main/Table_DeviceCategory.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_Orbiter.h"
#include "pluto_main/Table_Users.h"
#include "pluto_main/Table_Text_LS_AltVersions.h"
#include "pluto_main/Table_CommandGroup.h"
#include "pluto_main/Table_CommandGroup_Room.h"
#include "pluto_main/Table_CommandGroup_EntertainArea.h"
#include "pluto_main/Table_CommandGroup_D.h"
#include "pluto_main/Table_Icon.h"
#include "pluto_main/Table_Array.h"
#include "pluto_main/Table_CommandParameter.h"
#include "pluto_main/Table_DesignObjParameter.h"
#include "pluto_main/Table_DesignObjType.h"
#include "pluto_main/Table_Command.h"
#include "pluto_main/Table_Variable.h"
#include "pluto_main/Table_DeviceTemplate.h"
#include "pluto_main/Table_DeviceTemplate_DesignObj.h"

DesignObj_Generator::DesignObj_Generator(OrbiterGenerator *pGenerator,class Row_DesignObj * drDesignObj,class PlutoRectangle rPosition,class DesignObj_Generator *ocoParent,bool bAddToGenerated,bool bDontShare)
{
    m_pRow_DesignObjVariation=NULL;
    m_pRow_DesignObjVariation_Standard=NULL;
    m_bContainsArrays=false;
    m_bDontShare=false;
    m_bCanBeHidden=false;
    m_bChildrenBeforeText=false;
    m_bChildrenBehind=false;
    m_bDontMergeBackground=false;
    m_bHideByDefault=false;
    m_bValuesScaled=false;
    m_iVersion=0;
    m_iFloorplanPage=0;
    m_iFloorplanDevice=0; // Only used for floorplan objects
    m_iPK_CommandGroup_Touch_Extra=0;  // An extra action group to execute, used when this object is a button in an action group array
    m_pRow_Device_Goto=NULL;
    m_DesignObj_GeneratorGoto=NULL;
    m_pOrbiterGenerator=pGenerator;
    m_bRendered=false;

    m_mds=drDesignObj->Table_DesignObj_get()->Database_pluto_main_get();
    m_VariableMap = m_pOrbiterGenerator->m_mapVariable;
    m_pRow_DesignObj=drDesignObj;
    m_rPosition=rPosition;
    m_ocoParent=ocoParent;
    m_bDontShare=bDontShare;
    m_bUsingCache=false;

if( m_pRow_DesignObj->PK_DesignObj_get()==1326 )//2821 && bAddToGenerated )
{
    int k=2;
}
    // Put this at the top since we've got a goto below that may skip its initializatoin
    vector<Row_DesignObjVariation_DesignObj *> alArrays;

    // Pick the standard variation, the specialized variation we will use for parameters, and all matching variations for including child objects
    PickVariation(m_pOrbiterGenerator,m_pRow_DesignObj,&m_pRow_DesignObjVariation,&m_pRow_DesignObjVariation_Standard,&m_alDesignObjVariations);

    if( !m_pRow_DesignObjVariation_Standard )
    {
        cerr << "Aborting building of: " << m_pRow_DesignObj->PK_DesignObj_get() << " because there were no variations." << endl << "Attempts to use will fail at runtime." << endl;
        return;
    }

    if( bAddToGenerated )
    {
		cout << "Generating screen: " << drDesignObj->PK_DesignObj_get() << " in orbiter: " << pGenerator->m_pRow_Device->PK_Device_get() << endl;
        listDesignObj_Generator *al = m_pOrbiterGenerator->m_htGeneratedScreens[drDesignObj->PK_DesignObj_get()];
        if( al==NULL )
        {
            al=new listDesignObj_Generator();
            m_pOrbiterGenerator->m_htGeneratedScreens[drDesignObj->PK_DesignObj_get()]=al;
        }
        al->push_back(this);

        // If this is the main menu, there will be an array of them
        if( m_pRow_DesignObj->PK_DesignObj_get()==m_pOrbiterGenerator->m_pRow_DesignObj_MainMenu->PK_DesignObj_get() )
        {
            m_iVersion = m_pOrbiterGenerator->m_iLocation;
        }
        else if( m_pOrbiterGenerator->m_bOrbiterChanged==false )
        {
            // Let's see if we can just use a cached version
            Row_CachedScreens *pdrCachedScreen = m_mds->CachedScreens_get()->GetRow(m_pOrbiterGenerator->m_pRow_Orbiter->PK_Orbiter_get(),m_pRow_DesignObj->PK_DesignObj_get(),m_iVersion);
            if( pdrCachedScreen  )
            {
                time_t lModDate1 = StringUtils::SQLDateTime(pdrCachedScreen->Modification_LastGen_get());
                time_t lModDate2 = StringUtils::SQLDateTime(m_pRow_DesignObj->psc_mod_get());
                if( lModDate1==lModDate2 )
                {
                    string Filename = m_pOrbiterGenerator->m_sOutputPath + "screen " + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Orbiter->PK_Orbiter_get()) + "." +
                        StringUtils::itos(m_pRow_DesignObj->PK_DesignObj_get()) + "." + StringUtils::itos(m_iVersion) + "." + StringUtils::itos((int) lModDate1) + ".cache";
                    if( FileUtils::FileExists(Filename) )
                    {
                        if( SerializeRead(Filename) )
                        {
                            m_bUsingCache=true;
                            cout << "Not building screen " << StringUtils::itos(m_pRow_DesignObj->PK_DesignObj_get()) + "." + StringUtils::itos(m_iVersion) << " found valid cache" << endl;
                            return;
                        }
                    }
                }
            }
        }
        string Filespec = m_pOrbiterGenerator->m_sOutputPath + "*" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Orbiter->PK_Orbiter_get()) + "." +
            StringUtils::itos(m_pRow_DesignObj->PK_DesignObj_get()) + ".*";
        string Filespec2 = m_pOrbiterGenerator->m_sOutputPath + StringUtils::itos(m_pRow_DesignObj->PK_DesignObj_get()) + ".*";
#ifdef WIN32
        // Remove any old cached files
        string::size_type s;

        // Replace the \ with / for windows
        while( (s=Filespec.find("/"))!=string::npos )
            Filespec.replace(s,1,"\\");
        while( (s=Filespec2.find("/"))!=string::npos )
            Filespec2.replace(s,1,"\\");

        system( ("del \"" + Filespec + "\" 2> null").c_str() );
        system( ("del \"" + Filespec2 + "\" 2> null").c_str() );
#else
        // Remove any old cached files
        system( ("rm \"" + Filespec + "\" 2>/dev/null").c_str() );
        system( ("rm \"" + Filespec2 + "\" 2>/dev/null").c_str() );
#endif
    }

    if( !m_pRow_DesignObjVariation->FK_Criteria_Orbiter_isNull() )
        m_pOrbiterGenerator->m_mapUsedOrbiterCriteria[m_pRow_DesignObjVariation->FK_Criteria_Orbiter_get()]=true;

    // This is an object that is part of an array, and this button will execute the designated action group
    if( m_pOrbiterGenerator->m_iPK_CommandGroup!=0 )
    {
        m_iPK_CommandGroup_Touch_Extra = m_pOrbiterGenerator->m_iPK_CommandGroup;
        m_pOrbiterGenerator->m_iPK_CommandGroup=0;  // reset back to 0 so it doesn't affect the children
    }
    for(int GraphicType=1;GraphicType<=4;++GraphicType)
    {
        // Handle the background image
        string o;

        if( m_pRow_DesignObj->FK_DesignObjType_get()==DESIGNOBJTYPE_Floorplan_CONST )
        {
            if( GraphicType==1 )
            {
                m_iFloorplanPage = m_pOrbiterGenerator->m_iFloorplanPage;
                o = "/Floorplans/I" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Device->FK_Installation_get()) + "/" + "Page" + StringUtils::itos(m_iFloorplanPage) + ".png";
/*
                string[] sFiles = Directory.GetFiles(m_pOrbiterGenerator->m_GraphicsBasePath + "/Floorplans/I" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Device->FK_Installation_get()) + "/","Page" + StringUtils::itos(m_iFloorplanPage) + ".*");
                if( sFiles.Length>0 )
                {
                    o = sFiles[0].substr(sFiles[0].IndexOf("/Floorplans/"));
                }
*/
            }
        }
        else
        {
            if( GraphicType==1 )
                o=GetParm(DESIGNOBJPARAMETER_Graphic_Filename_CONST,true);
            else if( GraphicType==2 )
                o=GetParm(DESIGNOBJPARAMETER_Selected_Graphic_Fil_CONST,true);
            else if( GraphicType==3 )
                o=GetParm(DESIGNOBJPARAMETER_Highlighted_Graphic_CONST,true);
            else
                o=GetParm(DESIGNOBJPARAMETER_Alt_Graphic_File_CONST,true);
        }

        string sGraphicFile = o;

        // Todo -- temporarily disable MNG's while we try the new thing
        if( sGraphicFile.length()>0 && StringUtils::ToUpper(sGraphicFile).find(".MNG")==sGraphicFile.length()-4 )
        {
            sGraphicFile="";
        }

        if( sGraphicFile.length()>0 )
        {
            if( m_pOrbiterGenerator->m_bNoEffects )
            {
                sGraphicFile = StringUtils::Replace(sGraphicFile,".MNG",".PNG");
            }
            size_t scPos=0;
            for(;;)
            {
                // Handle the fact that if this is an alt file it may be a ; delimted list
                if( GraphicType==4 )
                {
                    size_t NewscPos = o.find(";",scPos);
                    if( NewscPos!=string::npos )
                    {
                        sGraphicFile = o.substr(scPos,NewscPos-scPos);
                        scPos = NewscPos+1;
                    }
                    else if( scPos>=o.length() )
                        break;
                    else
                    {
                        sGraphicFile = o.substr(scPos);
                        scPos=o.length(); // The last time around
                    }

                }

                Row_Image * drImage=NULL;
                // Be sure there are no windows paths in here.  Windows will work with forward slashes also, use them instead
                sGraphicFile = StringUtils::Replace(sGraphicFile,"\\","/");
                string sOriginalFile = sGraphicFile;
                if( sGraphicFile[0]=='/' )
                    sGraphicFile = m_pOrbiterGenerator->m_GraphicsBasePath + sGraphicFile;
                else
                    sGraphicFile = m_pOrbiterGenerator->m_GraphicsBasePath + "/" + m_pOrbiterGenerator->m_pRow_Skin->DataSubdirectory_get() + "/" + sGraphicFile;

                if( !FileUtils::FileExists(sGraphicFile) )
                {
//                  cout << "Cannot open: " << sGraphicFile << " - looking in basic skin" << endl;
                    sGraphicFile = m_pOrbiterGenerator->m_GraphicsBasePath + "/" + m_mds->Skin_get()->GetRow(1)->DataSubdirectory_get() + "/" + sOriginalFile;  // hack in skin 1
                    if( !FileUtils::FileExists(sGraphicFile) )
                    {
                        cout << "Error reading graphic: " << sGraphicFile << " DesignObj: " << m_pRow_DesignObjVariation->FK_DesignObj_get() << endl;
                        sGraphicFile = "";
                    }
                }

                if( sGraphicFile.length()>0 )
                {
Table_Image *p = m_mds->Image_get();
                    drImage = m_mds->Image_get()->GetRow(sGraphicFile);
                    if( drImage && (drImage->Width_get()==0 || drImage->Height_get()==0) )
                    {
                        cout << "Image: " << sGraphicFile << " has width/height=0";
                        break;
                    }

//                  time_t t1 = StringUtils::SQLDateTime(drImage->Date_get());
//                  time_t t2 = FileUtils::FileDate(sGraphicFile);
                    if( !drImage || drImage->Date_isNull() || StringUtils::SQLDateTime(drImage->Date_get())!=FileUtils::FileDate(sGraphicFile) )
                    {

                        if( StringUtils::ToUpper(sGraphicFile).find(".MNG")==sGraphicFile.length()-4 )
                        {
                            if( !drImage )
                            {
                                drImage = m_mds->Image_get()->AddRow();
                                drImage->PK_Image_set(sGraphicFile);
                                drImage->Width_set(0);
                                drImage->Height_set(0);
                                m_mds->Image_get()->Commit();
                            }
                            cout << "New MNG: " << sGraphicFile << " need to add size";
                            break;
                        }

                        // Calculate the width and height.  Brute force so we don't have to link another library
                        unsigned char buf[200];
                        FILE *file = fopen(sGraphicFile.c_str(),"rb");
                        size_t s_read = fread(buf,1,200,file);
                        size_t i;
                        for(i=0;i<s_read-15;++i)
                        {
                            if( strncmp((const char *)&buf[i],"IHDR",4)==0 )
                                break;
                        }

                        if( i>=s_read-15 )
                            throw "Cannot get width and height of " + sGraphicFile;

                        unsigned long Width=buf[i+6]*256+buf[i+7];
                        unsigned long Height=buf[i+10]*256+buf[i+11];
                        fclose(file);

                        if( !drImage )
                        {
                            drImage = m_mds->Image_get()->AddRow();
                            drImage->PK_Image_set(sGraphicFile);
                            drImage->Date_set(StringUtils::SQLDateTime(FileUtils::FileDate(sGraphicFile)));
                            drImage->Width_set(Width);
                            drImage->Height_set(Height);
                            m_mds->Image_get()->Commit();
                        }
                        else
                        {
                            drImage->Width_set(Width);
                            drImage->Height_set(Height);
                            drImage->Date_set(StringUtils::SQLDateTime(FileUtils::FileDate(sGraphicFile)));
                            m_mds->Image_get()->Commit();
                        }
                    }
                    if( GraphicType==1 )
                    {
                        int X=m_rPosition.X,Y=m_rPosition.Y;
                        Row_DesignObjVariation_DesignObjParameter * drOVCP_C_X = m_mds->DesignObjVariation_DesignObjParameter_get()->GetRow(m_pRow_DesignObjVariation->PK_DesignObjVariation_get(),DESIGNOBJPARAMETER_X_Position_CONST);
                        if( drOVCP_C_X==NULL && m_pRow_DesignObjVariation->PK_DesignObjVariation_get()!=m_pRow_DesignObjVariation_Standard->PK_DesignObjVariation_get() )
                            drOVCP_C_X = m_mds->DesignObjVariation_DesignObjParameter_get()->GetRow(m_pRow_DesignObjVariation->PK_DesignObjVariation_get(),DESIGNOBJPARAMETER_X_Position_CONST);

                        Row_DesignObjVariation_DesignObjParameter * drOVCP_C_Y = m_mds->DesignObjVariation_DesignObjParameter_get()->GetRow(m_pRow_DesignObjVariation->PK_DesignObjVariation_get(),DESIGNOBJPARAMETER_Y_Position_CONST);
                        if( drOVCP_C_Y==NULL && m_pRow_DesignObjVariation->PK_DesignObjVariation_get()!=m_pRow_DesignObjVariation_Standard->PK_DesignObjVariation_get() )
                            drOVCP_C_Y = m_mds->DesignObjVariation_DesignObjParameter_get()->GetRow(m_pRow_DesignObjVariation->PK_DesignObjVariation_get(),DESIGNOBJPARAMETER_Y_Position_CONST);

                        if( drOVCP_C_X && !drOVCP_C_X->Value_isNull() && drOVCP_C_X->Value_get()!="" )
                            X+=atoi(drOVCP_C_X->Value_get().c_str());
                        if( drOVCP_C_Y && !drOVCP_C_Y->Value_isNull() && drOVCP_C_Y->Value_get()!="" )
                            Y+=atoi(drOVCP_C_Y->Value_get().c_str());
                        m_rBackgroundPosition.Location(PlutoPoint(X,Y));
                    }

                    // If we have an image, and this is either the primary background, or a non-primary image but there was no primary (width & height=0) set it
                    if( drImage && (GraphicType==1 || (m_rBackgroundPosition.Width==0 && m_rBackgroundPosition.Height==0) ) )
                    {
                        m_sOriginalSize = PlutoSize(drImage->Width_get(),drImage->Height_get());
                        /* 10/6/2004 Aaron - Designer adds wrong width/heights sometimes, particularly with objects with multiple variations.  For now always use the actual w/h
                        if( m_rPosition.Width>0 )
                            m_rBackgroundPosition.Width = m_rPosition.Width;
                        else
                        {
                            m_rPosition.Width = drImage->Width_get();
                            m_rBackgroundPosition.Width = drImage->Width_get();
                        }
                        */
                        m_rPosition.Width = drImage->Width_get();
                        m_rBackgroundPosition.Width = drImage->Width_get();

                        /* 10/6/2004 Aaron - Designer adds wrong width/heights sometimes, particularly with objects with multiple variations.  For now always use the actual w/h
                        if( m_rPosition.Height>0 )
                            m_rBackgroundPosition.Height = m_rPosition.Height;
                        else
                        {
                            m_rPosition.Height = drImage->Height_get();
                            m_rBackgroundPosition.Height = drImage->Height_get();
                        }
                        */
                        m_rPosition.Height = drImage->Height_get();
                        m_rBackgroundPosition.Height = drImage->Height_get();
                    }

                    if( GraphicType==1 )
                        m_sBackgroundFile = sGraphicFile;
                    else if( GraphicType==2 )
                        m_sSelectedFile = sGraphicFile;
                    else if( GraphicType==3 )
                        m_sHighlightGraphicFilename = sGraphicFile;
                    else
                    {
                        m_vectAltGraphicFilename.push_back(sGraphicFile);
                    }
                }

                if( GraphicType!=4 )
                    break;

            }
        }
    }

    // Add all text objects

    for(size_t sov=0;sov<m_alDesignObjVariations.size();++sov)
    {
        Row_DesignObjVariation * drOV=m_alDesignObjVariations[sov];
        vector<class Row_DesignObjVariation_Text *> vectrovt;
        drOV->DesignObjVariation_Text_FK_DesignObjVariation_getrows(&vectrovt);
        for(size_t s=0;s<vectrovt.size();++s)
        {
            Row_DesignObjVariation_Text *drOVT = vectrovt[s];
            vector<class Row_DesignObjVariation_Text_Skin_Language *> vectovtsr;
            drOVT->DesignObjVariation_Text_Skin_Language_FK_DesignObjVariation_Text_getrows(&vectovtsr);
            Row_DesignObjVariation_Text_Skin_Language * drOVTSL_match=NULL;
            bool bMatchLanguage=false;
            bool bMatchSkin=false;

            for(size_t s2=0;s2<vectovtsr.size();++s2)
            {
                Row_DesignObjVariation_Text_Skin_Language * drOVTSL = vectovtsr[s2];
                if( !drOVTSL->FK_Skin_isNull() && drOVTSL->FK_Skin_get()==m_pOrbiterGenerator->m_pRow_Skin->PK_Skin_get() &&
                    !drOVTSL->FK_Language_isNull() && drOVTSL->FK_Language_get()==m_pOrbiterGenerator->m_pRow_Language->PK_Language_get() )
                {
                    drOVTSL_match = drOVTSL;
                    break;
                }
                else if( !drOVTSL->FK_Skin_isNull() && drOVTSL->FK_Skin_get()==m_pOrbiterGenerator->m_pRow_Skin->PK_Skin_get() &&
                    drOVTSL->FK_Language_isNull() )
                {
                    drOVTSL_match = drOVTSL;
                    bMatchSkin=true;
                }
                else if( !drOVTSL->FK_Language_isNull() && drOVTSL->FK_Language_get()==m_pOrbiterGenerator->m_pRow_Language->PK_Language_get() &&
                    drOVTSL->FK_Skin_isNull() && !bMatchSkin )
                {
                    drOVTSL_match = drOVTSL;
                    bMatchLanguage=true;
                }
                else if( drOVTSL->FK_Language_isNull() && drOVTSL->FK_Skin_isNull() && !bMatchSkin && !bMatchLanguage)
                {
                    drOVTSL_match = drOVTSL;
                }
            }

            // We found text to put on the screen
            if( drOVTSL_match!=NULL )
            {
                Row_Style *drStyle=NULL;
                if( !drOVTSL_match->FK_Style_isNull() )
                    drStyle = drOVTSL_match->FK_Style_getrow();
                else
                    drStyle = m_pOrbiterGenerator->m_pRow_Skin->FK_Style_getrow();

                if( !drStyle )
                {
                    cerr << "Cannot find style for object - specified style: " << drOVTSL_match->FK_Style_get();
                    throw "Cannot open style";
                }
                CGText *pCGText = new CGText(this,drOVTSL_match,m_pOrbiterGenerator->m_pRow_Orbiter);
if( pCGText->m_sText=="1" )
{
int k=2;
}
                m_vectDesignObjText.push_back(pCGText);
                PickStyleVariation(drStyle,m_pOrbiterGenerator,pCGText->m_mapTextStyle);

                if( !pCGText->m_mapTextStyle.size() )
                {
                    cerr << "Cannot find a matching style variation for " << drStyle->PK_Style_get() << endl;
                    throw "Cannot find a matching style variation";
                }

                // Be sure we have put all used styles in the map
                MapTextStyle::iterator it;
                for(it=pCGText->m_mapTextStyle.begin();it!=pCGText->m_mapTextStyle.end();++it)
                {
                    m_pOrbiterGenerator->m_htUsedStyleVariation[(*it).first]=1;
                }

                pCGText->m_rPosition.X += m_rPosition.X;  // Offset by our position
                pCGText->m_rPosition.Y += m_rPosition.Y;
            }
        }
    }

    // Add all the zones and actions

    for(size_t s=0;s<m_alDesignObjVariations.size();++s)
    {
        Row_DesignObjVariation * drOV = m_alDesignObjVariations[s];

        vector<class Row_DesignObjVariation_Zone *> vectovz;
        drOV->DesignObjVariation_Zone_FK_DesignObjVariation_getrows(&vectovz);
        for(size_t s2=0;s2<vectovz.size();++s2)
        {
            Row_DesignObjVariation_Zone * drZone = vectovz[s2];
            CGZone *oczone = new CGZone(drZone->FK_CommandGroup_D_getrow(),this);
            oczone->m_Rect = PlutoRectangle(drZone->X_get(),drZone->Y_get(),drZone->Width_get(),drZone->Height_get());
            if( oczone->m_Rect.X<=m_pOrbiterGenerator->m_sizeScreen->Width && oczone->m_Rect.Y<=m_pOrbiterGenerator->m_sizeScreen->Height )
                m_ZoneList.push_back(oczone);
        }
        if( !drOV->FK_CommandGroup_D_OnActivate_isNull() )
        {
            CGZone *oczone = new CGZone(drOV->FK_CommandGroup_D_OnActivate_getrow(),this);
            m_ZoneList.push_back(oczone);
        }
        // Use CGZone to build the actions for on load and unload
        if( !drOV->FK_CommandGroup_D_OnLoad_isNull())
        {
            CGZone *oz = new CGZone(drOV->FK_CommandGroup_D_OnLoad_getrow(),this);
            DesignObjCommandList::iterator itActions;
            for(itActions=oz->m_Commands.begin();itActions!=oz->m_Commands.end();++itActions)
            {
                CGCommand *oa = (CGCommand *) *itActions;
                m_Action_LoadList.push_back(oa);
            }
        }
        if( !drOV->FK_CommandGroup_D_OnUnload_isNull())
        {
            CGZone *oz = new CGZone(drOV->FK_CommandGroup_D_OnUnload_getrow(),this);
            DesignObjCommandList::iterator itActions;
            for(itActions=oz->m_Commands.begin();itActions!=oz->m_Commands.end();++itActions)
            {
                CGCommand *oa = (CGCommand *) *itActions;
                m_Action_UnloadList.push_back(oa);
            }
        }
        if( !drOV->FK_CommandGroup_D_OnTimeout_isNull())
        {
            CGZone *oz = new CGZone(drOV->FK_CommandGroup_D_OnTimeout_getrow(),this);
            DesignObjCommandList::iterator itActions;
            for(itActions=oz->m_Commands.begin();itActions!=oz->m_Commands.end();++itActions)
            {
                CGCommand *oa = (CGCommand *) *itActions;
                m_Action_TimeoutList.push_back(oa);
            }
        }
        if( !drOV->FK_CommandGroup_D_OnStartup_isNull())
        {
            CGZone *oz = new CGZone(drOV->FK_CommandGroup_D_OnStartup_getrow(),this);
            DesignObjCommandList::iterator itActions;
            for(itActions=oz->m_Commands.begin();itActions!=oz->m_Commands.end();++itActions)
            {
                CGCommand *oa = (CGCommand *) *itActions;
                m_Action_StartupList.push_back(oa);
            }
        }
    }

    if( m_iPK_CommandGroup_Touch_Extra!=0 )
    {
        Row_CommandGroup *drAG = m_mds->CommandGroup_get()->GetRow(m_iPK_CommandGroup_Touch_Extra);
        CGZone *oczone = new CGZone(drAG,this);
        m_ZoneList.push_back(oczone);
    }

    if( m_pRow_DesignObj->FK_DesignObjType_get()==DESIGNOBJTYPE_Floorplan_CONST && m_sOriginalSize.Width>0 && m_sOriginalSize.Height>0 )
    {
        // This is a floorplan object, first we're going to have to scale this ourselves
        // since the background is of an unknown size, and then
        // add all the children for this page and type
        PlutoSize cgs = m_rBackgroundPosition.Size();
        PlutoPoint cgp(cgs.Width,cgs.Height);
        PlutoSize szTargetSize(ScaleValue(&cgp,m_pOrbiterGenerator->m_pRow_Size->ScaleX_get(),m_pOrbiterGenerator->m_pRow_Size->ScaleY_get()));
        int ScaleWidth = szTargetSize.Width * 1000 / m_sOriginalSize.Width;
        int ScaleHeight = szTargetSize.Height * 1000 / m_sOriginalSize.Height;
        int ScaleFactor = min(ScaleWidth,ScaleHeight);

        // Adjust the width and the height so that we don't mess up the aspect ratio
        m_rBackgroundPosition.Width = m_sOriginalSize.Width * ScaleFactor / m_pOrbiterGenerator->m_pRow_Size->ScaleX_get();
        m_rBackgroundPosition.Height = m_sOriginalSize.Height * ScaleFactor / m_pOrbiterGenerator->m_pRow_Size->ScaleY_get();
        m_rPosition.Width = m_rBackgroundPosition.Width;
        m_rPosition.Height = m_rBackgroundPosition.Height;

        int FloorplanType = atoi(GetParm(DESIGNOBJPARAMETER_Type_CONST).c_str());
        Row_FloorplanType *drFloorplanType = m_mds->FloorplanType_get()->GetRow(FloorplanType);

//      string SQL = "SELECT * FROM " + string(DEVICE_DEVICEDATA_TABLE) +
        string SQL= "JOIN " + string(DEVICE_TABLE) + " ON " + DEVICE_DEVICEDATA_FK_DEVICE_FIELD + "=" + DEVICE_PK_DEVICE_FIELD +
            " JOIN " + FLOORPLANOBJECTTYPE_TABLE + " ON " + DEVICE_DEVICEDATA_IK_DEVICEDATA_FIELD + "=" + FLOORPLANOBJECTTYPE_PK_FLOORPLANOBJECTTYPE_FIELD +
            " WHERE " + DEVICE_FK_INSTALLATION_FIELD + "=" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Device->FK_Installation_get()) +
            " AND " + DEVICE_DEVICEDATA_FK_DEVICEDATA_FIELD + "=" + StringUtils::itos(DEVICEDATA_PK_FloorplanObjectType_CONST) +
            " AND " + FLOORPLANOBJECTTYPE_FK_FLOORPLANTYPE_FIELD + "=" + StringUtils::itos(FloorplanType);

        vector<Row_Device_DeviceData *> vectRow_Device_DeviceData;
        m_mds->Device_DeviceData_get()->GetRows(SQL,&vectRow_Device_DeviceData);
        for(size_t s=0;s<vectRow_Device_DeviceData.size();++s)
        {
            Row_Device_DeviceData *pRow_Device_DeviceData = vectRow_Device_DeviceData[s];
            Row_Device_DeviceData *pRow_Device_DeviceData_FPInfo = m_mds->Device_DeviceData_get()->GetRow(pRow_Device_DeviceData->FK_Device_get(),DEVICEDATA_Floorplan_Info_CONST);
            Row_Device_DeviceData *pRow_Device_DeviceData_ObjType = m_mds->Device_DeviceData_get()->GetRow(pRow_Device_DeviceData->FK_Device_get(),DEVICEDATA_PK_FloorplanObjectType_CONST);
            int FloorplanObjectType = atoi(pRow_Device_DeviceData_ObjType->IK_DeviceData_get().c_str());
            Row_FloorplanObjectType *pRow_FloorplanObjectType = m_mds->FloorplanObjectType_get()->GetRow(FloorplanObjectType);
            if( pRow_Device_DeviceData_FPInfo )
            {
                // Parse the parameters to see if any of these sensors are on this page
                string::size_type pos=0;
                while(true)
                {
                    string FPInfo_Value = pRow_Device_DeviceData_FPInfo->IK_DeviceData_get();
                    string sFloorplan = StringUtils::Tokenize(FPInfo_Value,string(","),pos);
                    if( sFloorplan.length()==0 )
                        break;
                    int X = atoi(StringUtils::Tokenize(FPInfo_Value,",",pos).c_str());
                    int Y = atoi(StringUtils::Tokenize(FPInfo_Value,",",pos).c_str());
                    if( atoi(sFloorplan.c_str())==m_pOrbiterGenerator->m_iFloorplanPage )
                    {
                        m_VariableMap[VARIABLE_Array_ID_CONST] = pRow_FloorplanObjectType->Description_get();
                        m_VariableMap[VARIABLE_Array_Desc_CONST] = pRow_Device_DeviceData_FPInfo->FK_Device_getrow()->Description_get();

                        // We got ourselves an object to appear on this map
                        X = (X * ScaleFactor) / 1000;
                        Y = (Y * ScaleFactor) / 1000;
                        PlutoRectangle rectangle(m_rPosition.X+X,m_rPosition.Y+Y,0,0);
                        DesignObj_Generator *pDesignObj_Generator = new DesignObj_Generator(m_pOrbiterGenerator,pRow_FloorplanObjectType->FK_DesignObj_Control_getrow(),
                            rectangle,this,false,false);  // See if it will fill in the wdith/height automatically
                        pDesignObj_Generator->m_bCanBeHidden = false;
                        pDesignObj_Generator->m_bHideByDefault = false;
                        pDesignObj_Generator->m_bChildrenBeforeText = false;
                        pDesignObj_Generator->m_bChildrenBehind = false;
                        pDesignObj_Generator->m_bDontMergeBackground = false;
                        pDesignObj_Generator->m_iFloorplanPage = m_iFloorplanPage;
                        pDesignObj_Generator->m_iFloorplanDevice = pRow_Device_DeviceData_FPInfo->FK_Device_get();

                        // Scale these here because we need to reset all the x/y positions since we may have used a different scale factor
                        pDesignObj_Generator->ScaleAllValues(m_pOrbiterGenerator->m_pRow_Size->ScaleX_get(),m_pOrbiterGenerator->m_pRow_Size->ScaleY_get(),NULL);
                        m_alChildDesignObjs.push_back(pDesignObj_Generator);
                        pDesignObj_Generator->m_rBackgroundPosition.X=X;
                        pDesignObj_Generator->m_rBackgroundPosition.Y=Y;
                        pDesignObj_Generator->m_rPosition.X=X;
                        pDesignObj_Generator->m_rPosition.Y=Y;

                        pDesignObj_Generator->m_pFloorplanFillPoint = new PlutoPoint(pRow_FloorplanObjectType->FillX_get() * ScaleFactor / 1000,pRow_FloorplanObjectType->FillY_get() * ScaleFactor / 1000);
                    }
                }
            }
        }
    }
    if( m_pRow_DesignObj->PK_DesignObj_get()==1683 || m_pRow_DesignObj->PK_DesignObj_get()==1677 )
    {
int k=2;
    }

    // Add all child objects, except for arrays, which we just store in alArrays
    for(size_t s=0;s<m_alDesignObjVariations.size();++s)
    {
        Row_DesignObjVariation * drOV=m_alDesignObjVariations[s];

        vector<Row_DesignObjVariation_DesignObj *> vectovo;
        drOV->DesignObjVariation_DesignObj_FK_DesignObjVariation_Parent_getrows(&vectovo);
        for(size_t s2=0;s2<vectovo.size();++s2)
        {
            Row_DesignObjVariation_DesignObj * drOVO = vectovo[s2];
if( drOVO->PK_DesignObjVariation_DesignObj_get()==6312 )
{
    int k=2;
}
            drDesignObj = drOVO->FK_DesignObj_Child_getrow();
            if( m_rPosition.X+drOVO->X_get()<m_pOrbiterGenerator->m_sizeScreen->Width && m_rPosition.Y+drOVO->Y_get()<m_pOrbiterGenerator->m_sizeScreen->Height )
            {
                if( drDesignObj->FK_DesignObjType_get()==DESIGNOBJTYPE_Array_CONST )
                    alArrays.push_back(drOVO);
                else
                {
                    if( drOVO->FK_DesignObj_Child_getrow()->FK_DesignObjType_get()==DESIGNOBJTYPE_Floorplan_CONST )
                    {
                        // Add 1 child for each floorplan page
                        int PageCount=0;
                        vector<Row_Floorplan *> vectRow_Floorplan;
                        m_mds->Floorplan_get()->GetRows(string(FLOORPLAN_FK_INSTALLATION_FIELD) + " Is Null OR " + DEVICE_FK_INSTALLATION_FIELD + "=" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Device->FK_Installation_get()),&vectRow_Floorplan);
                        for(size_t s=0;s<vectRow_Floorplan.size();++s)
                        {
                            Row_Floorplan *pRow_Floorplan = vectRow_Floorplan[s];
                            m_pOrbiterGenerator->m_iFloorplanPage = ++PageCount;
                            PlutoRectangle rectangle(m_rPosition.X+drOVO->X_get(),m_rPosition.Y+drOVO->Y_get(),drOVO->Width_get(),drOVO->Height_get());
                            DesignObj_Generator *pDesignObj_Generator = new DesignObj_Generator(m_pOrbiterGenerator,drOVO->FK_DesignObj_Child_getrow(),rectangle,this,false,false);
                            pDesignObj_Generator->m_bCanBeHidden = true;
                            pDesignObj_Generator->m_bHideByDefault = true;
                            pDesignObj_Generator->m_bChildrenBeforeText = drOVO->DisplayChildrenBeforeText_get()==1;
                            pDesignObj_Generator->m_bChildrenBehind = drOVO->DisplayChildrenBehindBackground_get()==1;
                            pDesignObj_Generator->m_bDontMergeBackground = drOVO->DontMergeBackground_get()==1;
                            m_alChildDesignObjs.push_back(pDesignObj_Generator);
                        }
                    }
                    else
                    {
                        DesignObj_Generator *pDesignObj_Generator = new DesignObj_Generator(m_pOrbiterGenerator,drOVO->FK_DesignObj_Child_getrow(),PlutoRectangle(m_rPosition.X+drOVO->X_get(),m_rPosition.Y+drOVO->Y_get(),drOVO->Width_get(),drOVO->Height_get()),this,false,false);
                        if( !pDesignObj_Generator->m_pRow_DesignObjVariation )
                        {
                            cout << "Not adding object: " << drOVO->FK_DesignObj_Child_get() << " to object: " << drOVO->FK_DesignObjVariation_Parent_getrow()->FK_DesignObj_get() << " because there are no qualifying variations." << endl;
                            delete pDesignObj_Generator; // Abort adding this object as a child, there were no variations
                        }
                        else
                        {
                            pDesignObj_Generator->m_bCanBeHidden = drOVO->CanBeHidden_get()==1;
                            pDesignObj_Generator->m_bHideByDefault = drOVO->HideByDefault_get()==1;
                            pDesignObj_Generator->m_bChildrenBeforeText = drOVO->DisplayChildrenBeforeText_get()==1;
                            pDesignObj_Generator->m_bChildrenBehind = drOVO->DisplayChildrenBehindBackground_get()==1;
                            pDesignObj_Generator->m_bDontMergeBackground = drOVO->DontMergeBackground_get()==1;
                            pDesignObj_Generator->m_bTabStop = drOVO->IsTabStop_get()==1;

                            pDesignObj_Generator->m_PK_DesignObj_Up=drOVO->FK_DesignObj_Up_get();
                            pDesignObj_Generator->m_PK_DesignObj_Down=drOVO->FK_DesignObj_Down_get();
                            pDesignObj_Generator->m_PK_DesignObj_Left=drOVO->FK_DesignObj_Left_get();
                            pDesignObj_Generator->m_PK_DesignObj_Right=drOVO->FK_DesignObj_Right_get();

                            pDesignObj_Generator->m_sPK_DesignObj_TiedTo=drOVO->sFK_DesignObj_TiedTo_get();
                            pDesignObj_Generator->m_sVisibleState=StringUtils::ToUpper(drOVO->VisibleStates_get());

                            m_alChildDesignObjs.push_back(pDesignObj_Generator);
                        }
                    }
                }
            }
        }
    }

    // Don't stretch out the floorplan objects
    if( m_pRow_DesignObj->FK_DesignObjType_get()!=DESIGNOBJTYPE_Floorplan_CONST )
    {
        // First go through all of the arrays and set the appropriate bounds so we can calculate the size of this object
        for(size_t s=0;s<m_alChildDesignObjs.size();++s)
        {
            DesignObj_Generator *oco = m_alChildDesignObjs[s];
            if( oco->m_rPosition.Width>0 )
            {
                if( m_rPosition.Width>0 )
                    m_rPosition = PlutoRectangle::Union(m_rPosition,oco->m_rPosition);
                else
                    m_rPosition  = oco->m_rPosition ;
            }
        }
    }

    // Now fill in all the arrays
    for(size_t s=0;s<alArrays.size();++s)
    {
        Row_DesignObjVariation_DesignObj * drOVO = alArrays[s];

        vector<class ArrayValue *> *alArrayValues = GetArrayValues(drOVO);
        PlutoRectangle rtArray(m_rPosition.X + drOVO->X_get(),m_rPosition.Y + drOVO->Y_get(),drOVO->Width_isNull() ? 0 : drOVO->Width_get(),drOVO->Height_isNull() ?  0 : drOVO->Height_get());
        if( rtArray.Right()>m_pOrbiterGenerator->m_sizeScreen->Width || rtArray.Width==0 )
            rtArray.Width = m_pOrbiterGenerator->m_sizeScreen->Width - rtArray.Left();
        if( rtArray.Bottom()>m_pOrbiterGenerator->m_sizeScreen->Height || rtArray.Height==0 )
            rtArray.Height = m_pOrbiterGenerator->m_sizeScreen->Height - rtArray.Top();

        CGArray *pCGArray = new CGArray(this,drOVO,alArrayValues,rtArray,0,0);
        if( pCGArray->m_bContainsMore)
        {
            if( m_alMPArray.size()>0 )
                throw "There is more than one multi-page array for object variation: " + drOVO->PK_DesignObjVariation_DesignObj_get();

            m_alMPArray.push_back(pCGArray);
            int Page=1;

            while( pCGArray->m_bContainsMore )
            {
                pCGArray = new CGArray(this,drOVO,alArrayValues,rtArray,pCGArray->m_iLastVisibleArrayEntry,Page++);
                m_alMPArray.push_back(pCGArray);
            }
        }
        else
            m_alNonMPArrays.push_back(pCGArray);
    }

    int PK_DesignObj_Goto = 0;
    int tmpDesignObj_Goto;

    DesignObjZoneList::iterator itZone;
    for(itZone=m_ZoneList.begin();itZone!=m_ZoneList.end();++itZone)
    {
        CGZone *oz = (CGZone *) *itZone;
        tmpDesignObj_Goto = LookForGoto(&oz->m_Commands);
        if( tmpDesignObj_Goto==-1 )
        {
            PK_DesignObj_Goto=-1;
            break;
        }
        if( tmpDesignObj_Goto!=0 )
        {
            if( PK_DesignObj_Goto!=0 )
                throw "DesignObj " + StringUtils::itos(m_pRow_DesignObj->PK_DesignObj_get()) + " " + m_pRow_DesignObj->Description_get() + " has more than 1 goto";
            else
                PK_DesignObj_Goto = tmpDesignObj_Goto;
        }
    }

    if( PK_DesignObj_Goto!=-1 )
    {
        tmpDesignObj_Goto = LookForGoto(&m_Action_LoadList);
        if( tmpDesignObj_Goto==-1 )
            PK_DesignObj_Goto=-1;
        else if( tmpDesignObj_Goto!=0 )
        {
            if( PK_DesignObj_Goto!=0 )
                throw "DesignObj " + StringUtils::itos(m_pRow_DesignObj->PK_DesignObj_get()) + " " + m_pRow_DesignObj->Description_get() + " has more than 1 goto";
            else
                PK_DesignObj_Goto = tmpDesignObj_Goto;
        }
    }

    if( PK_DesignObj_Goto>0 )
    {
        m_sSelectedFile = "";  // Don't bother with the selected graphic
        HandleGoto(PK_DesignObj_Goto);
    }

    /* why did I have more than 1 goto???
    // Build a list of gotos
    Hashtable htGotos = new Hashtable();
    if( PK_DesignObj_Goto!=0 )
        htGotos.Add(PK_DesignObj_Goto,1);
    if( m_pRow_Device_Goto!=NULL )
    {
        m_pOrbiterGenerator->m_htUsedDevices.Add(m_pRow_Device_Goto,1);
        foreach(DataRow drobj in m_pRow_Device_Goto->FK_DeviceTemplate_getrow().dr.GetChildRows(Database_pluto_main.MyRelations.DeviceTemplate_DesignObj_FK_DeviceTemplate))
        {
            Row_DeviceTemplate_DesignObj * drMDL = new Row_DeviceTemplate_DesignObj *(drobj);
            htGotos.Add(drMDL->FK_DesignObj_get(),1);
        }
        foreach(DataRow drobj in m_pRow_Device_Goto->FK_DeviceTemplate_getrow()->FK_Device_get()Category_DataRow.dr.GetChildRows(Database_pluto_main.MyRelations.DeviceCategory_DesignObj_FK_DeviceCategory))
        {
            Row_DeviceCategory_DesignObj * drDCO = new Row_DeviceCategory_DesignObj *(drobj);
            htGotos.Add(drDCO->FK_DesignObj_get(),1);
        }
    }

    IDictionaryEnumerator enGotos = htGotos.GetEnumerator();
    while( enGotos.MoveNext() )
    {
        PK_DesignObj_Goto = Convert.ToInt32(enGotos.Key);
        HandleGoto(PK_DesignObj_Goto);
        GC.Collect(); // The background file probably took a lot of memory
        Thread.Sleep(250);  // see if this does something to let the gc work, becuase it doesn't seem to be working!!!!!
    }
    */
}

DesignObj_Generator::~DesignObj_Generator()
{
}

int DesignObj_Generator::LookForGoto(DesignObjCommandList *alCommands)
{
    // Store a list of actions that we will and delete to remove the device goto's and replace them with variable assign and goto's
    DesignObjCommandList alAddCommands,alDeleteCommands;

    int PK_DesignObj_Goto=0;

    DesignObjCommandList::iterator itActions;
    for(itActions=alCommands->begin();itActions!=alCommands->end();++itActions)
    {
        CGCommand *oca = (CGCommand *) *itActions;
        if( oca->m_PK_Command == COMMAND_Goto_Screen_CONST )
        {
            map<int, string>::iterator itParm;
            for(itParm=oca->m_ParameterList.begin();itParm!=oca->m_ParameterList.end();++itParm)
            {
                if( (*itParm).first == COMMANDPARAMETER_PK_DesignObj_CONST )
                {
                    if(PK_DesignObj_Goto!=0)
                        throw "Multiple goto's in object " + StringUtils::itos(m_pRow_DesignObj->PK_DesignObj_get()) + " " + m_pRow_DesignObj->Description_get();

                    bool b;
                    size_t pos;

                    string objgoto = SubstituteVariables((*itParm).second,&b);
                    if( (pos = objgoto.find('.'))!=string::npos )
                        objgoto = objgoto.substr(0,pos);

                    // Be sure there's no substitution
                    if( objgoto.find("<%=")==string::npos )
                    {
                        try
                        {
                            PK_DesignObj_Goto = atoi(objgoto.c_str());
                        }
                        catch(...)
                        {
                            throw "Cannot go to screen: " + (*itParm).second + " from object: " + StringUtils::itos(m_pRow_DesignObj->PK_DesignObj_get());
                        }
                    }
                }
            }
            break;
        }

        /* not neeeded anymore
        else if( oca->m_PK_Command == ACTION_SWITCH_TO_CONTROLLER_CONST )
        {
            alDeleteCommands.Add(oca);
            PK_DesignObj_Goto=-1;

            int PK_Orbiter=0;
            for(size_t s=0;s<oca->m_alParms.size();++s)
            {
                CGCommandParm *ocap = oca->m_alParms[s];
                if( ocap->m_PK_CommandParameter == C_COMMANDPARAMETER_CONTROLLER_ID_CONST )
                {
                    PK_Orbiter = atoi(ocap->m_sValue.c_str());
                    break;
                }
            }
            if( PK_Orbiter!=0 )
            {
                // Keep this controller
                Row_Orbiter * drOrbiter = m_pOrbiterGenerator->m_pRow_Orbiter;
                if( m_pOrbiterGenerator->m_htGeneratedOrbiters.ContainsKey(PK_Orbiter) )
                {
                    m_sDesignObjGoto = m_pOrbiterGenerator->m_htGeneratedOrbiters[PK_Orbiter];
                }
                else
                {
                    // temporarily change to the 'goto controller'
                    m_pOrbiterGenerator->m_pRow_Orbiter=m_mds->Orbiter_get()->GetRow(PK_Orbiter);
                    m_bDontShare=true;
                    int MainMenu = m_pOrbiterGenerator->m_pRow_DesignObj_MainMenu->PK_DesignObj_get();
                    HandleGoto(MainMenu);
                    ArrayList al = (ArrayList) m_pOrbiterGenerator->m_htGeneratedScreens[MainMenu];
                    int Version = al.Count-1;
                    m_sDesignObjGoto = StringUtils::itos(MainMenu) + "." + StringUtils::itos(Version) + ".0";
                    m_pOrbiterGenerator->m_htGeneratedOrbiters[PK_Orbiter] = m_sDesignObjGoto;
                    m_pOrbiterGenerator->m_pRow_Orbiter = drOrbiter; // Change back
                }
                CGCommand *oa = new CGCommand(COMMAND_Goto_Screen_CONST,this);
                oa->m_alParms.push_back(new CGCommandParm(COMMANDPARAMETER_PK_DesignObj_CONST,m_sDesignObjGoto));
                alAddCommands.push_back(oa);
            }
        }
        */
    }

    for(itActions=alDeleteCommands.begin();itActions!=alDeleteCommands.end();++itActions)
    {
        CGCommand *oa = (CGCommand *) *itActions;
// todo C++     alCommands->erase(oa);
    }
    for(itActions=alAddCommands.begin();itActions!=alAddCommands.end();++itActions)
    {
        CGCommand *oa = (CGCommand *) *itActions;
// todo C++     alCommands->erase(oa);
    }
    return PK_DesignObj_Goto;
}

void DesignObj_Generator::HandleGoto(int PK_DesignObj_Goto)
{
    listDesignObj_Generator *pListScreens = m_pOrbiterGenerator->m_htGeneratedScreens[PK_DesignObj_Goto];
    if( PK_DesignObj_Goto==m_pOrbiterGenerator->m_pRow_DesignObj_MainMenu->PK_DesignObj_get() )
    {
        return; // All the main menu's are automatically included.  The system will figure out which one at runtime
    }

    if( !pListScreens )
    {
        pListScreens = new listDesignObj_Generator();
        m_pOrbiterGenerator->m_htGeneratedScreens[PK_DesignObj_Goto] = pListScreens;
    }
    else if( pListScreens->size()==0 )
    {
        // This means the screen is currently being created, and there's a goto that loops back to itself
        // because the object is added to the list only after it's created.  Do nothing in this case.
        return;
    }

    // Does this exist already?
    if( pListScreens->size()==1 && !m_bDontShare )
        m_DesignObj_GeneratorGoto = (*pListScreens)[0];
    else
    {
        // We need to generate the screen
        map<int,string> htNewVariables;
        map<int,string>::iterator it;
        for(it=m_pOrbiterGenerator->m_mapVariable.begin(); it!=m_pOrbiterGenerator->m_mapVariable.end(); ++it)
            htNewVariables[(*it).first]=(*it).second;

        // See if we have any variable assigns in our actions
        DesignObjZoneList::iterator itZone;
        for(itZone=m_ZoneList.begin();itZone!=m_ZoneList.end();++itZone)
        {
            CGZone *oz = (CGZone *) *itZone;

            DesignObjCommandList::iterator itActions;
            for(itActions=oz->m_Commands.begin();itActions!=oz->m_Commands.end();++itActions)
            {
                CGCommand *oa = (CGCommand *) *itActions;
                if( oa->m_PK_Command == COMMAND_Set_Variable_CONST )
                {
                    int PK_Variable=0;
                    string oValue="";
                    map<int, string>::iterator itParm;
                    for(itParm=oa->m_ParameterList.begin();itParm!=oa->m_ParameterList.end();++itParm)
                    {
                        if( (*itParm).first == COMMANDPARAMETER_PK_Variable_CONST )
                            PK_Variable = atoi((*itParm).second.c_str());
                        else if( (*itParm).first == COMMANDPARAMETER_Value_To_Assign_CONST )
                            oValue = (*itParm).second;
                    }

                    if( PK_Variable>0 )
                        htNewVariables[PK_Variable]=oValue;
                }
            }
        }

        Row_DesignObj * drDesignObj_new = m_mds->DesignObj_get()->GetRow(PK_DesignObj_Goto);
        if( !drDesignObj_new )
            throw "Illegal attempt to goto screen: " + StringUtils::itos(PK_DesignObj_Goto) + " from: " + StringUtils::itos(m_pRow_DesignObj->PK_DesignObj_get()) + " screen: " + StringUtils::itos(m_pOrbiterGenerator->m_iPK_DesignObj_Screen);

        map<int,string> *htPriorVariables = &m_pOrbiterGenerator->m_mapVariable;
        m_pOrbiterGenerator->m_mapVariable = htNewVariables;


        m_pOrbiterGenerator->m_iPK_DesignObj_Screen = PK_DesignObj_Goto;
        m_DesignObj_GeneratorGoto = new DesignObj_Generator(m_pOrbiterGenerator,drDesignObj_new,PlutoRectangle(0,0,0,0),NULL,pListScreens->size()==0,false);
//  I think this is not needed since the if( bAddToGenerated at the top of the constructor does this since pListScreens->size()==0???       pListScreens->push_back(m_DesignObj_GeneratorGoto);
//      m_pOrbiterGenerator->m_mapVariable = htPriorVariables;
        if( m_bDontShare )
            m_DesignObj_GeneratorGoto->m_iVersion=(int) pListScreens->size();
    }
}

void DesignObj_Generator::PickVariation(OrbiterGenerator *pGenerator,class Row_DesignObj *drDesignObj,class Row_DesignObjVariation **drDesignObjVariation,class Row_DesignObjVariation **drStandardVariation, vector<class Row_DesignObjVariation *> *alDesignObjVariations)
{
    vector<class Row_DesignObjVariation *> vectrov;
    drDesignObj->DesignObjVariation_FK_DesignObj_getrows(&vectrov);
    if( vectrov.size()==0 )
        throw "No variation for object: " + StringUtils::itos(drDesignObj->PK_DesignObj_get());

    for(size_t s=0;s<vectrov.size();++s)
    {
        Row_DesignObjVariation *drOV = vectrov[s];
        // see if a specific criteria was specified for this skin
        Row_Device *pDevice = pGenerator->mds.Device_get()->GetRow(pGenerator->m_pRow_Orbiter->PK_Orbiter_get());

        if( !pGenerator->m_pRow_Skin->FK_Criteria_D_isNull() )
        {
            if( drOV->FK_Criteria_D_isNull() && pGenerator->m_pRow_Skin->MergeStandardVariation_get()==1 )
            {
                *drStandardVariation = drOV;
                alDesignObjVariations->push_back(drOV);
            }
            else if( drOV->FK_Criteria_D_get() == pGenerator->m_pRow_Skin->FK_Criteria_D_get() )
            {
                *drDesignObjVariation = drOV;
                alDesignObjVariations->push_back(drOV);
            }
        }
        else
        {
            // We're going to have to figure it out ourselves

            // HACK _- todo
            if( !drOV->FK_Criteria_D_isNull() && drOV->FK_Criteria_D_get()!=1 && pDevice->FK_DeviceTemplate_get()==8 )  // normal tablet
                continue;  // Don't include the audi mmi prototype on the phone
            if( !drOV->FK_Criteria_D_isNull() && drOV->FK_Criteria_D_get()!=2 && pDevice->FK_DeviceTemplate_get()==24 )   // phone
                continue;  // Don't include the phone on a the audi mmi prototype

            // hack this in for audi
            *drStandardVariation = drOV;        //hack
            alDesignObjVariations->push_back(drOV);     // hack
        }
/*
        if( drOV->FK_Criteria_D_isNull() )
        {
            *drStandardVariation = drOV;
            alDesignObjVariations->push_back(drOV);
        }  // criteria: 3=pda, 4=non pda, 5=vga
        else if( drOV->FK_Criteria_D_isNull() || (pGenerator->m_pRow_Size->PK_Size_get()==1 && drOV->FK_Criteria_D_get()==3) ||
            (pGenerator->m_pRow_Size->PK_Size_get()!=1 && (drOV->FK_Criteria_D_get()==4 || drOV->FK_Criteria_D_get()==5)) )
        {
            *drDesignObjVariation = drOV;
            alDesignObjVariations->push_back(drOV);
        }
*/
    }

    // It's possible there's just no standard variation.  In that case the chosen variation will be standard.
    if( *drStandardVariation==NULL && *drDesignObjVariation!=NULL )
    {
        *drStandardVariation = *drDesignObjVariation;
    }
    else if( *drDesignObjVariation==NULL )
    {
        if( *drStandardVariation==NULL )  // Confirm they're not both null
        {
            cerr << "WARNING: Cannot find any variation for object: " << drDesignObj->PK_DesignObj_get() << endl;
        }
        else
            *drDesignObjVariation = *drStandardVariation;
    }
}

void DesignObj_Generator::PickStyleVariation(class Row_Style * drStyle,OrbiterGenerator *pGenerator,MapTextStyle &mapTextStyle)
{
    vector<Row_StyleVariation *> vectrsv;
    drStyle->StyleVariation_FK_Style_getrows(&vectrsv);
    if( vectrsv.size()==0 )
    {
        cerr << "WARNING! No variation for style: " << StringUtils::itos(drStyle->PK_Style_get()) << " reverting to 1" << endl;
        drStyle = drStyle->Table_Style_get()->GetRow(1);
        drStyle->StyleVariation_FK_Style_getrows(&vectrsv);
        if( vectrsv.size()==0 )
            throw "No variation for style: " + StringUtils::itos(drStyle->PK_Style_get());
    }

    // Find the best style for each variation
    TextStyle *pTextStyle = PickStyleVariation(vectrsv,pGenerator,0);
    if( pTextStyle )
    {
#pragma warning("if style specifies alternates for selected, highlighted, alt pick them and add them to the map && get rid of version below")
        pTextStyle->m_iVersion=0;
        mapTextStyle[0]=pTextStyle;
    }
}

TextStyle *DesignObj_Generator::PickStyleVariation(vector<Row_StyleVariation *> &vectrsv,OrbiterGenerator *pGenerator,int Version)
{
    class Row_StyleVariation *drCorrectMatch=NULL;

    Row_StyleVariation * drSV_NoSkin = NULL, *drSV_NoCriteria = NULL, *drSV_Neither = NULL;
    for(size_t s=0;s<vectrsv.size();++s)
    {
        Row_StyleVariation * drStyleVariation = vectrsv[s];
        if( !drStyleVariation->FK_Skin_isNull() && drStyleVariation->FK_Skin_get()!=pGenerator->m_pRow_Skin->PK_Skin_get() )
            continue;

        if( !drStyleVariation->FK_Criteria_D_isNull() && (
            (drStyleVariation->FK_Criteria_D_get()==3 && pGenerator->m_pRow_Size->PK_Size_get()!=1) ||
            (drStyleVariation->FK_Criteria_D_get()!=3 && pGenerator->m_pRow_Size->PK_Size_get()==1) ) )
                continue;

        // We don't have a confirmed non-match
        if( drStyleVariation->FK_Criteria_D_isNull() && drStyleVariation->FK_Skin_isNull() )
            drSV_Neither = drStyleVariation;
        else if( drStyleVariation->FK_Criteria_D_isNull() )
            drSV_NoCriteria = drStyleVariation;
        else if( drStyleVariation->FK_Skin_isNull() )
            drSV_NoSkin = drStyleVariation;
        else
        {
            drCorrectMatch= drStyleVariation;
            break;
        }
    }

    if( !drCorrectMatch )
    {
        if( drSV_NoCriteria!=NULL )
            drCorrectMatch= drSV_NoCriteria;
        else if( drSV_NoSkin!=NULL )
            drCorrectMatch= drSV_NoSkin;
        else
            drCorrectMatch= drSV_Neither;
    }

    TextStyle *pTextStyle = pGenerator->m_mapTextStyle_Find(drCorrectMatch->FK_Style_get());
    if( !pTextStyle )
    {
        pTextStyle = new TextStyle(drCorrectMatch);
        pTextStyle->m_iPixelHeight = drCorrectMatch->PixelHeight_get() * pGenerator->m_pRow_Size->ScaleY_get() / 1000;
        pGenerator->m_mapTextStyle[drCorrectMatch->FK_Style_get()]=pTextStyle;
    }
    return pTextStyle;
}

vector<class ArrayValue *> *DesignObj_Generator::GetArrayValues(Row_DesignObjVariation_DesignObj * drOVO)
{
    vector<class ArrayValue *> *alArray = new vector<class ArrayValue *>;

    Row_DesignObjVariation *drDesignObjVariation=NULL,*drStandardVariation=NULL;
    vector<class Row_DesignObjVariation *> alDesignObjVariations;
    PickVariation(m_pOrbiterGenerator,drOVO->FK_DesignObj_Child_getrow(),&drDesignObjVariation,&drStandardVariation,&alDesignObjVariations);

    string oArray = GetParm(DESIGNOBJPARAMETER_PK_Array_CONST,drDesignObjVariation);
    if( oArray.length()==0 )
        return alArray;

    int PK_Array = atoi(oArray.c_str());
    int PriorSort=-1;
    switch(PK_Array)
    {
        case ARRAY_Lighting_Scenarios_CONST:
        case ARRAY_Climate_Scenarios_CONST:
        case ARRAY_Security_Scenarios_CONST:
        case ARRAY_Communication_Scenarios_CONST:
        case ARRAY_Misc_Scenarios_CONST:
            if( m_pOrbiterGenerator->m_pRow_Room )
            {
                int PriorSort=-1;
                vector<class Row_CommandGroup_Room *> vectAGs;
                string sql = string(COMMANDGROUP_ROOM_FK_ROOM_FIELD) + "=" +
                    StringUtils::itos(m_pOrbiterGenerator->m_pRow_Room->PK_Room_get()) + " ORDER BY " + COMMANDGROUP_ROOM_SORT_FIELD;
                m_mds->CommandGroup_Room_get()->GetRows(sql,&vectAGs);

                for(size_t s=0;s<vectAGs.size();++s)
                {
                    Row_CommandGroup_Room *drAG_R = vectAGs[s];

                    if( drAG_R->FK_CommandGroup_getrow()->FK_Array_get()==PK_Array )
                    {
                        alArray->push_back(new ArrayValue(drAG_R->FK_CommandGroup_getrow()->AltID_isNull() ? StringUtils::itos(drAG_R->FK_CommandGroup_get()) : StringUtils::itos(drAG_R->FK_CommandGroup_getrow()->AltID_get()),
                            (!drAG_R->FK_CommandGroup_getrow()->FK_Icon_isNull() ? "~cb~" : "") +  drAG_R->FK_CommandGroup_getrow()->Description_get(),
                            drAG_R->FK_CommandGroup_getrow()->FK_Icon_isNull() ? NULL : drAG_R->FK_CommandGroup_getrow()->FK_Icon_getrow(),
                            drAG_R->FK_CommandGroup_getrow()->FK_Criteria_Orbiter_isNull() ? 0 : drAG_R->FK_CommandGroup_getrow()->FK_Criteria_Orbiter_get(),
                            drAG_R->FK_CommandGroup_getrow()->FK_DesignObj_isNull() ? 0 : drAG_R->FK_CommandGroup_getrow()->FK_DesignObj_get(),
                            drAG_R->FK_CommandGroup_get(),VARIABLE_PK_CommandGroup_CONST,drAG_R->FK_CommandGroup_getrow()->CanBeHidden_get()==1,false,PriorSort==drAG_R->Sort_get()));
                        PriorSort=drAG_R->Sort_get();
                    }
                }
            }
            break;
        case ARRAY_Media_Scenarios_CONST:
            if( m_pOrbiterGenerator->m_pRow_EntertainArea==NULL )
                break;
            PriorSort=-1;

            {
                vector<class Row_CommandGroup_EntertainArea *> vectEGs;
                string sql = string(COMMANDGROUP_ENTERTAINAREA_FK_ENTERTAINAREA_FIELD) + "=" + \
                    StringUtils::itos(m_pOrbiterGenerator->m_pRow_EntertainArea->PK_EntertainArea_get()) + " ORDER BY " + COMMANDGROUP_ENTERTAINAREA_SORT_FIELD;
                m_mds->CommandGroup_EntertainArea_get()->GetRows(sql,&vectEGs);

                for(size_t s=0;s<vectEGs.size();++s)
                {
                    Row_CommandGroup_EntertainArea *drAG_E = vectEGs[s];

                    if( drAG_E->FK_CommandGroup_getrow()->FK_Array_get()==PK_Array )
                    {
                        alArray->push_back(new ArrayValue(drAG_E->FK_CommandGroup_getrow()->AltID_isNull() ? StringUtils::itos(drAG_E->FK_CommandGroup_get()) : StringUtils::itos(drAG_E->FK_CommandGroup_getrow()->AltID_get()),
                            (!drAG_E->FK_CommandGroup_getrow()->FK_Icon_isNull() ? "~cb~" : "") +  drAG_E->FK_CommandGroup_getrow()->Description_get(),
                            drAG_E->FK_CommandGroup_getrow()->FK_Icon_isNull() ? NULL : drAG_E->FK_CommandGroup_getrow()->FK_Icon_getrow(),
                            drAG_E->FK_CommandGroup_getrow()->FK_Criteria_Orbiter_isNull() ? 0 : drAG_E->FK_CommandGroup_getrow()->FK_Criteria_Orbiter_get(),
                            drAG_E->FK_CommandGroup_getrow()->FK_DesignObj_isNull() ? 0 : drAG_E->FK_CommandGroup_getrow()->FK_DesignObj_get(),
                            drAG_E->FK_CommandGroup_get(),VARIABLE_PK_CommandGroup_CONST,drAG_E->FK_CommandGroup_getrow()->CanBeHidden_get()==1,false,PriorSort==drAG_E->Sort_get()));
                        PriorSort=drAG_E->Sort_get();
                    }
                }
            }
            break;

//      case C_ARRAY_VID_TUNE_DEVICE_CONST:
//      case C_ARRAY_LINPHONES_CONST:
        case ARRAY_Entertainment_Areas_CONST:
        case ARRAY_Video_Phones_CONST:
        case ARRAY_Viewable_Cameras_CONST:
        {
            int PK_DeviceTemplate=-1;
            switch(PK_Array)
            {
                case ARRAY_Entertainment_Areas_CONST:
                    PK_DeviceTemplate = ARRAY_Entertainment_Areas_CONST;
                    break;
/*              case C_ARRAY_VID_TUNE_DEVICE_CONST:
                    PK_DeviceTemplate = DeviceTemplate_VIDTUNE_CONST;
                    break;
                case C_ARRAY_LINPHONES_CONST:
                    PK_DeviceTemplate = DeviceTemplate_LINPHONE_CONST;XGrabKeyboard
                    break;
                case C_ARRAY_ORBITER_PHONES_CONST:
                    PK_DeviceTemplate = DeviceTemplate_CE_SIP_CONST;
                    break;
                case C_ARRAY_VIEWABLE_CAMERAS_CONST:
                    PK_DeviceTemplate = DeviceTemplate_ANALOG_CAMERA_CONST;
                    break;
*/
            }

            {
                vector<class Row_Device *> vectD;
                string sql = string(DEVICE_FK_DEVICETEMPLATE_FIELD) + "=" +
                    StringUtils::itos(PK_DeviceTemplate) + " AND " + DEVICE_FK_INSTALLATION_FIELD + "=" + StringUtils::itos(m_pOrbiterGenerator->m_pRow_Device->FK_Installation_get()) + " ORDER BY " + DEVICE_DESCRIPTION_FIELD;
                m_mds->Device_get()->GetRows(sql,&vectD);

                for(size_t s=0;s<vectD.size();++s)
                {
                    Row_Device *drDevice = vectD[s];
                    alArray->push_back(new ArrayValue(StringUtils::itos(drDevice->PK_Device_get()),drDevice->Description_get(),NULL,0,0,0,VARIABLE_PK_Device_CONST,drOVO->CanBeHidden_get()==1,drOVO->HideByDefault_get()==1,false));
                }
            }
            break;
        }

		case ARRAY_All_Phones_CONST:
		case ARRAY_Hard_Phones_CONST:
		case ARRAY_Soft_Phones_CONST:
		case ARRAY_Soft_Video_Phones_CONST:
		case ARRAY_Media_Directors_CONST:
        {
            int PK_DeviceCategory=-1;
            switch(PK_Array)
            {
                case ARRAY_All_Phones_CONST:
                    PK_DeviceCategory = DEVICECATEGORY_Phones_CONST;
                    break;
                case ARRAY_Hard_Phones_CONST:
                    PK_DeviceCategory = DEVICECATEGORY_Hard_Phones_CONST;
                    break;
                case ARRAY_Soft_Phones_CONST:
                    PK_DeviceCategory = DEVICECATEGORY_Soft_Phones_CONST;
                    break;
                case ARRAY_Soft_Video_Phones_CONST:
                    PK_DeviceCategory = DEVICECATEGORY_Video_Soft_Phones_CONST;
                    break;
                case ARRAY_Media_Directors_CONST:
//                    PK_DeviceCategory = DEVICECATEGORY_HTPC_CONST;
                    break;
            }

            {
                vector<class Row_Device *> vectD;
                string sql = string("SELECT Device.* FROM Device ") + 
					"JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate " +
					"JOIN DeviceCategory ON FK_DeviceCategory=DeviceCategory.PK_DeviceCategory " +
					"LEFT JOIN DeviceCategory As ParentCategory ON DeviceCategory.FK_DeviceCategory_Parent=ParentCategory.PK_DeviceCategory " +
					"WHERE DeviceCategory.PK_DeviceCategory="+StringUtils::itos(PK_DeviceCategory)+
					" OR ParentCategory.PK_DeviceCategory="+StringUtils::itos(PK_DeviceCategory)+" OR ParentCategory.FK_DeviceCategory_Parent="+StringUtils::itos(PK_DeviceCategory)+
                    " ORDER BY Device.Description";
                m_mds->Device_get()->GetRows(sql,&vectD);

                for(size_t s=0;s<vectD.size();++s)
                {
                    Row_Device *drDevice = vectD[s];
                    alArray->push_back(new ArrayValue(StringUtils::itos(drDevice->PK_Device_get()),drDevice->Description_get(),NULL,0,0,0,VARIABLE_PK_Device_CONST,drOVO->CanBeHidden_get()==1,drOVO->HideByDefault_get()==1,false));
                }
            }
            break;
        }


        case ARRAY_Phone_Users_CONST:
        case ARRAY_All_Users_CONST:
            {
                vector<Row_Installation_Users *> vectIUs;
                m_pOrbiterGenerator->m_pRow_Device->FK_Installation_getrow()->Installation_Users_FK_Installation_getrows(&vectIUs);
                for(size_t s=0;s<vectIUs.size();++s)
                {
                    Row_Installation_Users *drIU = vectIUs[s];
                    if( PK_Array==ARRAY_Phone_Users_CONST && drIU->FK_Users_getrow()->Extension_isNull() )
                        continue;
                    alArray->push_back(new ArrayValue(StringUtils::itos(drIU->FK_Users_getrow()->PK_Users_get()),drIU->FK_Users_getrow()->UserName_get(),NULL,0,0,
                        0,0,drOVO->CanBeHidden_get()==1,drOVO->HideByDefault_get()==1,false));
                }
            }
            break;
        case ARRAY_Locations_CONST:
            {
                DequeLocationInfo::iterator it;
                for(it=m_pOrbiterGenerator->m_dequeLocation.begin();it!=m_pOrbiterGenerator->m_dequeLocation.end();++it)
                {
                    LocationInfo *li = (*it);

                    alArray->push_back(new ArrayValue(StringUtils::itos(li->iLocation) + "," + StringUtils::itos(li->PK_Room) + "," + StringUtils::itos(li->PK_EntertainArea),li->Description,li->drIcon,0,0,0,VARIABLE_PK_Device_CONST,drOVO->CanBeHidden_get()==1,drOVO->HideByDefault_get()==1,false));
                }
            }
            break;
        case ARRAY_Floorplans_CONST:
            {
                vector<class Row_Floorplan *> vectFs;
                string sql = string(FLOORPLAN_FK_INSTALLATION_FIELD) + "=" + \
                    StringUtils::itos(m_pOrbiterGenerator->m_pRow_Device->FK_Installation_get()) + " ORDER BY " + FLOORPLAN_PAGE_FIELD;
                m_mds->Floorplan_get()->GetRows(sql,&vectFs);

                for(size_t s=0;s<vectFs.size();++s)
                {
                    Row_Floorplan *drF = vectFs[s];

                    alArray->push_back(new ArrayValue(StringUtils::itos(drF->Page_get()),drF->Description_get(),
                        drF->FK_Icon_isNull() ? NULL : drF->FK_Icon_getrow(),
                        0,0,0,VARIABLE_PK_CommandGroup_CONST,drOVO->CanBeHidden_get()==1,drOVO->HideByDefault_get()==1,false));
                }
            }
            break;

            /*
        case C_ARRAY_ROOMS_CONST:
            DataView dv = new DataView(HADataConfiguration.m_mdsCache->Room_get(),
                ROOM_FK_INSTALLATION_FIELD + "=" + m_mds->Orbiter_get()->GetRow(iOrbiter)->FK_Installation_get(),
                ROOM_DESCRIPTION_FIELD,DataViewRowState.CurrentRows);

            for(int i=0; i<dv.Count; i++ )
            {
                Row_Room * dr = new Row_Room *(dv[i].Row);
                int Icon = dr->FK_Icon_isNull() ? 0 : dr->FK_Icon_get();
                alValues.Add(new ArrayEntry(dr->PK_Room_get(),dr->Description_get(),Icon,i,0));
            }
            break;

        case C_ARRAY_LIGHTS_IN_ROOM_CONST:
        case C_ARRAY_DEVICES_IN_ROOM_CONST:
            string DeviceFilter = "";
            if( PK_Array==C_ARRAY_LIGHTS_IN_ROOM_CONST )
                DeviceFilter = DEVICE_FK_DEVICECATEGORY_FIELD + "=" + DEVICECATEGORY_LIGHTS_CONST;
            else
                DeviceFilter = DEVICE_FK_DEVICECATEGORY_FIELD + "<>" + DEVICECATEGORY_LIGHTS_CONST + " AND " +
                    DEVICE_FK_OBJECTHEADER_FIELD + " IS NOT NULL";

            int Room = Convert.ToInt32(m_VariableMap[VARIABLE_PK_Room_CONST]);

            DataView dv = new DataView(HADataConfiguration.m_mdsCache->Device_get(),
                DEVICE_FK_ROOM_FIELD + "=" + Room + " AND " +
                DeviceFilter,
                DEVICE_DESCRIPTION_FIELD,DataViewRowState.CurrentRows);

            for(int i=0; i<dv.Count; i++ )
            {
                Row_Device * dr = new Row_Device *(dv[i].Row);
                alValues.Add(new ArrayEntry(dr->PK_Device_get(),dr->Description_get(),0,i,0));
            }
            break;

        case C_ARRAY_ROOMS_LIGHTS_CONST:
        case PK_Array==C_ARRAY_ROOMS_DEVICES_CONST:
            SqlCommand cmd = new SqlCommand("SELECT * FROM " +
                (PK_Array==C_ARRAY_ROOMS_LIGHTS_CONST ? "v_RoomsAndLights" : "v_RoomsAndDevices") +
                " WHERE FK_Installation=" + m_mds->Orbiter_get()->GetRow(iOrbiter)->FK_Installation_get() +
                " ORDER BY Description",
                m_mds.m_conn,NULL);

            SqlDataReader dr = cmd.ExecuteReader();
            int i=0;

            while(dr.Read())
                alValues.Add(new ArrayEntry(Convert.ToInt32(dr["PK_Device"]),dr["Description"].ToString(),0,i++,0));
            break;
        */
    }
    return alArray;

}

void DesignObj_Generator::ScaleAllValues(int FactorX,int FactorY,class DesignObj_Generator *pTopmostObject)
{
if( this->m_pRow_DesignObj->PK_DesignObj_get()==2634 )
{
int k=2;
}
    if( !m_bValuesScaled )
    {
        PlutoPoint p3(m_rBackgroundPosition.Location());
        PlutoPoint p2(ScaleValue(&p3,FactorX,FactorY));
        m_rBackgroundPosition.Location(p2);
        PlutoSize plutoSize = m_rBackgroundPosition.Size();
        PlutoPoint p(plutoSize.Width,plutoSize.Height);
        PlutoSize plutoSize2(ScaleValue(&p,FactorX,FactorY));
        m_rBackgroundPosition.Size(plutoSize2);
        p=ScaleValue(&m_rPosition.Location(),FactorX,FactorY);
        m_rPosition.Location(p);
        PlutoSize plutoSize3=m_rPosition.Size();
        p=PlutoPoint(plutoSize3.Width,plutoSize3.Height);
        p=PlutoPoint(ScaleValue(&p,FactorX,FactorY));
        m_rPosition.Size(p);
        for(size_t s=0;s<m_vectDesignObjText.size();++s)
        {
            CGText *ot = (CGText *) m_vectDesignObjText[s];
            PlutoPoint p5 = ot->m_rPosition.Location();
            PlutoSize plutoSize4(*(ScaleValue(&p5,FactorX,FactorY)));
            ot->m_rPosition.Location(PlutoPoint(plutoSize4.Width,plutoSize4.Height));
            PlutoSize plutoSize5(ot->m_rPosition.Size());  // Linux won't let me pass these in
            //PlutoPoint p4(ot->m_rPosition.Size()); // Works on vs.net, but not gcc
            PlutoPoint p4(plutoSize5.Width,plutoSize5.Height);
            PlutoPoint p(ScaleValue(&p4,FactorX,FactorY));
            ot->m_rPosition.Size(p);
        }

        m_bValuesScaled=true;
    }

    for(size_t s=0;s<m_alChildDesignObjs.size();++s)
    {
        DesignObj_Generator *oc = m_alChildDesignObjs[s];
        oc->ScaleAllValues(FactorX,FactorY,pTopmostObject);
    }

    for(size_t s=0;s<m_alNonMPArrays.size();++s)
    {
        CGArray *oa = m_alNonMPArrays[s];
        if( pTopmostObject )
            pTopmostObject->m_bContainsArrays=true;

        for(size_t s2=0;s2<oa->m_alChildDesignObjs.size();++s2)
        {
            DesignObj_Generator *oc = oa->m_alChildDesignObjs[s2];
            oc->ScaleAllValues(FactorX,FactorY,pTopmostObject);
        }
    }
    for(size_t s=0;s<m_alMPArray.size();++s)
    {
        CGArray *oa = m_alMPArray[s];
        if( pTopmostObject )
            pTopmostObject->m_bContainsArrays=true;

		for(size_t s2=0;s2<oa->m_alChildDesignObjs.size();++s2)
        {
            DesignObj_Generator *oc = oa->m_alChildDesignObjs[s2];
            oc->ScaleAllValues(FactorX,FactorY,pTopmostObject);
        }
    }
}

PlutoPoint *DesignObj_Generator::ScaleValue(PlutoPoint *pt,int FactorX,int FactorY)
{
    return new PlutoPoint(pt->X * FactorX / 1000,pt->Y * FactorY / 1000);
}

string DesignObj_Generator::GetParm(int PK_DesignObjParameter)
{
    return GetParm(PK_DesignObjParameter,m_pRow_DesignObjVariation,false);
}
string DesignObj_Generator::GetParm(int PK_DesignObjParameter,bool bReplaceNulls)
{
    return GetParm(PK_DesignObjParameter,m_pRow_DesignObjVariation,bReplaceNulls);
}

string DesignObj_Generator::GetParm(int PK_DesignObjParameter,Row_DesignObjVariation * drDesignObjVariation)
{
    return GetParm(PK_DesignObjParameter,drDesignObjVariation,false);
}
string DesignObj_Generator::GetParm(int PK_DesignObjParameter,Row_DesignObjVariation * drDesignObjVariation,bool bReplaceNulls)
{
    string oValue="";

    try
    {
        Row_DesignObjVariation_DesignObjParameter * drODP = m_mds->DesignObjVariation_DesignObjParameter_get()->GetRow(drDesignObjVariation->PK_DesignObjVariation_get(),PK_DesignObjParameter);

        if( drDesignObjVariation->PK_DesignObjVariation_get()!=m_pRow_DesignObjVariation_Standard->PK_DesignObjVariation_get() &&
            (!drODP || (drODP->Value_isNull() && bReplaceNulls)) )
        {
            drODP = m_mds->DesignObjVariation_DesignObjParameter_get()->GetRow(m_pRow_DesignObjVariation_Standard->PK_DesignObjVariation_get(),PK_DesignObjParameter);
        }

        if( !drODP )
            return NULL;

        bool btemp;
        return SubstituteVariables(drODP->Value_get(),&btemp);
    }
    catch(...) {}

    return oValue;
}

string DesignObj_Generator::SubstituteVariables(string Text,bool *bContainsRunTimeVariables)
{
    *bContainsRunTimeVariables=false;
    size_t Pos = Text.find("<%=");
    while( Pos!=string::npos )
    {
        size_t EndPos = Text.find("%>",Pos);
        string sVariable = Text.substr(Pos+3,EndPos-Pos-3);
        bool bRunTime=true;  // If there's a :0, it means it is not run-time
        size_t Option=sVariable.find(':');
        if( Option!=string::npos && sVariable.substr(Option+1,1)=="0" )
            bRunTime = false;

        string sValue="**NO~VALUE**";  // Use a special value to differentiate between a real "empty" vs. a no-match

        if( sVariable[0]=='!' )
            sValue = StringUtils::itos(m_pOrbiterGenerator->m_pRow_Orbiter->PK_Orbiter_get());
        else if( sVariable.substr(0,2)=="MM" )
            sValue = StringUtils::itos(m_pOrbiterGenerator->m_pRow_DesignObj_MainMenu->PK_DesignObj_get());
        else if( sVariable.substr(0,2)=="MS" )
        {
            if( m_pOrbiterGenerator->m_pRow_DesignObj_Sleeping )
                sValue = StringUtils::itos(m_pOrbiterGenerator->m_pRow_DesignObj_Sleeping->PK_DesignObj_get());
            else
                sValue = StringUtils::itos(m_pOrbiterGenerator->m_pRow_DesignObj_MainMenu->PK_DesignObj_get());
        }
        else if( sVariable[0]=='U' && sVariable.length()>1 )  // <%=U592%> means user 592.  <%=U%> means current user, and is evaluated at runtime
        {
            int PK_User = atoi(sVariable.substr(2).c_str());
            Row_Users * drUser = m_mds->Users_get()->GetRow(PK_User);

            if( !drUser )
            {
                throw "User: " + StringUtils::itos(PK_User) + " in text " + Text + " is invalid";
            }

            sValue = drUser->Nickname_isNull() ? drUser->UserName_get() : drUser->Nickname_get();
        }
        /* todo
        else if( sVariable.substr(0,2)=="SP" )
        {
            Row_Device * drDeviceOrbiter = m_mds->Device_get()->GetRow(m_pOrbiterGenerator->m_pRow_Orbiter->PK_Orbiter_get());
            vector<Row_Device *> vectd;
            drDeviceOrbiter->Device_FK_Device_ControlledVia_getrows(&vectd);
            for(size_t s=0;s<vectd.size();++s)
            {
                Row_Device * drDeviceChild = vectd[s];
                if( drDeviceChild->FK_DeviceTemplate_get()==DeviceTemplate_CE_SIP_CONST )
                {
                    sValue = StringUtils::itos(drDeviceChild->PK_Device_get());
                    break;
                }
            }
        }
*/
        else if( sVariable[0]=='V' )
        {
            sValue = StringUtils::itos(m_iVersion);
        }
        else if( sVariable.substr(0,2)=="IM" )
        {
            if( m_pOrbiterGenerator->m_pRow_Icon!=NULL )
                sValue = m_pOrbiterGenerator->m_pRow_Icon->MainFileName_get();
            else
                sValue = "";
        }
        else if( sVariable.substr(0,2)=="IS" )
        {
            if( m_pOrbiterGenerator->m_pRow_Icon!=NULL && !m_pOrbiterGenerator->m_pRow_Icon->SelectedFileName_isNull() )
                sValue = m_pOrbiterGenerator->m_pRow_Icon->SelectedFileName_get();
            else
                sValue = "";
        }
        else if( sVariable.substr(0,2)=="IA" )
        {
            if( m_pOrbiterGenerator->m_pRow_Icon!=NULL && !m_pOrbiterGenerator->m_pRow_Icon->AltFileNames_isNull() )
                sValue = m_pOrbiterGenerator->m_pRow_Icon->AltFileNames_get();
            else
                sValue = "";
        }
        else if( sVariable[0]=='I' )
        {
            sValue = StringUtils::itos(m_pOrbiterGenerator->m_pRow_Device->FK_Installation_get());
        }
        else
        {
            try
            {
                int PK_Variable = atoi(Option>0 ? sVariable.substr(0,Option).c_str() : sVariable.c_str());

                if( bRunTime )
                {
                    *bContainsRunTimeVariables=true;
                }
                else
                {
                    if( PK_Variable==VARIABLE_Array_Desc_CONST )
                        sValue = m_pOrbiterGenerator->m_sDesc_Array;
                    else if( PK_Variable==VARIABLE_Array_ID_CONST )
                        sValue = m_pOrbiterGenerator->m_iID_Array;
                    else
                    {
                        string o = m_pOrbiterGenerator->m_mapVariable[PK_Variable];
                        if( o.length()>0 )
                            sValue = o;
                    }
                }
            }
            catch(...)
            {
                *bContainsRunTimeVariables=true;
                sValue=""; // just to be sure
            }  // Maybe it's a <%=S%> or some other locally used substitution, just let it go through
        }

        if( sValue!="**NO~VALUE**" )
        {
            Text = Text.substr(0,Pos) + sValue + Text.substr(EndPos+2);
            Pos = Text.find("<%=",Pos);  // Maybe the string substituted also has a substitute
        }
        else
            Pos = Text.find("<%=",Pos+1);
    }

    return Text;
}
