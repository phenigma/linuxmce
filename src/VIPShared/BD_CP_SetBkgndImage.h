#ifndef BD_CP_SetBkgndImage_h
#define BD_CP_SetBkgndImage_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_CP_SetBkgndImage : public BDCommand
{
	unsigned char m_iImageType;
	char *m_pImage;
	unsigned long m_ImageSize;

public:
	BD_CP_SetBkgndImage()  { m_ImageSize = 0; m_pImage = NULL; m_iImageType = 0; }
	BD_CP_SetBkgndImage( unsigned char ImageType, unsigned long ImageSize,const char *pImage );

	~BD_CP_SetBkgndImage();

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_SET_BKGND_IMAGE; }

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);

	virtual const char *Description() { return "Set background Image"; };
};
#endif
