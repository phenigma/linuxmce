/**
 *
 * @file sqlCVSProcessor.h
 * @brief header file for the request processor class specific to the sqlCVS program
 * @author
 * @todo notcommented
 *
 */

#ifndef sqlCVSprocessor_h
#define sqlCVSprocessor_h

#include "RA/RA_Processor.h"

/**
 * @brief request action processor class
 */
class sqlCVSprocessor : public RA_Processor
{
public:
	map<int,int> m_mapVerifiedUsers; // The confirmed users who are committing the changes
	class Table *m_pTable; // The table we are currently committing
	vector<string> m_vectFields;  // The fields we are currently committing

	sqlCVSprocessor() : RA_Processor(0,1) { m_pTable=NULL; }

};

#endif
