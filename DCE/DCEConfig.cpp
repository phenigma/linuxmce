/* 
	DCEConfig
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

#include "DCEConfig.h"
#include "PlutoUtils/StringUtils.h"
//------------------------------------------------------------------------------------------------------
DCEConfig::DCEConfig(string sFilename) : RA_Config()
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

    m_sDBHost               = ReadString("MySqlHost",m_sDBHost);
    m_sDBUser               = ReadString("MySqlUser",m_sDBUser);
    m_sDBPassword           = ReadString("MySqlPassword",m_sDBPassword);
    m_sDBName               = ReadString("MySqlDBName",m_sDBName);
    m_sDCERouter            = ReadString("DCERouter",m_sDCERouter);
    m_iDBPort               = ReadInteger("MySqlPort",m_iDBPort);
    m_iDCERouterPort        = ReadInteger("DCERouterPort",m_iDCERouterPort);
    m_iPK_Device_Computer   = ReadInteger("PK_Device",m_iPK_Device_Computer);
    m_iPK_Installation      = ReadInteger("PK_Installation",m_iPK_Installation);
    m_iPK_Distro            = ReadInteger("PK_Distro",m_iPK_Distro);

    m_sMenuPath             = ReadString("MenuPath",m_sMenuPath);
    m_sPicturePath          = ReadString("PicturePath",m_sPicturePath);
    m_sSisFilesPath         = ReadString("SisFilesPath",m_sSisFilesPath);
}
//------------------------------------------------------------------------------------------------------
/*virtual*/ DCEConfig::~DCEConfig()
{
    m_mapParameters.clear();
}
//------------------------------------------------------------------------------------------------------
/*inline*/ int DCEConfig::ReadInteger(string sToken, int iDefaultValue/* = 0*/)
{
    int iValue = iDefaultValue;

    if( m_mapParameters_Exists(sToken) )
    {
        try
        {
            iValue = atoi(m_mapParameters_Find(sToken).c_str());
        }
        catch(...) {}
    }

    return iValue;
}
//------------------------------------------------------------------------------------------------------
/*inline*/ string DCEConfig::ReadString(string sToken, string sDefaultValue/* = ""*/)
{
    string sValue = sDefaultValue;

    if( m_mapParameters_Exists(sToken) )
        sValue = m_mapParameters_Find(sToken);

    return sValue;
}
//------------------------------------------------------------------------------------------------------
