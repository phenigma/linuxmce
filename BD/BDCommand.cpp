/**
 *
 * @file BDCommand.cpp
 * @brief source file for the bidirectional command
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
#include "BD/BDCommand.h"

#ifndef SYMBIAN
#include "BD/BDCommandProcessor.h"
#endif //SYMBIAN

// changing them when I'm renaming the files
// the ones starting with BD are cleaned up
/** @todo remove this comment when done */


BDCommand::BDCommand()
{
	
}

void BDCommand::ConvertCommandToBinary()
{
	StartWriting();
}

void BDCommand::ConvertAckToBinary()
{
	StartWriting();
	string Response = "OKAY";
	Write_string( Response );
}

void BDCommand::ParseCommand(unsigned long dwSize, const char *pcData)
{
	StartReading( dwSize,(char *)pcData);
}

void BDCommand::ParseAck( unsigned long dwSize, const char *pcData )
{
	StartReading(dwSize,(char *)pcData);
	string Response;
	Read_string(Response);
}

