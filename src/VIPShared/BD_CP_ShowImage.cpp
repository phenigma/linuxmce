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

#ifdef VIPPHONE
#ifdef VIPDESIGN
#include "stdafx.h"
#include "VIPDesign/VIPDesignDoc.h"
#include "VIPShared/PlutoConfig.h"
#endif
#else
#ifndef SYMBIAN
#include <iostream>
#include <fstream>
#endif
#endif

#ifndef SYMBIAN
#include "PlutoUtils/CommonIncludes.h"
#include "BD/BDCommandProcessor.h"
#else
#include "VIPShared/VIPIncludes.h"
#include "PlutoMOAppUi.h"
#include "Logger.h"
#endif

#include "PlutoUtils/MyStl.h"
#include "BD_CP_ShowImage.h"

BD_CP_ShowImage::BD_CP_ShowImage(unsigned char ImageType,unsigned long ImageSize,const char *pImage) 
	
{
	m_iImageType=ImageType;
	m_ImageSize=ImageSize;
	m_pImage = new char[m_ImageSize];
	memcpy(m_pImage, pImage, m_ImageSize);
}

BD_CP_ShowImage::~BD_CP_ShowImage()
{
#ifndef SYMBIAN //VCMUtil is responsable for deleting the image
	if(NULL != m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
#endif
}

void BD_CP_ShowImage::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_unsigned_char(m_iImageType);
	Write_long(m_ImageSize);
	Write_block(m_pImage,m_ImageSize);
}

void BD_CP_ShowImage::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);

#ifdef VIPPHONE
	m_iImageType = Read_unsigned_char();
	m_ImageSize = Read_long();
	m_pImage = Read_block(m_ImageSize);

#ifdef SYMBIAN
	LOG("#	Received 'ShowImage' command  #\n");

	((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->OpenImage(m_iImageType, m_ImageSize, m_pImage);
	((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->Show();
#endif //SYMBIAN

#ifdef VIPDESIGN
	g_pPlutoConfig->m_pDoc->m_pImageStatic_Type=m_iImageType;
	g_pPlutoConfig->m_pDoc->m_pImageStatic_Size=m_ImageSize;
	g_pPlutoConfig->m_pDoc->m_pImageStatic_Data=m_pImage;
	g_pPlutoConfig->m_pDoc->InvalidateAllViews();
#endif //VIPDESIGN

#endif //VIPPHONE
}

bool BD_CP_ShowImage::ProcessCommand(BDCommandProcessor *pProcessor)
{
	return true;
}
