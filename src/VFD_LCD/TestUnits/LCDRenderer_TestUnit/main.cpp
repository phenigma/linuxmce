#include "../../Renderers/LCDRenderer.h"

int main( int argc, char* argv[] )
{
	string sSerialPort("/dev/ttyUSB0");
	DisplayState display_state;

	char c;
	for(int optnum=1;optnum<argc;++optnum)
	{
		c = argv[optnum][1];
		switch (c)
		{
		case 'h':
			display_state.m_sHeader = argv[++optnum];
			break;
		case 'd':
			display_state.m_sDescription = argv[++optnum];
			break;
		case 's':
			display_state.m_sStatusMessage = argv[++optnum];
			break;
		case 'f':
			display_state.m_sFooter = argv[++optnum];
			break;
		case 'p':
			sSerialPort = argv[++optnum];
			break;
		default:
			DCE::LoggerWrapper::GetInstance()->Write(LV_STATUS, 
				"\r\n\tUsage: LCDRenderer \r\n"
				"\t\t-p <port> \r\n"
				"\t\t-h <header> \r\n"
				"\t\t-d <description> \r\n"
				"\t\t-s <status> \r\n"
				"\t\t-f <footer> \r\n");
			break;
		};
	}

	LCDRenderer lcd_renderer(sSerialPort);
	lcd_renderer.Render(display_state);

	return 0;
}
