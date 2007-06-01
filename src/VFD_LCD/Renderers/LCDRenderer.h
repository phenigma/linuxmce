#ifndef __LCD_RENDERER_H__
#define __LCD_RENDERER_H__
//--------------------------------------------------------------------------------------------------------
#include "DCE/Logger.h"
#include "Serial/SerialPort.h"
//--------------------------------------------------------------------------------------------------------
#include "../LCDLogic/IRenderer.h"
//--------------------------------------------------------------------------------------------------------
class LCDRenderer : public IRenderer
{
	int m_fd;
	pluto_pthread_mutex_t m_ScreenMutex;

	auto_ptr<CSerialPort> m_spSerial;

public:

	LCDRenderer(string sSerialPort, unsigned int bps = 19200, eParityBitStop parity = epbsN81);
	~LCDRenderer();

	void Render(const DisplayState &display_state);
};
//--------------------------------------------------------------------------------------------------------
#endif //__MENU_LOADER_H__
