/*
 BD_CP_SendFile
 
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
#include "BD_CP_SendFile.h"
#include "PlutoUtils/PlutoDefs.h"

BD_CP_SendFile::BD_CP_SendFile(		
		char *pFileName, 
		char *pFileData, 
		unsigned long iFileNameSize,
		unsigned long iFileDataSize
) 
{	
	m_iFileNameSize = iFileNameSize;
	m_iFileDataSize = iFileDataSize;

	m_pFileName = (char *)malloc(iFileNameSize);
	memcpy(m_pFileName, pFileName, iFileNameSize);

	m_pFileData = (char *)malloc(iFileDataSize);
	memcpy(m_pFileData, pFileData, iFileDataSize);
}

BD_CP_SendFile::~BD_CP_SendFile()
{
	PLUTO_SAFE_DELETE(m_pFileName);
	PLUTO_SAFE_DELETE(m_pFileData);
}

void BD_CP_SendFile::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
	Write_long(m_iFileNameSize);
	Write_block(m_pFileName, m_iFileNameSize);
	Write_long(m_iFileDataSize);
	Write_block(m_pFileData, m_iFileDataSize);
}

void BD_CP_SendFile::ParseCommand(unsigned long size, const char *data)
{
	BDCommand::ParseCommand(size, data);

#ifdef VIPPHONE

#ifdef SYMBIAN
	unsigned long iFileNameSize = Read_unsigned_long();
	const char *pFileName = Read_block(iFileNameSize);
	unsigned long iFileDataSize = Read_unsigned_long();
	const char *pFileData = Read_block(iFileDataSize);

	LOG("#	Received 'SendFile' command  #\n");

	((CPlutoMOAppUi *)CCoeEnv::Static()->AppUi())->SaveFile(iFileNameSize, pFileName, iFileDataSize, pFileData);

#endif //SYMBIAN

#ifdef VIPDESIGN
	#pragma error "Not implemented!"
#endif //VIPDESIGN

#endif //VIPPHONE
}

bool BD_CP_SendFile::ProcessCommand(BDCommandProcessor *pProcessor)
{
	return true;
}
