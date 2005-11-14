#include "PlutoMediaFile.h"

#include "PlutoUtils/CommonIncludes.h"

//#include "PlutoUtils/FileUtils.h"
//#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
//#include "Logger.h"
//#include "UpdateMedia.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <list>

#ifndef WIN32
#include <dirent.h>
#include <attr/attributes.h>
#endif

#include "pluto_main/Table_MediaType.h"
#include "pluto_main/Table_DeviceTemplate_MediaType.h"

#include "pluto_media/Table_File.h"
#include "pluto_media/Table_Picture_File.h"
#include "pluto_media/Table_Attribute.h"
#include "pluto_media/Table_Picture_Attribute.h"
#include "pluto_media/Table_File_Attribute.h"

#include "id3info/id3info.h"
#include "MediaIdentifier.h"

using namespace std;
using namespace DCE;

#include "pluto_media/Database_pluto_media.h"
#include "pluto_main/Database_pluto_main.h"

//-----------------------------------------------------------------------------------------------------
//
//  PlutoMediaFile class
//
//-----------------------------------------------------------------------------------------------------
PlutoMediaFile::PlutoMediaFile(Database_pluto_media *pDatabase_pluto_media, 
                               Database_pluto_main *pDatabase_pluto_main,
                               string sDirectory, string sFile)
{
    m_pDatabase_pluto_media = pDatabase_pluto_media;
    m_pDatabase_pluto_main = pDatabase_pluto_main;
    m_sDirectory = sDirectory;
    m_sFile = sFile;
}
//-----------------------------------------------------------------------------------------------------
PlutoMediaFile::~PlutoMediaFile()
{
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::HandleFileNotInDatabase(int PK_MediaType)
{
    // Nope.  It's either a new file, or it was moved here from some other directory.  If so,
    // then the the attribute should be set.
    int PK_File = GetFileAttribute();
    cout << m_sDirectory << "/" << m_sFile << " not in db-attr: " << PK_File << endl;
    if( !PK_File )
    {
        // Is it a media file?
        if(!PK_MediaType)
            PK_MediaType = PlutoMediaIdentifier::Identify(m_sDirectory + "/" + m_sFile);

        cout << "Media Type is: " << PK_MediaType << endl;

        if(PK_MediaType)
            return AddFileToDatabase(PK_MediaType);
        else
        {
            cout << "not a media file" << endl;
            return 0;
        }
    }
    else
    {
        Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->GetRow(PK_File);
        if( !pRow_File )
        {
            SetFileAttribute(0);  // The file doesn't exist.  Shouldn't really happen
            cout << "Huh??  not in db now" << endl;
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
int PlutoMediaFile::AddFileToDatabase(int PK_MediaType)
{
    Row_File *pRow_File = m_pDatabase_pluto_media->File_get()->AddRow();
    pRow_File->Path_set(m_sDirectory);
    pRow_File->Filename_set(m_sFile);
    pRow_File->EK_MediaType_set(PK_MediaType);
    pRow_File->Table_File_get()->Commit();

    // Add attributes from ID3 tags
    map<int,string> mapAttributes;
    GetId3Info(m_sDirectory + "/" + m_sFile, mapAttributes);	

    for(map<int,string>::iterator it = mapAttributes.begin(); it != mapAttributes.end(); it++)
    {
        int PK_AttrType = (*it).first;
        string sValue = (*it).second;

        if(PK_AttrType <= 0)
        {
            cout << "PK_AttrType = " << PK_AttrType << " with value " << sValue << " - does not exist" << endl;
            continue;
        }

        Row_Attribute *pRow_Attribute = m_pDatabase_pluto_media->Attribute_get()->AddRow();
        pRow_Attribute->FK_AttributeType_set(PK_AttrType);
        pRow_Attribute->Name_set(sValue);
        pRow_Attribute->Table_Attribute_get()->Commit();

        Row_File_Attribute *pRow_File_Attribute = m_pDatabase_pluto_media->File_Attribute_get()->AddRow();
        pRow_File_Attribute->FK_File_set(pRow_File->PK_File_get());
        pRow_File_Attribute->FK_Attribute_set(pRow_Attribute->PK_Attribute_get());
        pRow_File_Attribute->Table_File_Attribute_get()->Commit();

        cout << "Added PK_AttrType = " << PK_AttrType << " with value " << sValue << endl;
    }

    cout << "Added " << m_sDirectory << "/" << m_sFile << " to db " << pRow_File->PK_File_get() << endl;
    return pRow_File->PK_File_get();
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SetFileAttribute(int PK_File)
{
    cout << "SetFileAttribute " << m_sDirectory << "/" << m_sFile << " " << PK_File  << endl;
    string sPK_File = StringUtils::itos(PK_File);

#ifndef WIN32
    attr_set( (m_sDirectory + "/" + m_sFile).c_str( ), "ID", sPK_File.c_str( ), sPK_File.length( ), 0 );
#endif

    //sync id3tags on the files with the attributes from the db
    map<int,string> mapAttributes;
    GetId3Info(m_sDirectory + "/" + m_sFile, mapAttributes);

    string SQL = 
        "SELECT Attribute.FK_AttributeType, Attribute.Name FROM File_Attribute " 
        "INNER JOIN Attribute ON Attribute.PK_Attribute = File_Attribute.FK_Attribute "
        "WHERE FK_File = " + sPK_File;

    PlutoSqlResult allresult,result;
    MYSQL_ROW row;
    if((allresult.r = m_pDatabase_pluto_media->mysql_query_result(SQL)))
    {
        while((row = mysql_fetch_row(allresult.r)))
        {
            string sFK_AttributeType = row[0];
            string sName = row[1];

            //create a new entry in id3 tag list or overwrite an old one
            mapAttributes[atoi(sFK_AttributeType.c_str())] = sName;
        }
    }

    if(mapAttributes.size())
        SetId3Info(m_sDirectory + "/" + m_sFile, mapAttributes);
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::GetFileAttribute()
{
#ifdef WIN32
    vector<Row_File *> vectRow_File;
    m_pDatabase_pluto_media->File_get()->GetRows("Path='" + StringUtils::SQLEscape(m_sDirectory) + 
        "' AND Filename='" + StringUtils::SQLEscape(m_sFile) + "'", &vectRow_File);
    if( vectRow_File.size() )
        return vectRow_File[0]->PK_File_get();
    else
        return 0;
#else
    int n = 79;
    char value[80];
    memset( value, 0, sizeof( value ) );

    if ( attr_get( (m_sDirectory + "/" + m_sFile).c_str( ), "ID", value, &n, 0 ) == 0 )
    {
        cout << "GetFileAttribute " << m_sDirectory << "/" << m_sFile << " " << value << endl;
        return atoi( value );
    }
#endif

    cout << "GetFileAttribute " << m_sDirectory << "/" << m_sFile << " not found " << endl;
    return 0;
}
//-----------------------------------------------------------------------------------------------------
void PlutoMediaFile::SetPicAttribute(int PK_Picture)
{
    cout << "SetPicAttribute " << m_sDirectory << "/" << m_sFile << " " << PK_Picture  << endl;

#ifndef WIN32
    string sPK_Picture = StringUtils::itos(PK_Picture);
    if( m_sDirectory.size() )
        attr_set( (m_sDirectory + "/" + m_sFile).c_str( ), "PIC", sPK_Picture.c_str( ), sPK_Picture.length( ), 0 );
    else
        attr_set( m_sFile.c_str( ), "PIC", sPK_Picture.c_str( ), sPK_Picture.length( ), 0 );
#endif
}
//-----------------------------------------------------------------------------------------------------
int PlutoMediaFile::GetPicAttribute(int PK_File)
{
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

    vector<Row_Picture_File *> vectPicture_File;
    m_pDatabase_pluto_media->Picture_File_get()->GetRows("FK_File=" + StringUtils::itos(PK_File),&vectPicture_File);
    cout << "Found " << (int) vectPicture_File.size() << " pics for file" << endl;
    if( vectPicture_File.size() )
    {
        SetPicAttribute(vectPicture_File[0]->FK_Picture_get());
        return vectPicture_File[0]->FK_Picture_get();  // The first pic for this directory
    }

    // Does one of the attributes have a picture
    vector<Row_Picture_Attribute *> vectPicture_Attribute;
    m_pDatabase_pluto_media->Picture_Attribute_get()->GetRows(
        "JOIN File_Attribute ON Picture_Attribute.FK_Attribute=File_Attribute.FK_Attribute "
        " JOIN Attribute ON Picture_Attribute.FK_Attribute=Attribute.PK_Attribute "
        " JOIN AttributeType ON Attribute.FK_AttributeType=AttributeType.PK_AttributeType "
        " WHERE FK_File=" + 
        StringUtils::itos(PK_File) +
        " ORDER BY `PicPriority`",&vectPicture_Attribute);

    cout << "Found " << (int) vectPicture_Attribute.size() << " pics for attribute" << endl;
    if( vectPicture_Attribute.size() )
    {
        SetPicAttribute(vectPicture_Attribute[0]->FK_Picture_get());
        return vectPicture_Attribute[0]->FK_Picture_get();  // The first pic for this directory
    }
    return 0;
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
