/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
//
// C++ Implementation: seriald
//
// Description:
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "HalTree.h"

#include "DCE/Logger.h"
#include "Serial/SerialPort.h"
#include "../PlutoUtils/FileUtils.h"
#include "../PlutoUtils/ProcessUtils.h"
#include "../PlutoUtils/StringUtils.h"
using namespace DCE;

HalDevice::~HalDevice()
{
	for(map<string,HalValue *>::iterator it=m_mapHalValue.begin();it!=m_mapHalValue.end();++it)
		delete it->second;
	m_mapHalValue.clear();
}

HalTree::~HalTree()
{
	PurgeTree();
}

void HalTree::PurgeTree()
{
	for(map<int,HalDevice *>::iterator it=m_mapHalDevice.begin();it!=m_mapHalDevice.end();++it)
		delete it->second;
	m_mapHalDevice.clear();
}

HalDevice *HalTree::GetDeviceWithParm(string sKey,string sValue)
{
	for(map<int,HalDevice *>::iterator it=m_mapHalDevice.begin();it!=m_mapHalDevice.end();++it)
	{
		HalDevice *pHalDevice = it->second;
		for(map<string,HalValue *>::iterator it2=pHalDevice->m_mapHalValue.begin();it2!=pHalDevice->m_mapHalValue.end();++it2)
		{
			if( it2->first==sKey && it2->second->m_sValueAsString==sValue )
				return pHalDevice;
		}
	}
	return NULL;
}


void HalTree::Populate()
{
	PurgeTree();

	string sBuffer;
	char * args[] = {"hal-device", NULL};
	if (!ProcessUtils::GetCommandOutput(args[0], args, sBuffer))
		return;
	
#ifdef WIN32
	size_t size;
	char *pBuffer = FileUtils::ReadFileIntoBuffer("/temp/haltree",size);
	if( !pBuffer )
		return;
	sBuffer = pBuffer;
#endif

	vector<string> vectLines;
	StringUtils::Tokenize(sBuffer,"\n",vectLines);

	HalDevice *pHalDevice = NULL;  // The current device we're adding
	for(vector<string>::iterator it=vectLines.begin();it!=vectLines.end();++it)
	{
		string::size_type pos;
		if( (pos=it->find(": udi = "))!=string::npos )  // Starting a new device
		{
			pHalDevice = new HalDevice();
			pHalDevice->m_iID = atoi( it->c_str() );
			pHalDevice->m_sUdi = it->substr(pos + 8,it->size()-10); // After the ' and skip the last '
			m_mapHalDevice[pHalDevice->m_iID]=pHalDevice;
		}
		else if( pHalDevice )
		{
			// Get the type.  It's at the end in (type)
			if( it->size()>10 && (*it)[it->size()-1]==')' )
			{
				pos = it->size()-1;
				while( pos && (*it)[pos]!='(' )
					pos--;

				if( pos<5 )
					continue; // Didn't find the opening (

				string sType = it->substr(pos+1,it->size()-pos-2);
				HalValue *pValue = HalValue::GetValue(sType);
				if( !pValue )
					continue; // Unknown type

				string::size_type pos_equal = it->find('=');
				if( pos_equal == string::npos || pos_equal>pos )
					continue; // Malformed

				string sToken = it->substr(0,pos_equal);
				StringUtils::TrimSpaces(sToken);
				string sValue = it->substr(pos_equal+3,pos-pos_equal-6);

				pValue->Assign(sValue);
				pHalDevice->m_mapHalValue[sToken]=pValue;
			}
		}
	}
}

HalValue *HalValue::GetValue(string sType)
{
	if( sType=="int" )
		return new HalValue_int;
	else if( sType=="string" )
		return new HalValue_string;
	else if( sType=="bool" )
		return new HalValue_bool;
	return NULL;
}

