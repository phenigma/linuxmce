#include "DCEConfig.h"
#include "PlutoUtils/StringUtils.h"

DCEConfig::DCEConfig(string sFilename) 
{ 
	m_sDBHost="dce_router"; m_sDBUser="root"; m_sDBPassword=""; m_sDBName="pluto_main"; m_sDCERouter="dce_router";
	m_iPK_Device_Computer=0; m_iDBPort=3306; m_iDCERouterPort=3450;
	m_iPK_Distro=m_iPK_Installation=-1;

	FILE *file = fopen(sFilename.c_str(),"rb");
	if( !file )
		return;
	fseek(file, 0L, SEEK_END);
	int Size = ftell(file);
	fseek(file, 0L, SEEK_SET);

	if( !Size )
	{
		fclose(file);
		return;
	}

	char *buffer = new char[Size+1];
	buffer[Size]=0;
	fread(buffer,Size,1,file);
	fclose(file);

	char *Line = buffer;
	while(Line)
	{
		while(Line[0]=='\r' || Line[0]=='\n')
			Line++;
		char *NextLF = strchr(Line,'\n');
		char *NextCR = strchr(Line,'\r');
		char *NextLine;
		if( NextLF && NextCR )
			NextLine = min(NextLF,NextCR);
		else if( NextCR )
			NextLine = NextCR;
		else
			NextLine = NextLF;

		// Skip over any lines that start with # or /
		if( Line[0]=='#' || Line[0]=='/' )
		{
			Line=NextLine;
			continue;
		}

		// Skip any lines that don't have an = in them
		char *Equal = strchr(Line,'=');
		if( !Equal || (NextLine && Equal>NextLine) )
		{
			Line=NextLine;
			continue;
		}

		// We've got a valid Line with an =
		char *DoubleSlash = strstr(Equal,"//");
		if( DoubleSlash && (!NextLine || DoubleSlash<NextLine) )
			*DoubleSlash=0;  // Ignore anything past a double // if it's in this line

		*Equal=0;
		if( NextLine )
			*NextLine=0;
		string Token=Line;
		string Value=(Equal+1);
		StringUtils::TrimSpaces(Token);
		StringUtils::TrimSpaces(Value);
		m_mapParameters[Token]=Value;

		if( NextLine )
			Line=NextLine+1;
		else
			break;
	}

	if( m_mapParameters_Exists("MySqlHost") )
		m_sDBHost=m_mapParameters_Find("MySqlHost");
	if( m_mapParameters_Exists("MySqlUser") )
		m_sDBUser=m_mapParameters_Find("MySqlUser");
	if( m_mapParameters_Exists("MySqlPassword") )
		m_sDBPassword=m_mapParameters_Find("MySqlPassword");
	if( m_mapParameters_Exists("MySqlDBName") )
		m_sDBName=m_mapParameters_Find("MySqlDBName");
	if( m_mapParameters_Exists("MySqlPort") )
		m_iDBPort=atoi(m_mapParameters_Find("MySqlPort").c_str());
	if( m_mapParameters_Exists("DCERouter") )
		m_sDBHost=m_mapParameters_Find("DCERouter");
	if( m_mapParameters_Exists("DCERouterPort") )
		m_iDCERouterPort=atoi(m_mapParameters_Find("DCERouterPort").c_str());
	if( m_mapParameters_Exists("PK_Device") )
		m_iPK_Device_Computer=atoi(m_mapParameters_Find("PK_Device").c_str());
	if( m_mapParameters_Exists("PK_Installation") )
		m_iPK_Installation=atoi(m_mapParameters_Find("PK_Installation").c_str());
	if( m_mapParameters_Exists("PK_Distro") )
		m_iPK_Distro=atoi(m_mapParameters_Find("PK_Distro").c_str());
}
