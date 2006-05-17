#include "PlutoMediaFile.h"

#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/Other.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Table_DeviceTemplate_MediaType.h"
#include "pluto_main/Table_Installation.h"

#include "pluto_media/Table_File.h"
#include "pluto_media/Table_Picture_File.h"
#include "pluto_media/Table_Picture.h"
#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Table_Picture_Attribute.h"
#include "pluto_media/Table_File_Attribute.h"
#include "pluto_media/Define_AttributeType.h"

#include "Media_Plugin/MediaAttributes_LowLevel.h"
#include "UpdateMedia/PlutoMediaAttributes.h"

#ifndef WIN32
#include "id3info/id3info.h"
#endif
#include "MediaIdentifier.h"

using namespace std;
using namespace DCE;

#include "pluto_media/Database_pluto_media.h"

//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaFile class
//
//-----------------------------------------------------------------------------------------------------
PlutoMediaFile::PlutoMediaFile(Database_pluto_media *pDatabase_pluto_media, int PK_Installation, 
							   string sDirectory, string sFile) 
{
	//initializations
    m_pDatabase_pluto_media = pDatabase_pluto_media;
    m_sDirectory = sDirectory;
    m_sFile = sFile;
	m_nOurInstallationID = PK_Installation;
	m_nPK_MediaType = 0;

	//get the path to id3 file
	string sAttributeFile = m_sDirectory + "/" + FileWithAttributes();

	//get all attributes
	LoadPlutoAttributes(sAttributeFile);

	g_pPlutoLogger->Write(LV_WARNING, "Processing path %s, file %s. Found %d attributes in id3 file", 
		m_sDirectory.c_str(), m_sFile.c_str(), m_pPlutoMediaAttributes->m_mapAttributes.size());
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaFile::~PlutoMediaFile()
{
	SyncDbAttributes();

	delete m_pPlutoMediaAttributes;
}
//-----------------------------------------------------------------------------------------------------
/*static*/ bool PlutoMediaFile::IsSupported(string sFileName)
{
	const string csSupportedExtensions("mp3:ogg:aac:flac");
	string sExtension = StringUtils::ToLower(FileUtils::FindExtension(sFileName));
    return csSupportedExtensions.find(sExtension) != string::npos;
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::HandleFileNotInDatabase(int PK_MediaType)
{
	m_nPK_MediaType = PK_MediaType;

    // Nope.  It's either a new file, or it was moved here from some other directory.  If so,
    // then the the attribute should be set.
    int PK_File = GetFileAttribute(false);
	g_pPlutoLogger->Write(LV_STATUS, "%s/%s not IN db-attr: %d", m_sDirectory.c_str(), m_sFile.c_str(), PK_File);

    if(!PK_File)
    {
        // Is it a media file?
        if(!PK_MediaType)
		{
            PK_MediaType = PlutoMediaIdentifier::Identify(m_sDirectory + "/" + m_sFile);
			m_nPK_MediaType = PK_MediaType;
		}

		g_pPlutoLogger->Write(LV_STATUS, "Media Type is: %d", PK_MediaType);

		if(PK_MediaType)
            return AddFileToDatabase(PK_MediaType);
        else
        {
            g_pPlutoLogger->Write(LV_STATUS, "not a media file");
            return 0;
        }
    }
    else
    {
        Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(PK_File);
        if( !pRow_File )
        {
            SetFileAttribute(0);  // The file doesn't exist.  Shouldn't really happen
			g_pPlutoLogger->Write(LV_STATUS, "Huh??  not in db now");
            return 0;
        }
        // it was in the database, but it's been moved to a different directory or renamed
        pRow_File->Path_set(m_sDirectory);
        pRow_File->Filename_set(m_sFile);
        pRow_File->Table_File_get()->Commit();
    }
    return PK_File;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SyncDbAttributes()
{
	// Is it a media file?
	if(!m_nPK_MediaType)
		m_nPK_MediaType = PlutoMediaIdentifier::Identify(m_sDirectory + "/" + m_sFile);
	
	//not a media file
	if(!m_nPK_MediaType)
		return;

	int PK_File = m_pPlutoMediaAttributes->m_nFileID;

	string SQL = 
		"SELECT FK_AttributeType, Name, Track, Section "
		"FROM Attribute JOIN File_Attribute ON PK_Attribute = FK_Attribute "
		"WHERE FK_File = " + StringUtils::ltos(PK_File);

	MapPlutoMediaAttributes mapPlutoMediaAttributes;

	PlutoSqlResult result;
	MYSQL_ROW row;
	if((result.r = m_pDatabase_pluto_media->mysql_query_result(SQL)))
	{
		while((row = mysql_fetch_row(result.r)) && NULL != row[0] && NULL != row[1])
		{
			int nFK_AttributeType = atoi(row[0]);
			string sName = row[1];
			int nTrack = NULL != row[2] ? atoi(row[2]) : 0;
			int nSection = NULL != row[3] ? atoi(row[3]) : 0;

			mapPlutoMediaAttributes.insert(
				std::make_pair(
					nFK_AttributeType, 
					new PlutoMediaAttribute(nFK_AttributeType, sName, nTrack, nSection)
				)
			);
		}
	}

	//Save any new attributes in the database
	for(MapPlutoMediaAttributes::iterator it = m_pPlutoMediaAttributes->m_mapAttributes.begin(),
		end = m_pPlutoMediaAttributes->m_mapAttributes.end(); it != end; ++it)
	{
		PlutoMediaAttribute *pPlutoMediaAttribute = it->second;

		if(m_pPlutoMediaAttributes->m_mapAttributes.find(pPlutoMediaAttribute->m_nType) == 
			m_pPlutoMediaAttributes->m_mapAttributes.end())
		{
			if(pPlutoMediaAttribute->m_nType > 0)
			{
				MediaAttributes_LowLevel mediaAttributes_LowLevel(m_pDatabase_pluto_media, m_nOurInstallationID);
				Row_Attribute *pRow_Attribute = mediaAttributes_LowLevel.GetAttributeFromDescription(m_nPK_MediaType,
					pPlutoMediaAttribute->m_nType, pPlutoMediaAttribute->m_sName);

				Row_File_Attribute *pRow_File_Attribute = m_pDatabase_pluto_media->File_Attribute_get()->AddRow();
				pRow_File_Attribute->FK_File_set(PK_File);
				pRow_File_Attribute->FK_Attribute_set(pRow_Attribute->PK_Attribute_get());
				pRow_File_Attribute->Section_set(pPlutoMediaAttribute->m_nSection);
				pRow_File_Attribute->Track_set(pPlutoMediaAttribute->m_nTrack);
				pRow_File_Attribute->Table_File_Attribute_get()->Commit();

				g_pPlutoLogger->Write(LV_STATUS, "Adding attribute to database: "
					"for PK_File %d, AttrID %d, AttrType = %d with value %s, section %d, track %d", 
					PK_File, pRow_Attribute->PK_Attribute_get(), pPlutoMediaAttribute->m_nType,
					pPlutoMediaAttribute->m_sName.c_str(), pPlutoMediaAttribute->m_nSection, pPlutoMediaAttribute->m_nTrack); 
			}
		}
	}
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::AddFileToDatabase(int PK_MediaType)
{
	m_nPK_MediaType = PK_MediaType;
	Row_File *pRow_File = NULL;

	//We'll have to be sure first that we won't create duplicates; check first is there are any
	//rows we can reuse, marked as 'Missing' (deleted before because the file was removed/moved)
	//However, this should never happen. Added this code temporary to trace a possible logic flaw.
	vector<Row_File *> vectRow_File;
	m_pDatabase_pluto_media->File_get()->GetRows("Path = '" + StringUtils::SQLEscape(m_sDirectory) + 
		"' AND Filename = '" + StringUtils::SQLEscape(m_sFile) + "' AND Missing = 1", &vectRow_File);

	//Any luck to reuse a missing file record?
	if(vectRow_File.size())
	{
		g_pPlutoLogger->Write(LV_STATUS, "PlutoMediaFile::AddFileToDatabase -> there is already "
			"a record in the database marked as missing for %s - %s file", m_sDirectory.c_str(), m_sFile.c_str());

		//Get the first record and reuse it
		pRow_File = vectRow_File[0];
		pRow_File->Missing_set(1);
		pRow_File->EK_MediaType_set(PK_MediaType);
		pRow_File->Table_File_get()->Commit();

		//This will get a vector with desired File_Attribute's rows
		vector<Row_File_Attribute *> vectRow_File_Attribute;
		m_pDatabase_pluto_media->File_Attribute_get()->GetRows("FK_File = " + StringUtils::ltos(pRow_File->PK_File_get()),
			&vectRow_File_Attribute);
		
        //Iterate through the vector and mark all the rows as deleted		
		for(vector<Row_File_Attribute *>::iterator it = vectRow_File_Attribute.begin();
			it != vectRow_File_Attribute.end(); ++it
		)
		{
			Row_File_Attribute *pRow_File_Attribute = *it;
			pRow_File_Attribute->Delete();
		}

		//If there are indeed rows to be deleted, commit the changes
		if(vectRow_File_Attribute.size())
			m_pDatabase_pluto_media->File_Attribute_get()->Commit();

		g_pPlutoLogger->Write(LV_STATUS, "PlutoMediaFile::AddFileToDatabase -> reusing PK_File %d. "
			"Deleted children records.", pRow_File->PK_File_get());
	}
	else
	{
		//No record to reuse; we'll create a new one
		pRow_File = m_pDatabase_pluto_media->File_get()->AddRow();
		pRow_File->Path_set(FileUtils::ExcludeTrailingSlash(m_sDirectory));
		pRow_File->Filename_set(m_sFile);
		pRow_File->EK_MediaType_set(PK_MediaType);
		pRow_File->Table_File_get()->Commit();

		g_pPlutoLogger->Write(LV_STATUS, "PlutoMediaFile::AddFileToDatabase -> created new record PK_File %d",
			pRow_File->PK_File_get());
	}

	//These are our installation and our file
	m_pPlutoMediaAttributes->m_nInstallationID = m_nOurInstallationID;
	m_pPlutoMediaAttributes->m_nFileID = pRow_File->PK_File_get();

	//Save any attributes found in the file
	for(MapPlutoMediaAttributes::iterator it = m_pPlutoMediaAttributes->m_mapAttributes.begin(),
		end = m_pPlutoMediaAttributes->m_mapAttributes.end(); it != end; ++it)
	{
		PlutoMediaAttribute *pPlutoMediaAttribute = it->second;

		if(pPlutoMediaAttribute->m_nType > 0)
		{
			MediaAttributes_LowLevel mediaAttributes_LowLevel(m_pDatabase_pluto_media, m_nOurInstallationID);
			Row_Attribute *pRow_Attribute = mediaAttributes_LowLevel.GetAttributeFromDescription(PK_MediaType,
				pPlutoMediaAttribute->m_nType, pPlutoMediaAttribute->m_sName);

			Row_File_Attribute *pRow_File_Attribute = m_pDatabase_pluto_media->File_Attribute_get()->AddRow();
			pRow_File_Attribute->FK_File_set(pRow_File->PK_File_get());
			pRow_File_Attribute->FK_Attribute_set(pRow_Attribute->PK_Attribute_get());
			pRow_File_Attribute->Section_set(pPlutoMediaAttribute->m_nSection);
			pRow_File_Attribute->Track_set(pPlutoMediaAttribute->m_nTrack);
			pRow_File_Attribute->Table_File_Attribute_get()->Commit();

			g_pPlutoLogger->Write(LV_STATUS, "Adding attribute to database: "
				"for PK_File %d, AttrID %d, AttrType = %d with value %s, section %d, track %d", 
				pRow_File->PK_File_get(), pRow_Attribute->PK_Attribute_get(), pPlutoMediaAttribute->m_nType,
				pPlutoMediaAttribute->m_sName.c_str(), pPlutoMediaAttribute->m_nSection, pPlutoMediaAttribute->m_nTrack);
		}
	}

	// Got a picture url? Let's download it!
	if(m_pPlutoMediaAttributes->m_sPictureUrl != "")
 	{
		//It's a "new" file, but we know the picture url
		//we'll download the picture and record in Picture table
		MediaAttributes_LowLevel mediaAttributes_LowLevel(m_pDatabase_pluto_media, m_nOurInstallationID);
		Row_Picture *pRow_Picture = mediaAttributes_LowLevel.AddPicture(NULL, 0, 
			FileUtils::FindExtension(m_pPlutoMediaAttributes->m_sPictureUrl), 
			m_pPlutoMediaAttributes->m_sPictureUrl
		);

		if(pRow_Picture)
		{
			m_pPlutoMediaAttributes->m_nPictureID = pRow_Picture->PK_Picture_get();

			Row_Picture_File *pRow_Picture_File = m_pDatabase_pluto_media->Picture_File_get()->AddRow();
			pRow_Picture_File->FK_File_set(pRow_File->PK_File_get());
			pRow_Picture_File->FK_Picture_set(pRow_Picture->PK_Picture_get());
			pRow_Picture_File->Table_Picture_File_get()->Commit();

			g_pPlutoLogger->Write(LV_STATUS, "Added picture to file: PK_File %d, PK_Picture %d",
				pRow_File->PK_File_get(), m_pPlutoMediaAttributes->m_nPictureID);
		}
		else
		{
			g_pPlutoLogger->Write(LV_STATUS, "Failed to add picture to file: PK_File %d, picture url: %s",
				pRow_File->PK_File_get(), m_pPlutoMediaAttributes->m_sPictureUrl.c_str());
		}
	}

	//Save everything in id3 file
	SavePlutoAttributes(m_sDirectory + "/" + FileWithAttributes());

	g_pPlutoLogger->Write(LV_STATUS, "Added %s/%s to db with PK_File = %d", m_sDirectory.c_str(), m_sFile.c_str(),
		pRow_File->PK_File_get());

    return pRow_File->PK_File_get();
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SetFileAttribute(int PK_File)
{
	g_pPlutoLogger->Write(LV_STATUS, "SetFileAttribute %s/%s %d", m_sDirectory.c_str(), m_sFile.c_str(), PK_File);

	//make sure it's our installation and file
	m_pPlutoMediaAttributes->m_nInstallationID = m_nOurInstallationID;
	m_pPlutoMediaAttributes->m_nFileID = PK_File;

	//save 'em
	SavePlutoAttributes(m_sDirectory + "/" + FileWithAttributes());
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::GetFileAttribute(bool bCreateId3File)
{
	if(m_pPlutoMediaAttributes->m_nInstallationID == m_nOurInstallationID && m_pPlutoMediaAttributes->m_nFileID != 0)
	{
		//same installation, same file; however, it's a good record?
		Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(m_pPlutoMediaAttributes->m_nFileID);
		if(NULL != pRow_File && pRow_File->Filename_get() == m_sFile && pRow_File->Path_get() == m_sDirectory)
			return pRow_File->PK_File_get();
	}

	g_pPlutoLogger->Write(LV_STATUS, "GetFileAttribute %s/%s not found", m_sDirectory.c_str(), m_sFile.c_str());
    return 0;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SetPicAttribute(int PK_Picture, string sPictureUrl)
{
	g_pPlutoLogger->Write(LV_STATUS, "SetPicAttribute %s/%s PK_Picture %d", m_sDirectory.c_str(), m_sFile.c_str(),
		PK_Picture);

	//set the right picture id and picture url
	m_pPlutoMediaAttributes->m_nPictureID = PK_Picture;
	m_pPlutoMediaAttributes->m_sPictureUrl = sPictureUrl;

	//save 'em
	SavePlutoAttributes(m_sDirectory + "/" + FileWithAttributes());
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::GetPicAttribute(int PK_File)
{
	//got the file in the database?
    if(!PK_File)
    {
        vector<Row_File *> vectRow_File;
        m_pDatabase_pluto_media->File_get()->GetRows("Path='" + StringUtils::SQLEscape(m_sDirectory) + 
            "' AND Filename='" + StringUtils::SQLEscape(m_sFile) + "'",
            &vectRow_File);
        if( vectRow_File.size() )
            PK_File = vectRow_File[0]->PK_File_get();
        else
            return 0;  // Can do nothing.  This isn't in the database
    }

	//got any picture associated with the file?
    vector<Row_Picture_File *> vectPicture_File;
    m_pDatabase_pluto_media->Picture_File_get()->GetRows("FK_File=" + StringUtils::itos(PK_File),&vectPicture_File);
    g_pPlutoLogger->Write(LV_STATUS, "Found %d pics for file", vectPicture_File.size());
    if( vectPicture_File.size() )
    {
		long PK_Picture = vectPicture_File[0]->FK_Picture_get();
		Row_Picture *pRow_Picture = m_pDatabase_pluto_media->Picture_get()->GetRow(PK_Picture);
		string sPictureUrl = NULL != pRow_Picture ? pRow_Picture->URL_get() : "";

        SetPicAttribute(PK_Picture, sPictureUrl);
        return PK_Picture;  // The first pic for this directory
    }

    //Does one of the attributes have a picture?
    vector<Row_Picture_Attribute *> vectPicture_Attribute;
    m_pDatabase_pluto_media->Picture_Attribute_get()->GetRows(
        "JOIN File_Attribute ON Picture_Attribute.FK_Attribute=File_Attribute.FK_Attribute "
        " JOIN Attribute ON Picture_Attribute.FK_Attribute=Attribute.PK_Attribute "
        " JOIN AttributeType ON Attribute.FK_AttributeType=AttributeType.PK_AttributeType "
        " WHERE FK_File=" + 
        StringUtils::itos(PK_File) +
        " ORDER BY `PicPriority`",&vectPicture_Attribute);

	g_pPlutoLogger->Write(LV_STATUS, "Found %d pics for attribute", (int) vectPicture_Attribute.size());
    if( vectPicture_Attribute.size())
    {
		long PK_Picture = vectPicture_Attribute[0]->FK_Picture_get();
		Row_Picture *pRow_Picture = m_pDatabase_pluto_media->Picture_get()->GetRow(PK_Picture);
		string sPictureUrl = NULL != pRow_Picture ? pRow_Picture->URL_get() : "";

        SetPicAttribute(PK_Picture, sPictureUrl);
        return PK_Picture;  // The first pic for this directory
    }
    return 0;
}
//-----------------------------------------------------------------------------------------------------
string PlutoMediaFile::FileWithAttributes(bool bCreateId3File)
{
	string sFileWithAttributes = m_sFile;
	if(!IsSupported(m_sFile))
	{
		sFileWithAttributes = FileUtils::FileWithoutExtension(m_sFile) + ".id3";
		if(FileUtils::FileExists(m_sDirectory + "/" + sFileWithAttributes))
			return sFileWithAttributes;

		if(!bCreateId3File)
			return "";

		if(!FileUtils::DirExists(m_sDirectory + "/" + sFileWithAttributes))
			FileUtils::WriteTextFile(m_sDirectory + "/" + sFileWithAttributes, ""); //touch it
	}

	return sFileWithAttributes;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SavePlutoAttributes(string sFullFileName)
{
#ifndef WIN32
	//Temporary map with attributes for common tags
	map<int, string> mapAttributes;
	for(MapPlutoMediaAttributes::iterator it = m_pPlutoMediaAttributes->m_mapAttributes.begin(), 
		end = m_pPlutoMediaAttributes->m_mapAttributes.end(); it != end; ++it)
	{
		mapAttributes[it->first] = it->second->m_sName;
	}

	//Save common id3 tags
	SetId3Info(sFullFileName, mapAttributes);

	//Save user defined text
	char *pDataCurrentPosition = NULL;
	char *pDataStartPosition = NULL;
	unsigned long ulSize = 0;

	m_pPlutoMediaAttributes->SerializeWrite();
	ulSize = m_pPlutoMediaAttributes->CurrentSize();
	pDataCurrentPosition = pDataStartPosition = m_pPlutoMediaAttributes->m_pcDataBlock;

	size_t Size = ulSize;
	SetUserDefinedInformation(sFullFileName, pDataStartPosition, Size);
	m_pPlutoMediaAttributes->FreeSerializeMemory();

	g_pPlutoLogger->Write(LV_WARNING, "Saving %d attributes in the id3 file %s",
		m_pPlutoMediaAttributes->m_mapAttributes.size(), sFullFileName.c_str());
#endif
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::LoadPlutoAttributes(string sFullFileName)
{
#ifndef WIN32
	m_pPlutoMediaAttributes = new PlutoMediaAttributes();

	//deserialize data from user defined tag
	char *pData = NULL;
	size_t Size = 0;
	GetUserDefinedInformation(sFullFileName, pData, Size);

	if(NULL != pData)
	{
		m_pPlutoMediaAttributes->SerializeRead((unsigned long)Size, pData);
		delete []pData;
		pData = NULL;
	}

	//get common id3 attributes
	map<int, string> mapAttributes;
	GetId3Info(sFullFileName, mapAttributes);	

	//merge attributes
	for(map<int, string>::iterator it = mapAttributes.begin(), end = mapAttributes.end(); it != end; ++it)
	{
		int nType = it->first;
		string sValue = it->second;

		MapPlutoMediaAttributes::iterator itm = m_pPlutoMediaAttributes->m_mapAttributes.find(nType);
		if(itm == m_pPlutoMediaAttributes->m_mapAttributes.end())
			m_pPlutoMediaAttributes->m_mapAttributes.insert(
				std::make_pair(
					nType, 
					new PlutoMediaAttribute(nType, sValue)
				)
			);
		else
			itm->second->m_sName = sValue;
	}
#endif
}

//-----------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaIdentifier class
//
//-----------------------------------------------------------------------------------------------------
map<string,int> PlutoMediaIdentifier::m_mapExtensions;
//-----------------------------------------------------------------------------------------------------
/*static*/ void PlutoMediaIdentifier::Activate(Database_pluto_main *pDatabase_pluto_main)
{
	if(m_mapExtensions.size()) //activate once
		return;

	g_pPlutoLogger->Write(LV_STATUS, "Activating Pluto Media Identifier...");
	
    vector<Row_MediaType *> vectRow_MediaType;
    pDatabase_pluto_main->MediaType_get()->GetRows("1=1",&vectRow_MediaType);
    for(size_t s=0;s<vectRow_MediaType.size();++s)
    {
        Row_MediaType *pRow_MediaType = vectRow_MediaType[s];
        string::size_type pos=0;
        string sExtensions = pRow_MediaType->Extensions_get();
        while(pos < sExtensions.size() )
            m_mapExtensions[StringUtils::Tokenize(sExtensions,",",pos)] = pRow_MediaType->PK_MediaType_get();
    }

    vector<Row_DeviceTemplate_MediaType *> vectRow_DeviceTemplate_MediaType;
    pDatabase_pluto_main->DeviceTemplate_MediaType_get()->GetRows("1=1",&vectRow_DeviceTemplate_MediaType);
    for(size_t s=0;s<vectRow_DeviceTemplate_MediaType.size();++s)
    {
        Row_DeviceTemplate_MediaType *pRow_DeviceTemplate_MediaType = vectRow_DeviceTemplate_MediaType[s];
        string::size_type pos=0;
        string sExtensions = pRow_DeviceTemplate_MediaType->Extensions_get();
        while(pos < sExtensions.size() )
            m_mapExtensions[StringUtils::Tokenize(sExtensions,",",pos)] = pRow_DeviceTemplate_MediaType->FK_MediaType_get();
    }

	g_pPlutoLogger->Write(LV_STATUS, "Pluto Media Identifier activated. Extensions %d", m_mapExtensions.size());
}
//-----------------------------------------------------------------------------------------------------
/*static*/ int PlutoMediaIdentifier::Identify(string sFilename)
{
    //we'll try first to identify the media based on the extension
    //if we don't find any extention to match it, we'll use MediaIdentifier which uses the magic from files
    string sExtension = FileUtils::FindExtension(sFilename);
    map<string, int>::iterator it = m_mapExtensions.find(sExtension);
    return it != m_mapExtensions.end() ? it->second : MediaIdentifier::GetFileMediaType(sFilename);
}
//-----------------------------------------------------------------------------------------------------
