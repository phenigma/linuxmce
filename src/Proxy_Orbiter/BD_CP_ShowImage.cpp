/*
 BD_CP_ShowImage
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */


#include <iostream>
#include <fstream>
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/PlutoDefs.h"
#include "BD/BDCommandProcessor.h"
#include "BD_CP_ShowImage.h"

BD_CP_ShowImage::BD_CP_ShowImage(unsigned char ImageType,unsigned long ImageSize,const char *pImage,
                                 unsigned long KeysListSize, const char* pRepeatedKeysList, bool bSignalStrengthScreen,
                                 bool bImageQualityScreen, unsigned long nImageQuality) 
	
{
	m_iImageType=ImageType;
	
    m_ImageSize=ImageSize;
	m_pImage = new char[m_ImageSize];
	memcpy(m_pImage, pImage, m_ImageSize);

    m_KeysListSize = KeysListSize;
    m_pRepeatedKeysList = new char[m_KeysListSize];
    memcpy(m_pRepeatedKeysList, pRepeatedKeysList, m_KeysListSize);

    m_bSignalStrengthScreen = bSignalStrengthScreen;
    m_bImageQualityScreen = bImageQualityScreen;
    m_nImageQuality = nImageQuality;
}

BD_CP_ShowImage::~BD_CP_ShowImage()
{
	PLUTO_SAFE_DELETE_ARRAY(m_pImage);
    PLUTO_SAFE_DELETE_ARRAY(m_pRepeatedKeysList);
}

void BD_CP_ShowImage::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_unsigned_char(m_iImageType);
	Write_long(m_ImageSize);
	Write_block(m_pImage, m_ImageSize);
    Write_long(m_KeysListSize);
    Write_block(m_pRepeatedKeysList, m_KeysListSize);
    Write_unsigned_char(m_bSignalStrengthScreen);
    Write_unsigned_char(m_bImageQualityScreen);
    Write_long(m_nImageQuality);
}

void BD_CP_ShowImage::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size, data);

	m_iImageType = Read_unsigned_char();
	m_ImageSize = Read_long();
	m_pImage = Read_block(m_ImageSize);
    m_KeysListSize = Read_long();
    m_pRepeatedKeysList = Read_block(m_KeysListSize);
    m_bSignalStrengthScreen = Read_unsigned_char();
    m_bImageQualityScreen = Read_unsigned_char();
    m_nImageQuality = Read_long();
}

bool BD_CP_ShowImage::ProcessCommand(BDCommandProcessor *pProcessor)
{
	return true;
}
