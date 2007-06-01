#include "../../Renderers/LCDRenderer.h"

int main( int argc, char* argv[] )
{
	DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, "Usage: LCDRenderer <port> <text to display>\n");

	string sSerialPort("/dev/ttyUSB0");
	string sTestText("This is a test");

	if(argc >= 2)
		sSerialPort = argv[1];

	if(argc >= 3)
		sTestText = argv[2];

	LCDRenderer lcd_renderer(sSerialPort);

	DisplayState display_state;

	display_state.m_sDescription = sTestText;
	lcd_renderer.Render(display_state);

	return 0;
}
