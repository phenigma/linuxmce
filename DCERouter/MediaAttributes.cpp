#include "PlutoUtils/CommonIncludes.h"	
#include "MediaAttributes.h"
#ifndef WIN32
#include <dirent.h>
#include <attr/attributes.h>
#endif
#include "PlutoUtils/MySqlHelper.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"
#include "DCEMI_PS_Media.h"
#include "DCEMI_PS_Main.h"
#include "DCE/Logger.h"

MediaAttributes::MediaAttributes(class DCEMI_PS_Media *pDCEMI_PS_Media) 
{ 
	m_pDCEMI_PS_Media=pDCEMI_PS_Media; 
	m_pDCEMI_PS_Main=m_pDCEMI_PS_Media->m_pDCEMI_PS_Main; 
}

int MediaAttributes::CreatedMedia(int PK_MED_C_Type,string Path,listMediaAttribute *plistMediaAttribute,listMediaPicture *plistMediaPicture)
{
	PlutoSqlResult result;
	MYSQL_ROW row;
	int PK_MED_File =0;
	string SQL = "SELECT PK_MED_File FROM MED_File WHERE Path='" + StringUtils::SQLEscape(Path) + "'";
	if( (result.r=m_pDCEMI_PS_Main->mysql_query_result(SQL)) && (row=mysql_fetch_row(result.r)) )
		PK_MED_File = atoi(row[0]);
	else
	{
		SQL = "INSERT INTO MED_File(FK_MED_C_Type,Path) VALUES(" + StringUtils::itos(PK_MED_C_Type) + ",'" + StringUtils::SQLEscape(Path) + "')";
		PK_MED_File = m_pDCEMI_PS_Main->threaded_mysql_query_withID(SQL);
	}
	if( !PK_MED_File )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"failed: %s",SQL.c_str());
	}
	else
	{
		UpdatedMedia(PK_MED_File,0,"",plistMediaAttribute,plistMediaPicture);
#ifndef WIN32
		string value = StringUtils::itos(PK_MED_File);
		attr_set(Path.c_str(), "ID", value.c_str(), value.length(), 0);
#endif
	}

	printf("Created Media %s reutrning %d\n",Path.c_str(),PK_MED_File);

	return PK_MED_File;
}

void MediaAttributes::UpdatedMedia(int PK_MED_File,int PK_MED_C_Type,string Path,listMediaAttribute *plistMediaAttribute,listMediaPicture *plistMediaPicture)
{
	if( PK_MED_C_Type )
	{
		string SQL = "UPDATE MED_File SET FK_MED_C_Type=" + StringUtils::itos(PK_MED_C_Type) + 
			" WHERE PK_MED_File=" + StringUtils::itos(PK_MED_File);

		m_pDCEMI_PS_Main->threaded_mysql_query_withID(SQL);
	}
	if( Path.length()!=0 )
	{
		string SQL = "UPDATE MED_File SET Path='" + StringUtils::SQLEscape(Path) + "'" +
			" WHERE PK_MED_File=" + StringUtils::itos(PK_MED_File);

		m_pDCEMI_PS_Main->threaded_mysql_query_withID(SQL);
	}

	if( plistMediaAttribute )
	{
		for(listMediaAttribute::iterator it=plistMediaAttribute->begin();it!=plistMediaAttribute->end();++it)
		{
			MediaAttribute *ma = (*it);
			AddAttribute(PK_MED_File,ma->PK_MED_Attribute,ma->PK_MED_C_AttributeType,ma->Name,ma->FirstName);
		}
	}
	if( plistMediaPicture )
	{
		for(listMediaPicture::iterator it=plistMediaPicture->begin();it!=plistMediaPicture->end();++it)
		{
			MediaPicture *mp = (*it);
			if( mp->PK_MED_Picture )
				SetPicture(mp->PK_MED_Picture,mp->Path);
			else
				AddPicture(mp->PK_MED_File,mp->PK_MED_Attribute,mp->Path);
		}
	}
}

int MediaAttributes::AddAttribute(int PK_MED_File,int PK_MED_Attribute,int PK_MED_C_AttributeType,string Name,string FirstName)
{
	printf("Add attribute file %d att %d Name %s \n",PK_MED_File,PK_MED_Attribute,Name.c_str());

	if( (Name.length()==0 || Name=="(null)") && (FirstName.length()==0 || FirstName=="(null)") )
		return 0;
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( !PK_MED_Attribute )
	{
		bool bCombineAsOne=false;
		string SQL = "SELECT CombineAsOne FROM MED_File "\
			"JOIN MED_C_Type_C_AttributeType ON MED_File.FK_MED_C_Type=MED_C_Type_C_AttributeType.FK_MED_C_Type "\
			"WHERE FK_MED_C_AttributeType=" + StringUtils::itos(PK_MED_C_AttributeType) + " AND PK_MED_File=" + StringUtils::itos(PK_MED_File);

		if( (result.r=m_pDCEMI_PS_Main->mysql_query_result(SQL)) && (row=mysql_fetch_row(result.r)) )
		{
			if( atoi(row[0])==1 )
				bCombineAsOne=true;
		}


		if( bCombineAsOne )
		{
			SQL = "SELECT PK_MED_Attribute FROM MED_Attribute WHERE FK_MED_C_AttributeType=" + 
				StringUtils::itos(PK_MED_C_AttributeType) + " AND Name='" + 
				StringUtils::SQLEscape(Name) + "' AND FirstName='" + StringUtils::SQLEscape(FirstName) + "'";

			if(	(result.r=m_pDCEMI_PS_Main->mysql_query_result(SQL)) && (row=mysql_fetch_row(result.r)) )
			PK_MED_Attribute = atoi(row[0]);
		}

		if( !PK_MED_Attribute )
		{
			SQL = "INSERT INTO MED_Attribute(FK_MED_C_AttributeType,Name,FirstName) VALUES(" +
				StringUtils::itos(PK_MED_C_AttributeType) + ",'" + StringUtils::SQLEscape(Name) + "','" +
				StringUtils::SQLEscape(FirstName) + "')";
			PK_MED_Attribute = m_pDCEMI_PS_Main->threaded_mysql_query_withID(SQL);
		}
	}
	if( PK_MED_Attribute )  // For sure we should have it now
	{
		string SQL = "INSERT INTO MED_File_Attribute VALUES(" + StringUtils::itos(PK_MED_File) + "," +
			StringUtils::itos(PK_MED_Attribute) + ")";
		m_pDCEMI_PS_Main->threaded_mysql_query(SQL,true); // If it fails, it just means the association is already there
		UpdateSearchTokens(PK_MED_Attribute);
	}
	return PK_MED_Attribute;
}

void MediaAttributes::ChangeAttribute(int OldPK_MED_C_AttributeType,int NewPK_MED_C_AttributeType,string OldName,string OldFirstName,string NewName,string NewFirstName)
{
}

void MediaAttributes::ChangeAttribute(int PK_MED_Attribute,string NewName,string NewFirstName)
{
}

void MediaAttributes::AddPicture(int PK_MED_File,int PK_MED_Attribute,string Path)
{
	// confirm the file exists
	FILE *file = fopen(Path.c_str(),"rb");
	if( !file )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Picture file %s doesn't exist",Path.c_str());
		return;
	}
    fseek(file, 0L, SEEK_END);
    size_t file_size=ftell(file);
    fseek(file, 0L, SEEK_SET);
	char *data_block=(char *)malloc(file_size+1);
    size_t bytesread=fread(data_block,1,file_size,file);
	fclose(file);
	if( bytesread!=file_size )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Picture file %s can't be read",Path.c_str());
		return;
	}

	PlutoSqlResult result;
	MYSQL_ROW row;

	int PK_MED_Picture=0;

	string SQL;
	for(int loop=0;loop<=1;++loop) 
	{
		if( loop==0 )
		{
			if( !PK_MED_File )
				continue;
			else
				SQL = "SELECT FK_MED_Picture,Extension FROM MED_Picture_File JOIN MED_Picture ON FK_MED_Picture=PK_MED_Picture WHERE FK_MED_File=" + StringUtils::itos(PK_MED_File);
		}
		else
		{
			if( !PK_MED_Attribute )
				continue;
			else
				SQL = "SELECT FK_MED_Picture,Extension FROM MED_Picture_Attribute JOIN MED_Picture ON FK_MED_Picture=PK_MED_Picture WHERE FK_MED_Attribute=" + StringUtils::itos(PK_MED_Attribute);
		}
		if( (result.r=m_pDCEMI_PS_Main->mysql_query_result(SQL)) )
		{
			while( (row=mysql_fetch_row(result.r)) )
			{
				int PKID = atoi(row[0]);
				char *Extension = row[1];
#ifdef WIN32
				file = fopen(("\\home\\mediapics\\" + StringUtils::itos(PKID) + "." + Extension).c_str(),"rb");
#else
				file = fopen(("/home/mediapics/" + StringUtils::itos(PKID) + "." + Extension).c_str(),"rb");
#endif
				if( !file )  // Can't find picture??
				{
					SQL = "DELETE FROM MED_Picture_File WHERE FK_MED_Picture=" + StringUtils::itos(PKID);
					m_pDCEMI_PS_Main->threaded_mysql_query(SQL);
					SQL = "DELETE FROM MED_Picture_Attribute WHERE FK_MED_Picture=" + StringUtils::itos(PKID);
					m_pDCEMI_PS_Main->threaded_mysql_query(SQL);
					SQL = "DELETE FROM MED_Picture WHERE PK_MED_Picture=" + StringUtils::itos(PKID);
					m_pDCEMI_PS_Main->threaded_mysql_query(SQL);
					continue;
				}
				fseek(file, 0L, SEEK_END);
				int file_size2=ftell(file);
				if( file_size2!=file_size )
				{
					fclose(file);
					continue;
				}
				
				// They're the same size.  compare them
				fseek(file, 0L, SEEK_SET);
				char *data_block2=(char *)malloc(file_size+1);
				fread(data_block2,1,file_size,file);
				fclose(file);

				if( memcmp(data_block,data_block2,file_size)==0 )
				{
					// This is the same file
					PK_MED_Picture = PKID;
					delete[] data_block2;
					break;
				}
				delete[] data_block2;
			}
		}
	}

	if( !PK_MED_Picture )
	{
		string Extension = FileUtils::FindExtension(Path);
		string SQL = "INSERT INTO MED_Picture(Extension) VALUES('" + Extension + "')";
		PK_MED_Picture = m_pDCEMI_PS_Main->threaded_mysql_query_withID(SQL);
		FILE *file;
#ifdef WIN32
		file = fopen(("\\home\\mediapics\\" + StringUtils::itos(PK_MED_Picture) + "." + Extension).c_str(),"wb");
#else
		file = fopen(("/home/mediapics/" + StringUtils::itos(PK_MED_Picture) + "." + Extension).c_str(),"wb");
#endif
		if( !file )  
			g_pPlutoLogger->Write(LV_CRITICAL,"Cannot write out pic: %d",PK_MED_Picture);
		else
		{
			fwrite(data_block,1,file_size,file);
			fclose(file);
		}
	}
	delete[] data_block;

	if( PK_MED_Picture )  // For sure we should have it now
	{
#ifdef WIN32
		if( SetPicture(PK_MED_Picture,Path) || true )  // This will fail to do the compression
#else
		if( SetPicture(PK_MED_Picture,Path)==0 )
#endif
		{
			if( PK_MED_File )
			{
				SQL = "INSERT INTO MED_Picture_File VALUES(" + StringUtils::itos(PK_MED_Picture) + "," +
					StringUtils::itos(PK_MED_File) + ")";
				m_pDCEMI_PS_Main->threaded_mysql_query(SQL,true);
			}
			if( PK_MED_Attribute )
			{
				SQL = "INSERT INTO MED_Picture_Attribute VALUES(" + StringUtils::itos(PK_MED_Picture) + "," +
					StringUtils::itos(PK_MED_Attribute) + ")";
				m_pDCEMI_PS_Main->threaded_mysql_query(SQL,true);
			}
		}
	}
}

int MediaAttributes::SetPicture(int PK_MED_Picture,string Path)
{
	string Extension = FileUtils::FindExtension(Path);
	int result;

	// Make compressed copies
	string Cmd = "/usr/local/pluto/bin/convert -sample 100x100 /home/mediapics/" + StringUtils::itos(PK_MED_Picture) + "." + Extension + 
		" /home/mediapics/" + StringUtils::itos(PK_MED_Picture) + "_tn." + Extension;
	if( (result=system(Cmd.c_str()))!=0 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Aborting thumbnail picture %s returned %d",Cmd.c_str(),result);
		return -1;
	}
/*
	Cmd = "/usr/local/pluto/bin/convert -sample 300x300 /home/mediapics/" + StringUtils::itos(PK_MED_Picture) + "." + Extension + 
		" /home/mediapics/" + StringUtils::itos(PK_MED_Picture) + "_s." + Extension;
	if( (result=system(Cmd.c_str()))!=0 )
	{
		g_pPlutoLogger->Write(LV_CRITICAL,"Aborting small picture %s returned %d",Cmd.c_str(),result);
		return -1;
	}
*/
	return 0;
}

void MediaAttributes::UpdateSearchTokens(int PK_MED_Attribute)
{
	PlutoSqlResult result;
	MYSQL_ROW row,row2;
	string SQL = "SELECT Name,FirstName FROM MED_Attribute WHERE PK_MED_Attribute=" + StringUtils::itos(PK_MED_Attribute);
	if( (result.r=m_pDCEMI_PS_Main->mysql_query_result(SQL)) && (row=mysql_fetch_row(result.r)) )
	{
		SQL = "DELETE FROM MED_SearchToken_Attribute WHERE FK_MED_Attribute=" + StringUtils::itos(PK_MED_Attribute);
		m_pDCEMI_PS_Main->threaded_mysql_query(SQL);
		for(int i=0;i<=1;++i)
		{
			char *ptr = row[i];
			string::size_type pos=0;
			while( ptr && *ptr )
			{
				string s(ptr);
				string Token=StringUtils::ToUpper(StringUtils::Tokenize(s," ",pos));
				if( Token.length()==0 )
					break;
				SQL = "SELECT PK_MED_SearchToken FROM MED_SearchToken WHERE Token='" + 
					StringUtils::SQLEscape(Token) + "'";
				int PK_MED_SearchToken=0;
				if( (result.r=m_pDCEMI_PS_Main->mysql_query_result(SQL)) && (row2=mysql_fetch_row(result.r)) )
				{
					PK_MED_SearchToken = atoi(row2[0]);
					printf("Found token (%d): %s\n",PK_MED_SearchToken,Token.c_str());
				}
				else
				{
					printf("Didn't find token: %d %s (%s)\n",PK_MED_SearchToken,Token.c_str(),SQL.c_str());
					SQL = "INSERT INTO MED_SearchToken(Token) VALUES('" + 
						StringUtils::SQLEscape(Token) + "')";
					PK_MED_SearchToken = m_pDCEMI_PS_Main->threaded_mysql_query_withID(SQL);
				}
				if( PK_MED_SearchToken )
				{
					SQL = "INSERT INTO MED_SearchToken_Attribute VALUES(" +
						StringUtils::itos(PK_MED_SearchToken) + "," + 
						StringUtils::itos(PK_MED_Attribute) + ")";
					m_pDCEMI_PS_Main->threaded_mysql_query(SQL,true);
				}
			}
		}
	}
	else
		g_pPlutoLogger->Write(LV_CRITICAL,"Cannot update tokens for %d",PK_MED_Attribute);
}
int macounter=0;
void MediaAttributes::ScanDirectory(string Path)
{
if( macounter>50 )
	return;
#ifdef WIN32
	intptr_t ptrFileList;
	_finddata_t finddata;
	ptrFileList = _findfirst( (Path+"*.*").c_str(),&finddata);
	while( ptrFileList!=-1 ) 
#else

	DIR *dirp = opendir( Path.c_str() );
	struct dirent entry;
	struct dirent *direntp = &entry;
	while ( dirp != NULL && (readdir_r( dirp, direntp, &direntp ) == 0) && direntp )
#endif
	{
#ifdef WIN32
		if( (finddata.attrib==_A_SUBDIR) && finddata.name[0]!='.')
			ScanDirectory(Path + finddata.name + "\\");
		string FullPath = Path + finddata.name;
		char *Name = finddata.name;
#else
		if( entry.d_type == DT_DIR && entry.d_name[0] != '.')
			ScanDirectory(Path + entry.d_name + "/");
		string FullPath  = Path + entry.d_name;
		char *Name = entry.d_name;
#endif
	
		// We've got a file
		if( string("discinfo")==Name )
		{
	printf("Foiund discinfo %d in  directory: %s\n",macounter++,Path.c_str());
			char buffer[6000];
			string::size_type start=0,stop=0;
			FILE *file=fopen(FullPath.c_str(),"rb");
			int bytes = (int) fread(buffer,1,6000,file);
			fclose(file);
			if( bytes==0 || bytes>5999 )
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"Error with file in %s bytes: %d",Path.c_str(),bytes);
				exit(1);
			}
			buffer[bytes]=0;
			string sBuffer(buffer),Token;

			// Asin
			start=sBuffer.find("=");
			stop=sBuffer.find("DiscID=");
			string Asin=sBuffer.substr(start+1,stop-start-1);

			// DiscID
			start=sBuffer.find("=",stop);
			stop=sBuffer.find("Category=",start);
			string DiscID =sBuffer.substr(start+1,stop-start-1);

			// Category
			start=sBuffer.find("=",stop);
			stop=sBuffer.find("Genre=",start);
			string Category=sBuffer.substr(start+1,stop-start-1);

			// Genre
			start=sBuffer.find("=",stop);
			stop=sBuffer.find("Artist=",start);
			string Genre=sBuffer.substr(start+1,stop-start-1);

			// Artist
			start=sBuffer.find("=",stop);
			stop=sBuffer.find("Title=",start);
			string Artist=sBuffer.substr(start+1,stop-start-1);

			// Title
			start=sBuffer.find("=",stop);
			stop=sBuffer.find("Track1_Name=",start);
			string Title=sBuffer.substr(start+1,stop-start-1);

			int iAlbum=0,iArtist=0,iAsin=0,iDisc=0,iCategory=0,iGenre=0;

			for(int i=1;i<999;++i)
			{
				start = sBuffer.find("Track" + StringUtils::itos(i) + "_Name=");
				start = sBuffer.find("=",start);
				stop = sBuffer.find("Track" + StringUtils::itos(i) + "_Artist=",start);
				string TrackName = sBuffer.substr(start+1,stop-start-1);
				start = sBuffer.find("=",stop);
				stop = sBuffer.find("Track" + StringUtils::itos(i+1) + "_Name=",start);
				string TrackArtist = (stop==string::npos) ? sBuffer.substr(start+1) : sBuffer.substr(start+1,stop-start-1);

				int PK_MED_File = CreatedMedia(1,Path + TrackName + ".flac",NULL,NULL);
				if( PK_MED_File )
				{
					AddAttribute(PK_MED_File,0,2,TrackArtist,"");
					iArtist=AddAttribute(PK_MED_File,iArtist,2,Artist,"");
					iAlbum=AddAttribute(PK_MED_File,iAlbum,3,Title,"");
					AddAttribute(PK_MED_File,0,4,TrackName,"");
					AddAttribute(PK_MED_File,0,5,StringUtils::itos(i),"");
					iAsin=AddAttribute(PK_MED_File,iAsin,6,Asin,"");
					iDisc=AddAttribute(PK_MED_File,iDisc,7,DiscID,"");
					iCategory=AddAttribute(PK_MED_File,iCategory,8,Category,"");
					iGenre=AddAttribute(PK_MED_File,iGenre,8,Genre,"");
					AddPicture(PK_MED_File,iAlbum,Path + "cover.jpg");
				}
				else
					g_pPlutoLogger->Write(LV_CRITICAL,"xxxx");


				if( stop==string::npos )
					break;
			}
		}
#ifdef WIN32
		if( _findnext(ptrFileList,&finddata)<0 )
			break;
#endif
	
	}
#ifdef WIN32
	_findclose(ptrFileList);
#else
	closedir( dirp );
#endif

}

listMediaAttribute *MediaAttributes::AttributesFromString(string Input)
{
	listMediaAttribute *plistMediaAttribute = new listMediaAttribute();

	string::size_type pos=0;
	while(true)
	{
		MediaAttribute *mi = new MediaAttribute();
		mi->PK_MED_Attribute = atoi(StringUtils::Tokenize(Input,"|",pos).c_str());
		mi->PK_MED_C_AttributeType = atoi(StringUtils::Tokenize(Input,"|",pos).c_str());
		mi->Name = StringUtils::Tokenize(Input,"|",pos);
		mi->FirstName = StringUtils::Tokenize(Input,"|",pos);
		if( !mi->PK_MED_Attribute && !mi->PK_MED_C_AttributeType )
		{
			delete mi;
			break;
		}
		plistMediaAttribute->push_back(mi);
	}

	return plistMediaAttribute;
}

string MediaAttributes::AttributesToString(listMediaAttribute *plistMediaAttribute)
{
	string Result;
	for(listMediaAttribute::iterator it = plistMediaAttribute->begin();it != plistMediaAttribute->end();++it)
	{
		MediaAttribute *mi = (*it);
		Result += StringUtils::itos(mi->PK_MED_Attribute) + "|";
		Result += StringUtils::itos(mi->PK_MED_C_AttributeType) + "|";
		Result += mi->Name + "|";
		Result += mi->FirstName + "|";
	}

	return Result;
}

listMediaPicture *MediaAttributes::PicturesFromString(string Input)
{
	listMediaPicture *plistMediaPicture = new listMediaPicture();

	string::size_type pos=0;
	while(true)
	{
		MediaPicture *mp = new MediaPicture();
		mp->Path = StringUtils::Tokenize(Input,"|",pos);
		if( mp->Path.length()==0 )
		{
			delete mp;
			break;
		}
		mp->Extension = FileUtils::FindExtension(mp->Path);
		plistMediaPicture->push_back(mp);
	}

	return plistMediaPicture;
}

string MediaAttributes::PicturesToString(listMediaPicture *plistMediaPicture)
{
	string Result;
	for(listMediaPicture::iterator it = plistMediaPicture->begin();it != plistMediaPicture->end();++it)
	{
		MediaPicture *mp = (*it);
		Result += mp->Path + "|";
	}

	return Result;
}

string MediaAttributes::GetFilePathFromFileID(int PK_MED_File)
{
	string SQL = "SELECT Path FROM MED_File WHERE PK_MED_File=" + StringUtils::itos(PK_MED_File);
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( (result.r=m_pDCEMI_PS_Main->mysql_query_result(SQL)) && (row=mysql_fetch_row(result.r)) )
		return row[0];
	return "";
}

int MediaAttributes::GetFileIDFromAttributeID(int PK_MED_Attribute)
{
	string Result;
	string SQL = "SELECT PK_MED_File FROM MED_File "\
		"JOIN MED_File_Attribute ON FK_MED_File=PK_MED_File "\
		"JOIN MED_Attribute ON FK_MED_Attribute=PK_MED_Attribute "\
		"JOIN MED_C_Type_C_AttributeType ON "\
		"MED_C_Type_C_AttributeType.FK_MED_C_Type=MED_File.FK_MED_C_Type "\
		"AND MED_C_Type_C_AttributeType.FK_MED_C_AttributeType=MED_Attribute.FK_MED_C_AttributeType "\
		"WHERE Identifier=1 AND FK_MED_Attribute=" + StringUtils::itos(PK_MED_Attribute);

	PlutoSqlResult result;
	MYSQL_ROW row;
	if( (result.r=m_pDCEMI_PS_Main->mysql_query_result(SQL)) && (row=mysql_fetch_row(result.r)) )
		return atoi(row[0]);
	return 0;
}

string MediaAttributes::GetFilePathsFromAttributeID(int PK_MED_Atribute)
{
	string Result;
	string SQL = "select DISTINCT Path "\
		"FROM MED_File_Attribute As Source "\
		"JOIN MED_File_Attribute As Dest "\
		"ON Source.FK_MED_File=Dest.FK_MED_File AND Source.FK_MED_Attribute=" + StringUtils::itos(PK_MED_Atribute) + " "\
		"JOIN MED_File ON Dest.FK_MED_File=PK_MED_File "\
		"ORDER BY Path";

	PlutoSqlResult result;
	MYSQL_ROW row;
	if( (result.r=m_pDCEMI_PS_Main->mysql_query_result(SQL)) )
	{
		while( (row=mysql_fetch_row(result.r)) )
		{
			Result += row[0];
			Result += "|";
		}
	}
	return Result;
}

int MediaAttributes::GetFileIDFromFilePath(string File)
{
#ifdef WIN32
	PlutoSqlResult result;
	MYSQL_ROW row;
	string::size_type s;
	while( (s=File.find("Z:/"))!=string::npos )
		File.replace(s,3,"/home/public/data/");
	while( (s=File.find("Z:\\"))!=string::npos )
		File.replace(s,3,"/home/public/data/");
	while( (s=File.find('\\'))!=string::npos )
		File.replace(s,1,"/");
	string SQL = "SELECT PK_MED_File FROM MED_File WHERE Path='" + StringUtils::SQLEscape(File) + "'";
	if( (result.r=m_pDCEMI_PS_Main->mysql_query_result(SQL)) && (row=mysql_fetch_row(result.r)) )
		return atoi(row[0]);

	return 0; // Can't do this in Windows
#else
	int n = 79;
	char value[80];
	memset(value, 0, sizeof(value));
	
	if ( attr_get(File.c_str(), "ID", value, &n, 0) == 0 )
	{
		int ID = atoi(value);
		string DatabaseFile = GetFilePathFromFileID(ID);
		if( File!=DatabaseFile )
		{
			if( DatabaseFile.length()==0 )
			{
				g_pPlutoLogger->Write(LV_CRITICAL,"There appears to be a foreign file in the system %d %s",value,File.c_str());
				return 0;
			}
			else
			{
				// Confirm that the file doesn't exist in the old location.  If it does, there are 2 with the same ID
				FILE *file = fopen(DatabaseFile.c_str(),"rb");
				if( file )
				{
					g_pPlutoLogger->Write(LV_CRITICAL,"There are 2 files with id %d %s and: %s",value,File.c_str(),DatabaseFile.c_str());
					return 0;
				}
				else
				{
					// They must have moved it
					g_pPlutoLogger->Write(LV_MEDIA,"File %d moved from %s to %s",ID,DatabaseFile.c_str(),File.c_str());
					string SQL = "UPDATE MED_File SET Path='" + StringUtils::SQLEscape(File) + "' WHERE PK_MED_File=" + value;
					m_pDCEMI_PS_Main->threaded_mysql_query(SQL);
				}
			}
		}
		return ID;
	}
	else
		return 0;
#endif
}

string MediaAttributes::GetAnyPictureUnderDirectory(string File,int *PK_MED_Picture,int MaxDepthToSearch)
{
	int FilesToScan=5;
#ifdef WIN32
	intptr_t ptrFileList;
	_finddata_t finddata;
	string::size_type s;
	while( (s=File.find('/'))!=string::npos )
		File.replace(s,1,"\\");

	ptrFileList = _findfirst( (File+"\\*.*").c_str(),&finddata);
	while( ptrFileList!=-1 ) 
	{
		if( MaxDepthToSearch && (finddata.attrib==_A_SUBDIR) && finddata.name[0]!='.')
		{
			string Extension = GetAnyPictureUnderDirectory(File + "\\" + finddata.name,PK_MED_Picture,MaxDepthToSearch-1);
			if( *PK_MED_Picture )
				return Extension;
		}
		else if (finddata.name[0] != '.')
		{
			FilesToScan--;
			string Extension = GetPictureFromFilePath(File + "\\" + finddata.name,PK_MED_Picture);
			if( *PK_MED_Picture )
				return Extension;
		}
		if(	!FilesToScan )
		{
			*PK_MED_Picture=0;
			return "";
		}
		if( _findnext(ptrFileList,&finddata)<0 )
			break;
	}

	_findclose(ptrFileList);

#else
	DIR *dirp = opendir( File.c_str() );
	struct dirent entry;
	struct dirent *direntp = &entry;
	if (dirp == NULL)
	{
		g_pPlutoLogger->Write( LV_CRITICAL, "opendir2 %s failed: %s", File.c_str(), strerror(errno));
		return "";
	}
	while ( dirp != NULL && (readdir_r( dirp, direntp, &direntp ) == 0) && direntp )
	{
		if( MaxDepthToSearch && (entry.d_type==DT_DIR) && entry.d_name[0]!='.')
		{
			string Extension = GetAnyPictureUnderDirectory(File + "/" + entry.d_name,PK_MED_Picture,MaxDepthToSearch-1);
			if( *PK_MED_Picture )
			{
				closedir( dirp );
				return Extension;
			}
		}
		else if (entry.d_name[0] != '.')
		{
			FilesToScan--;
			string Extension = GetPictureFromFilePath(File + "/" + entry.d_name,PK_MED_Picture);
			if( *PK_MED_Picture )
			{
				closedir( dirp );
				return Extension;
			}
		}
		if(	!FilesToScan )
		{
			closedir( dirp );
			*PK_MED_Picture=0;
			return "";
		}
	}
	closedir( dirp );
#endif
return "";
}

string MediaAttributes::GetPictureFromFilePath(string File,int *PK_MED_Picture)
{
	int PK_MED_File = GetFileIDFromFilePath(File);
	if( PK_MED_File )
		return GetPictureFromFileID(PK_MED_File,PK_MED_Picture);
	*PK_MED_Picture=0;
	return "";
}

string MediaAttributes::GetPictureFromFileID(int PK_MED_File,int *PK_MED_Picture)
{
	string SQL = "select PK_MED_Picture,Extension FROM MED_Picture_File JOIN MED_Picture ON "\
		"FK_MED_Picture=PK_MED_Picture "\
		"WHERE FK_MED_File=" + StringUtils::itos(PK_MED_File);
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( (result.r=m_pDCEMI_PS_Main->mysql_query_result(SQL)) && (row=mysql_fetch_row(result.r)) )
	{
		*PK_MED_Picture = atoi(row[0]);
		return row[1];
	}

	// Let's to see if any of the attributes associated with this file have pictures
	SQL = "SELECT FK_MED_Picture,Extension from MED_File_Attribute "\
		"JOIN MED_Picture_Attribute ON MED_Picture_Attribute.FK_MED_Attribute=MED_File_Attribute.FK_MED_Attribute "\
		"JOIN MED_Picture ON FK_MED_Picture=PK_MED_Picture "\
		"WHERE FK_MED_File=" + StringUtils::itos(PK_MED_File);
	if( (result.r=m_pDCEMI_PS_Main->mysql_query_result(SQL)) && (row=mysql_fetch_row(result.r)) )
	{
		*PK_MED_Picture = atoi(row[0]);
		return row[1];
	}

	*PK_MED_Picture=0;
	return "";
}

string MediaAttributes::GetPictureFromAttributeID(int PK_MED_Attribute,int *PK_MED_Picture)
{
	string SQL = "select PK_MED_Picture,Extension FROM MED_Picture_Attribute JOIN MED_Picture ON "\
		"FK_MED_Picture=PK_MED_Picture "\
		"WHERE FK_MED_Attribute=" + StringUtils::itos(PK_MED_Attribute);
	PlutoSqlResult result;
	MYSQL_ROW row;
	if( (result.r=m_pDCEMI_PS_Main->mysql_query_result(SQL)) && (row=mysql_fetch_row(result.r)) )
	{
		*PK_MED_Picture = atoi(row[0]);
		return row[1];
	}

	// Let's to see if any of the attributes associated with this file have pictures
	SQL = "select DISTINCT FK_MED_Picture,Extension "\
		"FROM MED_File_Attribute AS Source "\
		"JOIN MED_File_Attribute As Dest ON Source.FK_MED_File=Dest.FK_MED_File "\
		"JOIN MED_Picture_Attribute ON MED_Picture_Attribute.FK_MED_Attribute=Dest.FK_MED_Attribute "\
		"JOIN MED_Picture ON FK_MED_Picture=PK_MED_Picture "\
		"WHERE Source.FK_MED_Attribute=" + StringUtils::itos(PK_MED_Attribute);

	if( (result.r=m_pDCEMI_PS_Main->mysql_query_result(SQL)) && (row=mysql_fetch_row(result.r)) )
	{
		*PK_MED_Picture = atoi(row[0]);
		return row[1];
	}

	*PK_MED_Picture=0;
	return "";
}
int MediaAttributes::GetAttributeFromFileID(int PK_MED_File)
{
	string SQL = "select PK_MED_Attribute FROM MED_File_Attribute "\
		"JOIN MED_Attribute ON FK_MED_Attribute=PK_MED_Attribute "\
		"JOIN MED_File ON FK_MED_File=PK_MED_File "\
		"JOIN MED_C_Type_C_AttributeType ON MED_C_Type_C_AttributeType.FK_MED_C_Type=MED_File.FK_MED_C_Type "\
		"AND MED_C_Type_C_AttributeType.FK_MED_C_AttributeType=MED_Attribute.FK_MED_C_AttributeType "\
		"WHERE Identifier=1 AND PK_MED_File=" + StringUtils::itos(PK_MED_File);

	PlutoSqlResult result;
	MYSQL_ROW row;

	if( (result.r=m_pDCEMI_PS_Main->mysql_query_result(SQL)) && (row=mysql_fetch_row(result.r)) )
		return atoi(row[0]);
	return 0;
}
int MediaAttributes::GetAttributeFromFilePath(string File)
{
	string SQL = "select PK_MED_Attribute FROM MED_File_Attribute "\
		"JOIN MED_Attribute ON FK_MED_Attribute=PK_MED_Attribute "\
		"JOIN MED_File ON FK_MED_File=PK_MED_File "\
		"JOIN MED_C_Type_C_AttributeType ON MED_C_Type_C_AttributeType.FK_MED_C_Type=MED_File.FK_MED_C_Type "\
		"AND MED_C_Type_C_AttributeType.FK_MED_C_AttributeType=MED_Attribute.FK_MED_C_AttributeType "\
		"WHERE Identifier=1 AND Path='" + StringUtils::SQLEscape(File) + "'";

	PlutoSqlResult result;
	MYSQL_ROW row;

	if( (result.r=m_pDCEMI_PS_Main->mysql_query_result(SQL)) && (row=mysql_fetch_row(result.r)) )
		return atoi(row[0]);
	return 0;
}
