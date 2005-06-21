#ifndef BD_CP_ShowImage_h
#define BD_CP_ShowImage_h

#include "BD/BDCommand.h"
#include "VIPShared/PlutoPhoneCommands.h"

class BD_CP_ShowImage : public BDCommand
{
	unsigned char m_iImageType;
	char *m_pImage;
	unsigned long m_ImageSize;
    unsigned long m_KeysListSize;
    char *m_pRepeatedKeysList;

    bool m_bSignalStrengthScreen;

public:
	BD_CP_ShowImage()  {m_ImageSize = 0; m_pImage = NULL; m_iImageType = 0;}
	BD_CP_ShowImage(unsigned char ImageType,unsigned long ImageSize,const char *pImage,
        unsigned long KeysListSize, const char* pRepeatedKeysList, bool bSignalStrengthScreen);

	~BD_CP_ShowImage();

	virtual void ConvertCommandToBinary();
	virtual void ParseCommand(unsigned long size,const char *data);

	// *** PURE VIRTUAL FUNCTIONS
	virtual unsigned long ID() { return BD_CP_SHOW_IMAGE; }

	virtual bool ProcessCommand(class BDCommandProcessor *pProcessor);

	virtual const char *Description() { return "Show Image"; };
};
#endif
