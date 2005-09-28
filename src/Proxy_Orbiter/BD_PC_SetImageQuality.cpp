#include "PlutoUtils/CommonIncludes.h"
#include "BDCommandProcessor_Proxy_Orbiter.h"
#include "BD_PC_SetImageQuality.h"

BD_PC_SetImageQuality::BD_PC_SetImageQuality(unsigned long nImageQuality) 
{
    m_nImageQuality = nImageQuality;
}

void BD_PC_SetImageQuality::ConvertCommandToBinary()
{
	BDCommand::ConvertCommandToBinary();
    Write_long(m_nImageQuality);
}

void BD_PC_SetImageQuality::ParseCommand(unsigned long size,const char *data)
{
	BDCommand::ParseCommand(size,data);
    m_nImageQuality = Read_long();
}

bool BD_PC_SetImageQuality::ProcessCommand(BDCommandProcessor *pProcessor)
{
    BDCommandProcessor_Proxy_Orbiter *m_pProcessor = 
        (BDCommandProcessor_Proxy_Orbiter *)pProcessor;

//	m_pProcessor->SetImageQuality(m_nImageQuality);

    return true;
}

