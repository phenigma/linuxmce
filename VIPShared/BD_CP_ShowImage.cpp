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
	m_pImage=(char *)malloc(m_ImageSize);
	memcpy(m_pImage,pImage,m_ImageSize);
}

BD_CP_ShowImage::~BD_CP_ShowImage()
{
	if(NULL != m_pImage)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
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

#ifdef SYMBIAN
	unsigned char Type = Read_unsigned_char();
	unsigned long Size = Read_long();
	const char *Data = Read_block(Size);

	LOG("#	Received 'ShowImage' command  #\n");

	((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->OpenImage(Type, Size, Data);
	((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->Show();
#endif //SYMBIAN

#ifdef VIPDESIGN
	g_pPlutoConfig->m_pDoc->m_pImageStatic_Type=Read_unsigned_char();
	g_pPlutoConfig->m_pDoc->m_pImageStatic_Size=Read_long();
	g_pPlutoConfig->m_pDoc->m_pImageStatic_Data=Read_block(g_pPlutoConfig->m_pDoc->m_pImageStatic_Size);
	g_pPlutoConfig->m_pDoc->InvalidateAllViews();
#endif //VIPDESIGN

#endif //VIPPHONE
}

bool BD_CP_ShowImage::ProcessCommand(BDCommandProcessor *pProcessor)
{
	return true;
}
