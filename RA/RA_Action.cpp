/**
 *
 * @file RA_Action.cpp
 * @brief source file for the request processor class
 * @author
 * @todo notcommented
 *
 */

 /**
  *
  * Copyright Notice goes here
  *
  */



#include "VIPShared/VIPIncludes.h"

#include "RA_Action.h"


RA_Action::RA_Action()
{
    // common-sense initial values
    m_pcRequest = NULL;
    m_dwActionSize = m_dwActionChecksum = 0;
    m_pcAction = NULL;
}

RA_Action::RA_Action( unsigned long dwSize, const char *pcData )
{
    m_pcRequest=NULL;
    StartReading( dwSize,(char *)pcData);
    m_dwActionSize=m_dwActionChecksum=0;
    m_pcAction=NULL;
}

void RA_Action::ConvertActionToBinary()
{
    StartWriting();
    Write_unsigned_long( ID() );
    m_dwActionSize = (unsigned long) ( m_pcCurrentPosition - m_pcDataBlock );
    m_pcAction = m_pcDataBlock;
}
