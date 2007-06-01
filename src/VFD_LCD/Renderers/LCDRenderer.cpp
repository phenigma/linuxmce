#include "LCDRenderer.h"
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

	string sText = display_state.m_sDescription;

	if(NULL != m_spSerial.get())
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_WARNING, "LCDRenderer: Sending '%s' to LCD", sText.c_str());
		m_spSerial->Write(const_cast<char *>(sText.c_str()), sText.length());
	}
	else
	{
		DCE::LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "LCDRenderer: Won't send '%s' to LCD. The connection is down.", sText.c_str());
	}
}
//--------------------------------------------------------------------------------------------------------