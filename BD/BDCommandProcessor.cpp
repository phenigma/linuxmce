/**
 *
 * @file BDCommandProcessor.cpp
 * @brief source file for the bidirec
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
#include "PlutoUtils/MyStl.h"
#ifndef SYMBIAN
	#include "DCE/Logger.h"
#endif

#include "BD/BDCommandProcessor.h"
#include "BD/BDCommand.h"
#include "BD/BD_HaveNothing.h"
#include "BD/BD_WhatDoYouHave.h"


#define _LOG_(log) 
#ifdef APPSERVER
	//const char *Role="PC:";
	#define Role "PC:"
#else
	//const char *Role="Phone:";
	#define Role "Phone"
#endif

#ifdef SYMBIAN
	#define LD(x,y,z,cmd)
#else
	#define LD(x,y,z,cmd) {FILE *x=fopen("C:\\dialog.txt","a");  fprintf(x,"%s: %d %s (%s)\n",Role,(int) y,z,cmd); fclose(x);}
#endif

using namespace DCE;

BDCommandProcessor::BDCommandProcessor( string sMacAddressPhone ) 
#ifndef SYMBIAN
	: m_CommandMutex("command"), m_PollingMutex("Polling")
#endif
{
#ifndef SYMBIAN	
	printf( "btcp const: %p\n\n", g_pPlutoLogger );
#endif

	m_bDead = false;
	m_bExit = false;
	m_pcReceiveCmdData = NULL;
	m_pcReceiveAckData = NULL;
	m_pcReceiveCmdData = NULL;
	m_pcReceiveAckHeader = NULL;
	m_pCommand_Sent = NULL;
	m_sMacAddressPhone = sMacAddressPhone;
#ifndef SYMBIAN
	m_CommandMutex.Init( NULL );
	pthread_cond_init( &m_PollingCond, NULL );
	m_PollingMutex.Init( NULL );

	printf( "end of btcp const: %p\n\n", g_pPlutoLogger );
#endif
}

bool BDCommandProcessor::SendCommand( bool &bImmediateCallback )
{
	bImmediateCallback = false;
	
	// This also sends the Command, and starts receiving the acknowledge
#ifndef SYMBIAN
	PLUTO_SAFETY_LOCK( cm, m_CommandMutex );
#endif
	if( MYSTL_SIZEOF_LIST( m_listCommands ) == 0 )
	{
#if (defined(VIPPHONE) || defined(SYMBIAN))
		m_pCommand_Sent = new BD_WhatDoYouHave();
		_LOG_( "BD_WhatDoYouHave" );
#else
		m_pCommand_Sent = new BD_HaveNothing();
		_LOG_( "BD_HaveNothing" );
#endif
	}
	else
	{
		MYSTL_EXTRACT_FIRST( m_listCommands, BDCommand, pC );
		m_pCommand_Sent = pC;
	}
#ifndef SYMBIAN
	cm.Release();
#endif
	m_pCommand_Sent->ConvertCommandToBinary();
	SendLong( m_pCommand_Sent->ID() );
	SendLong( m_pCommand_Sent->GetCommandOrAckSize() );
	SendData( m_pCommand_Sent->GetCommandOrAckSize(), m_pCommand_Sent->GetCommandOrAckData() );
	m_pCommand_Sent->FreeSerializeMemory();

	LD( f1, m_pCommand_Sent->ID(), "Sent Command", m_pCommand_Sent->Description() );
	
	// If we sent a "What do you have", we're going to get a command in return - not a normal acknowledge
	if( NULL != m_pcReceiveAckHeader ) // we didn't send a WhatDoYouHave
	{
		delete m_pcReceiveAckHeader;
		m_pcReceiveAckHeader = NULL;
	}

	if( m_pCommand_Sent->ID() == BD_PC_WHAT_DO_YOU_HAVE )  // sent the WhatDoYouHave
		m_pcReceiveAckHeader = ReceiveData(8);
	else
		m_pcReceiveAckHeader = ReceiveData(4);

	if( !m_pcReceiveAckHeader ) // got nothing
	{
		delete m_pCommand_Sent;
		m_pCommand_Sent = NULL;

		return false;
	}

	// got something
	
	long *pdwSize;
	if( m_pCommand_Sent->ID() == BD_PC_WHAT_DO_YOU_HAVE )
		pdwSize = (long *)(m_pcReceiveAckHeader + 4); // header starts from +4
	else
		pdwSize = (long *)(m_pcReceiveAckHeader); // normal header

	delete m_pcReceiveAckData;
	m_pcReceiveAckData = NULL;

	if( *pdwSize )
	{
		if ( NULL != m_pcReceiveAckData ) // got some ack data, clearing it
		{
			delete m_pcReceiveAckData;
			m_pcReceiveAckData = NULL;
		}

		m_pcReceiveAckData = ReceiveData( *pdwSize ); // getting the data for this header
		if( !m_pcReceiveAckData ) // got some
		{
			delete m_pCommand_Sent; // no need for it anymore
			m_pCommand_Sent = NULL;
			return false;
		}
	}

	if( m_pCommand_Sent->ID() == BD_PC_WHAT_DO_YOU_HAVE ) // the ack will be a command that's parsed directly
	{
		long *pdwType = (long *)m_pcReceiveAckHeader;
		long *pdwSize = (long *)(m_pcReceiveAckHeader + 4);
		ReceiveCommand( *pdwType, *pdwSize, m_pcReceiveAckData );
		if( *pdwType != BD_CP_HAVE_NOTHING )
			bImmediateCallback = true; // that's when I set it on true
	}
	else
	{
		long *pdwSize = (long *)(m_pcReceiveAckHeader);
		m_pCommand_Sent->ParseAck( *pdwSize, m_pcReceiveAckData );
	}
	delete m_pCommand_Sent;
	m_pCommand_Sent=NULL;
	return true;
}

bool BDCommandProcessor::ReceiveCommand( unsigned long dwType, unsigned long dwSize, char *pcData )
{

	// if we're passed in Data, then we explicitly parse it, otherwise we do a three
	// state thing to handle Symbian's lack of multi-threading
	
	if( dwType )
	{
		BDCommand *pCommand = BuildCommandFromData( dwType ); // creates a new command to parse the data
		pCommand->ParseCommand( dwSize, pcData );
		pCommand->ProcessCommand( this ); // takes the command processor and processes it
		pCommand->ConvertAckToBinary();
		SendLong( pCommand->GetCommandOrAckSize() );
		SendData( pCommand->GetCommandOrAckSize(), pCommand->GetCommandOrAckData() );
		pCommand->FreeSerializeMemory();

		LD( f1, pCommand->ID(), pCommand->ID() == BD_PC_WHAT_DO_YOU_HAVE ? "Sent command" : "OK", pCommand->Description() );
		_LOG_( ( pCommand->ID() == BD_PC_WHAT_DO_YOU_HAVE ? "Sent command" : "OK" ) );
		_LOG_( pCommand->Description() );
		
		if( NULL != pCommand )
		{
			delete pCommand;
			pCommand = NULL;
		}

		return true;
	}
	else
	{
		if( NULL != m_pcReceiveCmdData )
		{
#ifndef SYMBIAN
			printf( "delete receive cmd: %p\n", m_pcReceiveCmdData );
#endif
			delete m_pcReceiveCmdData;
			m_pcReceiveCmdData = NULL;
		}

		m_pcReceiveCmdData = ReceiveData( 8 );
#ifndef SYMBIAN
		printf( "create receive cmd from receive data: %p\n", m_pcReceiveCmdData );
#endif
		if( !m_pcReceiveCmdData ) // no data received
			return false;

		long *dwSize = (long *)(m_pcReceiveCmdData + 4);

		if( *dwSize )
		{
			if( NULL != m_pcReceiveCmdData ) /** @todo ask if "if" is neccessary */
			{
				delete m_pcReceiveCmdData;
				m_pcReceiveCmdData = NULL;
			}

			m_pcReceiveCmdData = ReceiveData(*dwSize);
			if( !m_pcReceiveCmdData )
				return false;
		}
		
		long *dwType = (long *) m_pcReceiveCmdData;
		dwSize = (long *)(m_pcReceiveCmdData + 4);
		BDCommand *pCommand = BuildCommandFromData( *dwType );
		if( pCommand->ID() == BD_PC_WHAT_DO_YOU_HAVE ) // received a WhatDoYouHave
		{
			bool bDummy;
			SendCommand( bDummy ); // response to WhatDoYouHave
		}
		else // a normal command received
		{
			pCommand->ParseCommand( *dwSize, m_pcReceiveCmdData );
			pCommand->ProcessCommand( this );
			pCommand->ConvertAckToBinary();
			SendLong( pCommand->GetCommandOrAckSize() );
			SendData( pCommand->GetCommandOrAckSize(), pCommand->GetCommandOrAckData() );
			pCommand->FreeSerializeMemory();
			LD( f1, pCommand->ID(), pCommand->ID()==BD_PC_WHAT_DO_YOU_HAVE ? "Sent command" : "OK", pCommand->Description() );
		}

		return true;
	}
	return false; // never get here
}

void BDCommandProcessor::SendLong(long l)
{
	SendData( 4,(const char *)&l );
}

long BDCommandProcessor::ReceiveLong()
{
	const char *pcData = ReceiveData(4);
	long *l = (long *)pcData;
	return *l;
}

void BDCommandProcessor::AddCommand( class BDCommand *pCommand )
{
#ifndef SYMBIAN
	PLUTO_SAFETY_LOCK( cm, m_CommandMutex );
#endif
	MYSTL_ADDTO_LIST( m_listCommands, pCommand );
	
	// If we're sitting waiting for a keypress, be sure to wake up that thread now
	
#ifndef SYMBIAN
	pthread_cond_broadcast( &m_PollingCond );
#endif
}

// Whatever project uses this must implement this function
extern class BDCommand *BuildCommandFromData( unsigned long dwType );

BDCommand *BDCommandProcessor::BuildCommandFromData( unsigned long dwType )
{
	switch( dwType )
	{
	case BD_PC_WHAT_DO_YOU_HAVE:
		return new BD_WhatDoYouHave();

	case BD_CP_HAVE_NOTHING:
		return new BD_HaveNothing();
	default:
		// Log Error
		return ::BuildCommandFromData(dwType);
	};
}

