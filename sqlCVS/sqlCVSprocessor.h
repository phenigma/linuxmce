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

namespace sqlCVS
{
	class Table;
	class Repository;
}

/**
 * @brief request action processor class
 */
class sqlCVSprocessor : public RA_Processor
{
public:
	sqlCVS::Table *m_pTable; // The table we are currently committing
	vector<string> *m_pvectFields;
	int m_i_psc_batch;
	sqlCVS::Repository *m_pRepository;  // The repository we're currently committing

	sqlCVSprocessor() : RA_Processor(0,1) { m_pTable=NULL; m_pvectFields=NULL; m_i_psc_batch=1; m_pRepository=NULL; }

};

#endif
