#include "LCDRenderer.h"

#define LINE_WIDTH 24
#define NUM_LINES   4
#define END_OF_LINE "\r\n"
//--------------------------------------------------------------------------------------------------------
LCDRenderer::LCDRenderer(string sSerialPort, unsigned int BPS, eParityBitStop ParityBitStop) : 
	m_ScreenMutex("screen rendering")
{
	m_ScreenMutex.Init(NULL);

    try 
	{
		m_spSerial.reset(new CSerialPort(sSerialPort, BPS, ParityBitStop));
		Sleep(100);
	} 
	catch(...) 
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "Failed Opening serial port: %s.", sSerialPort.c_str());
		m_spSerial.reset(NULL);
    }
}
//--------------------------------------------------------------------------------------------------------
LCDRenderer::~LCDRenderer()
{
	pthread_mutex_destroy(&m_ScreenMutex.mutex);
}
//--------------------------------------------------------------------------------------------------------
void LCDRenderer::Render(const DisplayState &display_state)
{
	PLUTO_SAFETY_LOCK(sm, m_ScreenMutex);

	string sText = 
		AsLine(display_state.m_sHeader) + 
		AsLine(display_state.m_sDescription) + 
		AsLine(display_state.m_sStatusMessage) + 
		AsEndLine(display_state.m_sFooter);

	if(NULL != m_spSerial.get())
	{
		char pClear[] = {0x1B, '[', '2', 'J'}; //ASCII CODES

		//clear screen
		m_spSerial->Write(pClear, 4);

		//send text
		DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING, "LCDRenderer: Sending '%s' to LCD", sText.c_str());
		m_spSerial->Write(const_cast<char *>(sText.c_str()), sText.length());
	}
	else
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "LCDRenderer: Won't send '%s' to LCD. The connection is down.", sText.c_str());
	}
}
//--------------------------------------------------------------------------------------------------------
string LCDRenderer::AsLine(string sText)
{
	string sResult;

	if(sText.size() <= LINE_WIDTH - 1)
		sResult = sText + END_OF_LINE;
	else if(sText.size() == LINE_WIDTH)
		sResult = sText;
	else
		sResult = sText.substr(0, LINE_WIDTH);

	return sResult;
}
//--------------------------------------------------------------------------------------------------------
string LCDRenderer::AsEndLine(string sText)
{
	string sResult;

	if(sText.size() <= LINE_WIDTH)
		sResult = sText;
	else
		sResult = sText.substr(0, LINE_WIDTH);

	return sResult;
}
//--------------------------------------------------------------------------------------------------------
