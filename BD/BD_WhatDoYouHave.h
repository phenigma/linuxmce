/**
 *
 * @file BD_WhatDoYouHave.h
 * @brief header file for the bidirectional WhatDoYouHave command classs
 * @author
 * @todo notcommented
 *
 */


#ifndef BDPCWHATDOYOUHAVE
#define BDPCWHATDOYOUHAVE

#include "BD/BDCommand.h"
#include "BDCommandProcessor.h"

/**
 * @brief the command sent to find out if the outher side has any commands
 * the ack in this case will be a set of commands
 */
class BD_WhatDoYouHave : public BDCommand 
{
public:

	/**
	 * @brief empty constructor (nothing to be done)
	 */
	BD_WhatDoYouHave()  {}
	
	
	/**
	 * @brief there's no Ack sent for this, we send a command instead
	 */
        virtual void ConvertAckToBinary() {};
	
	/**
	 * @brief nothing to be done
	 */
        virtual void ParseAck(unsigned long size,const char *data) {};


	/**
	 * @brief just returns the BD_PC_WHAT_DO_YOU_HAVE defined value
	 */
	virtual unsigned long ID() { return BD_PC_WHAT_DO_YOU_HAVE; }

	/**
	 * @brief nothing to be done
	 */
	virtual bool ProcessCommand( class BDCommandProcessor *pProcessor ) { return true; };

	virtual const char *Description() { return "What do you have?"; };
};


#endif
