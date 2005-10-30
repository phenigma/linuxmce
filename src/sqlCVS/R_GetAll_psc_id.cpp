/*
 R_GetAll_psc_id
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */


/**
 *
 * @file R_GetAll_psc_id.cpp
 * @brief source file for the R_GetAll_psc_id class
 *
 */
 
#include "R_GetAll_psc_id.h"
#include "PlutoUtils/CommonIncludes.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "sqlCVSprocessor.h"
#include "Database.h"
#include "Repository.h"
#include "CommonFunctions.h"

#include <iostream>
#include <sstream>

using namespace sqlCVS;

R_GetAll_psc_id::R_GetAll_psc_id( vector<int> *p_vectRestrictions )
{
	m_vectRestrictions=*p_vectRestrictions;
}

bool R_GetAll_psc_id::ProcessRequest( class RA_Processor *pRA_Processor )
{
	string::size_type pos=0;

	vector<Table *> vectTable;  // Tables we will include
	int sizeDataBlock=0;  // Size of the data we will need
	while( pos<m_sTable.size() )
	{
		string sTableName = StringUtils::Tokenize(m_sTable,"\t",pos);
		Table *pTable = g_GlobalConfig.m_pDatabase->m_mapTable_Find( sTableName );
		if( !pTable )
		{
			cerr << "R_GetAll_psc_id::ProcessRequest Problem retrieving tables: " << m_sTable << endl;
			m_cProcessOutcome=INTERNAL_ERROR;
			return true; /**<< Request successfully processed */
		}

		sizeDataBlock += pTable->m_size_id_restrict_batch_block + sTableName.size() + 2;
		vectTable.push_back(pTable);
	}

	sizeDataBlock += 4;
	cout << "R_GetAll_psc_id " << vectTable.size() << " tables " << sizeDataBlock << " bytes" << endl;
	m_PlutoDataBlock.m_pBlock = new char[sizeDataBlock];
	m_PlutoDataBlock.m_dwSize = sizeDataBlock;
	char *pPtr = m_PlutoDataBlock.m_pBlock;
	int *pInt = (int *) pPtr;
	*pInt = (int) vectTable.size();
	pPtr += sizeof(int);
	for(size_t s=0;s<vectTable.size();++s)
	{
		Table *pTable = vectTable[s];
		strcpy(pPtr,pTable->Name_get().c_str());
		pPtr += pTable->Name_get().size() +1;
		memcpy((void *) pPtr,pTable->m_id_restrict_batch_block,pTable->m_size_id_restrict_batch_block);
		pPtr += pTable->m_size_id_restrict_batch_block;
	}

	m_cProcessOutcome=SUCCESSFULLY_PROCESSED;
	return true; /**<< Request successfully processed */
}

bool R_GetAll_psc_id::ParseResponse(unsigned long dwSize, const char *pcData)
{
	if( !RA_Request::ParseResponse(dwSize, pcData) )
		return false;

	char *pPtr = m_PlutoDataBlock.m_pBlock;
	if( !pPtr )
		return false; // Shouldn't happen

	// Find the greatest restriction
	int psc_restrict_max=-1;
	for(size_t s=0;s<m_vectRestrictions.size();++s)
		psc_restrict_max = max(psc_restrict_max,m_vectRestrictions[s]);

	bool *p_bRestrict = psc_restrict_max==-1 ? NULL : new bool[psc_restrict_max+1];
	if( p_bRestrict )
	{
		for(size_t s=0;s<psc_restrict_max;++s)
			p_bRestrict[s]=false;
		for(size_t s=0;s<m_vectRestrictions.size();++s)
			p_bRestrict[m_vectRestrictions[s]]=true;
	}
	int *pInt = (int *) pPtr;
	int NumTables = *pInt;
	pPtr += sizeof(int);

	for(int iTable=0;iTable<NumTables;++iTable)
	{
		string sTableName = pPtr;
		pPtr += sTableName.size()+1;
		pInt = (int *) pPtr;
		int NumRecords = *(pInt++);
		vector< pair<int,int> > *pVect = &m_map_vectAll_psc_id[sTableName];
		int psc_restrict_last=-1;
		bool bMultipleRestrictions=false;
		for(int iRecord=0;iRecord<NumRecords;++iRecord)
		{
			int psc_id = *(pInt++);
			int psc_batch = *(pInt++);
			int psc_restrict = *(pInt++);
			if( psc_restrict==0 || (p_bRestrict && p_bRestrict[psc_restrict]) )
			{
				pVect->push_back( make_pair<int,int> (psc_id,psc_batch) );
				if( psc_restrict_last==-1 )
					psc_restrict_last = psc_restrict;
				else if( psc_restrict_last != psc_restrict )
					bMultipleRestrictions=true;
			}
		}
		pPtr = (char *) pInt;
		if( bMultipleRestrictions )
			sort(pVect->begin(), pVect->end());
	}
	delete[] p_bRestrict;
	return true;
}

